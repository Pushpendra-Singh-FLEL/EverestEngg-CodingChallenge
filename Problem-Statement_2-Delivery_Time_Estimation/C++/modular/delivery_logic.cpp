#include <vector>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <queue>
#include "delivery_logic.h"

std::unordered_map<std::string, Offer> Delivery::_offers = std::unordered_map<std::string, Offer>();

std::string Delivery::buildDateTimeString()
{
    auto sys_clock = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm time_info = *std::localtime(&sys_clock);
    std::ostringstream oss;
    oss << std::put_time(&time_info, "%d-%m-%Y-%H-%M-%S");

    return oss.str();
}

std::ofstream Delivery::_logFile = []
{
    std::string fileName = "Log_" + Delivery::buildDateTimeString() + ".txt";

    return std::ofstream(std::move(fileName));
}();

void Delivery::kp(std::vector<compositeValue> &compositeObjects,
                  int max_carriable_weight,
                  std::vector<bool> &availability,
                  std::vector<compositeValue> &availableComputations)
{
    for (size_t i = 1; i <= compositeObjects.size(); i++)
    {
        if (availability[i - 1])
        {
            for (size_t j = max_carriable_weight; j >= compositeObjects[i - 1].GetWeight(); j--)
            {
                compositeValue np;
                np = std::move(availableComputations[j - compositeObjects[i - 1].GetWeight()] + compositeObjects[i - 1]);
                if (np > availableComputations[j])
                {
                    availableComputations[j] = std::move(np);
                }
            }
        }
    }
}

void Delivery::SetUpDelivery(std::string filePath, bool useFileLogging, std::ostream &out)
{
    if (useFileLogging)
    {
        _offers = std::move(IngestOffers(filePath, _logFile));
    }
    else
    {
        _offers = std::move(IngestOffers(filePath, out));
    }
}

void Delivery::TearDownDelivery()
{
    if (_logFile.is_open())
    {
        _logFile.close();
    }
}

void Delivery::ReloadOffers(std::string filePath)
{
    _offers = std::move(IngestOffers(filePath, _logFile));
}

void Delivery::ExecuteWorkflow(std::istream &is, std::ostream &os)
{
    long long base_delivery_cost = 0;
    int no_of_packages = 0, pkg_weight_in_kg = 0, pkg_distance_in_km = 0;
    std::string offer_id = "", pkg_id = "";

    is >> base_delivery_cost >> no_of_packages;

    std::vector<Package> packages;

    for (size_t i = 0; i < no_of_packages; i++)
    {
        is >> pkg_id >> pkg_weight_in_kg >> pkg_distance_in_km >> offer_id;
        Package pkg(pkg_id, pkg_weight_in_kg, pkg_distance_in_km);

        auto offer = _offers.find(offer_id);

        if (offer != _offers.end())
        {
            pkg.CalculateCost(base_delivery_cost, offer->second);
        }
        else
        {
            pkg.CalculateCost(base_delivery_cost, Offer());
        }

        packages.emplace_back(std::move(pkg));
    }

    int no_of_vehicles = 0, max_speed = 0, max_carriable_weight = 0;

    is >> no_of_vehicles >> max_speed >> max_carriable_weight;

    // Delivery_Time(packages, no_of_vehicles, max_speed, max_carriable_weight);
    Delivery_Time(packages, no_of_vehicles, max_speed, max_carriable_weight);

    for (size_t i = 0; i < packages.size(); i++)
    {
        os << packages[i];
    }
}

void Delivery::Delivery_Time(std::vector<Package> &packages, int no_of_vehicles, int max_speed, int max_carriable_weight)
{
    int no_of_packages = packages.size();
    std::vector<compositeValue> availableComputations(max_carriable_weight + 1, compositeValue());
    std::vector<bool> availability(no_of_packages, true);

    auto agent_queue = std::move(get_agent_queue(no_of_vehicles));
    auto compositeObjects = std::move(get_pre_computed_composite_objects(packages));

    while (no_of_packages)
    {
        kp(compositeObjects, max_carriable_weight, availability, availableComputations);
        auto &best = availableComputations[max_carriable_weight];

        sort(best.bag.begin(), best.bag.end(),
             [&packages](const int pkg1, const int pkg2)
             {
                 return packages[pkg1].getDistance() < packages[pkg2].getDistance();
             });

        int available_agent = agent_queue.top(), max_agent_busy_time = 0;

        for (auto &&idx : best.bag)
        {
            availability[idx] = false;
            max_agent_busy_time = available_agent + ((static_cast<long long>(packages[idx].getDistance()) * 100) / max_speed);
            packages[idx].setDeliveryTime(static_cast<float>(max_agent_busy_time) / 100);
        }

        agent_queue.pop();
        agent_queue.push(max_agent_busy_time * 2);

        no_of_packages -= best.bag.size();
        
        for (auto &&ac : availableComputations)
        {
            ac.reset();
        }
    }
}