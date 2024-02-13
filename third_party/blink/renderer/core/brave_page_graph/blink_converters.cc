/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/third_party/blink/renderer/core/brave_page_graph/blink_converters.h"

#include <memory>

#include "base/no_destructor.h"
#include "brave/v8/include/v8-isolate-page-graph-utils.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/web_v8_value_converter.h"
#include "third_party/blink/renderer/bindings/core/v8/native_value_traits_impl.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/events/event_listener.h"
#include "third_party/blink/renderer/core/typed_arrays/flexible_array_buffer_view.h"

namespace blink {

static base::Value V8ValueToJson(v8::Isolate* isolate,
                                 v8::Local<v8::Value> v8_value) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Value> serialized_value =
      v8::page_graph::SerializeValue(context, v8_value);
  if (!serialized_value.IsEmpty()) {
    static base::NoDestructor<std::unique_ptr<WebV8ValueConverter>> converter(
        Platform::Current()->CreateWebV8ValueConverter());
    auto value =
        (*converter)
            ->FromV8Value(serialized_value, isolate->GetCurrentContext());
    if (value) {
      return std::move(*value);
    }
  }
  return {};
}

template <>
base::Value ToPageGraphBlinkArg(
    ScriptState* script_state,
    const bindings::NativeValueTraitsAnyAdapter& adapter) {
  return ToPageGraphBlinkArg(script_state, static_cast<ScriptValue>(adapter));
}

template <>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const v8::Local<v8::Value>& arg) {
  if (arg.IsEmpty()) {
    return base::Value();
  }
  v8::String::Utf8Value utf8_string(script_state->GetIsolate(), arg);
  std::string_view result(*utf8_string, utf8_string.length());
  if (result.starts_with("[object ")) {
    return V8ValueToJson(script_state->GetIsolate(), arg);
  }
  return base::Value(result);
}

template <>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const ScriptValue& script_value) {
  ScriptState::Scope scope(script_state);
  v8::Local<v8::Value> value = script_value.V8Value();
  return ToPageGraphBlinkArg(script_state, value);
  // return base::Value(
  //     ToCoreString(script_state->GetIsolate(),
  //                  value->ToString(script_state->GetContext()).ToLocalChecked())
  //         .Utf8());
}

template <>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const ScriptPromise& script_promise) {
  return ToPageGraphBlinkArg(script_state, script_promise.AsScriptValue());
}

template <>
base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                const FlexibleArrayBufferView& array) {
  if (array.IsNull()) {
    return ToPageGraphBlinkArg(script_state, base::span<uint8_t>());
  }
  const base::span<const uint8_t> data_view(
      static_cast<const uint8_t*>(array.BaseAddressMaybeOnStack()),
      array.ByteLength());
  return ToPageGraphBlinkArg(script_state, data_view);
}

base::Value ToPageGraphBlinkArg(ScriptState* script_state,
                                blink::EventListener* event_listener) {
  return event_listener ? base::Value(event_listener->ScriptBody().Utf8())
                        : base::Value();
}

template <>
PageGraphBlinkReceiverData ToPageGraphBlinkReceiverData(Document* document) {
  PageGraphBlinkReceiverData data;
  data.Set("cookie_url", base::Value(document->CookieURL().GetString().Utf8()));
  return data;
}

std::optional<base::AutoReset<bool>> ScopedPageGraphCall() {
  thread_local static bool in_page_graph_call = false;
  if (in_page_graph_call) {
    return std::nullopt;
  }
  return base::AutoReset<bool>(&in_page_graph_call, true);
}

}  // namespace blink
