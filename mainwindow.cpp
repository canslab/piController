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
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // if the remote device is connected, this variable will be 1 otherwise 0
    m_bSocketThreadConnected = false;

    // curl thread memory allocation
    m_curlThread = new CurlThread("http://devjhlab.iptime.org:8080/?action=stream");

    // socket thread memory allocation
    m_socketThread = new SocketThread();

    // when mCurlThread's event loop has died, mCurlThread should be removed.
    connect(m_curlThread, &CurlThread::finished, m_curlThread, &CurlThread::deleteLater);
    connect(m_curlThread, &CurlThread::imageIsReady, this, &MainWindow::showVideoAtLabel);

    //  mSocketThread -----> GUI Thread (This Thread)
    connect(m_socketThread, SIGNAL(yourReadDone(int,char*,int)), this, SLOT(whenReadJobDone(int, char*, int)));
    connect(m_socketThread, SIGNAL(youCanRead()), this, SLOT(whenReadIsReady()));
    connect(m_socketThread, SIGNAL(youConnected()), this, SLOT(whenConnectionDone()));
    connect(m_socketThread, SIGNAL(youDisconnected()), this, SLOT(whenDisconnectionDone()));

    // GUI Thread ----> mSocketThread
    connect(this, SIGNAL(requestRead(char*,int)), m_socketThread, SLOT(readFromSockect(char*,int)));
    connect(this, SIGNAL(requestConnection(const char*, unsigned short)), m_socketThread, SLOT(connectToHost(const char*, unsigned short)));
    connect(this, SIGNAL(requestDisconnection()), m_socketThread, SLOT(disconnectFromHost()));
    connect(this, SIGNAL(requestWriting(const char*,int)), m_socketThread, SLOT(writeToSocket(const char*,int)));


    // run SocketThread
    m_socketThread->start();

    m_videoLabelRect = m_ui->videoLabel->geometry();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
/************************************************************************
 *          normal method
 * *********************************************************************/
void MainWindow::on_cameraOn_clicked()
{
    m_timer.start();
    m_curlThread->start();
}

// When CurlThread is ready to send you a frame, this slot would be called...
//  ( because you register signal-slot relationship in your initializer )
void MainWindow::showVideoAtLabel(cv::Mat *frame)
{
    m_ui->cameraOn->setEnabled(false);

    // elaseped time == about 0 ~ 1 msec
    cv::Mat decodedImage = *frame;

    cv::cvtColor(decodedImage, decodedImage, CV_BGR2RGB);
    QImage img = QImage((uchar*)decodedImage.data, decodedImage.cols, decodedImage.rows, decodedImage.step,
                        QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img);
    m_ui->videoLabel->setPixmap(pix);
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
        m_ui->test->setText("Up!");
        break;
    case Qt::Key_A:     /* Left Button */
        m_ui->test->setText("Left!");
        break;
    case Qt::Key_S:     /* Down Button */
        m_ui->test->setText("Down!");
        break;
    case Qt::Key_D:     /* Right Button */
        m_ui->test->setText("Right!");
        break;
    case Qt::Key_L:
        m_ui->test->setText("Led!");
        break;
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    const int videoLabelX = m_videoLabelRect.x();
    const int videoLabelY = m_videoLabelRect.y();
    QPoint mosPos = event->pos();

    auto labelText = QString::asprintf("x = %d, y = %d", mosPos.x() - videoLabelX, mosPos.y() - videoLabelY);
    m_ui->mousePosLabel->setText(labelText);
}

/*****************************************
 *          private SLOTS
 *
 * **************************************/
void MainWindow::whenReadIsReady()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] you are able to read something";
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
    char *buffer = new char[10];

    emit requestRead(buffer, 10);
}

void MainWindow::whenReadJobDone(int errorCode, char *buffer, int maxLength)
{
    assert(buffer != nullptr);
    qDebug() << "[SOCKET @ " << this->thread() << "] read job is done! , buffer contents are below";
    qDebug() << buffer;

    // deallocate used memory
    delete buffer;
}

void MainWindow::whenConnectionDone()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] connected! ";
    m_bSocketThreadConnected = true;
    m_ui->connectButton->setEnabled(false);
}

void MainWindow::whenDisconnectionDone()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] disconnected! ";
    m_bSocketThreadConnected = false;
    m_ui->connectButton->setEnabled(true);
}

void MainWindow::on_connectButton_clicked()
{
    qDebug() << "[SOCKET @ " << this->thread() << "] connect trying...";
    emit requestConnection("devjhlab.iptime.org", 55555);
}

void MainWindow::on_disconnect_button_clicked()
{
    assert(m_bSocketThreadConnected == true);
    emit requestDisconnection();
}

void MainWindow::on_sendButton_clicked()
{
    assert(m_bSocketThreadConnected == true);
    qDebug() << "[SOCKET @ " << this->thread() << "] write request button clicked ";
    emit requestWriting("o", 2);
}

void MainWindow::on_backwardButton_clicked()
{
    assert(m_bSocketThreadConnected == true);
    qDebug() << "[SOCKET @ " << this->thread() << "] write request button clicked ";
    emit requestWriting("x", 2);
}
