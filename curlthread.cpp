#include "curlthread.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <QTime>

using namespace cv;

// static function declaration.
static int findHeader(unsigned char target[], const unsigned char *memory, int memorySize, int cachedPos);

// member functions
CurlThread::CurlThread(const char *pszURL, QObject *parent)
    :QThread(parent)
{
    // init libcurl
    this->mCtx = curl_easy_init();
    curl_easy_setopt(mCtx, CURLOPT_URL, pszURL);
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
    // allocation for memory and initialization
    mMemory = (MemStruct*)malloc(sizeof(MemStruct));
    mMemory->size = 0;

    CURLcode res;

    if (mCtx != nullptr)
    {
        res = curl_easy_perform(mCtx);
    }
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
            cachedX = findHeader(soi, mem->pMemory, mem->size, cachedX);
        }
        if (cachedY == -1)  // check whether we need to find End Header ( 0xffd9 )
        {
            cachedY = findHeader(eoi, mem->pMemory, mem->size, cachedY);
        }

        if (cachedX != -1 && cachedY != -1)
        {
//            if (cachedX < cachedY)
//            {
                // calculate the new buffer size
                unsigned int newBufferSize = (mem->size - (cachedY + 2));
                unsigned int imageSize = (cachedY - cachedX + 2);

                masterThread->mFrameMat = imdecode(Mat(1, imageSize, CV_8UC1, &(mem->pMemory[cachedX])), CV_LOAD_IMAGE_UNCHANGED);
                cv::resize(masterThread->mFrameMat, masterThread->mFrameMat, cv::Size(640, 480));

                // when your image is ready, notify the gui thread that image is ready
                emit masterThread->imageIsReady(&(masterThread->mFrameMat));

                // memory copy
                memcpy(mem->pMemory, &(mem->pMemory[cachedY+2]), newBufferSize);
                mem->size = newBufferSize;

                // reset cache (find position)
                cachedX = cachedY = -1;
//            }
        }
    }
    return realSize;
}

/*****************************************
 * static function and isn't included in CurlWorker Class
 *
 * [ Objectives ]
 *
 * it finds where the target is.
 * cachedPos is used to help this routine find more faster.
 *
 * [ Parameter description ]
 *
 * target = pointer to array that want to be found
 * memory = pointer to memory array that would be investigated
 * memorySize = the size of memory
 * cachedPos = to help this routine find position of the target easy.
 *
 * [ Return description ]
 *
 * -1         : no found
 * otheriwse  : the position of the target. (index would start from zero)
 *
 *****************************************/
static int findHeader(unsigned char target[], const unsigned char *memory, int memorySize, int cachedPos)
{
    assert(cachedPos >= -1);
    assert(target != nullptr);
    assert(memory != nullptr);
    assert(memorySize > 0);

    int foundPosition = -1;
    int leftSeeker = cachedPos - 1;
    int rightSeeker = cachedPos;

    bool leftEnd = false;
    bool rightEnd = false;


//    while (leftSeeker >= 1 || rightSeeker < memorySize)
    while (leftEnd == false || rightEnd == false)
    {
        if (leftSeeker >= 1)
        {
            if (memory[leftSeeker] == target[1] && memory[leftSeeker - 1] == target[0])
            {
                foundPosition = leftSeeker - 1;
                break;
            }
            leftSeeker--;
        }
        else
        {
            leftEnd = true;
        }

        if (rightSeeker < memorySize)
        {
            if (memory[rightSeeker] == target[0] && memory[rightSeeker + 1] == target[1])
            {
                foundPosition = rightSeeker;
                break;
            }
            rightSeeker++;
        }
        else
        {
            rightEnd = true;
        }
    }
    return foundPosition;
}

