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

    mVideoLabelRect = ui->videoLabel->geometry();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cameraOn_clicked()
{
    myTime.start();
    mCurlThread->start();
}

// this slot would be called every 50 msec
void MainWindow::showVideoAtLabel(cv::Mat *frame)
{
    ui->cameraOn->setEnabled(false);

    // elaseped time == about 0 ~ 1 msec
    cv::Mat decodedImage = *frame;

    cv::cvtColor(decodedImage, decodedImage, CV_BGR2RGB);
    QImage img = QImage((uchar*)decodedImage.data, decodedImage.cols, decodedImage.rows, decodedImage.step,
                        QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img);
    ui->videoLabel->setPixmap(pix);
}

/********************************************************************************
 *  event handler, ex) mouse event, keyboard event
 *
 * ******************************************************************************/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_W:     /* Up Button */
        ui->test->setText("Up!");
        break;
    case Qt::Key_A:     /* Left Button */
        ui->test->setText("Left!");
        break;
    case Qt::Key_S:     /* Down Button */
        ui->test->setText("Down!");
        break;
    case Qt::Key_D:     /* Right Button */
        ui->test->setText("Right!");
        break;
    case Qt::Key_L:
        ui->test->setText("Led!");
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    const int videoLabelX = mVideoLabelRect.x();
    const int videoLabelY = mVideoLabelRect.y();
    QPoint mosPos = event->pos();

    auto labelText = QString::asprintf("x = %d, y = %d", mosPos.x() - videoLabelX, mosPos.y() - videoLabelY);
    ui->mousePosLabel->setText(labelText);
}
