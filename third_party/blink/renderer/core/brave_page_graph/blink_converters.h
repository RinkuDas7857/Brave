/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_BRAVE_PAGE_GRAPH_BLINK_CONVERTERS_H_
#define BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_BRAVE_PAGE_GRAPH_BLINK_CONVERTERS_H_

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "base/auto_reset.h"
#include "base/containers/span.h"
#include "brave/third_party/blink/renderer/core/brave_page_graph/blink_probe_types.h"
#include "third_party/blink/renderer/bindings/core/v8/to_v8_traits.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/typed_arrays/typed_flexible_array_buffer_view.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class EventListener;
class FlexibleArrayBufferView;
class NativeValueTraitsAnyAdapter;
class ScriptPromise;
class ScriptValue;

namespace pg_internal {

template <typename T>
using strip_type_qualifiers_t =
    std::remove_pointer_t<std::remove_cv_t<std::remove_reference_t<T>>>;

template <typename T>
concept has_tov8 =
    std::is_same_v<decltype(ToV8Traits<strip_type_qualifiers_t<T>>::ToV8(
                       std::declval<ScriptState*>(),
                       std::declval<T&>())),
                   v8::Local<v8::Value>>;

template <typename T>
concept is_iterable = requires(T t) {
  { std::begin(t) } -> std::same_as<decltype(std::begin(t))>;
  { std::end(t) } -> std::same_as<decltype(std::end(t))>;
};

template <typename T>
concept convert_as_base_value =
    requires(T a) {
      { base::Value(a) } -> std::same_as<base::Value>;
    } || std::is_same_v<T, uint32_t> || std::is_same_v<T, int64_t> ||
    std::is_same_v<T, uint64_t>;

template <typename T>
concept convert_as_string =
    !convert_as_base_value<T> && std::is_convertible_v<T, String>;

template <typename T>
concept convert_via_tov8 =
    !convert_as_base_value<T> && !convert_as_string<T> && has_tov8<T>;

template <typename T>
concept convert_via_tov8_ref =
    !convert_as_base_value<T> && !convert_as_string<T> && !has_tov8<T> &&
    has_tov8<std::add_pointer_t<T>>;

template <typename T>
concept convert_as_iterable =
    !convert_as_base_value<T> && !convert_as_string<T> && !has_tov8<T> &&
    !convert_via_tov8_ref<T> && is_iterable<T>;

template <typename T>
concept convert_as_enum_value =
    !convert_as_base_value<T> && !convert_as_string<T> && !has_tov8<T> &&
    !convert_via_tov8_ref<T> && !is_iterable<T> && std::is_enum_v<T>;

template <typename T>
concept convert_unsupported =
    !convert_as_base_value<T> && !convert_as_string<T> && !has_tov8<T> &&
    !convert_via_tov8_ref<T> && !is_iterable<T> && !std::is_enum_v<T>;

}  // namespace pg_internal

// Override to convert WebAPI call argument to base::Value value.
template <typename T>
  requires(pg_internal::convert_unsupported<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, const T& value);

template <typename T>
  requires(pg_internal::convert_as_base_value<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, const T& value) {
  if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, int64_t> ||
                std::is_same_v<T, uint64_t>) {
    return base::Value(static_cast<double>(value));
  } else {
    return base::Value(value);
  }
}

template <typename T>
  requires(pg_internal::convert_as_string<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, const T& value) {
  return base::Value(String(value).Utf8());
}

// Override to convert WebAPI call argument to V8 value and to String value.
template <typename T>
  requires(pg_internal::convert_via_tov8<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, T& value) {
  if constexpr (std::is_pointer_v<T>) {
    if (!value) {
      return base::Value();
    }
  }
  ScriptState::Scope scope(script_state);
  v8::Local<v8::Value> v8_value =
      ToV8Traits<pg_internal::strip_type_qualifiers_t<T>>::ToV8(script_state,
                                                                value);
  return ToPageGraphBlinkArg(script_state, v8_value);
}

template <typename T>
  requires(pg_internal::convert_via_tov8_ref<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, T& value) {
  if constexpr (std::is_pointer_v<T>) {
    if (!value) {
      return base::Value();
    }
  }
  ScriptState::Scope scope(script_state);
  v8::Local<v8::Value> v8_value =
      ToV8Traits<pg_internal::strip_type_qualifiers_t<T>>::ToV8(script_state,
                                                                &value);
  return ToPageGraphBlinkArg(script_state, v8_value);
}

template <typename T>
  requires(pg_internal::convert_as_iterable<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, const T& values) {
  base::Value::List list_values;
  for (const auto& value : values) {
    list_values.Append(ToPageGraphBlinkArg(script_state, value));
  }
  return base::Value(std::move(list_values));
}

template <typename... Ts, std::size_t... Is>
void ToPageGraphBlinkArgImpl(ScriptState* script_state,
                             base::Value::List& list_values,
                             const std::tuple<Ts...>& values,
                             std::index_sequence<Is...>) {
  (list_values.Append(ToPageGraphBlinkArg(script_state, std::get<Is>(values))),
   ...);
}

template <typename... Ts>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const std::tuple<Ts...>& values) {
  base::Value::List list_values;
  ToPageGraphBlinkArgImpl(script_state, list_values, values,
                          std::index_sequence_for<Ts...>{});
  return base::Value(std::move(list_values));
}

template <typename T>
  requires(pg_internal::convert_as_enum_value<T>)
base::Value ToPageGraphBlinkArg(ScriptState* script_state, const T& value) {
  return base::Value(static_cast<std::underlying_type_t<T>>(value));
}

template <typename T1, typename T2>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const std::pair<T1, T2>& values) {
  return ToPageGraphBlinkArg(script_state,
                             std::tie(values.first, values.second));
}

template <typename T>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const TypedFlexibleArrayBufferView<T>& array) {
  if (array.IsNull()) {
    return ToPageGraphBlinkArg(script_state, base::span<T>());
  }
  const base::span<const T> data_view(array.DataMaybeOnStack(), array.length());
  return ToPageGraphBlinkArg(script_state, data_view);
}

template <typename T>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const std::optional<T>& opt_value) {
  if (!opt_value) {
    return base::Value();
  }
  return ToPageGraphBlinkArg(script_state, opt_value.value());
}

template <>
CORE_EXPORT base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                            const v8::Local<v8::Value>& arg);

template <>
CORE_EXPORT base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                            const ScriptValue& script_value);

template <>
CORE_EXPORT base::Value
ToPageGraphBlinkArg<bindings::NativeValueTraitsAnyAdapter>(
    ScriptState* script_state,
    const bindings::NativeValueTraitsAnyAdapter& adapter);

template <>
CORE_EXPORT base::Value ToPageGraphBlinkArg<ScriptPromise>(
    ScriptState* script_state,
    const ScriptPromise& script_promise);

template <>
CORE_EXPORT base::Value ToPageGraphBlinkArg(
    ScriptState* script_state,
    const FlexibleArrayBufferView& array);

CORE_EXPORT base::Value ToPageGraphBlinkArg(
    ScriptState* script_state,
    blink::EventListener* event_listener);

// Override to pass additional blink receiver class data during WebAPI call.
template <typename T>
PageGraphBlinkReceiverData ToPageGraphBlinkReceiverData(T*) {
  return {};
}

template <>
CORE_EXPORT PageGraphBlinkReceiverData
ToPageGraphBlinkReceiverData(Document* document);

CORE_EXPORT std::optional<base::AutoReset<bool>> ScopedPageGraphCall();

}  // namespace blink

#endif  // BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_BRAVE_PAGE_GRAPH_BLINK_CONVERTERS_H_
