#include <QApplication>
#include <QTableView>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QDebug>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>

#include <xlsxdocument.h>
#include <xlsxworkbook.h>
#include <xlsxworksheet.h>
#include <xlsxglobal.h>
using namespace QXlsx;

#include "DatabaseConnector.h"
#include "DatabaseInitializer.h"
#include "inventorytablemodel.h"
#include "itemdialog.h"

void updateCategoryFilter(QComboBox *filterComboBox, InventoryTableModel *model) {
    filterComboBox->clear();
    filterComboBox->addItem(QObject::tr("All"), "");
    QStringList categories = model->uniqueCategories();
    for (const QString &category : categories) {
        filterComboBox->addItem(category, category);
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Підключення до бази даних
    DatabaseConnector dbConnector("QSQLITE", ":memory:");
    if (!dbConnector.open()) {
        qWarning() << "Failed to open database:" << dbConnector.lastError().text();
        return 1;
    }

    // Ініціалізація бази даних з тестовими записами
    QSqlError err = DatabaseInitializer::initializeTables();
    if (err.type() != QSqlError::NoError) {
        qWarning() << "Database initialization error:" << err.text();
        return 1;
    }

    // Налаштування моделі
    InventoryTableModel *model = new InventoryTableModel;

    // Інтерфейс користувача
    QWidget mainWindow;
    mainWindow.setWindowTitle(QObject::tr("Inventory Manager"));
    mainWindow.setFixedSize(639, 350);

    QTableView *view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setColumnWidth(0, 51);
    view->setColumnWidth(1, 183);
    view->setColumnWidth(2, 183);
    view->setColumnWidth(3, 183);

    // Кнопки додавання та видалення записів
    QPushButton *addButton = new QPushButton(QObject::tr("Add Item"));
    QPushButton *removeButton = new QPushButton(QObject::tr("Remove Item"));

    // Елементи керування для сортування
    QComboBox *sortComboBox = new QComboBox;
    sortComboBox->addItem(QObject::tr("ID"), 0);
    sortComboBox->addItem(QObject::tr("Name"), 1);
    sortComboBox->addItem(QObject::tr("Category"), 2);
    sortComboBox->addItem(QObject::tr("Quantity"), 3);

    QCheckBox *descendingCheckBox = new QCheckBox(QObject::tr("Descending"));
    QPushButton *sortButton = new QPushButton(QObject::tr("Sort"));

    QObject::connect(sortButton, &QPushButton::clicked, &mainWindow, [model, sortComboBox, descendingCheckBox]() {
        int column = sortComboBox->currentData().toInt();
        model->sortByColumn(column, descendingCheckBox->isChecked() ? Qt::DescendingOrder : Qt::AscendingOrder);
    });

    // Елементи керування для фільтрації
    QComboBox *filterComboBox = new QComboBox;
    filterComboBox->addItem(QObject::tr("All"), "");
    filterComboBox->addItem(QObject::tr("Electronics"), "Electronics");
    filterComboBox->addItem(QObject::tr("Furniture"), "Furniture");
    QPushButton *filterButton = new QPushButton(QObject::tr("Filter by Category"));

    QObject::connect(filterButton, &QPushButton::clicked, &mainWindow, [model, filterComboBox]() {
        model->filterByCategory(filterComboBox->currentData().toString());
    });

    QObject::connect(addButton, &QPushButton::clicked, &mainWindow, [model, &mainWindow, filterComboBox]() {
        ItemDialog dialog(&mainWindow);
        if (dialog.exec() == QDialog::Accepted) {
            QString name = dialog.itemName();
            QString category = dialog.itemCategory();
            int quantity = dialog.itemQuantity();

            if (!name.isEmpty()) {
                model->addItem(name, category, quantity);
            } else {
                QMessageBox::warning(&mainWindow, "Error", "Item name cannot be empty.");
            }
            updateCategoryFilter(filterComboBox, model);
        }
    });

    QObject::connect(removeButton, &QPushButton::clicked, &mainWindow, [model, view, &mainWindow, filterComboBox]() {
        QItemSelectionModel *selectionModel = view->selectionModel();
        QModelIndexList selectedRows = selectionModel->selectedRows();
        if (!selectedRows.isEmpty()) {
            int row = selectedRows.first().row();
            model->deleteItem(row);
            updateCategoryFilter(filterComboBox, model);
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Remove Item"), QObject::tr("Please select an item to remove."));
        }
    });

    QPushButton *exportButton = new QPushButton(QObject::tr("Export to Excel"));
    QPushButton *importButton = new QPushButton(QObject::tr("Import from Excel"));

    QObject::connect(exportButton, &QPushButton::clicked, &mainWindow, [model]() {
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Export to Excel", "", "*.xlsx");
        if (!fileName.isEmpty()) {
            if (!fileName.endsWith(".xlsx")) {
                fileName += ".xlsx";
            }

            Document xlsx;
            xlsx.write(1, 1, "ID");
            xlsx.write(1, 2, "Name");
            xlsx.write(1, 3, "Category");
            xlsx.write(1, 4, "Quantity");

            for (int row = 0; row < model->rowCount(); ++row) {
                for (int col = 0; col < model->columnCount(); ++col) {
                    xlsx.write(row + 2, col + 1, model->data(model->index(row, col)).toString());
                }
            }

            xlsx.setColumnWidth(1, 3);
            xlsx.setColumnWidth(2, 11);
            xlsx.setColumnWidth(3, 11);
            xlsx.setColumnWidth(4, 11);

            if (xlsx.saveAs(fileName)) {
                QMessageBox::information(nullptr, "Success", "Data exported successfully.");
            } else {
                QMessageBox::warning(nullptr, "Error", "Failed to export data.");
            }
        }
    });

    QObject::connect(importButton, &QPushButton::clicked, &mainWindow, [model]() {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Import from Excel", "", "*.xlsx");
        if (!fileName.isEmpty()) {
            Document xlsx(fileName);
            if (!xlsx.load()) {
                QMessageBox::warning(nullptr, "Error", "Failed to open file.");
                return;
            }

            model->removeRows(0, model->rowCount()); // Очищення таблиці перед імпортом

            int row = 2; // Початок даних
            while (xlsx.cellAt(row, 1) != 0) {
                QString name = xlsx.cellAt(row, 2)->value().toString();
                QString category = xlsx.cellAt(row, 3)->value().toString();
                int quantity = xlsx.cellAt(row, 4)->value().toInt();

                model->addItem(name, category, quantity);
                ++row;
            }

            QMessageBox::information(nullptr, "Success", "Data imported successfully.");
        }
    });

    // Основний макет
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view);

    // Макет для кнопок додавання та видалення
    QHBoxLayout *editLayout = new QHBoxLayout;
    editLayout->addWidget(addButton);
    editLayout->addWidget(removeButton);
    layout->addLayout(editLayout);

    // Макет для сортування
    QHBoxLayout *sortLayout = new QHBoxLayout;
    sortLayout->addWidget(new QLabel(QObject::tr("Sort by:")));
    sortLayout->addWidget(sortComboBox);
    sortLayout->addWidget(descendingCheckBox);
    sortLayout->addWidget(sortButton);
    layout->addLayout(sortLayout);

    // Макет для фільтрації
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel(QObject::tr("Filter by Category:")));
    filterLayout->addWidget(filterComboBox);
    filterLayout->addWidget(filterButton);
    layout->addLayout(filterLayout);

    QHBoxLayout *exportimportLayout = new QHBoxLayout;
    exportimportLayout->addWidget(exportButton);
    exportimportLayout->addWidget(importButton);
    layout->addLayout(exportimportLayout);

    mainWindow.setLayout(layout);

    // Додавання тестових записів
    model->addItem("Laptop", "Electronics", 10);
    model->addItem("Table", "Furniture", 5);
    model->addItem("Chair", "Furniture", 15);
    mainWindow.show();

    return app.exec();
}
