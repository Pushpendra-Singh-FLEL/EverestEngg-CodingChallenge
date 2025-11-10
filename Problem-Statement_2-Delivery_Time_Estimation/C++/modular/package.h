#include<string>
#include <iomanip>
#include <ostream>
#include "offer.h"

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
    Package(std::string id, int weight, int distance, long long wt_multiplier = 10, long long dist_multiplier = 5);

    void CalculateCost(long long, Offer);

    void resetWeightMultipliers(long long multiplier) { wt_multiplier = multiplier; }
    void resetDistanceMultipliers(long long multiplier) { dist_multiplier = multiplier; }

    int getWeight() { return weight; }
    int getDistance() { return distance; }
    float getDeliveryTime() { return delivery_time; }

    void setDeliveryTime(float dt)  { delivery_time = dt; }

    friend std::ostream& operator <<(std::ostream& os, const Package& pkg);
};