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
    void on_NatGasTriggered();
    void on_HydroTriggered();
    void on_InternetTriggered();
    void initInetWindow();
    void updateInetDataProviders();
    void updateNatGasProviders();
    DataProvider dataProvider;
};
#endif // MAINWINDOW_H
