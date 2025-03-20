#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <vector>
#include "datahelpers.h"

class DataProvider
{
private:
    vector<Provider> providers;
    map<int, Customer> customers;
public:
    DataProvider();
    vector<Provider> getProviders();
    vector<Customer> getCustomers();
};

#endif // DATAPROVIDER_H
