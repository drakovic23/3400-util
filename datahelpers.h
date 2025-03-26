#ifndef DATAHELPERS_H
#define DATAHELPERS_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <chrono>
#include <random>



using namespace std;

enum class UtilityType
{
    NaturalGas,
    InternetTV,
    InternetMobile,
    InternetHomePhone,
    HydroElectric,
    HydroWater,
    HydroSewerage
};

struct UtilityService
{
    UtilityType type;
    double meterRate;
};

class Provider
{
public:
    int id; //Provider ID
    string name;
    map<UtilityType, UtilityService> services;

    Provider(int pid, const std::string& pname) : id(pid), name(pname) {}

    void initServices()
    {
        auto randRate = [](double min, double max) {
            return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
        };

        services[UtilityType::NaturalGas] = {UtilityType::NaturalGas, randRate(4.0, 8.0)};
        services[UtilityType::InternetTV] = {UtilityType::InternetTV, randRate(8.0, 15.0)};
        services[UtilityType::InternetMobile] = {UtilityType::InternetMobile, randRate(6.0, 12.0)};
        services[UtilityType::InternetHomePhone] = {UtilityType::InternetHomePhone, randRate(5.0, 10.0)};
        services[UtilityType::HydroElectric] = {UtilityType::HydroElectric, randRate(12.0, 20.0)};
        services[UtilityType::HydroWater] = {UtilityType::HydroWater, randRate(10.0, 18.0)};
        services[UtilityType::HydroSewerage] = {UtilityType::HydroSewerage, randRate(5.0, 12.0)};
    }


};

class Bill
{
public:
    std::chrono::system_clock::time_point issueDate;
    std::chrono::system_clock::time_point dueDate;

    double amount; //Amount to be paid
    bool isPaid;

    Bill(std::chrono::system_clock::time_point issueDate, double amt)
        : issueDate(issueDate), amount(amt)
    {
        dueDate = issueDate + std::chrono::hours(24 * 30);
    }

    bool isOverDue(std::chrono::system_clock::time_point currentDate) const
    {
        return !isPaid && (currentDate > dueDate);
    }
};

class Subscription
{
public:
    Provider* provider; //The provider for this service/subscription
    UtilityService service;
    vector<Bill> bills; //A vector holding billings for this subscription

    Subscription(Provider* prov, const UtilityService &serv)
        : provider(prov), service(serv){}

    void addBill(const Bill& bill)
    {
        bills.push_back(bill);
    }
};

class Customer
{
public:
    int id;
    string name;
    vector<Subscription> subscriptions; //A vector of services this customer is subscribed to

    void addSubscription(const Subscription& sub)
    {
        subscriptions.push_back(sub);
    }
};

#endif // DATAHELPERS_H

