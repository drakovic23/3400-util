#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataprovider.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // Page switches
    void on_NatGasTriggered();
    void on_HydroTriggered();
    void on_InternetTriggered();

    // Internet
    void initInetWindow();
    void updateInetDataProviders();

    // ========== Hydro Section (Amro) ==========
    void initHydroWindow();
    void updateHydroDataProviders();
    void populateHydroCustomerBox();
    void displayHydroBillsForCustomer();

    // ========== End Hydro Section (Amro) ==========

    void updateNatGasProviders();
    void updateNatGasBillingTable();
    void showInetCustomerContext(const QPoint &pos);
    DataProvider dataProvider;
};

#endif // MAINWINDOW_H
