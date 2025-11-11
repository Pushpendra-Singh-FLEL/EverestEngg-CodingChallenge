#include <unordered_map>
#include <iostream>
#include <functional>
#include <queue>
#include <fstream>
#include "offer.h"
#include "package.h"

class Delivery
{
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

    static std::unordered_map<std::string, Offer> _offers;

    static std::ofstream _logFile;

    static std::string buildDateTimeString();

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
        std::vector<bool> availability(packages.size(), false);

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
                   std::vector<compositeValue> &availableComputations);

public:
    static void SetUpDelivery(std::string filePath = "json_files\\offers.json", bool useFileLogging = true, std::ostream &out = std::cout);

    static void TearDownDelivery();

    static void ExecuteWorkflow(std::istream &is = std::cin, std::ostream &os = std::cout);

    static void ReloadOffers(std::string filePath);

    static void Delivery_Time(std::vector<Package> &packages, int no_of_vehicles, int max_speed, int max_carriable_weight);
};