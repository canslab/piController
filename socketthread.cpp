#include "socketthread.h"
#include <assert.h>

/**************************************************************
 *                    normal methods                          *
 * ***********************************************************/
void SocketThread::run()
{

    this->exec();
}

/***************************************************************
 *
 *                PUBLIC SLOTS
 *
 *                        SIGNAL
 *          User(clinet) -------> this(QThread)
 *
 *
 ***************************************************************/

/* when the user request connection, connectToHost will be called. */
/* It connects to the remote address desginated by string argument */
void SocketThread::connectToHost(const std::string &url, uint16_t nPort)
{
    assert(url.c_str() != nullptr);

    // connect to host using url, nPort
    // because the type of url is not consitent with QString,
    // it should be converted into (const char*) type.
    // c_str() does.
    this->mSocket->connectToHost(url.c_str(), nPort);
}

void SocketThread::disconnectFromHost()
{
    assert(bConnected == true);
    bConnected = false;
    this->mSocket->disconnectFromHost();
}

/***************** Excpetion SLOT ****************************
 *
 *   It reads from socket and save it to the buffer,
 *   and then it emits signals to the user, like Read Is DONE!!
 *   so, you can use your buffer!
 *
 *   CAUTION: it emits readDone signal,
 *
 *   [ readDone's parameter description ]
 *     errorCode = 0      : normal
 *     errorCode = 1      : error
 *     errorCode = 2      : EOF
 *
 *    User(Clinet) -----> this(QThread) ------> User(Clinet)
 *
 * ***********************************************************/

void SocketThread::readFromSockect(char *buffer, int readUpTo)
{
    assert(bConnected == true);
    assert(buffer != nullptr);

    // errorCode = 0     : normal
    // errorCode = 1     : error
    // errorCode = 2     : EOF
    int errorCode = 0;

    auto readByteSize = this->mSocket->read(buffer, readUpTo);

    if (readByteSize == -1)
    {
        errorCode = 1;
    }
    else if(readByteSize == 0)
    {
        errorCode = 2;
    }
    else
    {
        errorCode = 0;
    }

    // emit read done!
    emit yourReadDone(errorCode, buffer, readByteSize);
}

/***************************************************************
 *
 *                      PROTECTED SLOTS
 *
 *                 SIGNAL                   SIGNAL
 *        mSocket  ------>   this(QThread)  ----->  User(client)
 *
 *     it should notify user to cope with this situation.
 *
 * ************************************************************/
void SocketThread::whenConnectedDone()
{
    bConnected = true;
    emit youConnected();
}

void SocketThread::whenDisconnected()
{
    bConnected = false;
    emit youDisconnected();
}

void SocketThread::whenReadyRead()
{
    emit yourReadIsReady();
}

/***************************************************************
 *
 *               Constructor & Destructor
 *
 * ************************************************************/

SocketThread::SocketThread(QObject *parent)
    :QThread(parent)
{
    bConnected = false;

    // socket memory allocation.
    mSocket = new QTcpSocket();

    // associate signal(QTcpSocket) & slot(this=QThread)
    connect(mSocket, SIGNAL(connected()), this, SLOT(whenConnectedDone()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(whenDisconnected()));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(whenReadyRead()));

    // move thread affinity to this(QTHread)
    // After this statement, mSocket's evernt processing will continue in this thread.
    // not in the gui thread(previous thread)
    mSocket->moveToThread(this);
}

SocketThread::~SocketThread()
{
    assert(mSocket != nullptr);
    delete mSocket;
}
