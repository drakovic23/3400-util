#include "mainwindow.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "./ui_mainwindow.h"
#include "./.qtcreator/datahelpers.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    //This is used to generate the customer and provider information
    //Initialize our customer data
    vector<Provider> providers =
        { //TODO: Change names to something more realistic
         Provider(1, "Provider 1"),
         Provider(2, "Provider 2"),
         Provider(3, "Provider 3"),
         Provider(4, "Provider 4"),
         Provider(5, "Provider 5"),
         Provider(6, "Provider 6"),
         Provider(7, "Provider 7"),
         Provider(8, "Provider 8"),
         Provider(9, "Provider 9"),
         Provider(10, "Provider 10"),
         };

    for(size_t i = 0; i < providers.size();i++)
        providers[i].initServices();

    //TODO: Should randomize this later
    //Need to initialize and add customers here
    map<int, Customer> customers; //Map holds a KVP of customers (customer id -> Customer)
    for(size_t i = 0;i < 100;i++) //Generate 100 customers
    {
        Customer c(i, "Customer_" + std::to_string(i));

        c.addSubscription(Subscription(&providers[0], providers[0].services[UtilityType::HydroElectric]));
        c.addSubscription(Subscription(&providers[2], providers[2].services[UtilityType::InternetTV]));

        customers[i] = c;
    }
    //

    ui->setupUi(this);
    connect(ui->actionNatGas, &QAction::triggered, this, &MainWindow::on_NatGasTriggered);
    connect(ui->actionHydro, &QAction::triggered, this, &MainWindow::on_HydroTriggered);
    connect(ui->actionInternetService, &QAction::triggered, this, &MainWindow::on_InternetTriggered);

}

MainWindow::~MainWindow()
{
    delete ui;
}

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
