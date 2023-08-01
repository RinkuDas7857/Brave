/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include <vector>

#include "base/memory/scoped_refptr.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_util.h"
#include "brave/components/brave_search_conversion/utils.h"
#include "brave/components/commander/common/constants.h"
#include "brave/components/commander/common/features.h"
#include "brave/components/omnibox/browser/brave_bookmark_provider.h"
#include "brave/components/omnibox/browser/brave_history_quick_provider.h"
#include "brave/components/omnibox/browser/brave_history_url_provider.h"
#include "brave/components/omnibox/browser/brave_local_history_zero_suggest_provider.h"
#include "brave/components/omnibox/browser/brave_search_provider.h"
#include "brave/components/omnibox/browser/brave_shortcuts_provider.h"
#include "brave/components/omnibox/browser/promotion_provider.h"
#include "brave/components/omnibox/browser/promotion_utils.h"
#include "brave/components/omnibox/browser/topsites_provider.h"
#include "components/omnibox/browser/autocomplete_controller.h"
#include "components/omnibox/browser/autocomplete_input.h"
#include "components/omnibox/browser/autocomplete_match.h"
#include "components/omnibox/browser/clipboard_provider.h"
#include "components/omnibox/browser/history_cluster_provider.h"
#include "components/omnibox/browser/history_fuzzy_provider.h"

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)
#include "brave/components/omnibox/browser/commander_provider.h"
#endif

using brave_search_conversion::IsBraveSearchConversionFeatureEnabled;

namespace {
// If this input has triggered the commander then only show commander results.
void MaybeShowCommands(AutocompleteResult* result,
                       const AutocompleteInput& input) {
#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)
  // If this input isn't a command, return and don't do any work.
  if (!commander::CommanderEnabled() ||
      !base::StartsWith(input.text(), commander::kCommandPrefix)) {
    return;
  }

  uint32_t seen_commands = 0;

  // Move all the commands to the top of the Omnibox suggestions.
  for (uint32_t i = 0; i < result->size(); ++i) {
    auto* match = result->match_at(i);
    if (match->provider->type() != AutocompleteProvider::TYPE_BRAVE_COMMANDER) {
      continue;
    }

    result->ReorderMatch(result->begin() + i, seen_commands++);
  }

  // Remove all results after the commands.
  for (auto it = result->end() - 1; it >= result->begin() + seen_commands;
       --it) {
    result->RemoveMatch(it);
  }
#endif
}

void MaybeAddCommanderProvider(AutocompleteController::Providers& providers,
                               AutocompleteController* controller) {
#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)
  if (commander::CommanderEnabled()) {
    providers.push_back(base::MakeRefCounted<commander::CommanderProvider>(
        controller->autocomplete_provider_client(), controller));
  }
#endif
}
}  // namespace

#define SearchProvider BraveSearchProvider
#define HistoryQuickProvider BraveHistoryQuickProvider
#define HistoryURLProvider BraveHistoryURLProvider
#define LocalHistoryZeroSuggestProvider BraveLocalHistoryZeroSuggestProvider
#define BookmarkProvider BraveBookmarkProvider
#define ShortcutsProvider BraveShortcutsProvider
#define BRAVE_AUTOCOMPLETE_CONTROLLER_AUTOCOMPLETE_CONTROLLER         \
  MaybeAddCommanderProvider(providers_, this);                        \
  providers_.push_back(new TopSitesProvider(provider_client_.get())); \
  if (IsBraveSearchConversionFeatureEnabled() &&                      \
      !provider_client_->IsOffTheRecord())                            \
    providers_.push_back(new PromotionProvider(provider_client_.get()));

// This sort should be done right after AutocompleteResult::SortAndCull() in
// the AutocompleteController::SortCullAndAnnotateResult() to make our sorting
// run last but before notifying.
#define BRAVE_AUTOCOMPLETE_CONTROLLER_UPDATE_RESULT \
  SortBraveSearchPromotionMatch(&result_);          \
  MaybeShowCommands(&result_, input_);

#include "src/components/omnibox/browser/autocomplete_controller.cc"

#undef BRAVE_AUTOCOMPLETE_CONTROLLER_UPDATE_RESULT
#undef BRAVE_AUTOCOMPLETE_CONTROLLER_AUTOCOMPLETE_CONTROLLER
#undef ShortcutsProvider
#undef BookmarkProvider
#undef LocalHistoryZeroSuggestProvider
#undef HistoryURLProvider
#undef HistoryQuickProvider
#undef SearchProvider