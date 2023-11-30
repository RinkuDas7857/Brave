/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_player/brave_player_service_delegate_impl.h"

BravePlayerServiceDelegateImpl::BravePlayerServiceDelegateImpl() = default;

BravePlayerServiceDelegateImpl::~BravePlayerServiceDelegateImpl() = default;

#if !defined(TOOLKIT_VIEWS)
void BravePlayerServiceDelegateImpl::ShowAdBlockAdjustmentSuggestion(
    content::WebContents* contents) {
  NOTIMPLEMENTED();
}
#endif
