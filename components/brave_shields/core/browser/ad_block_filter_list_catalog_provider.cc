// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/brave_shields/core/browser/ad_block_filter_list_catalog_provider.h"

#include <string>
#include <utility>

#include "base/files/file_path.h"
#include "base/task/thread_pool.h"
#include "brave/components/brave_shields/core/browser/ad_block_component_installer.h"
#include "brave/components/brave_shields/resources/grit/adblock_bundle.h"
#include "components/component_updater/component_updater_service.h"
#include "ui/base/resource/resource_bundle.h"

constexpr char kListCatalogFile[] = "list_catalog.json";

namespace {

std::string LoadBundledResource() {
  return ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
      IDR_BRAVE_ADBLOCK_LIST_CATALOG);
}

}  // namespace

namespace brave_shields {

AdBlockFilterListCatalogProvider::AdBlockFilterListCatalogProvider(
    component_updater::ComponentUpdateService* cus) {
  base::SequencedTaskRunner::GetCurrentDefault()->PostTaskAndReplyWithResult(
      FROM_HERE, base::BindOnce(&LoadBundledResource),
      base::BindOnce(
          &AdBlockFilterListCatalogProvider::OnFilterListCatalogLoaded,
          weak_factory_.GetWeakPtr()));
  // Can be nullptr in unit tests
  if (cus) {
    RegisterAdBlockFilterListCatalogComponent(
        cus,
        base::BindRepeating(&AdBlockFilterListCatalogProvider::OnComponentReady,
                            weak_factory_.GetWeakPtr()));
  }
}

AdBlockFilterListCatalogProvider::~AdBlockFilterListCatalogProvider() = default;

void AdBlockFilterListCatalogProvider::AddObserver(
    AdBlockFilterListCatalogProvider::Observer* observer) {
  observers_.AddObserver(observer);
}

void AdBlockFilterListCatalogProvider::RemoveObserver(
    AdBlockFilterListCatalogProvider::Observer* observer) {
  observers_.RemoveObserver(observer);
}

void AdBlockFilterListCatalogProvider::OnFilterListCatalogLoaded(
    const std::string& catalog_json) {
  for (auto& observer : observers_) {
    observer.OnFilterListCatalogLoaded(catalog_json);
  }
}

void AdBlockFilterListCatalogProvider::OnComponentReady(
    const base::FilePath& path) {
  component_path_ = path;

  // Load the filter list catalog (as a string)
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(&brave_component_updater::GetDATFileAsString,
                     component_path_.AppendASCII(kListCatalogFile)),
      base::BindOnce(
          &AdBlockFilterListCatalogProvider::OnFilterListCatalogLoaded,
          weak_factory_.GetWeakPtr()));
}

void AdBlockFilterListCatalogProvider::LoadFilterListCatalog(
    base::OnceCallback<void(const std::string& catalog_json)> cb) {
  if (component_path_.empty()) {
    // If the path is not ready yet, don't run the callback. An update should be
    // pushed soon.
    return;
  }

  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(&brave_component_updater::GetDATFileAsString,
                     component_path_.AppendASCII(kListCatalogFile)),
      std::move(cb));
}

}  // namespace brave_shields
