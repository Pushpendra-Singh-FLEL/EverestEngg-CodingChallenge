# EverestEngg-CodingChallenge

Below are the two problems for which solutions have been outlined within this document and captured in the repository:

1. Build a command line application to estimate the total delivery cost of each package with an offer code (if applicable).
2. Build a command line application to calculate the estimated delivery time for every package by maximizing no. of packages in every shipment.

## Overview Of The Solution

In the absence of any directive to choose a specific language, I chose to use ***C++*** to implement my solutions to the problem provided. However I ended up implementing it in **C#** as well which though working has ideas missing that I wanted to take as the logical step-up from the **C++** imlementation.

### Directory Structure
The folder contains two directories **Problem-Statement_1-Delivery_Cost** and **Problem-Statement_2-Delivery_Time_Estimation** for each of the two problems provided.
Below is the directory structure higlighting relevant files,

```
Everest-Engg-Coding_Challenge
  |
  |---Problem-Statement_1-Delivery_Cost
  |             |--C++
  |                 |-- delivery_cost.h
  |                 |-- main.cpp
  |                 |-- tester.cpp
  |                 |-- json.hpp
  |                 |-- offers.json
  |
  |
  |---Problem-Statement_2-Delivery_Time_Estimation
  |             |--C++
  |                 |-- modular
  |                 |      |-- json_files
  |                 |              |-- invalid_offers.json
  |                 |              |-- malformed_offers.json
  |                 |              |-- offers.json
  |                 |      |-- json.hpp
  |                 |      |-- offer.h
  |                 |      |-- package.h
  |                 |      |-- delivery_logic.h
  |                 |      |-- offer.cpp
  |                 |      |-- package.cpp
  |                 |      |-- delivery_logic.cpp
  |                 |      |-- main.cpp
  |                 |      |-- tester.cpp
  |                 |-- delivery_time.h
  |                 |-- main.cpp
  |                 |-- tester.cpp
  |                 |-- json.hpp
  |                 |-- offers.json
  |             |
  |             |--C_Sharp_8.0
  |                 |
  |                 |-- json_files
  |                 |       |-- offers.json
  |                 |-- DeliveryTime.sln
  |                 |-- DeliveryTime.csproj
  |                 |-- Delivery.cs
  |                 |-- Helpers.cs
  |                 |-- Package.cs
  |                 |-- Program.cs
  |
  |--- README.md
```

### Compilation
#### Using MSVC Compiler
To compile the code using Microsoft Visual Studio compiler execute the below command :
```
cl /EHsc /std:c++14 file_1 file_2 ... -o output.exe
```
***Note*** : `cl` can only be invoked from the visual studio cmd prompt, provided its installed.

#### Using GNU Compiler
To compile the code using Microsoft Visual Studio compiler execute the below command :
```
g++ -std=c++14 file_1 file_2 ... -o output.exe
```

### Problem Statement 1 : Estimate Delivery Cost
For this problem a single file contains the entire logic of the problem `delivery_cost.h`. There are two other files:
- `main.cpp` : contains the `main` entry point function for **main cmdline application**.
- `tester.cpp` : contains the `main` entry point function for the **testcases**.

To compile the cmdline application run the following :
```bash
cl /EHsc /std:c++14 main.cpp -o cost_estimation.exe
```

To compile the tester application run the following :
```bash
cl /EHsc /std:c++14 tester.cpp -o cost_estimation_tester.exe
```

#### Key Highlights
- offers are loaded at runtime from json file containing the offers. The file is loaded from the "***Current Working Directory***". Below is the file format :
  ```
  [
    {
        "code" : string,
        "discount_perc" : integer,
        "min_distance" : integer,
        "max_distance" : integer,
        "min_weight" : integer,
        "max_weight" : integer
    },
    .
    .
    .
  ]
  ```
  **NOTE** : This ingestion of offers is performed by `std::unordered_map<std::string, Offer> IngestOffers(std::string filePath, bool useFileLogging, std::ostream &out)`. Any errors/warning are appended to the "Logs_DDMMYYYYHHMMSS.txt" which is created at the start of the application, if the `useFileLogging` is set to `true` (by default the value for `useFileLogging` is set to `true`). If the flag is set to `false` and no `ostream` object is provided, the errors will be logged to the console.

- Any `Offers` that fails the validation is rejected and is not available during the entire duration that the application is alive. Below are the verifications that are performed to ascertain the correctness of an Offer
  - offer code is not empty.
  - %age discount is a non-negative and less than 100.
  - min_distance, max_distance, min_weight, max_weight is non-negative integer.
  - min_distance <= max_distance and min_weight <= max_weight.
  - No data-points are missing.

  **NOTE** : This validation is performed by `bool Offer::IsValid()`

- The application starts by calling `SetUpDelivery(std::string filePath, bool useFileLogging, std::ostream &out)`. The function takes a `filePath` of the `Offers.json`, by default it tries loading from the "***Current Working Directory***" where the executable is executed from. If the loading/parsing of the offers file fail, a warning is logged into the log file. It also takes a flag `useFileLogging`(to indicate if the any errors while loading the offer needs to be logged to a file or to `ostream` object passed as the third argument : *by default it logs any failures to a file*).

- The business logic is encapsulated inside `Delivery::ExecuteWorkflow(std::istream &is = std::cin, std::ostream &os = std::cout)`. The function is responsible for taking input and printing output besides ***delivery cost*** computation. The function takes an input stream from which the input is read and an output stream to which the output is sent to. By default the input and output is streamed from and to console respectively.

- There is also function to `Delivery::ReloadOffers`, however there is not much use in the current form within the application.

- The delivery cost for a package is performed by `Package::CalculateCost(long long base_delivery_cost, Offer of)` that takes in `base_delivery_cost` and the `Offer` to apply on it. This way the package deliver cost can be recalculated with a different `base_delivery_cost`.

- The package class also exposes the ability to reset the weight and distance multiplier besides the default value of 10 and 5 respectively set in the `Package` constructor.

#### Limitations
1. Currently only weight multiplier(`Package::wt_multiplier`), distance multiplier(`Package::dist_multiplier`), and base_delivery_cost(`Package::base_delivery_cost`) are marked as `long long`.<br>
`Package::cost` and `Package::discount` are marked as `double`.<br>
All others are marked as int. Below are the variables marked as intger:
  - all weight related vaiables : **Anything above 2<sup>31</sup> and we are looking to transport planetary objects**.
  - all distance related vaiables : **Anything above 20,375 km (circumference of earth / 2) and we are looking outer-space travel**.
  - number of available agents : **The population of earth is currently at approximately 2<sup>33</sup>**.
  - speed of light : **The speed of light is approximately 2<sup>30.01</sup>, beyond that we are talking about parallel universe or in the science fiction realm**.

2. Because **C++** by default rounds up any double value, therefore overflow cam happen during delivery cost computation.

### Problem Statement 2 : Estimate Delivery Time Estimation
For this problem a single file contains the entire logic of the problem `delivery_time.h`. There are two other files:
- `main.cpp` : contains the `main` entry point function for **main cmdline application**.
- `tester.cpp` : contains the `main` entry point function for the **testcases**.


To compile the cmdline application run the following :
```bash
cl /EHsc /std:c++14 main.cpp -o cost_estimation.exe
```

To compile the tester application run the following :
```bash
cl /EHsc /std:c++14 tester.cpp -o cost_estimation_tester.exe
```

For this solution there is also a somewhat modular solution available, where instead of single file containing the entire logic, separate headers and source code(`cpp`) files are published. This solution can be found inside **modular** directory contained within **Problem-Statement_2-Delivery_Time_Estimation** directory.

To compile the cmdline application run the following :
```bash
cl /EHsc /std:c++14 package.cpp offer.cpp delivery_logic.cpp main.cpp -o time_estimation.exe
```

To compile the tester application run the following :
```bash
cl /EHsc /std:c++14 package.cpp offer.cpp delivery_logic.cpp tester.cpp -o time_estimation_tester.exe
```

Note : Since problem 2 is the logical continuation of problem 1, all ideas with regards to cost computation stays intact.

#### Key Highlights
- offers are loaded at runtime from json file containing the offers. The file is loaded from the "***Current Working Directory***". Below is the file format :
  ```
  [
    {
        "code" : string,
        "discount_perc" : integer,
        "min_distance" : integer,
        "max_distance" : integer,
        "min_weight" : integer,
        "max_weight" : integer
    },
    .
    .
    .
  ]
  ```
  **NOTE** : This ingestion of offers is performed by `std::unordered_map<std::string, Offer> IngestOffers(std::string filePath, bool useFileLogging, std::ostream &out)`. Any errors/warning are appended to the "Logs_DDMMYYYYHHMMSS.txt" which is created at the start of the application, if the `useFileLogging` is set to `true` (by default the value for `useFileLogging` is set to `true`). If the flag is set to `false` and no `ostream` object is provided, the errors will be logged to the console.

- Any `Offers` that fails the validation is rejected and is not available during the entire duration that the application is alive. Below are the verifications that are performed to ascertain the correctness of an Offer
  - offer code is not empty.
  - %age discount is a non-negative and less than 100.
  - min_distance, max_distance, min_weight, max_weight is non-negative integer.
  - min_distance <= max_distance and min_weight <= max_weight.
  - No data-points are missing.

  **NOTE** : This validation is performed by `bool Offer::IsValid()`

- The application starts by calling `SetUpDelivery(std::string filePath, bool useFileLogging, std::ostream &out)`. The function takes a `filePath` of the `Offers.json`, by default it tries loading from the "***Current Working Directory***" where the executable is executed from. If the loading/parsing of the offers file fail, a warning is logged into the log file. It also takes a flag `useFileLogging`(to indicate if the any errors while loading the offer needs to be logged to a file or to `ostream` object passed as the third argument : *by default it logs any failures to a file*).

- The business logic is encapsulated inside `Delivery::ExecuteWorkflow(std::istream &is = std::cin, std::ostream &os = std::cout)`. The function is responsible for taking input and printing output besides ***delivery cost*** and ***delivery time*** computation. The function takes an input stream from which the input is read and an output stream to which the output is sent to. By default the input and output is streamed from and to console respectively.

- There is also function to `Delivery::ReloadOffers`, however there is not much use in the current form within the application.

- The delivery cost for a package is performed by `Package::CalculateCost(long long base_delivery_cost, Offer of)` that takes in `base_delivery_cost` and the `Offer` to apply on it. This way the package deliver cost can be recalculated with a different `base_delivery_cost`.

- The package class also exposes the ability to reset the weight and distance multiplier besides the default value of 10 and 5 respectively set in the `Package` constructor.

- The ***delivery time*** computation is encapsulated within `Delivery::Delivery_Time`. Two ideas are encapsulated to arriving at the solution for delivery time which are as follows:
  - Selecting X number of packages for delivery based on maximum weight. In case of multiple selections with same weight using the maximum number of packages to break the tie. This idea is encapsulated inside `Delivery::kp`.
  - Using min-heap to select the delivery agent which is available at the earliest. This is performed within `Delivery::Delivery_Time` after selection is returned by `Delivery::kp`.

- The package selection logic is captured inside `Delivery::kp`. In its current form the selection is recursion based with memoization. 

#### Limitations

1. Currently only weight multiplier(`Package::wt_multiplier`), distance multiplier(`Package::dist_multiplier`), and base_delivery_cost(`Package::base_delivery_cost`) are marked as `long long`.<br>
`Package::cost` and `Package::discount` are marked as `double`.<br>
`Package::delivery_time` is marked as float which can store values upto 2<sup>18</sup> without loss of precision.<br>
All others are marked as int. Below are the variables marked as intger:
  - all weight related vaiables : **Anything above 2<sup>31</sup> and we are looking to transport planetary objects**.
  - all distance related vaiables : **Anything above 20,375 km (circumference of earth / 2) and we are looking outer-space travel**.
  - number of available agents : **The population of earth is currently at approximately 2<sup>33</sup>**.
  - speed of light : **The speed of light is approximately 2<sup>30.01</sup>, beyond that we are talking about parallel universe or in the science fiction realm**.

2. Because **C++** by default rounds up any float\double value, therefore overflow cam happen during delivery cost computation..

3. Because **C++** by default rounds up any float value, therefore during delivery time calculation, the distance is multiplied by 100 to avoid any rounding errors.

4. In modular folder there is also a source file named `file_main.cpp` using which the code can be executed to utilize files for input and output, currently its a POC, haven't yet tested it, but given that `ExecuteWorkflow` takes input and output stream the idea does not seem far fetch.
