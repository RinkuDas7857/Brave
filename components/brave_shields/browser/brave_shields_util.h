/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_BRAVE_SHIELDS_UTIL_H_
#define BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_BRAVE_SHIELDS_UTIL_H_

#include <stdint.h>
#include <string>

#include "components/content_settings/core/common/content_settings_pattern.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "services/network/public/mojom/referrer_policy.mojom.h"

#include "brave/components/brave_shields/common/block_decision.h"

namespace content {
struct Referrer;
}

class GURL;
class HostContentSettingsMap;
class PrefService;

namespace brave_shields {

class ReferrerWhitelistService;

enum ControlType { ALLOW = 0, BLOCK, BLOCK_THIRD_PARTY, DEFAULT, INVALID };

ContentSettingsPattern GetPatternFromURL(const GURL& url);
std::string ControlTypeToString(ControlType type);
ControlType ControlTypeFromString(const std::string& string);

void SetBraveShieldsEnabled(HostContentSettingsMap* map,
                            bool enable,
                            const GURL& url,
                            PrefService* local_state = nullptr);
// reset to the default value
void ResetBraveShieldsEnabled(HostContentSettingsMap* map,
                              const GURL& url);
bool GetBraveShieldsEnabled(HostContentSettingsMap* map, const GURL& url);

void SetAdControlType(HostContentSettingsMap* map,
                      ControlType type,
                      const GURL& url,
                      PrefService* local_state = nullptr);
ControlType GetAdControlType(HostContentSettingsMap* map, const GURL& url);

void SetCosmeticFilteringControlType(HostContentSettingsMap* map,
                                     ControlType type,
                                     const GURL& url,
                                     PrefService* local_state = nullptr);
ControlType GetCosmeticFilteringControlType(HostContentSettingsMap* map,
                                            const GURL& url);
bool ShouldDoCosmeticFiltering(HostContentSettingsMap* map, const GURL& url);
bool IsFirstPartyCosmeticFilteringEnabled(HostContentSettingsMap* map,
                                          const GURL& url);

void SetCookieControlType(HostContentSettingsMap* map,
                          ControlType type,
                          const GURL& url,
                          PrefService* local_state = nullptr);
ControlType GetCookieControlType(HostContentSettingsMap* map, const GURL& url);

// Referrers is always set along with cookies so there is no setter and
// these is just included for backwards compat.
bool AllowReferrers(HostContentSettingsMap* map, const GURL& url);

void SetFingerprintingControlType(HostContentSettingsMap* map,
                                  ControlType type,
                                  const GURL& url,
                                  PrefService* local_state = nullptr);
ControlType GetFingerprintingControlType(HostContentSettingsMap* map,
                                         const GURL& url);

void SetHTTPSEverywhereEnabled(HostContentSettingsMap* map,
                               bool enable,
                               const GURL& url,
                               PrefService* local_state = nullptr);
// reset to the default value
void ResetHTTPSEverywhereEnabled(HostContentSettingsMap* map,
                                 const GURL& url);
bool GetHTTPSEverywhereEnabled(HostContentSettingsMap* map, const GURL& url);

void SetNoScriptControlType(HostContentSettingsMap* map,
                            ControlType type,
                            const GURL& url,
                            PrefService* local_state = nullptr);
ControlType GetNoScriptControlType(HostContentSettingsMap* map,
                                   const GURL& url);

void DispatchBlockedEventFromIO(const GURL& request_url,
                                int render_frame_id,
                                int render_process_id,
                                int frame_tree_node_id,
                                const BlockDecision* block_decision);

void DispatchBlockedEvent(const GURL& request_url,
                          int render_frame_id,
                          int render_process_id,
                          int frame_tree_node_id,
                          const BlockDecision* block_decision);

bool MaybeChangeReferrer(
    bool allow_referrers,
    bool shields_up,
    const GURL& current_referrer,
    const GURL& tab_origin,
    const GURL& target_url,
    network::mojom::ReferrerPolicy policy,
    brave_shields::ReferrerWhitelistService* referrer_whitelist_service,
    content::Referrer* output_referrer);


}  // namespace brave_shields

#endif  // BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_BRAVE_SHIELDS_UTIL_H_
