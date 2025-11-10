using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Delivery_Time
{
    internal class Package(string id, int weight, int distance, long wt_multiplier = 10, long dist_multiplier = 5)
    {
        private readonly string _id = id;
        private long _wt_multiplier = wt_multiplier;
        private long _dist_multiplier = dist_multiplier;
        private int _weight = weight;
        private int _distance = distance;
        private double _discount = 0.0;
        private double _cost = 0.0;

        public int Weight
        {
            get { return _weight; }
            set
            {
                if (_weight < 0)
                {
                    _weight = 0;
                }
                else
                {
                    _weight = value;
                }
            }
        }

        public int Distance
        {
            get { return _distance; }
            set
            {
                if (_distance < 0)
                {
                    _distance = 0;
                }
                else
                {
                    _distance = value;
                }
            }
        }

        public float DeliveryTime { get; set; }

        public void CalculateCost(long base_delivery_cost, Offer? of)
        {
            _cost = base_delivery_cost + (_wt_multiplier * Weight) + (_dist_multiplier * Distance);

            if (of != null)
            {
                if (Distance >= of.MinDistance && Distance < of.MaxDistance)
                {
                    if (Weight >= of.MinWeight && Weight < of.MaxWeight)
                    {
                        _discount = (_cost * of.DiscountPerc) / (float)100;
                    }
                }
                _cost = (_cost - _discount);
            }
        }

        public void ResetWeightMultipliers(long multiplier) => _wt_multiplier = multiplier;

        public void ResetDistanceMultipliers(long multiplier) => _dist_multiplier = multiplier;


        public override string ToString()
        {
            return $"{_id} {_discount:F2} {_cost:F2} {DeliveryTime:F2}";
        }
    }
}
