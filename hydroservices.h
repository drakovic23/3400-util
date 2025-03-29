// hydroservices.h
#ifndef HYDROSERVICES_H
#define HYDROSERVICES_H

#include <QString>
#include <QDate>

class HydroService {
protected:
    QString customerName;
    int customerID;
    double meterReading; // in units
    double fixedRate;    // fixed monthly charge
    double perUnitRate;  // rate per unit
    bool isPaid;
    QDate billDate;
    QDate paymentDate;

public:
    HydroService(QString name, int id, double fixed, double rate)
        : customerName(name), customerID(id), fixedRate(fixed), perUnitRate(rate), isPaid(false) {}

    virtual ~HydroService() {}

    void setMeterReading(double reading) {
        meterReading = reading;
    }

    void setBillDate(QDate date) {
        billDate = date;
    }

    void setPaymentDate(QDate date) {
        paymentDate = date;
        isPaid = true;
    }

    double calculateTotalBill() const {
        return fixedRate + (meterReading * perUnitRate);
    }

    bool isOverdue() const {
        return !isPaid && billDate.daysTo(QDate::currentDate()) > 30;
    }

    virtual QString getServiceType() const = 0;

    QString getCustomerName() const { return customerName; }
    int getCustomerID() const { return customerID; }
    bool getPaymentStatus() const { return isPaid; }
    QDate getBillDate() const { return billDate; }
    QDate getPaymentDate() const { return paymentDate; }
};

class ElectricService : public HydroService {
public:
    ElectricService(QString name, int id)
        : HydroService(name, id, 30.0, 0.15) {}

    QString getServiceType() const override {
        return "Electric";
    }
};

class WaterService : public HydroService {
public:
    WaterService(QString name, int id)
        : HydroService(name, id, 20.0, 0.10) {}

    QString getServiceType() const override {
        return "Water";
    }
};

class SewerageService : public HydroService {
public:
    SewerageService(QString name, int id)
        : HydroService(name, id, 25.0, 0.12) {}

    QString getServiceType() const override {
        return "Sewerage";
    }
};

#endif // HYDROSERVICES_H
