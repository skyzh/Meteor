#ifndef DB_H
#define DB_H

#include <QMutex>
#include <QSqlDatabase>

class DB {
    static void setup_db(QSqlDatabase &db);
public:
    static QMutex _db_mutex;

    DB();

    ~DB();

    static void init();

    QSqlDatabase get();
};

#endif // DB_H
