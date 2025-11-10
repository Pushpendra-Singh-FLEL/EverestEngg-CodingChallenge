#include "delivery_time.h"

int main()
{
    Delivery::SetUpDelivery();
    Delivery::ExecuteWorkflow();
    Delivery::TearDownDelivery();
}