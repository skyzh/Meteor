#ifndef DB_H
#define DB_H

#include <QMutex>
#include <QSqlDatabase>

class DB {
    static const bool DEVELOPMENT_MODE = true;

    static void setup_db(QSqlDatabase &db);
public:
    static QMutex _db_mutex;

    static void init();

    DB();

    ~DB();

    QSqlDatabase get();
};

#endif // DB_H
