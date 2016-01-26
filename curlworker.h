/*******************************************************************************
 *
 * Data     : 2016/01/26
 * autoher  : Jangho Park @ Hanyang Univ. CSE
 * email    : devjh1106@gmail.com
 *
 *
 *
 * CurlWorker is the worker which is included in the specific thread(exec)
 *
 * It fetches data from the Internet using HTTP protocol ,
 * and in my application, it is used to load data from mpjpg-streamer(pi Side)
 *
 * It emits siganl which indicates jpeg image is ready
 *
 *
 *********************************************************************************/

#ifndef CURLWORKER_H
#define CURLWORKER_H

#include <QObject>
#include <curl/curl.h>
class CurlWorker : public QObject
{
    Q_OBJECT

public:
    struct MemoryStruct
    {
        unsigned char *pMemory;
        int size;
    };

public:
    explicit CurlWorker(const char *pszURL, QObject *parent = 0);
    ~CurlWorker();

signals:
    // [CAUTION] : imageData should be deallocated at use-side.
    void imageIsReady(int imageSize, const unsigned char *imageData);

public slots:
    void processStream();

private:
    size_t writeMemoryCallback(void*, size_t, size_t, void*);

private:
    MemoryStruct *mMemory;
    CURL *mCtx;
};

#endif // CURLWORKER_H
