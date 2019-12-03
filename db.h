#ifndef DB_H
#define DB_H

#include <QMutex>
#include <QSqlDatabase>

class DB {
public:
    static QMutex _db_mutex;
    DB();
    ~DB();
    QSqlDatabase get();
};

#endif // DB_H
