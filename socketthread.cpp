#include "socketthread.h"
#include <assert.h>

/**************************************************************
 *                    normal methods                          *
 * ***********************************************************/
void SocketThread::run()
{
    qDebug() << this->thread();
    // initialization
    m_readSizeAtOnce = 0;
    m_bConnected = false;

    // socket memory allocation.
    m_socket = new QTcpSocket();
    qDebug() << m_socket->thread();

    // associate signal(QTcpSocket) & slot(this=QThread)
    connect(m_socket, SIGNAL(connected()), this, SLOT(whenConnectedDone()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(whenDisconnected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(whenReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(whenErrorOccured(QAbstractSocket::SocketError)));

    // move thread affinity to this(QTHread)
    // After this statement, mSocket's evernt processing will continue in this thread.
    // not in the gui thread(previous thread)
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
void SocketThread::connectToHost(const char *url, unsigned short nPort)
{
    assert(url != nullptr);

    qDebug() << "[SOCKET @ " << this->thread() << "] connectToHost() called.";
    // connect to host using url, nPort
    // because the type of url is not consitent with QString,
    // it should be converted into (const char*) type.
    // c_str() does.
    this->m_socket->connectToHost(url, nPort);
}

void SocketThread::disconnectFromHost()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] disconnectedFromHost() called.";
    assert(m_bConnected == true);
    m_bConnected = false;
    this->m_socket->disconnectFromHost();
}

/* when the user want to send data to socket */
size_t SocketThread::writeToSocket(char *buffer, int bufferSize)
{
    assert(buffer != nullptr);
    assert(bufferSize >= 0);
    assert(this->m_bConnected == true);

    qDebug() << "[SOCKET @ " << this->thread() << "] writeToSocket() called.";

    // write to socket.
    auto writtenLength = m_socket->write(buffer, bufferSize);
    qDebug() << "[SOCKET @ " << this->thread() << "] writeToSocket(), " << writtenLength << " byte wrote! ";

    if (writtenLength != bufferSize)
    {
        qDebug() << "[SOCKET @ " << this->thread() << "] writtenLength != intended size";
        // if the actual size of the written data is different from
        // size of the buffer, it would be the ERROR case.

    }
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
    qDebug() << "[SOCKET @ " << this->thread() << "] readFromSocket() called.";
    assert(m_bConnected == true);
    assert(buffer != nullptr);

    // errorCode = 0     : normal
    // errorCode = 1     : error
    // errorCode = 2     : EOF
    int errorCode = 0;

    auto readByteSize = this->m_socket->read(buffer, readUpTo);

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
    qDebug() << "[SOCKET @ " << this->thread() << "] whenConnectionDone() called.";
    m_bConnected = true;
    emit youConnected();
}

void SocketThread::whenDisconnected()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] whenDisconnected() called.";
    m_bConnected = false;

    // emit the signal that notify the client "You disconnected from the remote device!"
    emit youDisconnected();
}

void SocketThread::whenReadyRead()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] whenReadyRead() called.";
    emit youCanRead();
}

void SocketThread::whenErrorOccured(QAbstractSocket::SocketError error)
{
    qDebug() << "[SOCKET @ " << this->thread() << "] whenErrorOccured() called.";
    switch (error)
    {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "[SOCKET] Remote Host Closed Error..";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "[SOCKET] Host Not Found Error";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "[SOCKET] Connection Refused Error..";
        break;
    default:
        qDebug() << m_socket->errorString();
    }

}

/***************************************************************
 *
 *               Constructor & Destructor
 *
 * ************************************************************/

SocketThread::SocketThread(QObject *parent)
    :QThread(parent)
{
    // all slot routines will be managed by the new Thread ( == SocketThread )
    // not GUI Thread that made this socket thread.
    this->moveToThread(this);
}

SocketThread::~SocketThread()
{
    assert(m_socket != nullptr);

    // before deallocate socket, close the stream
    m_socket->close();
    delete m_socket;
}
