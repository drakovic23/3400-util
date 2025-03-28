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
#include <QDateTime>
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


    //For the income/sales chart

    //Select TV or some service
    //Then update the chart


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
                //Loop through all the bills for this utility type and add it to the map
                for(size_t x = 0; x < customers[i].subscriptions[j].bills.size();x++)
                {
                    providerSales[customers[i].subscriptions[j].provider.id] += customers[i].subscriptions[j].bills[x].amount;
                }

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
    //*salesSet << 150 << 200 << 250 << 100 << 200 << 300;

    // Create a bar series and append the set
    QBarSeries *series = new QBarSeries();
    series->append(salesSet);

    // Create the chart and add the series
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Total Income / Sales per provider for selected service");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Create category axis (for example, months)
    QStringList categories;

    for(auto const& [key, val] : providerSales)
    {
        //Key is the customer

        categories << QString("Provider ") + QString::number(key);
    }

    //categories << "Provider 1" << "Provider 2" << "Provider 3" << "Provider 4" << "Provider 5" << "Provider 6";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Create a value axis for the sales data
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Assuming you have promoted a widget to QChartView in your UI (e.g., named chartView)
    ui->iNetChartView->setChart(chart);
    ui->iNetChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::initInetWindow()
{
    //Combo box on Internet Page
    QStringList inetDropDownOptions = {"TV", "Mobile Phone", "Home Phone"};
    ui->inetComboBox->addItems(inetDropDownOptions);
    //Table widget
    ui->inetTableWidget->setColumnCount(2);
    ui->inetTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate");

   // ui->inetComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //ui->inetServiceLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}



//NatGas section

void MainWindow::updateNatGasProviders()
{
    vector<Provider> providers = dataProvider.getProviders();

    ui->gasProviderTableWidget->setRowCount(0);

    ui->gasProviderTableWidget->setColumnCount(3);
    ui->gasProviderTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate" << "Total Sales");


    for (const Provider& provider : providers)
    {
        //check if provider offers Natural Gas
        if (provider.services.find(UtilityType::NaturalGas) != provider.services.end())
        {

        //get info
        UtilityService natGasService = provider.services.at(UtilityType::NaturalGas);

        //calculate total sales for this provider
        map<int, Customer> customers = dataProvider.getCustomers();
        double totalSales = 0.0;

        //loop through customers
        for(const auto& [id, customer] : customers)
        {
            for (const Subscription& sub : customer.subscriptions) {
                //check if this subscription belongs to nautural gas and belongs to this provider
                if (sub.service.type == UtilityType::NaturalGas && sub.provider.id == provider.id)
                {
                    for(const Bill& bill : sub.bills){
                        if (bill.isPaid)
                            totalSales += bill.amount;
                    }
                }
            }
        }


        //create a new row in table
        int row = ui->gasProviderTableWidget->rowCount();
        ui->gasProviderTableWidget->insertRow(row);

        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(provider.name));
        QTableWidgetItem* rateItem = new QTableWidgetItem(QString("$%1").arg(natGasService.meterRate, 0, 'f', 2));
        QTableWidgetItem* salesItem = new QTableWidgetItem(QString("$%1").arg(totalSales, 0, 'f', 2));


        ui->gasProviderTableWidget->setItem(row, 0, nameItem);
        ui->gasProviderTableWidget->setItem(row, 1, rateItem);
        ui->gasProviderTableWidget->setItem(row, 2, salesItem);

        }
    }

    ui->gasProviderTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

void MainWindow:: updateNatGasBillingTable(){

    ui->gasBillingTableWidget->setRowCount(0);
    ui->gasBillingTableWidget->setColumnCount(5);
    ui->gasBillingTableWidget->setIconSize(QSize(54,54));
    ui->gasBillingTableWidget->setHorizontalHeaderLabels(QStringList() << "Customer" << "Rate" << "Amount Due" << "Due Date" << "Overdue");


    //get current time to check overdue
    auto now = std::chrono::system_clock::now();

    map<int, Customer> customers = dataProvider.getCustomers();

    for (const auto& [id, customer] : customers)
    {
        for (const Subscription& sub : customer.subscriptions){
            //filter for natural gas subscriptions
            if(sub.service.type == UtilityType::NaturalGas)
            {
                for (const Bill& bill : sub.bills)
                {
                    int row = ui->gasBillingTableWidget->rowCount();
                    ui->gasBillingTableWidget->insertRow(row);

                    QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(customer.name));
                    QTableWidgetItem* rateItem = new QTableWidgetItem(QString("$%1").arg(sub.service.meterRate, 0, 'f', 2));
                    QTableWidgetItem* amountItem = new QTableWidgetItem(QString("$%1").arg(bill.amount, 0, 'f', 2));


                    //due date
                    std::time_t dueT = std::chrono::system_clock::to_time_t(bill.dueDate);
                    QDateTime qtDueDate = QDateTime::fromSecsSinceEpoch(dueT);
                    QString dueDateStr = qtDueDate.toString("dd-MM-yyyy");
                    QTableWidgetItem* dueDateItem = new QTableWidgetItem(dueDateStr);

                    //check overdue status
                    Bill billCopy = bill;
                    bool overdue = billCopy.isOverDue(now);
                    QTableWidgetItem* overdueItem = new QTableWidgetItem();
                    QIcon statusIcon(overdue ? QPixmap(":/icons/resources/icons/overdue.png") : QPixmap(":/icons/resources/icons/ontime.png"));
                    overdueItem->setIcon(statusIcon);
                    overdueItem->setToolTip(overdue ? "Overdue" : "On Time");


                    ui->gasBillingTableWidget->setItem(row, 0, nameItem);
                    ui->gasBillingTableWidget->setItem(row, 1, rateItem);
                    ui->gasBillingTableWidget->setItem(row, 2, amountItem);
                    ui->gasBillingTableWidget->setItem(row, 3, dueDateItem);
                    ui->gasBillingTableWidget->setItem(row, 4, overdueItem);
                }
            }

        }
    }

    ui->gasBillingTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}



//NatGas section


//Triggers for changing the page on the stacked widget
void MainWindow::on_NatGasTriggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    updateNatGasProviders();
    updateNatGasBillingTable();
}

void MainWindow::on_HydroTriggered()
{

    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_InternetTriggered()
{

    ui->stackedWidget->setCurrentIndex(2);
}
