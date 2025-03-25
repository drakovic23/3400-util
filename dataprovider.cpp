#include "dataprovider.h"
#include <cstdlib>   // For rand()
#include <ctime>     // For time()

DataProvider::DataProvider()
{
    srand(static_cast<unsigned>(time(nullptr))); // Seed RNG once

    // Initialize 10 providers
    vector<Provider> providers =
        {
            Provider(1, "Provider 1"),
            Provider(2, "Provider 2"),
            Provider(3, "Provider 3"),
            Provider(4, "Provider 4"),
            Provider(5, "Provider 5"),
            Provider(6, "Provider 6"),
            Provider(7, "Provider 7"),
            Provider(8, "Provider 8"),
            Provider(9, "Provider 9"),
            Provider(10, "Provider 10")
        };

    for (Provider& p : providers)
        p.initServices();

    // ========== Customer Generation ==========
    map<int, Customer> customers;

    for (int i = 0; i < 100; ++i) {
        Customer c(i, "Customer_" + std::to_string(i));

        // Round-robin assign provider
        Provider* assignedProvider = &providers[i % providers.size()];

        // ========== Hydro Section (Amro) ==========
        for (UtilityType type : {UtilityType::HydroElectric, UtilityType::HydroWater, UtilityType::HydroSewerage}) {
            Subscription s(assignedProvider, assignedProvider->services[type]);

            int numBills = 1 + rand() % 3; // Random 1–3 bills
            for (int j = 0; j < numBills; ++j) {
                auto now = std::chrono::system_clock::now();
                double usage = 10 + rand() % 20;
                Bill b(now, usage * assignedProvider->services[type].meterRate);
                b.isPaid = rand() % 2 == 0;
                s.addBill(b);
            }

            c.addSubscription(s);
        }
        // ========== End Hydro Section (Amro) ==========

        // Optional: keep existing internet subscription
        c.addSubscription(Subscription(&providers[2], providers[2].services[UtilityType::InternetTV]));

        customers[i] = c;
    }

    this->customers = customers;
    this->providers = providers;
}

vector<Provider> DataProvider::getProviders()
{
    return providers;
}

map<int, Customer> DataProvider::getCustomers()
{
    return customers;
}
