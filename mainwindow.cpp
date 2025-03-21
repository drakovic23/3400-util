#include "mainwindow.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "./ui_mainwindow.h"
#include "./datahelpers.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Connect our toolbar buttons
    connect(ui->actionNatGas, &QAction::triggered, this, &MainWindow::on_NatGasTriggered);
    connect(ui->actionHydro, &QAction::triggered, this, &MainWindow::on_HydroTriggered);
    connect(ui->actionInternetService, &QAction::triggered, this, &MainWindow::on_InternetTriggered);

    //The main data provider for retrieving customers, providers etc.
    DataProvider mainDataProvider{};
    this->dataProvider = mainDataProvider;

    //Initialize the internet page
    initInetWindow();
    //Connect the dropdown on the inet page
    connect(ui->inetComboBox, &QComboBox::currentTextChanged,this, &MainWindow::updateInetDataProviders);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//TODO: This needs some styling
void MainWindow::updateInetDataProviders()
{
    vector<Provider> providers = dataProvider.getProviders();
    ui->inetTableWidget->setRowCount(0);
    //UtilityType::InternetHomePhone
    //Holds the corresponding index to the utility type
    map<int, UtilityType> utilityTypes =
    {
        {0, UtilityType::InternetTV},
        {1, UtilityType::InternetMobile},
        {2, UtilityType::InternetHomePhone}
    };
    int selectedIndex = ui->inetComboBox->currentIndex();

    UtilityType type = utilityTypes[selectedIndex];
    //Populate the table widget
    for (const Provider &provider : providers)
    {
        int row = ui->inetTableWidget->rowCount();
        ui->inetTableWidget->insertRow(row);

        //QTableWidgetItem *providerItem = new QTableWidgetItem(provider.name);
        QTableWidgetItem *providerItem = new QTableWidgetItem(QString::fromStdString(provider.name));
        QTableWidgetItem *rateItem = new QTableWidgetItem(QString("$%4").arg(provider.services.at(type).meterRate));

        ui->inetTableWidget->setItem(row, 0, providerItem);
        ui->inetTableWidget->setItem(row, 1, rateItem);
    }
}

void MainWindow::initInetWindow()
{
    //Combo box on Internet Page
    QStringList inetDropDownOptions = {"TV", "Mobile Phone", "Home Phone"};
    ui->inetComboBox->addItems(inetDropDownOptions);
    //Table widget
    ui->inetTableWidget->setColumnCount(2);
    ui->inetTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate");
}

//Triggers for changing the page on the stacked widget
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
