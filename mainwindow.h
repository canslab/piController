#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QKeyEvent>
#include "curlthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_cameraOn_clicked();
    void showVideoAtLabel(cv::Mat *frameMat);


private:
    Ui::MainWindow *ui;
    CurlThread *mCurlThread;


    QTime myTime;
};

#endif // MAINWINDOW_H
