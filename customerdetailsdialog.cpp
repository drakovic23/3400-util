#include <chrono>
#include <QDateTime>
#include "customerdetailsdialog.h"
#include "ui_customerdetailsdialog.h"
#include "dataprovider.h"
#include "datahelpers.h"

CustomerDetailsDialog::CustomerDetailsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::customerDetailsDialog)
{
    ui->setupUi(this);
}

CustomerDetailsDialog::CustomerDetailsDialog(const Customer customer, QWidget *parent = nullptr)
    : QDialog(parent), ui(new Ui::customerDetailsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Viewing Customer : " + QString::fromStdString(customer.name));
    //ui->nameLabel->setText(QString::fromStdString(customer.name));
    initCustomerDetails(customer);
}

void CustomerDetailsDialog::initCustomerDetails(const Customer &customer)
{
    ui->nameLabel->setText(QString::fromStdString(customer.name));
    ui->idLabel->setText(QString("%2").arg(customer.id));

    //Setup subscription table widget
    ui->subscriptionsTable->setRowCount(0);
    ui->subscriptionsTable->setColumnCount(2);
    ui->subscriptionsTable->setHorizontalHeaderLabels(QStringList() << "Provider" << "Utility Type");

    //Setup billing table
    ui->billingTable->setRowCount(0);
    ui->billingTable->setColumnCount(5);
    ui->billingTable->setHorizontalHeaderLabels(QStringList() << "Provider" << "Service" << "Amount" << "Due Date" << "Overdue");
    for(size_t i = 0;i < customer.subscriptions.size();i++)
    {
        //Subscriptions
        int row = ui->subscriptionsTable->rowCount();
        ui->subscriptionsTable->insertRow(row);

        QTableWidgetItem *providerName = new QTableWidgetItem(QString::fromStdString(customer.subscriptions[i].provider.name));
        string utilityTypeName = DataProvider::getUtilityAsString(customer.subscriptions[i].service.type);
        QTableWidgetItem *utilityType = new QTableWidgetItem(QString::fromStdString(utilityTypeName));

        ui->subscriptionsTable->setItem(row, 0, providerName);
        ui->subscriptionsTable->setItem(row, 1, utilityType);

        //Billing

        for(size_t j = 0; j < customer.subscriptions[i].bills.size(); j++)
        {
            int billingRow = ui->billingTable->rowCount();
            ui->billingTable->insertRow(billingRow);

            double amount = customer.subscriptions[i].bills[j].amount;
            std::time_t dateT = std::chrono::system_clock::to_time_t(customer.subscriptions[i].bills[j].dueDate);
            QDateTime date = QDateTime::fromSecsSinceEpoch(dateT);
            string isOverdue = customer.subscriptions[i].bills[j].isOverDue(std::chrono::system_clock::now()) ? "Overdue!" : "No";
            QTableWidgetItem *amountDue = new QTableWidgetItem(QString("%3").arg(amount));
            QTableWidgetItem *dateDue = new QTableWidgetItem(date.toString("dd-MM-yyyy"));
            QTableWidgetItem *overDue = new QTableWidgetItem(QString::fromStdString(isOverdue));
            QTableWidgetItem *providerNameBilling = new QTableWidgetItem(QString::fromStdString(customer.subscriptions[i].provider.name));
            QTableWidgetItem *utilityTypeBilling = new QTableWidgetItem(QString::fromStdString(utilityTypeName));

            ui->billingTable->setItem(row, 0, providerNameBilling);
            ui->billingTable->setItem(row, 1, utilityTypeBilling);
            ui->billingTable->setItem(row, 2, amountDue);
            ui->billingTable->setItem(row, 3, dateDue);
            ui->billingTable->setItem(row, 4, overDue);

        }

    }


}

CustomerDetailsDialog::~CustomerDetailsDialog()
{
    delete ui;
}
