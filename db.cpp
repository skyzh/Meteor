#include "db.h"

QMutex DB::_db_mutex;

QSqlDatabase DB::get() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "global_conn");
    db.setDatabaseName("dataset.db");
    return db;
}

DB::DB() {
    _db_mutex.lock();

}

DB::~DB() {
    QSqlDatabase::removeDatabase("global_conn");
    _db_mutex.unlock();
}
