using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace Delivery_Time
{
    public sealed record Offer
    {
        [JsonPropertyName("code")]
        [Required]
        public required string Code { get; init; }

        [JsonPropertyName("discount_perc")]
        [Range(0, 100, ErrorMessage = "discount_perc must be between 0 and 100.")]
        public int DiscountPerc { get; init; }

        [JsonPropertyName("min_distance")]
        [Range(0, int.MaxValue, ErrorMessage = "min_distance must be >= 0.")]
        public int MinDistance { get; init; }

        [JsonPropertyName("max_distance")]
        [Range(0, int.MaxValue, ErrorMessage = "max_distance must be >= 0.")]
        public int MaxDistance { get; init; }

        [JsonPropertyName("min_weight")]
        [Range(0, int.MaxValue, ErrorMessage = "min_weight must be >= 0.")]
        public int MinWeight { get; init; }

        [JsonPropertyName("max_weight")]
        [Range(0, int.MaxValue, ErrorMessage = "max_weight must be >= 0.")]
        public int MaxWeight { get; init; }

        public override string ToString()
            => $"{Code}: {DiscountPerc}% | distance {MinDistance}-{MaxDistance} | weight {MinWeight}-{MaxWeight}";
    }
}
