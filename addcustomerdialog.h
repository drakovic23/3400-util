#ifndef ADDCUSTOMERDIALOG_H
#define ADDCUSTOMERDIALOG_H

#include <QDialog>
#include "datahelpers.h"

namespace Ui {
class AddCustomerDialog;
}

class AddCustomerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCustomerDialog(QWidget *parent = nullptr);
    ~AddCustomerDialog();


    Customer getCreatedCustomer(const std::vector<Provider>& providers);

private:
    Ui::AddCustomerDialog *ui;
};

#endif
