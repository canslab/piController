#ifndef CURLWORKER_H
#define CURLWORKER_H

#include <QObject>

class CurlWorker : public QObject
{
    Q_OBJECT
public:
    explicit CurlWorker(QObject *parent = 0);

signals:

public slots:
};

#endif // CURLWORKER_H