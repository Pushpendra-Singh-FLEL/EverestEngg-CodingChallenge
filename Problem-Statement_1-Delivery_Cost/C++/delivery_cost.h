#include <iostream>
#include <chrono>
#include <queue>
#include <fstream>
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

    void setDeliveryTime(float dt) { delivery_time = dt; }

    friend std::ostream &operator<<(std::ostream &os, const Package &pkg)
    {
        os << pkg.id << std::fixed << std::setprecision(2) << " " << pkg.discount << " " << pkg.cost << std::endl;
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
        const int weight_mu_factor = 10, distance_mu_factor = 5;

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

        for (size_t i = 0; i < packages.size(); i++)
        {
            os << packages[i];
        }
    }
};

std::unordered_map<std::string, Offer> Delivery::_offers = std::unordered_map<std::string, Offer>();

std::ofstream Delivery::_logFile = []
{
    std::string fileName = "Log_" + Delivery::buildDateTimeString() + ".txt";

    return std::ofstream(std::move(fileName));
}();
