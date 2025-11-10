using Delivery_Time;
using DeliveryTime;
using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace Delivery_Time
{
    internal static class Delivery
    {
        class compositeValue : IEquatable<compositeValue>
        {
            int count;

            public compositeValue(int wt, int ct, int itemIdx)
            {
                this.Weight = wt;
                this.count = ct;
                Bag = [];
                Bag.Add(itemIdx);
            }

            public compositeValue()
            {
                this.count = 0;
                Weight = 0;
                Bag = [];
            }

            public compositeValue(compositeValue other)
            {
                Weight = other.Weight;
                count = other.count;
                Bag = other.Bag;
                other.Bag.Clear();
            }

            public List<int> Bag
            {
                get;
                private set;
            }

            public int Weight { get; private set; }

            public void Reset()
            {
                Weight = 0;
                count = 0;
                Bag.Clear();
            }

            public static bool operator >(compositeValue left, compositeValue right)
            {
                if (left.count > right.count)
                    return true;
                else if (left.count == right.count && left.Weight > right.Weight)
                    return true;
                else
                    return false;
            }

            public static bool operator <(compositeValue left, compositeValue right)
            {
                if (left.count < right.count)
                    return true;
                else if (left.count == right.count && left.Weight < right.Weight)
                    return true;
                else
                    return false;
            }

            public static compositeValue operator +(compositeValue left, compositeValue right)
            {
                compositeValue nv = new()
                {
                    Weight = left.Weight + right.Weight,
                    count = left.count + right.count
                };

                foreach (var i in right.Bag)
                {
                    nv.Bag.Add(i);
                }
                foreach (var i in left.Bag)
                {
                    nv.Bag.Add(i);
                }
                return nv;
            }

            public bool Equals(compositeValue? other)
            {
                return count == other?.count && Weight == other?.Weight;
            }
        }

        private static readonly Dictionary<string, Offer> _offers = [];

        static Func<int, PriorityQueue<int, int>> get_agent_queue = no_of_vehicles =>
        {
            PriorityQueue<int, int> agent_queue = new();

            for (int i = 0; i < no_of_vehicles; i++)
            {
                agent_queue.Enqueue(0, 0);
            }
            return agent_queue;
        };

        static compositeValue[] get_pre_computed_composite_objects(List<Package> packages)
        {
            compositeValue[] compositeObjects = new compositeValue[packages.Count];

            for (int i = 0; i < packages.Count; i++)
            {
                compositeObjects[i] = new compositeValue(packages[i].Weight, 1, i);
            }
            return compositeObjects;
        }

        static compositeValue KP(compositeValue[] compositePackages, int max_carriable_weight, bool[] availability, compositeValue[] availableComputations)
        {

            for (int i = 1; i <= compositePackages.Length; i++)
            {
                if (availability[i - 1])
                {
                    for (int j = max_carriable_weight; j >= compositePackages[i - 1].Weight; j--)
                    {
                        compositeValue np;
                        np = availableComputations[j - compositePackages[i - 1].Weight] + compositePackages[i - 1];
                        if (np > availableComputations[j])
                        {
                            availableComputations[j] = np;
                        }
                    }
                }
            }

            return availableComputations[max_carriable_weight];
        }

        public static async void SetUpDelivery(TextWriter writer, string filePath = "json_files\\offers.json")
        {
            var json = await File.ReadAllTextAsync(filePath);

            var options = new JsonSerializerOptions
            {
                PropertyNameCaseInsensitive = true,
                AllowTrailingCommas = true,
                ReadCommentHandling = JsonCommentHandling.Skip,
                NumberHandling = JsonNumberHandling.AllowReadingFromString // tolerate numbers as strings
            };

            var (offers, warnings) = await Helpers.LoadOffersSafelyAsync(json, options);

            foreach (var offer in offers)
            {
                _offers[offer.Code] = offer;
            }

            writer.WriteLine($"Loaded {offers.Count} valid offer(s).");
            foreach (var o in offers)
                writer.WriteLine(" • " + o);

            if (warnings.Count > 0)
            {
                writer.WriteLine("\nWarnings:");
                foreach (var w in warnings)
                    writer.WriteLine(" - " + w);
            }

        }

        public static void TearDownDelivery()
        {

        }

        public static void ExecuteWorkflow(TextReader reader, TextWriter writer)
        {
            long base_delivery_cost = 0;
            int no_of_packages = 0;
            string offer_id = "", pkg_id;

            string? input = reader.ReadLine();
            if (input != null)
            {
                if (!long.TryParse(input.Split(' ')[0], out base_delivery_cost))
                {
                    writer.WriteLine("Unable to parse the base_delivery_cost");
                }

                if (!int.TryParse(input.Split(' ')[1], out no_of_packages))
                {
                    writer.WriteLine("Unable to parse the no_of_packages");
                }
            }


            List<Package> packages = [];
            bool isValidPackage = true;


            for (uint i = 0; i < no_of_packages; i++)
            {
                isValidPackage = true;
                var pd = reader.ReadLine()?.Split(' ');

                if (pd != null)
                {
                    pkg_id = pd[0];

                    if (!int.TryParse(pd[1], out int pkg_weight_in_kg))
                    {
                        writer.WriteLine("Unable to parse the Weight of the package!! Please Enter Natural Numbers");
                    }
                    else if (pkg_weight_in_kg < 1)
                    {
                        writer.WriteLine("Invalid Package weight!! Weight cannot be less than 1 kg");
                        isValidPackage = false;
                    }

                    if (!int.TryParse(pd[2], out int pkg_distance_in_km))
                    {
                        writer.WriteLine("Unable to parse the no_of_packages");
                    }
                    else if (pkg_distance_in_km < 1)
                    {
                        writer.WriteLine("Invalid Package Delivery Distance!! Distance cannot be less than 1 km");
                        isValidPackage = false;
                    }

                    offer_id = pd[3];

                    if (isValidPackage)
                    {
                        Package pkg = new(pkg_id, pkg_weight_in_kg, pkg_distance_in_km);
                        if (_offers.TryGetValue(offer_id, out var offer))
                        {
                            pkg.CalculateCost(base_delivery_cost, offer);
                        }
                        else
                        {
                            pkg.CalculateCost(base_delivery_cost, null);
                        }

                        packages.Add(pkg);
                    }
                    else
                    {
                        writer.WriteLine($"Package {pkg_id} will not participate in the computation");
                    }
                }
            }

            int no_of_vehicles = 0, max_speed = 0, max_carriable_weight = 0;
            bool canDeliveryComputationBePerformed = true;

            var dt = reader.ReadLine()?.Split(' ');

            if (dt != null && dt.Length > 0)
            {
                if (!int.TryParse(dt[0], out no_of_vehicles))
                {
                    writer.WriteLine("Unable to parse the number of available vehicles!! Please Enter Natural Numbers");
                }
                else if (no_of_vehicles < 1)
                {
                    writer.WriteLine("Invalid number of available vehicles!! Value cannot be less than 1.");
                    canDeliveryComputationBePerformed = false;
                }

                if (!int.TryParse(dt[1], out max_speed))
                {
                    writer.WriteLine("Unable to parse the Max Speed!! Please Enter Natural Numbers");
                }
                else if (max_speed < 1)
                {
                    canDeliveryComputationBePerformed = false;
                }
                if (!int.TryParse(dt[2], out max_carriable_weight))
                {
                    writer.WriteLine("Unable to parse the Max Carriable Weight!! Please Enter Natural Numbers");
                }
                else if (max_carriable_weight < 1)
                {
                    canDeliveryComputationBePerformed = false;
                }
            }

            if (canDeliveryComputationBePerformed)
            {
                Delivery_Time(packages, no_of_vehicles, max_speed, max_carriable_weight);
            }

            foreach (var pkg in packages)
            {
                writer.WriteLine(pkg);
            }
        }

        public static void ReloadOffers(string filePath)
        {

        }

        public static void Delivery_Time(List<Package> packages, int no_of_vehicles, int max_speed, int max_carriable_weight)
        {
            int no_of_packages = packages.Count;
            compositeValue[] availableComputations = new compositeValue[max_carriable_weight + 1];
            bool[] availability = new bool[no_of_packages];

            for (int i = 0; i < no_of_packages; i++)
            {
                availability[i] = true;
            }

            var agent_queue = get_agent_queue(no_of_vehicles);
            compositeValue[] compositeObjects = get_pre_computed_composite_objects(packages);
            for (int i = 0; i < max_carriable_weight + 1; i++)
            {
                availableComputations[i] = new();
            }

            while (no_of_packages > 0)
            {
                KP(compositeObjects, max_carriable_weight, availability, availableComputations);

                var best = availableComputations[max_carriable_weight];

                best.Bag.Sort((i, j) => packages[i].Distance.CompareTo(packages[j].Distance));

                var available_agent = agent_queue.Peek();

                int max_agent_busy_time = 0;

                foreach (var idx in best.Bag)
                {
                    int pkgDeliveryTime = (int)(available_agent + ((float)(((long)packages[idx].Distance) * 100) / max_speed));
                    availability[idx] = false;
                    if (pkgDeliveryTime > max_agent_busy_time)
                    {
                        max_agent_busy_time = pkgDeliveryTime;
                    }
                    Console.WriteLine($"{max_agent_busy_time}");
                    packages[idx].DeliveryTime = (float)(max_agent_busy_time) / 100;
                }

                agent_queue.Dequeue();
                agent_queue.Enqueue(max_agent_busy_time * 2, max_agent_busy_time * 2);

                no_of_packages -= best.Bag.Count;

                foreach (var ac in availableComputations)
                {
                    ac.Reset();
                }
            }
        }

    }
}
