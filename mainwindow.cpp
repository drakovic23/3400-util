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
#include <QMenu>
#include <QMessageBox>
#include "./ui_mainwindow.h"
#include "./datahelpers.h"
#include "./customerdetailsdialog.h"
#include "./addcustomerdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Connect the toolbar to triggers
    connect(ui->actionNatGas, &QAction::triggered, this, &MainWindow::on_NatGasTriggered);
    connect(ui->actionHydro, &QAction::triggered, this, &MainWindow::on_HydroTriggered);
    connect(ui->actionInternetService, &QAction::triggered, this, &MainWindow::on_InternetTriggered);
    connect(ui->actionAddCustomer, &QAction::triggered, this, &MainWindow::openAddCustomerDialog);

    //Connect the dropdown on the inet page
    connect(ui->inetComboBox, &QComboBox::currentTextChanged,this, &MainWindow::updateInetDataProviders);
    //Connect the customer table on the inet page to a custom context mnenu
    connect(ui->inetCustomerTable, &QTableWidget::customContextMenuRequested,this, &MainWindow::showInetCustomerContext);
    //The main data provider for retrieving customers, providers etc.
    DataProvider mainDataProvider{};
    this->dataProvider = mainDataProvider;

    // Initialize both internet and hydro tabs
    initInetWindow();
    initHydroWindow();

    //connect(ui->inetComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateInetDataProviders);
    connect(ui->hydroComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateHydroDataProviders);


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
        QTableWidgetItem *rateItem = new QTableWidgetItem(QString("$%1").arg(provider.services.at(type).meterRate));

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

// ========== HYDRO SECTION (Amro) ==========

void MainWindow::initHydroWindow()
{
    QStringList hydroOptions = {"Electric", "Water", "Sewerage"};
    ui->hydroComboBox->addItems(hydroOptions);

    ui->hydroTableWidget->setColumnCount(3);
    ui->hydroTableWidget->setHorizontalHeaderLabels(QStringList() << "Provider" << "Rate" << "Total Sales");

    ui->hydroBillTableWidget->setColumnCount(4);
    ui->hydroBillTableWidget->setHorizontalHeaderLabels(QStringList() << "Customer" << "Date" << "Amount" << "Status");
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
    QTableWidgetItem* totalSalesItem = new QTableWidgetItem(QString("$%1").arg(calculateTotalSales(provider.id, type), 0, 'f', 2));
    ui->hydroTableWidget->setItem(row, 2, totalSalesItem);
    }
    updateHydroCustomerBillsByService();
}

void MainWindow::updateHydroCustomerBillsByService()
{
    ui->hydroBillTableWidget->setRowCount(0);

    map<int, UtilityType> utilityTypes = {
        {0, UtilityType::HydroElectric},
        {1, UtilityType::HydroWater},
        {2, UtilityType::HydroSewerage}
    };

    int selectedIndex = ui->hydroComboBox->currentIndex();
    if (selectedIndex < 0) return;
    UtilityType selectedType = utilityTypes[selectedIndex];

    auto now = std::chrono::system_clock::now();
    const auto& customers = dataProvider.getCustomers();

    for (const auto& [id, customer] : customers)
    {
        for (const Subscription& sub : customer.subscriptions)
        {
            if (sub.service.type == selectedType)
            {
                for (const Bill& bill : sub.bills)
                {
                    int row = ui->hydroBillTableWidget->rowCount();
                    ui->hydroBillTableWidget->insertRow(row);

                    QString name = QString::fromStdString(customer.name);
                    std::time_t issue = std::chrono::system_clock::to_time_t(bill.issueDate);
                    QString dateStr = QString::fromStdString(std::string(std::ctime(&issue))).trimmed();
                    QString statusStr = bill.isPaid ? "✅ Paid" :
                                            (bill.isOverDue(now) ? "❌ Overdue" : "⌛ Unpaid");

                    ui->hydroBillTableWidget->setItem(row, 0, new QTableWidgetItem(name));
                    ui->hydroBillTableWidget->setItem(row, 1, new QTableWidgetItem(dateStr));
                    ui->hydroBillTableWidget->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(bill.amount, 0, 'f', 2)));
                    ui->hydroBillTableWidget->setItem(row, 3, new QTableWidgetItem(statusStr));
                }


            }
        }
    }
} // Hydro

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
        try
        {
            CustomerDetailsDialog *dialog = new CustomerDetailsDialog(dataProvider.findCustomer(customerId), this);
            dialog->exec();
        }
        catch (const std::out_of_range &ex)
        {
            //If customer not found there's some internal data error
            //Display a warning box and do not display dialog
            QMessageBox::warning(this, "Customer Not Found", ex.what());
        }
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
    ui->inetTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->inetTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    //For the customer table
    ui->inetCustomerTable->setColumnCount(2);
    ui->inetCustomerTable->setHorizontalHeaderLabels(QStringList() << "Customer Name" << "Provider");
    ui->inetCustomerTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->inetCustomerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //So the last column takes up any remainig space
    ui->inetCustomerTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
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


double MainWindow::calculateTotalSales(int providerId, UtilityType type)
{
    double total = 0.0;
    auto customers = dataProvider.getCustomers();
    for (const auto& [id, customer] : customers)
    {
        for (const Subscription& sub : customer.subscriptions)
        {
            if (sub.provider.id == providerId && sub.service.type == type)
            {
                for (const Bill& bill : sub.bills)
                {
                    if (bill.isPaid)
                        total += bill.amount;
                }
            }
        }
    }
    return total;
}

void MainWindow::openAddCustomerDialog()
{
    AddCustomerDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        Customer newCustomer = dialog.getCreatedCustomer(dataProvider.getProviders());
        int newId = static_cast<int>(dataProvider.getCustomers().size());
        newCustomer.id = newId;

        //add a bill so the customer shows up
        auto now = std::chrono::system_clock::now();
        Bill initialBill(now - std::chrono::hours(24 * 30), 350.0);
        initialBill.isPaid = false;
        if (!newCustomer.subscriptions.empty())
            newCustomer.subscriptions[0].addBill(initialBill);

        dataProvider.getCustomers()[newId] = newCustomer;

        QMessageBox::information(this, "Customer Added", "Customer successfully added.");
        updateHydroCustomerBillsByService();
        updateNatGasBillingTable();
        updateInetDataProviders();
    }

}
