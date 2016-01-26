#include "curlthread.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

// static function declaration.
static int findHeader(unsigned char target[], const unsigned char *memory, int memorySize, int cachedPos);
static void* CURL_realloc(void *ptr, size_t size);

// member functions
CurlThread::CurlThread(const char *pszURL, QObject *parent)
    :QThread(parent)
{
    // allocation for memory and initialization
    mMemory = (MemStruct*)malloc(sizeof(MemStruct));
    mMemory->pMemory = nullptr;
    mMemory->size = 0;

    // init libcurl
    this->mCtx = curl_easy_init();
    curl_easy_setopt(mCtx, CURLOPT_URL, "http://devjhlab.iptime.org:8080/?action=stream");
    curl_easy_setopt(mCtx, CURLOPT_WRITEFUNCTION, &CurlThread::writeMemoryCallback);

    // callback should be able to refer to which thread called him.
    curl_easy_setopt(mCtx, CURLOPT_WRITEDATA, (void*)(this));
}

CurlThread::~CurlThread()
{
    free(mMemory->pMemory);
    free(mMemory);
    curl_easy_cleanup(mCtx);
}


void CurlThread::run()
{
    CURLcode res;

    if (mCtx != nullptr)
    {
        res = curl_easy_perform(mCtx);
    }

    // enter event loop
    exec();
}

/*************************************************************************************************
 *
 *          static member function
 *
 *
 *************************************************************************************************/
// static member function
size_t
CurlThread::writeMemoryCallback (void *receivedPtr, size_t size, size_t nmemb, void *userData)
{
    static int cachedX = -1;
    static int cachedY = -1;
    static unsigned char soi[] = { 0xff, 0xd8 };
    static unsigned char eoi[] = { 0xff, 0xd9 };

    size_t realSize = size * nmemb;

    CurlThread *masterThread = (CurlThread*)userData;
    struct MemStruct *mem = (MemStruct*)(masterThread->mMemory);

    // memory expansion.
    mem->pMemory = (unsigned char*)CURL_realloc(mem->pMemory, mem->size + realSize +1);

    if (mem->pMemory)
    {
        // append
        memcpy(&(mem->pMemory[mem->size]),receivedPtr, realSize);
        // change the size to the (old size + added size)
        mem->size += realSize;
        // make the last memory value to 0
        mem->pMemory[mem->size] = 0;

        if (cachedX == -1)  // check whether we need to find Start Header ( 0xffd8 )
        {
            cachedX = findHeader(soi, mem->pMemory, mem->size, 0);
        }
        if (cachedY == -1)  // check whether we need to find End Header ( 0xffd9 )
        {
            cachedY = findHeader(eoi, mem->pMemory, mem->size, 0);
        }

        if (cachedX != -1 && cachedY != -1)
        {
            if (cachedX < cachedY)
            {
                // calculate the new buffer size
                unsigned int newBufferSize = (mem->size - (cachedY + 2));
                unsigned int imageSize = (cachedY - cachedX + 2);

                // image memory copy
                unsigned char *imageBuffer = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
                memcpy(imageBuffer, &(mem->pMemory[cachedX]), imageSize);

                // memory copy
                unsigned char *newBuffer = (unsigned char*)malloc(sizeof(unsigned char) * newBufferSize);
                memcpy(newBuffer, &(mem->pMemory[cachedY+2]), newBufferSize);

                // deallocate the old buffer
                free(mem->pMemory);

                // save the adress of new buffer to the pMemory
                // also size of the new buffer is assigned
                mem->pMemory = newBuffer;
                mem->size = newBufferSize;

                // reset cache (find position)
                cachedX = cachedY = -1;

                // emit SIGNAL!
                emit masterThread->imageIsReady(imageSize, imageBuffer);

                // little bit sleeping...
                QThread::msleep(1);
            }
        }
    }


    return realSize;
}

/*****************************************
 * static function and isn't included in CurlWorker Class
 *****************************************/
static int findHeader(unsigned char target[], const unsigned char *memory, int memorySize, int cachedPos)
{
    int foundPosition = -1;

    for (int i = cachedPos; i < memorySize; ++i)
    {
        // first find 0xff
        if (memory[i] == target[0] && memory[i+1] == target[1])
        {
            foundPosition = i;
        }
    }

    return foundPosition;
}

static void* CURL_realloc(void *ptr, size_t size)
{
    if (ptr)
    {
        return realloc(ptr, size);
    }
    else
    {
        return malloc(size);
    }
}
