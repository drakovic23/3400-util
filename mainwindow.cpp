#include "mainwindow.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <ctime>
#include "./ui_mainwindow.h"
#include "./datahelpers.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNatGas, &QAction::triggered, this, &MainWindow::on_NatGasTriggered);
    connect(ui->actionHydro, &QAction::triggered, this, &MainWindow::on_HydroTriggered);
    connect(ui->actionInternetService, &QAction::triggered, this, &MainWindow::on_InternetTriggered);

    dataProvider = DataProvider();

    // Initialize both internet and hydro tabs
    initInetWindow();
    initHydroWindow();

    connect(ui->inetComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateInetDataProviders);
    connect(ui->hydroComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateHydroDataProviders);
    connect(ui->hydroCustomerBox, &QComboBox::currentIndexChanged, this, &MainWindow::displayHydroBillsForCustomer);

    populateHydroCustomerBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ========== INTERNET SECTION ==========
void MainWindow::initInetWindow()
{
    QStringList inetDropDownOptions = {"TV", "Mobile Phone", "Home Phone"};
    ui->inetComboBox->addItems(inetDropDownOptions);
    ui->inetTableWidget->setColumnCount(2);
    ui->inetTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate");
}

void MainWindow::updateInetDataProviders()
{
    vector<Provider> providers = dataProvider.getProviders();
    ui->inetTableWidget->setRowCount(0);

    map<int, UtilityType> utilityTypes =
        {
            {0, UtilityType::InternetTV},
            {1, UtilityType::InternetMobile},
            {2, UtilityType::InternetHomePhone}
        };

    int selectedIndex = ui->inetComboBox->currentIndex();
    UtilityType type = utilityTypes[selectedIndex];

    for (const Provider &provider : providers)
    {
        int row = ui->inetTableWidget->rowCount();
        ui->inetTableWidget->insertRow(row);

        QTableWidgetItem *providerItem = new QTableWidgetItem(QString::fromStdString(provider.name));
        QTableWidgetItem *rateItem = new QTableWidgetItem(QString("$%1").arg(provider.services.at(type).meterRate));

        ui->inetTableWidget->setItem(row, 0, providerItem);
        ui->inetTableWidget->setItem(row, 1, rateItem);
    }
}

// ========== HYDRO SECTION (Amro) ==========

void MainWindow::initHydroWindow()
{
    QStringList hydroOptions = {"Electric", "Water", "Sewerage"};
    ui->hydroComboBox->addItems(hydroOptions);

    ui->hydroTableWidget->setColumnCount(2);
    ui->hydroTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate");

    ui->hydroBillTableWidget->setColumnCount(4);
    ui->hydroBillTableWidget->setHorizontalHeaderLabels(QStringList() << "Service" << "Date" << "Amount" << "Status");
}

void MainWindow::updateHydroDataProviders()
{
    vector<Provider> providers = dataProvider.getProviders();
    ui->hydroTableWidget->setRowCount(0);

    map<int, UtilityType> utilityTypes =
        {
            {0, UtilityType::HydroElectric},
            {1, UtilityType::HydroWater},
            {2, UtilityType::HydroSewerage}
        };

    int selectedIndex = ui->hydroComboBox->currentIndex();
    UtilityType type = utilityTypes[selectedIndex];

    for (const Provider& provider : providers)
    {
        int row = ui->hydroTableWidget->rowCount();
        ui->hydroTableWidget->insertRow(row);

        QTableWidgetItem* providerItem = new QTableWidgetItem(QString::fromStdString(provider.name));
        QTableWidgetItem* rateItem = new QTableWidgetItem(QString("$%1").arg(provider.services.at(type).meterRate));

        ui->hydroTableWidget->setItem(row, 0, providerItem);
        ui->hydroTableWidget->setItem(row, 1, rateItem);
    }
}

void MainWindow::populateHydroCustomerBox()
{
    ui->hydroCustomerBox->clear();
    for (const auto& [id, customer] : dataProvider.getCustomers()) {
        QString label = QString("ID %1 - %2").arg(id).arg(QString::fromStdString(customer.name));
        ui->hydroCustomerBox->addItem(label, id);
    }
}

// ========== Hydro Section (Amro) ==========
void MainWindow::displayHydroBillsForCustomer()
{
    ui->hydroBillTableWidget->setRowCount(0);  // Clear existing bills

    int customerIndex = ui->hydroCustomerBox->currentIndex();
    if (customerIndex < 0) return;

    int customerId = ui->hydroCustomerBox->itemData(customerIndex).toInt();
    const auto& customers = dataProvider.getCustomers();
    auto it = customers.find(customerId);
    if (it == customers.end()) return;

    const Customer& customer = it->second;

    for (const Subscription& sub : customer.subscriptions)
    {
        if (sub.service.type == UtilityType::HydroElectric ||
            sub.service.type == UtilityType::HydroWater ||
            sub.service.type == UtilityType::HydroSewerage)
        {
            QString serviceName =
                sub.service.type == UtilityType::HydroElectric ? "Electric" :
                    sub.service.type == UtilityType::HydroWater ? "Water" : "Sewerage";

            for (const Bill& bill : sub.bills)
            {
                std::time_t issue = std::chrono::system_clock::to_time_t(bill.issueDate);
                QString dateStr = QString::fromStdString(std::string(std::ctime(&issue))).trimmed();
                QString statusStr = bill.isPaid ? "Paid" :
                                        (bill.isOverDue(std::chrono::system_clock::now()) ? "Overdue" : "Unpaid");

                int row = ui->hydroBillTableWidget->rowCount();
                ui->hydroBillTableWidget->insertRow(row);

                ui->hydroBillTableWidget->setItem(row, 0, new QTableWidgetItem(serviceName));
                ui->hydroBillTableWidget->setItem(row, 1, new QTableWidgetItem(dateStr));
                ui->hydroBillTableWidget->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(bill.amount, 0, 'f', 2)));
                ui->hydroBillTableWidget->setItem(row, 3, new QTableWidgetItem(statusStr));
            }
        }
    }
}


// ========== END HYDRO SECTION (Amro) ==========

// PAGE NAVIGATION
void MainWindow::on_NatGasTriggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_HydroTriggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_InternetTriggered()
{
    ui->stackedWidget->setCurrentIndex(2);
}
