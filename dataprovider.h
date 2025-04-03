#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <vector>
#include "datahelpers.h"

class DataProvider
{
private:
    vector<Provider> providers;
    map<int, Customer> customers;
    static map<UtilityType, string> utilitiesAsString;
public:
    DataProvider();
    vector<Provider>& getProviders();
    map<int, Customer>& getCustomers();
    Customer findCustomer(int id);
    static string getUtilityAsString(UtilityType type);
};

#endif // DATAPROVIDER_H
