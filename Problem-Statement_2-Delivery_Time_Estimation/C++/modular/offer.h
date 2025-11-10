#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

struct Offer
{
    std::string code;
    int discount_perc;
    int min_dist;
    int max_dist;
    int min_weight;
    int max_weight;

    bool isValid();
};

std::unordered_map<std::string, Offer> IngestOffers(std::string filename, std::ostream& os = std::cout);