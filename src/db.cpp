#include "db.h"
#include <QDebug>
#include <QSqlError>

QMutex DB::_db_mutex;

QSqlDatabase DB::get() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "global_conn");
    setup_db(db);
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    return db;
}

DB::DB() {
    _db_mutex.lock();
}

DB::~DB() {
    QSqlDatabase::removeDatabase("global_conn");
    _db_mutex.unlock();
}

void DB::init() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "global_conn_cache");
    setup_db(db);
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    if (!db.open()) {
        qDebug() << "Failed to open cache db: " << db.lastError();
    }
}

void DB::setup_db(QSqlDatabase &db) {
    if (DEVELOPMENT_MODE) {
        db.setDatabaseName("file:/Users/skyzh/Work/Qt/dataset.db?cache=shared");
    } else {
        db.setDatabaseName("file::memory:?cache=shared");
    }
}
