#include <iostream>
#include "delivery_logic.h"

int main()
{
    Delivery::SetUpDelivery("json_files\\offers.json", false);
    Delivery::ExecuteWorkflow();
    Delivery::TearDownDelivery();
}