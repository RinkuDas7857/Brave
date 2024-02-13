/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/third_party/blink/renderer/core/brave_page_graph/graph_item/edge/storage/edge_storage_set.h"

#include "brave/third_party/blink/renderer/core/brave_page_graph/graph_item/node/actor/node_script.h"
#include "brave/third_party/blink/renderer/core/brave_page_graph/graph_item/node/storage/node_storage.h"
#include "brave/third_party/blink/renderer/core/brave_page_graph/graphml.h"
#include "third_party/blink/renderer/platform/wtf/text/text_stream.h"

namespace brave_page_graph {

EdgeStorageSet::EdgeStorageSet(GraphItemContext* context,
                               NodeScript* out_node,
                               NodeStorage* in_node,
                               const String& key,
                               const blink::PageGraphBlinkArg& value)
    : EdgeStorage(context, out_node, in_node, key),
      value_(blink::PageGraphBaseValueToString(value)) {}

EdgeStorageSet::~EdgeStorageSet() = default;

ItemName EdgeStorageSet::GetItemName() const {
  return "storage set";
}

ItemDesc EdgeStorageSet::GetItemDesc() const {
  WTF::TextStream ts;
  ts << EdgeStorage::GetItemDesc() << " [value: " << value_ << "]";
  return ts.Release();
}

void EdgeStorageSet::AddGraphMLAttributes(xmlDocPtr doc,
                                          xmlNodePtr parent_node) const {
  EdgeStorage::AddGraphMLAttributes(doc, parent_node);
  GraphMLAttrDefForType(kGraphMLAttrDefValue)
      ->AddValueNode(doc, parent_node, value_);
}

bool EdgeStorageSet::IsEdgeStorageSet() const {
  return true;
}

}  // namespace brave_page_graph
