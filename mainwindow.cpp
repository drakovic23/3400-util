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
    connect(ui->comboBox, &QComboBox::currentTextChanged,this, &MainWindow::updateInetDataProviders);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateInetDataProviders()
{
    vector<Provider> providers = dataProvider.getProviders();
}

void MainWindow::initInetWindow()
{
    //Combo box on Internet Page
    QStringList inetDropDownOptions = {"TV", "Mobile Phone", "Home Phone"};
    ui->comboBox->addItems(inetDropDownOptions);
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
