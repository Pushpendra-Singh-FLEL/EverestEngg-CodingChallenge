#include "package.h"

Package::Package(std::string id, int weight, int distance,
                 long long wt_multiplier, long long dist_multiplier) : id{std::move(id)},
                                                                    weight{weight}, distance{distance},
                                                                    wt_multiplier{wt_multiplier}, dist_multiplier{dist_multiplier} {}

void Package::CalculateCost(long long base_delivery_cost, Offer of)
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

std::ostream &operator<<(std::ostream &os, const Package& pkg)
{
    os << pkg.id << std::fixed << std::setprecision(2) << " " << pkg.discount << " " << pkg.cost << " ";
    os << pkg.delivery_time << std::endl;
    return os;
}
