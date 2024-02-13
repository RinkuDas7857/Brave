/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_BRAVE_PAGE_GRAPH_BLINK_PROBE_TYPES_H_
#define BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_BRAVE_PAGE_GRAPH_BLINK_PROBE_TYPES_H_

#include <string>
#include <utility>

#include "base/values.h"
#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

using PageGraphBlinkArg = base::Value;
using PageGraphBlinkArgs = base::Value::List;
using PageGraphBlinkReceiverData = base::Value::Dict;

enum class PageGraphBindingType {
  kAttribute,
  kConstant,
  kConstructor,
  kMethod,
};

enum class PageGraphBindingEvent {
  kAttributeGet,
  kAttributeSet,
  kConstantGet,
  kConstructorCall,
  kMethodCall,
};

template <typename... Args>
base::Value::List CreatePageGraphBlinkArgs(Args&&... args) {
  base::Value::List list;
  list.reserve(sizeof...(Args));
  (list.Append(std::forward<Args>(args)), ...);
  return list;
}

CORE_EXPORT std::string PageGraphBaseValueToString(base::ValueView args);

}  // namespace blink

#endif  // BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_BRAVE_PAGE_GRAPH_BLINK_PROBE_TYPES_H_
