#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <array>
#include "delivery_logic.h"

void malformed_json_offers()
{
    std::stringstream oss, iss;
    Delivery::SetUpDelivery("json_files\\malformed_offers.json", false, oss);
    std::string expected_error = "JSON parse error : [json.exception.parse_error.101] parse error at line 26, column 1: syntax error while parsing array - unexpected end of input; expected ']'";
    std::string errStr;
    std::getline(oss, errStr, '\n');
    std::cout << errStr << std::endl;

    try
    {
        if (errStr.compare(expected_error) != 0)
            throw std::exception("");
    }
    catch (const std::exception &e)
    {
        std::cout << "Test : malformed_offers_json FAILED" << '\n';
        return;
    }
    std::cout << "Test : malformed_offers_json PASSED" << '\n';
}

void invalid_offers_json()
{
    std::stringstream oss, iss;
    Delivery::SetUpDelivery("json_files\\invalid_offers.json", false, oss);
    std::string errStr;
    std::array<std::pair<std::string, std::string>, 8> expected_errors =
        {
            std::make_pair("Negative Discount", "Offer : NEG_DISCOUNT has invalid values, and hence is ignored!!"),
            std::make_pair("Negative minimum Distance", "Offer : NEG_MIN_DISTANCE has invalid values, and hence is ignored!!"),
            std::make_pair("Negative maximum Distance", "Offer : NEG_MAX_DISTANCE has invalid values, and hence is ignored!!"),
            std::make_pair("Negative minimum Weight", "Offer : NEG_MIN_WEIGHT has invalid values, and hence is ignored!!"),
            std::make_pair("Negative maximum Weight", "Offer : NEG_MAX_WEIGHT has invalid values, and hence is ignored!!"),
            std::make_pair("Invalid Min/Max Range for Weight", "Offer : INALID_RANGE_WEIGHT has invalid values, and hence is ignored!!"),
            std::make_pair("Invalid Min/Max Range for Distance", "Offer : INALID_RANGE_DISTANCE has invalid values, and hence is ignored!!"),
            std::make_pair("Invalid Discount %age(> 100)", "Offer : INALID_DISCOUNT has invalid values, and hence is ignored!!")};

    int idx = 0;

    std::cout << "Test : invalid_offers_json : " << '\n';

    while (std::getline(oss, errStr, '\n'))
    {
        if (errStr.compare(expected_errors[idx].second) != 0)
        {
            std::cout << "\t Test " << expected_errors[idx].first << " : FAILED" << '\n';
        }
        else
        {
            std::cout << "\t Test " << expected_errors[idx].first << " : PASSED" << '\n';
        }
        idx++;
    }
}

void package_cost_computation_without_offer()
{
    std::stringstream oss;
    std::string expected_output = "pkg_id01 0.00 1100.00 0.00";

    Package pkg("pkg_id01", 80, 40);
    pkg.CalculateCost(100, Offer{"OFR01", 8, 2, 10, 2, 10});
    oss << pkg;

    std::string output;
    std::getline(oss, output, '\n');

    if (output.compare(expected_output) != 0)
    {
        std::cout << "Test : package_cost_computation_without_offer FAILED" << '\n';
        return;
    }
    std::cout << "Test : package_cost_computation_without_offer PASSED" << '\n';
}
void package_cost_computation_with_Invalid_offer()
{
    std::stringstream oss;
    std::string expected_output = "pkg_id02 0.00 1100.00 0.00";

    Package pkg("pkg_id02", 80, 40);
    pkg.CalculateCost(100, Offer());
    oss << pkg;

    std::string output;
    std::getline(oss, output, '\n');

    if (output.compare(expected_output) != 0)
    {
        std::cout << "Test : package_cost_computation_without_offer FAILED" << '\n';
        return;
    }
    std::cout << "Test : package_cost_computation_without_offer PASSED" << '\n';
}

void package_cost_computation_with_offer()
{
    std::stringstream oss;
    std::string expected_output = "pkg_id03 72.00 828.00 0.00";

    Package pkg("pkg_id03", 40, 80);
    pkg.CalculateCost(100, Offer{"OFR01", 8, 2, 81, 2, 41});
    oss << pkg;

    std::string output;
    std::getline(oss, output, '\n');

    if (output.compare(expected_output) != 0)
    {
        std::cout << "Test : package_cost_computation_with_offer FAILED" << '\n';
        return;
    }
    std::cout << "Test : package_cost_computation_with_offer PASSED" << '\n';
}

void package_cost_computation_with_different_base_multiplier_cost()
{
    std::stringstream oss;
    std::string expected_output = "pkg_id04 80.80 929.20 0.00";

    Package pkg("pkg_id04", 40, 80);
    pkg.CalculateCost(210, Offer{"OFR01", 8, 2, 81, 2, 41});
    oss << pkg;

    std::string output;
    std::getline(oss, output, '\n');

    if (output.compare(expected_output) != 0)
    {
        std::cout << "Test : package_cost_computation_with_different_base_multiplier_cost FAILED" << '\n';
        return;
    }
    std::cout << "Test : package_cost_computation_with_different_base_multiplier_cost PASSED" << '\n';
}

void package_cost_computation_with_different_weight_multiplier()
{
    std::stringstream oss;
    std::string expected_output = "pkg_id05 96.80 1113.20 0.00";

    Package pkg("pkg_id05", 40, 80);
    pkg.resetWeightMultipliers(15);
    pkg.CalculateCost(210, Offer{"OFR01", 8, 2, 81, 2, 41});
    oss << pkg;

    std::string output;
    std::getline(oss, output, '\n');

    if (output.compare(expected_output) != 0)
    {
        std::cout << "Test : package_cost_computation_with_different_weight_multiplier FAILED" << '\n';
        return;
    }
    std::cout << "Test : package_cost_computation_with_different_weight_multiplier PASSED" << '\n';
}

void package_cost_computation_with_different_distance_multiplier()
{
    std::stringstream oss;
    std::string expected_output = "pkg_id06 144.80 1665.20 0.00";

    Package pkg("pkg_id06", 40, 80);
    pkg.resetDistanceMultipliers(15);
    pkg.CalculateCost(210, Offer{"OFR01", 8, 2, 81, 2, 41});
    oss << pkg;

    std::string output;
    std::getline(oss, output, '\n');

    if (output.compare(expected_output) != 0)
    {
        std::cout << "Test : package_cost_computation_with_different_distance_multiplier FAILED" << '\n';
        return;
    }
    std::cout << "Test : package_cost_computation_with_different_distance_multiplier PASSED" << '\n';
}

void package_time_computation()
{
    std::stringstream oss, iss;
    std::vector<float> expected_delivery_time = {3.98f, 1.78f, 1.42f, 0.85f, 4.19f};
    std::vector<Package> pkgs =
        {
            Package("pkg_id01", 50, 30),
            Package("pkg_id02", 75, 125),
            Package("pkg_id03", 175, 100),
            Package("pkg_id04", 110, 60),
            Package("pkg_id05", 155, 95)};
    const int no_of_vehicles = 2, max_speed = 70, max_carriable_weight = 200;
    Delivery::Delivery_Time(pkgs, no_of_vehicles, max_speed, max_carriable_weight);

    for (size_t i = 0; i < pkgs.size(); i++)
    {
        if (pkgs[i].getDeliveryTime() != expected_delivery_time[i])
        {
            std::cout << "Test : package_time_computation FAILED" << '\n';
            return;
        }
    }
    std::cout << "Test : package_time_computation PASSED" << '\n';
}

void package_time_computation_with_weight_tie()
{
    std::stringstream oss, iss;
    std::vector<float> expected_delivery_time = {0.42f, 1.78f, 1.42f, 4.41f, 4.19f, 1.24f};
    std::vector<Package> pkgs =
        {
            Package("pkg_id01", 50, 30),
            Package("pkg_id02", 75, 125),
            Package("pkg_id03", 175, 100),
            Package("pkg_id04", 110, 60),
            Package("pkg_id05", 155, 95),
            Package("pkg_id05", 60, 87),
        };
    const int no_of_vehicles = 2, max_speed = 70, max_carriable_weight = 200;
    Delivery::Delivery_Time(pkgs, no_of_vehicles, max_speed, max_carriable_weight);

    for (size_t i = 0; i < pkgs.size(); i++)
    {
        if (pkgs[i].getDeliveryTime() != expected_delivery_time[i])
        {
            std::cout << "Test : package_time_computation FAILED" << '\n';
            return;
        }
    }
    std::cout << "Test : package_time_computation PASSED" << '\n';
}

void workflow_integration_test()
{
    std::stringstream oss, iss;
    Delivery::SetUpDelivery("json_files\\offers.json", false, oss);
    iss << "100 5\nPKG1 50 30 OFR001\nPKG2 75 125 OFFR0008\nPKG3 175 100 OFFR003\nPKG4 110 60 OFFR002\nPKG5 155 95 NA \n2 70 200\n";
    Delivery::ExecuteWorkflow(oss, iss);
    std::string output = "";
    std::array<std::pair<std::string, std::string>, 8> expected_errors =
        {
            std::make_pair("PKG1", "PKG1 0.00 750.00 3.98"),
            std::make_pair("PKG2", "PKG2 0.00 1475.00 1.78"),
            std::make_pair("PKG3", "PKG3 0.00 2350.00 1.42"),
            std::make_pair("PKG4", "PKG4 105.00 1395.00 0.85"),
            std::make_pair("PKG5", "PKG5 0.00 2125.00 4.19")};

    int idx = 0;

    std::cout << "Test : workflow_integration_test : " << '\n';
    bool testFailed = false;

    while (std::getline(oss, output, '\n'))
    {
        if (output.compare(expected_errors[idx].second) != 0)
        {
            std::cout << "\t Values for Package Id : " << expected_errors[idx].first << " does not Match" << '\n';
            testFailed = true;
        }
        idx++;
    }
    if (testFailed)
    {
        std::cout << "\t Test workflow_integration_test : FAILED" << '\n';
    }
    else
    {
        std::cout << "\t Test workflow_integration_test : PASSED" << '\n';
    }
}

void workflow_integration_test_with_weight_tie()
{
    std::stringstream oss, iss;
    Delivery::SetUpDelivery("json_files\\offers.json", false, oss);
    iss << "100 6\nPKG1 50 30 OFR001\nPKG2 75 125 OFFR0008\nPKG3 175 100 OFFR003\nPKG4 110 60 OFFR002\nPKG5 155 95 NA \nPKG6 60 87 OFFR002 \n3 85 200\n";
    Delivery::ExecuteWorkflow(oss, iss);
    std::string output = "";
    std::array<std::pair<std::string, std::string>, 8> expected_errors =
        {
            std::make_pair("PKG1", "PKG1 0.00 750.00 0.35"),
            std::make_pair("PKG2", "PKG2 0.00 1475.00 1.47"),
            std::make_pair("PKG3", "PKG3 0.00 2350.00 1.17"),
            std::make_pair("PKG4", "PKG4 105.00 1395.00 2.92"),
            std::make_pair("PKG5", "PKG5 0.00 2125.00 1.11"),
            std::make_pair("PKG6", "PKG6 79.45 1055.55 1.02")
        };

    int idx = 0;

    std::cout << "Test : workflow_integration_test_with_weight_tie : " << '\n';
    bool testFailed = false;

    while (std::getline(oss, output, '\n'))
    {
        if (output.compare(expected_errors[idx].second) != 0)
        {
            std::cout << "\t Values for Package Id : " << expected_errors[idx].first << " does not Match" << '\n';
            testFailed = true;
        }
        idx++;
    }
    if (testFailed)
    {
        std::cout << "\t Test workflow_integration_test_with_weight_tie : FAILED" << '\n';
    }
    else
    {
        std::cout << "\t Test workflow_integration_test_with_weight_tie : PASSED" << '\n';
    }
}

int main()
{
    malformed_json_offers();
    invalid_offers_json();
    package_cost_computation_without_offer();
    package_cost_computation_with_Invalid_offer();
    package_cost_computation_with_offer();
    package_cost_computation_with_different_base_multiplier_cost();
    package_cost_computation_with_different_weight_multiplier();
    package_cost_computation_with_different_distance_multiplier();
    package_time_computation();
    package_time_computation_with_weight_tie();
    workflow_integration_test();
    workflow_integration_test_with_weight_tie();
}