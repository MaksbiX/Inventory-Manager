#include "itemdialog.h"
#include <QPushButton>
#include <QLabel>

ItemDialog::ItemDialog(QWidget *parent)
    : QDialog(parent),
    nameEdit(new QLineEdit(this)),
    categoryEdit(new QLineEdit(this)),
    quantitySpinBox(new QSpinBox(this))
{
    setWindowTitle("Add New Item");
    setFixedWidth(175);

    quantitySpinBox->setRange(0, 10000);
    quantitySpinBox->setValue(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Item Name:"));
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(new QLabel("Category:"));
    mainLayout->addWidget(categoryEdit);
    mainLayout->addWidget(new QLabel("Quantity:"));
    mainLayout->addWidget(quantitySpinBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    okButton->setEnabled(false);

    auto updateOkButtonState = [this, okButton]() {
        bool isNameValid = !nameEdit->text().trimmed().isEmpty();
        bool isCategoryValid = !categoryEdit->text().trimmed().isEmpty();
        okButton->setEnabled(isNameValid && isCategoryValid);
    };

    connect(nameEdit, &QLineEdit::textChanged, this, updateOkButtonState);
    connect(categoryEdit, &QLineEdit::textChanged, this, updateOkButtonState);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(mainLayout);
}

QString ItemDialog::itemName() const {
    return nameEdit->text().trimmed();
}

QString ItemDialog::itemCategory() const {
    return categoryEdit->text().trimmed();
}

int ItemDialog::itemQuantity() const {
    return quantitySpinBox->value();
}
