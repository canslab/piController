#ifndef CURLTHREAD_H
#define CURLTHREAD_H
#include <QThread>
#include <curl/curl.h>

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
    void imageIsReady(int imageSize, unsigned char *imageData);

public:
    static size_t writeMemoryCallback (void *receivedPtr, size_t size, size_t nmemb, void *userData);

protected:
    virtual void run();

private:
    CURL *mCtx;
    MemStruct *mMemory;

};

#endif // CURLTHREAD_H
