#ifndef WORKER_H
#define WORKER_H

#include <QAbstractItemModel>

class worker : public QObject
{
    Q_OBJECT

public:
    explicit worker(QObject *parent = nullptr);


private:
};

#endif // WORKER_H
