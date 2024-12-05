#ifndef INVENTORYTABLEMODEL_H
#define INVENTORYTABLEMODEL_H

#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QSet>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

class InventoryTableModel : public QSqlTableModel {
    Q_OBJECT

public:
    InventoryTableModel(QObject *parent = nullptr) : QSqlTableModel(parent) {
        setTable("inventory");
        setEditStrategy(QSqlTableModel::OnManualSubmit);
        select();
    }

    QStringList uniqueCategories() {
        QStringList categories;
        QSqlQuery query("SELECT DISTINCT category FROM inventory");
        while (query.next()) {
            categories << query.value(0).toString();
        }
        return categories;
    }

    void sortByColumn(int column, Qt::SortOrder order = Qt::AscendingOrder) {
        setSort(column, order);
        select();
    }

    void filterByCategory(const QString &category) {
        if (category.isEmpty()) {
            setFilter("");
        } else {
            setFilter(QString("category = '%1'").arg(category));
        }
        select();
    }

    bool addItem(const QString &name, const QString &category, int quantity) {
        int row = rowCount();
        insertRow(row);
        setData(index(row, 1), name);
        setData(index(row, 2), category);
        setData(index(row, 3), quantity);
        return submitAll();
    }

    bool deleteItem(int row) {
        removeRow(row);
        return submitAll();
    }
};

#endif // INVENTORYTABLEMODEL_H
