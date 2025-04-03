#include "addcustomerdialog.h"
#include "ui_addcustomerdialog.h"
#include "dataprovider.h"
#include "datahelpers.h"

AddCustomerDialog::AddCustomerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCustomerDialog)
{
    ui->setupUi(this);

    ui->nameLineEdit->setPlaceholderText("Enter customer name");

    ui->utilityComboBox->addItem("Select a service", -1);
    ui->utilityComboBox->addItem("Natural Gas", static_cast<int>(UtilityType::NaturalGas));
    ui->utilityComboBox->addItem("Internet - TV", static_cast<int>(UtilityType::InternetTV));
    ui->utilityComboBox->addItem("Internet - Mobile", static_cast<int>(UtilityType::InternetMobile));
    ui->utilityComboBox->addItem("Internet - Home Phone", static_cast<int>(UtilityType::InternetHomePhone));
    ui->utilityComboBox->addItem("Hydro - Electric", static_cast<int>(UtilityType::HydroElectric));
    ui->utilityComboBox->addItem("Hydro - Water", static_cast<int>(UtilityType::HydroWater));
    ui->utilityComboBox->addItem("Hydro - Sewerage", static_cast<int>(UtilityType::HydroSewerage));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddCustomerDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddCustomerDialog::reject);
}

AddCustomerDialog::~AddCustomerDialog()
{
    delete ui;
}

Customer AddCustomerDialog::getCreatedCustomer(const std::vector<Provider>& providers)
{
    Customer customer;
    customer.name = ui->nameLineEdit->text().toStdString();
    customer.id = -1; //will be assigned in mainWindow

    int index = ui->utilityComboBox->currentIndex();
    UtilityType selected = static_cast<UtilityType>(ui->utilityComboBox->itemData(index).toInt());

    //find a provider that offers the selected utility
    for (const Provider& prov : providers)
    {
        if (prov.services.count(selected))
        {
            Subscription sub(prov, prov.services.at(selected));
            customer.addSubscription(sub);
            break;
        }
    }

    return customer;
}
