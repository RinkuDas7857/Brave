// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_AI_CHAT_CONTENT_BROWSER_PAGE_CONTENT_FETCHER_H_
#define BRAVE_COMPONENTS_AI_CHAT_CONTENT_BROWSER_PAGE_CONTENT_FETCHER_H_

#include <cstdint>
#include <string>

#include "base/functional/callback_forward.h"
#include "base/memory/scoped_refptr.h"
#include "include/core/SkBitmap.h"

namespace content {
class WebContents;
}  // namespace content

namespace network {
class SharedURLLoaderFactory;
}  // namespace network

class SkBitmap;
namespace ai_chat {

using FetchPageContentCallback =
    base::OnceCallback<void(std::string page_content,
                            bool is_video,
                            std::string invalidation_token)>;
void FetchPageContent(content::WebContents* web_contents,
                      std::string_view invalidation_token,
                      const std::optional<std::vector<SkBitmap>>& images,
                      uint32_t max_page_content_length,
                      FetchPageContentCallback callback,
                      scoped_refptr<network::SharedURLLoaderFactory>
                          url_loader_factory = nullptr);

}  // namespace ai_chat

#endif  // BRAVE_COMPONENTS_AI_CHAT_CONTENT_BROWSER_PAGE_CONTENT_FETCHER_H_
