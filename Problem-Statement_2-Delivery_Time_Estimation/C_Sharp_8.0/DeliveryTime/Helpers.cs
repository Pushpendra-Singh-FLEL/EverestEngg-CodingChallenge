using Delivery_Time;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace DeliveryTime
{
    internal static class Helpers
    {


        public static async Task<(List<Offer> offers, List<string> warnings)>
            LoadOffersSafelyAsync(string json, JsonSerializerOptions options)
        {
            var warnings = new List<string>();
            var results = new List<Offer>();

            using var doc = JsonDocument.Parse(json, new JsonDocumentOptions
            {
                AllowTrailingCommas = true,
                CommentHandling = JsonCommentHandling.Skip
            });

            if (doc.RootElement.ValueKind != JsonValueKind.Array)
            {
                warnings.Add($"Root must be a JSON array (found: {doc.RootElement.ValueKind}).");
                return (results, warnings);
            }

            int index = 0;
            foreach (var elem in doc.RootElement.EnumerateArray())
            {
                index++;
                try
                {
                    var offer = elem.Deserialize<Offer>(options);
                    if (offer is null)
                    {
                        warnings.Add($"[{index}] Could not deserialize offer (null).");
                        continue;
                    }

                    // DataAnnotations + cross-field rules
                    if (!Validate(offer, out var err))
                    {
                        warnings.Add($"[{index}] Invalid offer: {err}");
                        continue;
                    }

                    results.Add(offer);
                }
                catch (JsonException jx)
                {
                    warnings.Add($"[{index}] JSON error at path '{jx.Path}': {jx.Message}");
                }
                catch (Exception ex)
                {
                    warnings.Add($"[{index}] Unexpected error: {ex.Message}");
                }
            }

            return (results, warnings);
        }

        private static bool Validate(Offer offer, [NotNullWhen(false)] out string? error)
        {
            // 1) Attribute-based checks
            var ctx = new ValidationContext(offer);
            var results = new List<ValidationResult>();
            var ok = Validator.TryValidateObject(offer, ctx, results, validateAllProperties: true);
            if (!ok)
            {
                error = string.Join("; ", results.Select(r => r.ErrorMessage));
                return false;
            }

            // 2) Cross-field logic
            if (offer.MinDistance > offer.MaxDistance)
                return Fail("min_distance cannot be greater than max_distance.", out error);

            if (offer.MinWeight > offer.MaxWeight)
                return Fail("min_weight cannot be greater than max_weight.", out error);

            error = null;
            return true;

            static bool Fail(string e, out string? err) { err = e; return false; }
        }
    }
}
