#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include "json.hpp"

using json = nlohmann::json;

struct Offer
{
    std::string code;
    int discount_perc;
    int min_dist;
    int max_dist;
    int min_weight;
    int max_weight;

    bool isValid()
    {
        if (code.empty())
        {
            return false;
        }

        if (min_dist < 0 || max_dist < 0 || min_weight < 0 || max_weight < 0 || discount_perc < 0)
        {
            return false;
        }

        if (min_dist > max_dist || min_weight > max_weight || discount_perc > 100)
        {
            return false;
        }

        return true;
    }
};

std::unordered_map<std::string, Offer> IngestOffers(std::string filename, std::ostream &os = std::cout)
{
    std::unordered_map<std::string, Offer> offers;
    std::ifstream offer_file(filename);
    if (!offer_file.is_open())
    {
        os << "Could not open Offers file! Ensure it is placed at the location of executable\n";
        return offers;
    }

    json offers_json;
    try
    {
        offers_json = json::parse(offer_file);

        for (auto &offer : offers_json)
        {
            Offer item = {
                static_cast<std::string>(offer.value("code", "")),
                offer.value("discount_perc", -1),
                offer.value("min_distance", -1),
                offer.value("max_distance", -1),
                offer.value("min_weight", -1),
                offer.value("max_weight", -1)};

            if (item.isValid())
            {
                offers.emplace(item.code, std::move(item));
            }
            else
            {
                os << "Offer : " << item.code << " has invalid values, and hence is ignored!!\n";
            }
        }
    }
    catch (const json::parse_error &e)
    {
        os << "JSON parse error : " << e.what() << '\n'
           << "Byte position: " << e.byte << "\n";
    }

    return offers;
}

class Package
{
    long long wt_multiplier;
    long long dist_multiplier;
    std::string id;
    int weight = 0;
    int distance = 0;
    double discount = 0.0f;
    double cost = 0.0f;
    float delivery_time = 0.0f;

public:
    Package(std::string id, int weight, int distance, long long wt_multiplier = 10, long long dist_multiplier = 5) : id{std::move(id)},
                                                                                                                     weight{weight}, distance{distance},
                                                                                                                     wt_multiplier{wt_multiplier}, dist_multiplier{dist_multiplier} {}

    void CalculateCost(long long base_delivery_cost, Offer of)
    {
        cost = base_delivery_cost + (wt_multiplier * weight) + (dist_multiplier * distance);
        if (distance >= of.min_dist && distance < of.max_dist)
        {
            if (weight >= of.min_weight && weight < of.max_weight)
            {
                discount = (cost * of.discount_perc) / 100;
            }
        }
        cost = (cost - discount);
    }

    void resetWeightMultipliers(long long multiplier) { wt_multiplier = multiplier; }
    void resetDistanceMultipliers(long long multiplier) { dist_multiplier = multiplier; }

    int getWeight() { return weight; }
    int getDistance() { return distance; }
    float getDeliveryTime() { return delivery_time; }

    void setDeliveryTime(float dt) { delivery_time = dt; }

    friend std::ostream &operator<<(std::ostream &os, const Package &pkg)
    {
        os << pkg.id << std::fixed << std::setprecision(2) << " " << pkg.discount << " " << pkg.cost << " ";
        os << pkg.delivery_time << std::endl;
        return os;
    }
};

class Delivery
{
    static std::unordered_map<std::string, Offer> _offers;

    static std::ofstream _logFile;

    static std::string buildDateTimeString()
    {
        auto sys_clock = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::tm time_info = *std::localtime(&sys_clock);
        std::ostringstream oss;
        oss << std::put_time(&time_info, "%d-%m-%Y-%H-%M-%S");

        return oss.str();
    }

    struct compositeValue
    {
        int weight;
        int count;
        std::vector<size_t> bag;

        compositeValue(int wt, int ct, size_t itemIdx) : weight{wt}, count{ct}
        {
            bag = std::move(std::vector<size_t>(1, itemIdx));
        }
        compositeValue() : weight{0}, count{0}, bag{std::move(std::vector<size_t>())} {}

        compositeValue(const compositeValue &other)
        {
            weight = other.weight;
            count = other.count;
            bag = std::vector<size_t>();
            for (auto &&i : other.bag)
            {
                bag.push_back(i);
            }
        }

        compositeValue(compositeValue &&other)
        {
            weight = other.weight;
            count = other.count;
            bag = std::move(other.bag);
        }

        int GetWeight() { return weight; }

        void reset()
        {
            weight = 0;
            count = 0;
            bag.clear();
        }

        bool operator>(const compositeValue &other)
        {
            if (count > other.count)
                return true;
            else if (count == other.count && weight > other.weight)
                return true;
            else
                return false;
        }

        void operator=(const compositeValue &other) = delete;

        void operator=(compositeValue &&other)
        {
            weight = other.weight;
            count = other.count;
            bag = std::move(other.bag);
        }

        compositeValue operator+(const compositeValue &other)
        {
            compositeValue nv;
            nv.weight = weight + other.weight;
            nv.count = count + other.count;
            for (auto &&i : other.bag)
            {
                nv.bag.push_back(i);
            }
            for (auto &&i : bag)
            {
                nv.bag.push_back(i);
            }

            return nv;
        }
    };

    static auto get_agent_queue(int no_of_vehicles) -> std::priority_queue<int, std::vector<int>, std::greater<int>>
    {
        std::priority_queue<int, std::vector<int>, std::greater<int>> agent_queue;

        for (size_t i = 0; i < no_of_vehicles; i++)
        {
            agent_queue.push(0);
        }
        return agent_queue;
    }

    static auto get_pre_computed_composite_objects(std::vector<Package> &packages) -> std::vector<compositeValue>
    {
        std::vector<compositeValue> compositeObjects;

        for (size_t i = 0; i < packages.size(); i++)
        {
            compositeObjects.push_back(compositeValue(packages[i].getWeight(), 1, i));
        }
        return compositeObjects;
    }

    static auto buildAvailability(std::vector<Package> &packages, int max_carriable_weight, int &no_of_packages) -> std::vector<bool>
    {
        std::vector<bool> availability (packages.size(), false);

        for (size_t i = 0; i < packages.size(); i++)
        {
            if (packages[i].getWeight() <= max_carriable_weight)
            {
                availability[i] = true;
                no_of_packages++;
            }
        }
        return availability;
    }

    static void kp(std::vector<compositeValue> &compositeObjects,
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

    Delivery() = delete;

public:
    static void SetUpDelivery(std::string filePath = "offers.json", bool useFileLogging = true, std::ostream &out = std::cout)
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

    static void TearDownDelivery()
    {
        if (_logFile.is_open())
        {
            _logFile.close();
        }
    }

    static void ReloadOffers(std::string filePath)
    {
        _offers = std::move(IngestOffers(filePath, _logFile));
    }

    static void ExecuteWorkflow(std::istream &is = std::cin, std::ostream &os = std::cout)
    {
        long long base_delivery_cost = 0;
        int no_of_packages = 0, pkg_weight_in_kg = 0, pkg_distance_in_km = 0;
        std::string offer_id = "", pkg_id = "";

        is >> base_delivery_cost >> no_of_packages;

        std::vector<Package> packages;

        float cost = 0.0f, discount = 0.0f;

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

        Delivery_Time(packages, no_of_vehicles, max_speed, max_carriable_weight);

        for (size_t i = 0; i < packages.size(); i++)
        {
            os << packages[i];
        }
    }

    static void Delivery_Time(std::vector<Package> &packages, int no_of_vehicles, int max_speed, int max_carriable_weight)
    {
        int no_of_packages = 0;
        std::vector<compositeValue> availableComputations(max_carriable_weight + 1, compositeValue());
        std::vector<bool> availability = std::move(buildAvailability(packages, max_carriable_weight, no_of_packages));

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
};

std::unordered_map<std::string, Offer> Delivery::_offers = std::unordered_map<std::string, Offer>();

std::ofstream Delivery::_logFile = []
{
    std::string fileName = "Log_" + Delivery::buildDateTimeString() + ".txt";

    return std::ofstream(std::move(fileName));
}();