#ifndef DATABASECONNECTOR_H
#define DATABASECONNECTOR_H

#include <QSqlDatabase>
#include <QSqlError>

class DatabaseConnector {
public:
    DatabaseConnector(const QString &driver, const QString &dbName) {
        db = QSqlDatabase::addDatabase(driver);
        db.setDatabaseName(dbName);
    }

    bool open() {
        return db.open();
    }

    QSqlError lastError() const {
        return db.lastError();
    }

    void close() {
        db.close();
    }

private:
    QSqlDatabase db;
};

#endif // DATABASECONNECTOR_H
