#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <QPixmap>
#include <QImage>
#include <QtCore>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mCurlThread = new CurlThread("http://devjhlab.iptime.org:8080/?action=stream");

    // when mCurlThread's event loop has died, mCurlThread should be removed.
    connect(mCurlThread, &CurlThread::finished, mCurlThread, &CurlThread::deleteLater);
    connect(mCurlThread, &CurlThread::imageIsReady, this, &MainWindow::showVideoAtLabel);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Q)
    {
        ui->test->setText("QQ!!");
    }
}

void MainWindow::on_cameraOn_clicked()
{
    myTime.start();
    mCurlThread->start();
}

// this slot would be called every 50 msec
void MainWindow::showVideoAtLabel(cv::Mat *frame)
{
//    qDebug() << myTime.elapsed();
    ui->cameraOn->setEnabled(false);

    // elaseped time == about 0 ~ 1 msec
    cv::Mat decodedImage = *frame;

    cv::cvtColor(decodedImage, decodedImage, CV_BGR2RGB);
    QImage img = QImage((uchar*)decodedImage.data, decodedImage.cols, decodedImage.rows, decodedImage.step,
                        QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img);
    ui->videoLabel->setPixmap(pix);
}
