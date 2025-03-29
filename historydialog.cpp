// historydialog.cpp
#include "historydialog.h"
#include "ui_historydialog.h"
#include <QDir>
#include <QListWidgetItem>
#include <QMessageBox>

HistoryDialog::HistoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryDialog)
{
    ui->setupUi(this);
}

HistoryDialog::~HistoryDialog()
{
    delete ui;
}

void HistoryDialog::on_searchButton_clicked()
{
    ui->fileList->clear();

    QString customerId = ui->idInput->text().trimmed();
    if (customerId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a customer ID.");
        return;
    }

    QDir dir = QDir::current();
    QStringList files = dir.entryList(QStringList() << "bill_" + customerId + "_*.txt", QDir::Files);

    if (files.isEmpty()) {
        ui->fileList->addItem("No bills found for ID: " + customerId);
        return;
    }

    for (const QString &file : files) {
        ui->fileList->addItem(file);
    }
}

void HistoryDialog::on_fileList_itemClicked(QListWidgetItem *item)
{
    QString fileName = item->text();
    if (!fileName.startsWith("bill_")) return; // avoid clicking "No bills found"
    QString fullPath = QDir::current().absoluteFilePath(fileName);
    emit billSelected(fullPath);
    this->accept(); // close dialog
}
