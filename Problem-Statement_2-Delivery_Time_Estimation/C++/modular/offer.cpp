#include <fstream>
#include "offer.h"
#include "json.hpp"

using json = nlohmann::json;

bool Offer::isValid()
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

std::unordered_map<std::string, Offer> IngestOffers(std::string filename, std::ostream& os)
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
                offer.value("max_weight", -1)
            };

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
        os << "JSON parse error : " << e.what() << '\n' << "Byte position: " << e.byte << "\n";
    }

    return offers;
}