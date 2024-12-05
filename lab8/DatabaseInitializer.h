#ifndef DATABASEINITIALIZER_H
#define DATABASEINITIALIZER_H

#include <QSqlQuery>
#include <QSqlError>
#include <QString>

class DatabaseInitializer {
public:
    static QSqlError initializeTables() {
        QSqlQuery query;

        // Створюємо таблицю
        if (!query.exec("CREATE TABLE IF NOT EXISTS inventory (id INTEGER PRIMARY KEY, name TEXT, category TEXT, quantity INTEGER)"))
            return query.lastError();

        return QSqlError();
    }
};

#endif // DATABASEINITIALIZER_H
