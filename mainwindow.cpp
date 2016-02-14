#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <QPixmap>
#include <QImage>
#include <QtCore>
#include "assert.h"


/************************************************************************
 *          constructor & destructor
 * *********************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // curl thread
    mCurlThread = new CurlThread("http://devjhlab.iptime.org:8080/?action=stream");

    // socket thread
    mSocketThread = new SocketThread(this);

    // when mCurlThread's event loop has died, mCurlThread should be removed.
    connect(mCurlThread, &CurlThread::finished, mCurlThread, &CurlThread::deleteLater);
    connect(mCurlThread, &CurlThread::imageIsReady, this, &MainWindow::showVideoAtLabel);

    //  mSocketThread -----> GUI Thread (This Thread)
    connect(mSocketThread, SIGNAL(youCanRead()), this, SLOT(whenReadIsReady()));
    connect(mSocketThread, SIGNAL(yourReadDone(int,char*,int)), this, SLOT(whenReadJobDone(int, char*, int)));
    connect(mSocketThread, SIGNAL(youConnected()), this, SLOT(whenConnectionDone()));
    connect(mSocketThread, SIGNAL(youDisconnected()), this, SLOT(whenDisconnectionDone()));

    // GUI Thread ----> mSocketThread
    connect(this, SIGNAL(requestRead(char*,int)), mSocketThread, SLOT(readFromSockect(char*,int)));
    connect(this, SIGNAL(requestConnection(std::string, uint16_t)), mSocketThread, SLOT(connectToHost(std::string, uint16_t)));
    connect(this, SIGNAL(requestDisconnection()), mSocketThread, SLOT(disconnectFromHost()));

    // run SocketThread
    mSocketThread->start();

    mVideoLabelRect = ui->videoLabel->geometry();
}

MainWindow::~MainWindow()
{
    delete ui;
}
/************************************************************************
 *          normal method
 * *********************************************************************/
void MainWindow::on_cameraOn_clicked()
{
    myTime.start();
    mCurlThread->start();
}

// When CurlThread is ready to send you a frame, this slot would be called...
//  ( because you register signal-slot relationship in your initializer )
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

/*****************************************
 *          private SLOTS
 *
 * **************************************/
void MainWindow::whenReadIsReady()
{
    /* *************************************************************************
     *
     *      when you are able to read the data from remote PC,
     *      you should make an buffer to read from.
     *      and give its address to Socket Thread.
     *      Socket Thread will return the written buffer
     *      you can access that buffer from  MainWindow::whenReadJobDone() method.
     *
     * *************************************************************************/

    // ready for buffer, protocol size's length is 8
    char *buffer = new char[8];

    emit requestRead(buffer, 8);
}

void MainWindow::whenReadJobDone(int errorCode, char *buffer, int maxLength)
{
    assert(buffer != nullptr);
    qDebug() << buffer[0];

    // deallocate used memory
    delete buffer;
}

void MainWindow::whenConnectionDone()
{
    qDebug() << "[SOCKET] connected..";
}

void MainWindow::whenDisconnectionDone()
{
    qDebug() << "[SOCKET] Disconnected..";
}

void MainWindow::on_connectButton_clicked()
{
    ui->connectButton->setEnabled(false);
    qDebug() << "[SOCKET] Connect Trying...";
    emit requestConnection("devjhlab.iptime.org", 55555);
}

void MainWindow::on_disconnect_button_clicked()
{
    emit requestDisconnection();
}
