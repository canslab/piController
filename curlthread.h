#ifndef CURLTHREAD_H
#define CURLTHREAD_H
#include <QThread>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>

using namespace cv;

struct MemStruct
{
    unsigned char *pMemory;
    unsigned int size;
};

// there should be no slots!!
class CurlThread : public QThread
{
    Q_OBJECT  

public:    
    explicit CurlThread(const char *pszURL, QObject *parent = 0);
    ~CurlThread();

signals:
    // when image is ready, CurlThread emit this signal to notify the client to receive Mat data
    void imageIsReady(cv::Mat *frameMat);

public:
    static size_t writeMemoryCallback (void *receivedPtr, size_t size, size_t nmemb, void *userData);

protected:
    virtual void run();

private:
    CURL *mCtx;
    MemStruct *mMemory;
    Mat mFrameMat;
};

#endif // CURLTHREAD_H
