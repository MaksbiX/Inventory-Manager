#include "InventoryTableModel.h"
#include <QSqlRecord>
#include <QMessageBox>

InventoryTableModel::InventoryTableModel(QObject *parent)
    : QSqlTableModel(parent)
{
    // Ініціалізація моделі, наприклад, встановлення імені таблиці
    setTable("items");
    select();  // Завантаження даних з таблиці
}

void InventoryTableModel::addItem(const QString &name, const QString &category, int quantity)
{
    // Перевірка на порожнє ім'я
    if (name.isEmpty()) {
        QMessageBox::warning(nullptr, "Помилка", "Назва елемента не може бути порожньою.");
        return;
    }

    // Створення нового запису
    QSqlRecord newRecord = record();
    newRecord.setValue("name", name);
    newRecord.setValue("category", category);
    newRecord.setValue("quantity", quantity);

    // Додавання нового рядка в таблицю
    insertRecord(rowCount(), newRecord);
    submitAll();  // Збереження змін у базі даних

    // Оновлення категорій
    updateCategories();
}

void InventoryTableModel::updateCategories()
{
    // Оновлюємо категорії на основі наявних записів у таблиці
    QSet<QString> updatedCategories;
    for (int i = 0; i < rowCount(); ++i) {
        QString category = record(i).value("category").toString();
        updatedCategories.insert(category);
    }
    m_categories = QStringList(updatedCategories.begin(), updatedCategories.end());
}

QStringList InventoryTableModel::categories() const
{
    return m_categories;
}

void InventoryTableModel::sortByColumn(int column, Qt::SortOrder order)
{
    // Сортування за вказаним стовпцем
    sort(column, order);
}

void InventoryTableModel::filterByCategory(const QString &category)
{
    // Фільтрація по категорії
    if (category.isEmpty()) {
        setFilter("");  // Якщо категорія порожня, показати всі елементи
    } else {
        setFilter(QString("category = '%1'").arg(category));  // Фільтрація за категорією
    }
    select();  // Оновлюємо модель після фільтрації
}
