#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <string>

class SocketThread : public QThread
{
    Q_OBJECT
signals:
    /***********************************************************
     *
     *      this(QThread) ----->  User(Client)
     *
     * *********************************************************/

    // when there are available data in the read stream, whenReadyRead() slot will
    // emit yourReadIsReady()
    void youCanRead();
    void yourReadDone(int errorCode, char *buffer, int contentSize);
    void youDisconnected();
    void youConnected();

public slots:
    /***************************************************************
     * Client to this
     *                PUBLIC SLOTS
     *
     *                        SIGNAL
     *          User(clinet) -------> this(QThread)
     *
     *
     ***************************************************************/

    /* when the user request connection, connectToHost will be called. */
    /* It connects to the remote address desginated by string argument */
    void connectToHost(const std::string& url, uint16_t nPort);

    /* when the user request disconnection, disconnectFromHost() will be invoked */
    void disconnectFromHost();

    /***************** Excpetion SLOT ****************************
     * Client to this
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
    void readFromSockect(char *buffer, int readUpTo);

protected slots:
    /*******************************************************************
     * mSocket to this
     *                      PROTECTED SLOTS
     *
     *                 SIGNAL                   [  SIGNAL              ]
     *        mSocket  ------>   this(QThread)  [ ----->  User(client) ]
     *
     *     it should notify user to cope with this situation.
     *
     * *****************************************************************/

    /* when the connection is end, whenConnectedDone() will be invoked */
    void whenConnectedDone();

    /* when the remote device disconnected, whenDisconnected() will be invoked */
    void whenDisconnected();

    /* when there are available data in the read stream, it will be invoked */
    void whenReadyRead();

public:
    explicit SocketThread(QObject *parent = 0);
    ~SocketThread();

protected:
    virtual void run();

protected:
    QTcpSocket *mSocket;
    std::pair<std::string, uint16_t> remoteAddress;

    int readSizeAtOnce;
    bool bConnected;
};

#endif // SOCKETTHREAD_H
