#include "mainwindow.h"
#include <QApplication>
#include "protobuf/MsgChunk.pb.h"

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
