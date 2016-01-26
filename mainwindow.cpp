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
//    delete mCurlThread;
}

void MainWindow::on_cameraOn_clicked()
{
    mCurlThread->start();
}

void MainWindow::showVideoAtLabel(int imageSize, unsigned char *imageData)
{
    ui->cameraOn->setEnabled(false);
    auto decodedImage = cv::imdecode(cv::Mat(1, imageSize, CV_8UC1, imageData),CV_LOAD_IMAGE_UNCHANGED);
    cv::resize(decodedImage,decodedImage, cv::Size(ui->videoLabel->width(), ui->videoLabel->height()));
    cv::cvtColor(decodedImage, decodedImage, CV_BGR2RGB);

    QImage img = QImage((uchar*)decodedImage.data, decodedImage.cols, decodedImage.rows, decodedImage.step,
                        QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img);
    ui->videoLabel->setPixmap(pix);

    // free the image Data
    free(imageData);

    cv::waitKey(1);
}
