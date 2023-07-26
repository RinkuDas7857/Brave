/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_IOS_BROWSER_API_LEDGER_LEDGER_CLIENT_IOS_H_
#define BRAVE_IOS_BROWSER_API_LEDGER_LEDGER_CLIENT_IOS_H_

#import <Foundation/Foundation.h>
#include <string>
#include <vector>

#include "brave/components/brave_rewards/common/mojom/bat_ledger.mojom.h"

#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"

@protocol LedgerClientBridge;

class LedgerClientIOS : public brave_rewards::mojom::LedgerClient {
 public:
  explicit LedgerClientIOS(id<LedgerClientBridge> bridge);
  ~LedgerClientIOS() override;

  mojo::PendingAssociatedRemote<brave_rewards::mojom::LedgerClient>
  MakeRemote();

 private:
  __unsafe_unretained id<LedgerClientBridge> bridge_;
  mojo::AssociatedReceiver<brave_rewards::mojom::LedgerClient> receiver_;

  void FetchFavIcon(const std::string& url,
                    const std::string& favicon_key,
                    FetchFavIconCallback callback) override;
  void LoadLedgerState(LoadLedgerStateCallback callback) override;
  void LoadPublisherState(LoadPublisherStateCallback callback) override;
  void LoadURL(brave_rewards::mojom::UrlRequestPtr request,
               LoadURLCallback callback) override;
  void Log(const std::string& file,
           int32_t line,
           int32_t verbose_level,
           const std::string& message) override;
  void OnPanelPublisherInfo(
      brave_rewards::mojom::Result result,
      brave_rewards::mojom::PublisherInfoPtr publisher_info,
      uint64_t windowId) override;
  void OnPublisherRegistryUpdated() override;
  void OnPublisherUpdated(const std::string& publisher_id) override;
  void OnReconcileComplete(
      brave_rewards::mojom::Result result,
      brave_rewards::mojom::ContributionInfoPtr contribution) override;
  void PublisherListNormalized(
      std::vector<brave_rewards::mojom::PublisherInfoPtr> list) override;
  void OnContributeUnverifiedPublishers(
      brave_rewards::mojom::Result result,
      const std::string& publisher_key,
      const std::string& publisher_name) override;
  void SetBooleanState(const std::string& name,
                       bool value,
                       SetBooleanStateCallback callback) override;
  void GetBooleanState(const std::string& name,
                       GetBooleanStateCallback callback) override;
  void SetIntegerState(const std::string& name,
                       int32_t value,
                       SetIntegerStateCallback callback) override;
  void GetIntegerState(const std::string& name,
                       GetIntegerStateCallback callback) override;
  void SetDoubleState(const std::string& name,
                      double value,
                      SetDoubleStateCallback callback) override;
  void GetDoubleState(const std::string& name,
                      GetDoubleStateCallback callback) override;
  void SetStringState(const std::string& name,
                      const std::string& value,
                      SetStringStateCallback) override;
  void GetStringState(const std::string& name,
                      GetStringStateCallback callback) override;
  void SetInt64State(const std::string& name,
                     int64_t value,
                     SetInt64StateCallback callback) override;
  void GetInt64State(const std::string& name,
                     GetInt64StateCallback callback) override;
  void SetUint64State(const std::string& name,
                      uint64_t value,
                      SetUint64StateCallback callback) override;
  void GetUint64State(const std::string& name,
                      GetUint64StateCallback callback) override;
  void SetValueState(const std::string& name,
                     base::Value value,
                     SetValueStateCallback callback) override;
  void GetValueState(const std::string& name,
                     GetValueStateCallback callback) override;
  void SetTimeState(const std::string& name,
                    base::Time value,
                    SetTimeStateCallback callback) override;
  void GetTimeState(const std::string& name,
                    GetTimeStateCallback callback) override;
  void ClearState(const std::string& name,
                  ClearStateCallback callback) override;
  void GetLegacyWallet(GetLegacyWalletCallback callback) override;
  void ShowNotification(const std::string& type,
                        const std::vector<std::string>& args,
                        ShowNotificationCallback callback) override;
  void IsBitFlyerRegion(IsBitFlyerRegionCallback callback) override;
  void GetClientInfo(GetClientInfoCallback callback) override;
  void UnblindedTokensReady() override;
  void ReconcileStampReset() override;
  void RunDBTransaction(brave_rewards::mojom::DBTransactionPtr transaction,
                        RunDBTransactionCallback callback) override;
  void PendingContributionSaved(
      const brave_rewards::mojom::Result result) override;
  void ClearAllNotifications() override;
  void ExternalWalletConnected() override;
  void ExternalWalletLoggedOut() override;
  void ExternalWalletReconnected() override;
  void DeleteLog(DeleteLogCallback callback) override;
  void EncryptString(const std::string& value,
                     EncryptStringCallback callback) override;
  void DecryptString(const std::string& value,
                     DecryptStringCallback callback) override;
};

#endif  // BRAVE_IOS_BROWSER_API_LEDGER_LEDGER_CLIENT_IOS_H_