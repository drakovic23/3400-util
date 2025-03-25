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

    void initServices() //TODO: Overload or adjust for different rates
    {
        services[UtilityType::NaturalGas] = {UtilityType::NaturalGas, 5.1};
        services[UtilityType::InternetTV] = {UtilityType::InternetTV, 10.20};
        services[UtilityType::InternetMobile] = {UtilityType::InternetMobile, 8.25};
        services[UtilityType::InternetHomePhone] = {UtilityType::InternetHomePhone, 7.22};
        services[UtilityType::HydroElectric] = {UtilityType::HydroElectric, 15.43};
        services[UtilityType::HydroWater] = {UtilityType::HydroWater, 12.99};
        services[UtilityType::HydroSewerage] = {UtilityType::HydroSewerage, 6.88};
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

    bool isOverDue(std::chrono::system_clock::time_point currentDate)
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

