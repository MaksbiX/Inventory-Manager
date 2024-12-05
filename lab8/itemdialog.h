#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

class ItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemDialog(QWidget *parent = nullptr);

    QString itemName() const;
    QString itemCategory() const;
    int itemQuantity() const;

private:
    QLineEdit *nameEdit;
    QLineEdit *categoryEdit;
    QSpinBox *quantitySpinBox;
};

#endif // ITEMDIALOG_H
