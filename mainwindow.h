
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "dataprovider.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_NatGasTriggered();
    void on_HydroTriggered();
    void on_InternetTriggered();

    void updateInetDataProviders();
    void showInetCustomerContext(const QPoint &pos);
    void initInetWindow();

    void updateNatGasProviders();
    void updateNatGasBillingTable();

    void initHydroWindow();
    void updateHydroDataProviders();
    void updateHydroCustomerBillsByService();
    double calculateTotalSales(int providerId, UtilityType type);
    void openAddCustomerDialog();

private:
    Ui::MainWindow *ui;
    DataProvider dataProvider;
};

#endif // MAINWINDOW_H
