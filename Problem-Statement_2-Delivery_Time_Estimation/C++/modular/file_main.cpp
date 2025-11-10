#include "delivery_logic.h"

int main()
{
    std::ifstream is("input.txt");
    std::ofstream os("output.txt");
    Delivery::SetUpDelivery();
    Delivery::ExecuteWorkflow(is, os);
    Delivery::TearDownDelivery();

    if (is.is_open())
    {
        is.close();
    }
    if (os.is_open())
    {
        os.close();
    }
}