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
    void requestRead(char *buffer, int maxLength);
    void requestConnection(const std::string &address, uint16_t port);
    void requestDisconnection();

public:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void whenReadIsReady();
    void whenReadJobDone(int errorCode, char *buffer, int maxLength);
    void whenConnectionDone();
    void whenDisconnectionDone();

    void on_cameraOn_clicked();
    void showVideoAtLabel(cv::Mat *frameMat);

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    CurlThread *mCurlThread;
    SocketThread *mSocketThread;

    QTime myTime;

private:    /* constant */
    // it contains the size info of video label
    QRect mVideoLabelRect;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
