/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/resources/behavioral/conversions/conversions_info.h"

#include <utility>

#include "base/values.h"
#include "brave/components/brave_ads/core/internal/conversions/conversions_feature.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace brave_ads {

ConversionsInfo::ConversionsInfo() = default;

ConversionsInfo::ConversionsInfo(ConversionsInfo&& other) noexcept = default;

ConversionsInfo& ConversionsInfo::operator=(ConversionsInfo&& other) noexcept =
    default;

ConversionsInfo::~ConversionsInfo() = default;

// static
base::expected<ConversionsInfo, std::string> ConversionsInfo::CreateFromValue(
    const base::Value::Dict dict) {
  ConversionsInfo conversions;

  if (absl::optional<int> version = dict.FindInt("version")) {
    if (kConversionsResourceVersion.Get() != *version) {
      return base::unexpected("Failed to load from JSON, version mismatch");
    }
    conversions.version = *version;
  }

  const auto* conversion_id_patterns_dict =
      dict.FindDict("conversion_id_patterns");
  if (!conversion_id_patterns_dict) {
    return base::unexpected(
        "Failed to load from JSON, conversion id patterns missing");
  }

  for (const auto [url_pattern, conversion_id_pattern] :
       *conversion_id_patterns_dict) {
    const base::Value::Dict* conversion_id_pattern_dict =
        conversion_id_pattern.GetIfDict();

    if (!conversion_id_pattern_dict) {
      return base::unexpected(
          "Failed to load from JSON, conversion id pattern not of type dict");
    }

    const std::string* const id_pattern =
        conversion_id_pattern_dict->FindString("id_pattern");
    if (!id_pattern || id_pattern->empty()) {
      return base::unexpected(
          "Failed to load from JSON, pattern id_pattern missing");
    }

    const std::string* const search_in =
        conversion_id_pattern_dict->FindString("search_in");
    if (!search_in || search_in->empty()) {
      return base::unexpected(
          "Failed to load from JSON, pattern search_in missing");
    }

    conversions.id_patterns[url_pattern] = {*id_pattern, url_pattern,
                                            *search_in};
  }

  return conversions;
}

}  // namespace brave_ads