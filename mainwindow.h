#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QKeyEvent>
#include <string>
#include "curlthread.h"
#include "socketthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    /*************************************************************************
     *
     * 1 : If you want to request read operation to Socket THREAD, you MUST
     *     emit requestRead(char*, int) signal
     *
     * 2 : If you want to connect to the remote device via socket, you MUST
     *     emit requestConnection(string&, uint16_t) with ip address and port number
     *
     * 3 : If you want to disconnect from the remote device, you MUST
     *     emit requestDisconnection() signal
     *
     * ***********************************************************************/
    void requestRead(char *buffer, int maxLength);
    void requestConnection(const std::string &address, uint16_t port);
    void requestDisconnection();

public:
    // keyboard & mouse event handler
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    // related to socket communication
    /* *************************************************************************
     *
     *  1 : when you are able to read the data from remote PC,
     *      whenReadIsReady() will be invoked.
     *
     *  2 : As soon as the written buffer is ready,
     *      whenReadJobDone() will be called.
     *
     *      you can access that buffer in whenReadJobDone(), and you should deallocate
     *      the written buffer from that function.
     *
     * *************************************************************************/
    void whenReadIsReady();                                                // 1
    void whenReadJobDone(int errorCode, char *buffer, int maxLength);      // 2

    /*****************************************************************************
     *
     *  1  : When your connection establishes, whenConnectionDone() will be called
     *  2  : Or, your connection disconnected, whenDisconnectionDone() will be called
     *
     * ****************************************************************************/
    void whenConnectionDone();          // 1
    void whenDisconnectionDone();       // 2

    /*******************************************************************************
     *
     *  i) When you clicked 'camera' button, on_cameraOn_clicked() will be called
     *
     *
     * ****************************************************************************/
    void on_cameraOn_clicked();
    void showVideoAtLabel(cv::Mat *frameMat);

    void on_connectButton_clicked();
    void on_disconnect_button_clicked();

private:
    Ui::MainWindow *ui;             // UI pointer
    CurlThread *mCurlThread;        // curl   THREAD, it streams the video from the remote device
    SocketThread *mSocketThread;    // socket THREAD, it connectes to the remote device, and do communication with it.

    QTime myTime;                   // for debugging

private:
    /* constant */
    // it contains the size info of video label
    QRect mVideoLabelRect;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
