// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_WEBUI_REALBOX_REALBOX_HANDLER_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_WEBUI_REALBOX_REALBOX_HANDLER_H_

#define edit_model      \
  edit_model_unused() { \
    return nullptr;     \
  }                     \
                        \
 public:                \
  OmniboxEditModel* edit_model

#include "src/chrome/browser/ui/webui/realbox/realbox_handler.h"

#undef edit_model

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_WEBUI_REALBOX_REALBOX_HANDLER_H_
