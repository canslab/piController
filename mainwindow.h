#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:
    void on_cameraOn_clicked();
    void showVideoAtLabel(int imageSize, unsigned char *imageData);


private:
    Ui::MainWindow *ui;
    CurlThread *mCurlThread;
};

#endif // MAINWINDOW_H
