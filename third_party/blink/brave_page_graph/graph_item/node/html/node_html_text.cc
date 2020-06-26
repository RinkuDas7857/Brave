/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/third_party/blink/brave_page_graph/graph_item/node/html/node_html_text.h"

#include <string>

#include "base/logging.h"

#include "third_party/blink/renderer/core/dom/dom_node_ids.h"

#include "brave/third_party/blink/brave_page_graph/graphml.h"
#include "brave/third_party/blink/brave_page_graph/page_graph.h"
#include "brave/third_party/blink/brave_page_graph/types.h"

#include "brave/third_party/blink/brave_page_graph/graph_item/edge/edge_text_change.h"

#include "brave/third_party/blink/brave_page_graph/graph_item/edge/node/edge_node_create.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/edge/node/edge_node_delete.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/edge/node/edge_node_insert.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/edge/node/edge_node_remove.h"

#include "brave/third_party/blink/brave_page_graph/graph_item/node/node.h"

#include "brave/third_party/blink/brave_page_graph/graph_item/node/html/node_html.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/node/html/node_html_element.h"

using ::std::string;
using ::std::to_string;

using ::blink::DOMNodeId;
using ::blink::DynamicTo;

namespace brave_page_graph {

NodeHTMLText::NodeHTMLText(PageGraph* const graph, const DOMNodeId node_id,
    const string& text) :
      NodeHTML(graph, node_id),
      text_(text) {}

NodeHTMLText::~NodeHTMLText() {}

ItemName NodeHTMLText::GetItemName() const {
  return "text node";
}

ItemDesc NodeHTMLText::GetItemDesc() const {
  return NodeHTML::GetItemDesc() + " [length: " + to_string(text_.size()) + "]";
}

GraphMLXMLList NodeHTMLText::GetGraphMLAttributes() const {
  GraphMLXMLList attrs = NodeHTML::GetGraphMLAttributes();
  attrs.push_back(GraphMLAttrDefForType(kGraphMLAttrDefNodeText)
      ->ToValue(text_));
  return attrs;
}

void NodeHTMLText::AddInEdge(const Edge* const in_edge) {
  NodeHTML::AddInEdge(in_edge);
  if (const EdgeNodeRemove* const remove_node_in_edge =
          DynamicTo<EdgeNodeRemove>(in_edge)) {
    // HTML Text nodes can't be removed if they were never inserted into the
    // tree.
    if (GetParentNode()) {
      GetParentNode()->RemoveChildNode(this);
    }
    SetParentNode(nullptr);
  } else if (const EdgeNodeInsert* const insert_node_in_edge =
                 DynamicTo<EdgeNodeInsert>(in_edge)) {
    SetParentNode(insert_node_in_edge->GetParentNode());
    GetParentNode()->PlaceChildNodeAfterSiblingNode(this,
        insert_node_in_edge->GetPriorSiblingNode());
  } else if (const EdgeTextChange* const text_change_in_edge =
                 DynamicTo<EdgeTextChange>(in_edge)) {
    text_ = text_change_in_edge->GetText();
  }
}

bool NodeHTMLText::IsNodeHTMLText() const {
  return true;
}

}  // namespace brave_page_graph
