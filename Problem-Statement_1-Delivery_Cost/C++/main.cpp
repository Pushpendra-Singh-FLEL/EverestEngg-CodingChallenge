#include "delivery_cost.h"

int main()
{
    Delivery::SetUpDelivery();
    Delivery::ExecuteWorkflow();
    Delivery::TearDownDelivery();
}