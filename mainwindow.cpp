#include "mainwindow.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QList>
#include <QStringList>
#include <QMenu>
#include "./ui_mainwindow.h"
#include "./datahelpers.h"
#include "./customerdetailsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Connect our toolbar buttons
    connect(ui->actionNatGas, &QAction::triggered, this, &MainWindow::on_NatGasTriggered);
    connect(ui->actionHydro, &QAction::triggered, this, &MainWindow::on_HydroTriggered);
    connect(ui->actionInternetService, &QAction::triggered, this, &MainWindow::on_InternetTriggered);
    //Connect the dropdown on the inet page
    connect(ui->inetComboBox, &QComboBox::currentTextChanged,this, &MainWindow::updateInetDataProviders);
    //Connect the customer table on the inet page to a custom context mnenu
    connect(ui->inetCustomerTable, &QTableWidget::customContextMenuRequested,this, &MainWindow::showInetCustomerContext);
    //The main data provider for retrieving customers, providers etc.
    DataProvider mainDataProvider{};
    this->dataProvider = mainDataProvider;

    //Initialize the internet page
    initInetWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Handles what happens when the drop down on the inet page is selected
void MainWindow::updateInetDataProviders()
{
    vector<Provider> providers = dataProvider.getProviders();
    ui->inetTableWidget->setRowCount(0);
    ui->inetCustomerTable->setRowCount(0);

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

        QTableWidgetItem *providerItem = new QTableWidgetItem(QString::fromStdString(provider.name));
        QTableWidgetItem *rateItem = new QTableWidgetItem(QString("$%4").arg(provider.services.at(type).meterRate));

        ui->inetTableWidget->setItem(row, 0, providerItem);
        ui->inetTableWidget->setItem(row, 1, rateItem);
    }

    // Get sales for this service type and provider
    map<int, float> providerSales; //Provider id -> total sales for selected service type
    map<int, Customer> customers = dataProvider.getCustomers();

    for(size_t i = 0; i < customers.size(); i++)
    {
        for(size_t j = 0; j < customers[i].subscriptions.size();j++)
        {
            //If the customer has a subscription with the selected utility type
            if(customers[i].subscriptions[j].service.type == utilityTypes[selectedIndex])
            {
                //Loop through all the bills for this utility type and add it to the map for total sales
                for(size_t x = 0; x < customers[i].subscriptions[j].bills.size();x++)
                {
                    providerSales[customers[i].subscriptions[j].provider.id] += customers[i].subscriptions[j].bills[x].amount;
                }

                //This is used to populate the table that holds customers with the selected utility type.
                int row = ui->inetCustomerTable->rowCount();
                ui->inetCustomerTable->insertRow(row);

                QTableWidgetItem *customerName = new QTableWidgetItem(QString::fromStdString(customers[i].name));
                QTableWidgetItem *providerName = new QTableWidgetItem(QString::fromStdString(customers[i].subscriptions[j].provider.name));
                //Embed the customer ID for easy lookup
                customerName->setData(Qt::UserRole, customers[i].id);
                ui->inetCustomerTable->setItem(row, 0, customerName);
                ui->inetCustomerTable->setItem(row, 1, providerName);
            }
        }
    }

    // Create a set of data
    QBarSet *salesSet = new QBarSet("Sales");

    for(auto const& [key, val] : providerSales)
    {
        //Since the map is ordered we can simply output to our barset

        *salesSet << val;
    }

    QBarSeries *series = new QBarSeries();
    series->append(salesSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Total Sales / provider for selected service");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;

    for(auto const& [key, val] : providerSales)
    {
        //Key is the provider #
        categories << QString("Provider ") + QString::number(key);
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(90);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->iNetChartView->setChart(chart);
    ui->iNetChartView->setRenderHint(QPainter::Antialiasing);
}

//For the context menu in the customer table widget
void MainWindow::showInetCustomerContext(const QPoint &pos)
{
    QPoint globalPos = ui->inetCustomerTable->viewport()->mapToGlobal(pos);
    // Get the item at the clicked position in the context window
    QTableWidgetItem *item = ui->inetCustomerTable->itemAt(pos);
    if (!item) //If no item selected
        return;

    QMenu contextMenu;
    QAction *viewCustomerAction = contextMenu.addAction("View Customer");
    QAction *selectedAction = contextMenu.exec(globalPos);

    if (selectedAction == viewCustomerAction) {
        int row = ui->inetCustomerTable->row(item); //Need this to get the customer name
        QTableWidgetItem *tableItem = ui->inetCustomerTable->item(row, 0);
        int customerId = tableItem->data(Qt::UserRole).toInt();
        CustomerDetailsDialog *dialog = new CustomerDetailsDialog(dataProvider.findCustomer(customerId), this);
        //Display the dialog

        dialog->exec();
    }
}


void MainWindow::initInetWindow()
{
    //Combo box on Internet Page
    QStringList inetDropDownOptions = {"TV", "Mobile Phone", "Home Phone"};
    ui->inetComboBox->addItems(inetDropDownOptions);
    //For the rates table
    ui->inetTableWidget->setColumnCount(2);
    ui->inetTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate");
    //For the customer table
    ui->inetCustomerTable->setColumnCount(2);
    ui->inetCustomerTable->setHorizontalHeaderLabels(QStringList() << "Customer Name" << "Provider");
    ui->inetCustomerTable->setContextMenuPolicy(Qt::CustomContextMenu);
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
