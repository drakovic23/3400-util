#include "dataprovider.h"
#include <random> // Used to generate random data

//Default constructor generates the customer and provider information
DataProvider::DataProvider()
{
    //Initialize our customer data
    vector<Provider> providers =
        { //TODO: Change names to something more realistic
         Provider(1, "Provider 1"),
         Provider(2, "Provider 2"),
         Provider(3, "Provider 3"),
         Provider(4, "Provider 4"),
         Provider(5, "Provider 5"),
         Provider(6, "Provider 6"),
         Provider(7, "Provider 7"),
         Provider(8, "Provider 8"),
         Provider(9, "Provider 9"),
         Provider(10, "Provider 10"),
         };

    for(size_t i = 0; i < providers.size();i++)
        providers[i].initServices();

    this->providers = providers;

    //Below is used for randomizing the subs and providers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> providerDist(0, providers.size() - 1);
    std::vector<UtilityType> utilityTypes = {
        UtilityType::HydroElectric,
        UtilityType::HydroWater,
        UtilityType::HydroSewerage,
        UtilityType::InternetTV,
        UtilityType::InternetMobile,
        UtilityType::InternetHomePhone,
        UtilityType::NaturalGas,
    };
    std::uniform_int_distribution<> utilityDist(0, utilityTypes.size() - 1);

    map<int, Customer> customers; //Map holds a KVP of customers (customer id -> Customer)
    for(size_t i = 0;i < 100;i++) //Generate 100 customers
    {
        Customer c(i, "Customer_" + std::to_string(i));

        int providerIndex1 = providerDist(gen);
        int providerIndex2 = providerDist(gen);
        UtilityType randomUtility1 = utilityTypes[utilityDist(gen)];
        UtilityType randomUtility2 = utilityTypes[utilityDist(gen)];

        c.addSubscription(Subscription(providers[providerIndex1], providers[providerIndex1].services[randomUtility1]));
        c.addSubscription(Subscription(providers[providerIndex2], providers[providerIndex2].services[randomUtility2]));

        customers[i] = c;
    }

    std::uniform_real_distribution<float> amountDist(100.0f, 500.0f);
    //Adding some bills to customers
    for(size_t i = 0;i < 100;i++)
    {
        for(size_t j = 0;j < customers[i].subscriptions.size();j++)
        {
            auto today = std::chrono::system_clock::now();
            float randomAmount = amountDist(gen);
            if(i % 2 == 0)
            {
                auto thirtyDaysAgo = today - std::chrono::hours(24 * 30);
                Bill newBill(thirtyDaysAgo, randomAmount);
                customers[i].subscriptions[j].addBill(newBill);
            }
            else
            {
                auto sixtyDaysAgo = today - std::chrono::hours(24 * 60);
                Bill newBill(sixtyDaysAgo, randomAmount);
                newBill.isPaid = true;
                customers[i].subscriptions[j].addBill(newBill);
            }

        }
    }

    this->customers = customers;
}

vector<Provider> DataProvider::getProviders()
{
    return providers;
}

map<int, Customer> DataProvider::getCustomers()
{
    return customers;
}
