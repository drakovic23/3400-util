#ifndef CUSTOMERDETAILSDIALOG_H
#define CUSTOMERDETAILSDIALOG_H

#include <QDialog>
#include "datahelpers.h"

namespace Ui {
class customerDetailsDialog;
}

class CustomerDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomerDetailsDialog(QWidget *parent = nullptr);
    CustomerDetailsDialog(const Customer customer, QWidget *parent);
    ~CustomerDetailsDialog();

private:
    Ui::customerDetailsDialog *ui;
    void initCustomerDetails(const Customer &customer);
};

#endif // CUSTOMERDETAILSDIALOG_H
