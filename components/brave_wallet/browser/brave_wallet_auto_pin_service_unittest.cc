// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/brave_wallet/browser/brave_wallet_auto_pin_service.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/test/bind.h"
#include "base/time/time_override.h"
#include "brave/components/brave_wallet/browser/brave_wallet_pin_service.h"
#include "brave/components/brave_wallet/browser/brave_wallet_prefs.h"
#include "brave/components/brave_wallet/browser/json_rpc_service.h"
#include "brave/components/brave_wallet/browser/pref_names.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "brave/components/ipfs/pin/ipfs_local_pin_service.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "components/prefs/testing_pref_service.h"
#include "content/public/test/browser_task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::_;

namespace brave_wallet {

namespace {

class MockBraveWalletPinService : public BraveWalletPinService {
 public:
  MockBraveWalletPinService()
      : BraveWalletPinService(nullptr, nullptr, nullptr) {}
  MOCK_METHOD3(AddPin,
               void(mojom::BlockchainTokenPtr,
                    const absl::optional<std::string>&,
                    BraveWalletPinService::AddPinCallback callback));
  MOCK_METHOD3(RemovePin,
               void(mojom::BlockchainTokenPtr,
                    const absl::optional<std::string>&,
                    BraveWalletPinService::RemovePinCallback callback));
  MOCK_METHOD3(Validate,
               void(mojom::BlockchainTokenPtr,
                    const absl::optional<std::string>&,
                    BraveWalletPinService::ValidateCallback));
  MOCK_METHOD1(GetTokens,
               std::set<std::string>(const absl::optional<std::string>&));
  MOCK_METHOD2(GetTokenStatus,
               mojom::TokenPinStatusPtr(const absl::optional<std::string>&,
                                        const mojom::BlockchainTokenPtr&));
  MOCK_METHOD2(GetLastValidateTime,
               absl::optional<base::Time>(const absl::optional<std::string>&,
                                          const mojom::BlockchainTokenPtr&));
  MOCK_METHOD2(MarkAsPendingForPinning,
               void(const mojom::BlockchainTokenPtr&,
                    const absl::optional<std::string>&));
  MOCK_METHOD2(MarkAsPendingForUnpinning,
               void(const mojom::BlockchainTokenPtr&,
                    const absl::optional<std::string>&));
};

class MockBraveWalletService : public BraveWalletService {
 public:
  MOCK_METHOD1(GetAllUserAssets,
               void(BraveWalletService::GetUserAssetsCallback));
};

MATCHER_P(TokenPathMatches, path, "") {
  return arg == BraveWalletPinService::TokenFromPath(path);
}

}  // namespace

class BraveWalletAutoPinServiceTest : public testing::Test {
 public:
  BraveWalletAutoPinServiceTest() = default;

  BraveWalletAutoPinService* service() {
    return brave_wallet_auto_pin_service_.get();
  }

 protected:
  void SetUp() override {
    auto* registry = pref_service_.registry();
    registry->RegisterBooleanPref(kAutoPinEnabled, true);
    brave_wallet_auto_pin_service_ =
        std::make_unique<BraveWalletAutoPinService>(
            GetPrefs(), GetBraveWalletService(), GetBraveWalletPinService());
  }

  PrefService* GetPrefs() { return &pref_service_; }

  testing::NiceMock<MockBraveWalletPinService>* GetBraveWalletPinService() {
    return &brave_wallet_pin_service_;
  }

  testing::NiceMock<MockBraveWalletService>* GetBraveWalletService() {
    return &brave_wallet_service_;
  }

  void SetAutoPinEnabled(bool value) {}

  testing::NiceMock<MockBraveWalletPinService> brave_wallet_pin_service_;
  testing::NiceMock<MockBraveWalletService> brave_wallet_service_;

  std::unique_ptr<BraveWalletAutoPinService> brave_wallet_auto_pin_service_;

  TestingPrefServiceSimple pref_service_;
  content::BrowserTaskEnvironment task_environment_;
};

TEST_F(BraveWalletAutoPinServiceTest, Autopin_WhenTokenAdded) {
  service()->SetAutoPinEnabled(true);

  ON_CALL(*GetBraveWalletPinService(), AddPin(_, _, _))
      .WillByDefault(
          ::testing::Invoke([](BlockchainTokenPtr token,
                               const absl::optional<std::string>& service,
                               BraveWalletPinService::AddPinCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));
  EXPECT_CALL(*GetBraveWalletPinService(), AddPin(_, _, _)).Times(3);

  {
    mojom::BlockchainTokenPtr token = BraveWalletPinService::TokenFromPath(
        "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
    service()->OnTokenAdded(std::move(token));
  }

  {
    mojom::BlockchainTokenPtr token = BraveWalletPinService::TokenFromPath(
        "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2");
    service()->OnTokenAdded(std::move(token));
  }

  {
    mojom::BlockchainTokenPtr token = BraveWalletPinService::TokenFromPath(
        "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3");
    service()->OnTokenAdded(std::move(token));
  }
}

TEST_F(BraveWalletAutoPinServiceTest, TokenRemoved) {
  ON_CALL(*GetBraveWalletPinService(), AddPin(_, _, _))
      .WillByDefault(
          ::testing::Invoke([](BlockchainTokenPtr token,
                               const absl::optional<std::string>& service,
                               BraveWalletPinService::AddPinCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));
}

TEST_F(BraveWalletAutoPinServiceTest, UnpinUnknownTokens_WhenRestore) {
  std::set<std::string> known_tokens;

  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3");

  ON_CALL(*GetBraveWalletPinService(),
          GetTokenStatus(testing::Eq(absl::nullopt), _))
      .WillByDefault(
          ::testing::Invoke([](absl::optional<std::string> service,
                               const mojom::BlockchainTokenPtr& token)
                                -> mojom::TokenPinStatusPtr {
            mojom::TokenPinStatusPtr status = mojom::TokenPinStatus::New();
            status->code = mojom::TokenPinStatusCode::STATUS_PINNED;
            status->validate_time = base::Time::Now();
            return status;
          }));
  ON_CALL(*GetBraveWalletPinService(), GetTokens(_))
      .WillByDefault(::testing::Return(known_tokens));
  ON_CALL(*GetBraveWalletService(), GetAllUserAssets(_))
      .WillByDefault(::testing::Invoke([](BraveWalletService::
                                              GetUserAssetsCallback callback) {
        std::vector<mojom::BlockchainTokenPtr> result;
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"));
        std::move(callback).Run(std::move(result));
      }));

  EXPECT_CALL(*GetBraveWalletPinService(),
              RemovePin(TokenPathMatches(
                            "nft.local.60.0x1."
                            "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3"),
                        testing::Eq(absl::nullopt), _))
      .Times(1);

  BraveWalletAutoPinService auto_pin_service(
      GetPrefs(), GetBraveWalletService(), GetBraveWalletPinService());
}

TEST_F(BraveWalletAutoPinServiceTest, ValidateOldTokens_WhenRestore) {
  std::set<std::string> known_tokens;

  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x4");
  ON_CALL(*GetBraveWalletPinService(),
          GetTokenStatus(testing::Eq(absl::nullopt), _))
      .WillByDefault(
          ::testing::Invoke([](absl::optional<std::string> service,
                               const mojom::BlockchainTokenPtr& token)
                                -> mojom::TokenPinStatusPtr {
            mojom::TokenPinStatusPtr status = mojom::TokenPinStatus::New();

            if ("0x1" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_PINNED;
              status->validate_time = base::Time::Now() - base::Days(20);
            } else if ("0x2" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_PINNED;
            } else if ("0x3" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_PINNED;
              status->validate_time = base::Time::Now() + base::Days(20);
            } else if ("0x4" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_NOT_PINNED;
            }
            return status;
          }));
  ON_CALL(*GetBraveWalletPinService(), GetTokens(_))
      .WillByDefault(::testing::Return(known_tokens));
  ON_CALL(*GetBraveWalletService(), GetAllUserAssets(_))
      .WillByDefault(::testing::Invoke([](BraveWalletService::
                                              GetUserAssetsCallback callback) {
        std::vector<mojom::BlockchainTokenPtr> result;
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x4"));
        std::move(callback).Run(std::move(result));
      }));

  ON_CALL(*GetBraveWalletPinService(), Validate(_, _, _))
      .WillByDefault(::testing::Invoke(
          [](BlockchainTokenPtr token,
             const absl::optional<std::string>& service,
             BraveWalletPinService::ValidateCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));

  EXPECT_CALL(*GetBraveWalletPinService(),
              Validate(TokenPathMatches(
                           "nft.local.60.0x1."
                           "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"),
                       testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(*GetBraveWalletPinService(),
              Validate(TokenPathMatches(
                           "nft.local.60.0x1."
                           "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"),
                       testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(*GetBraveWalletPinService(),
              Validate(TokenPathMatches(
                           "nft.local.60.0x1."
                           "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3"),
                       testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(*GetBraveWalletPinService(),
              Validate(TokenPathMatches(
                           "nft.local.60.0x1."
                           "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x4"),
                       testing::Eq(absl::nullopt), _))
      .Times(0);

  BraveWalletAutoPinService auto_pin_service(
      GetPrefs(), GetBraveWalletService(), GetBraveWalletPinService());
}

TEST_F(BraveWalletAutoPinServiceTest, PinContinue_WhenRestore) {
  std::set<std::string> known_tokens;

  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3");

  ON_CALL(*GetBraveWalletPinService(),
          GetTokenStatus(testing::Eq(absl::nullopt), _))
      .WillByDefault(
          ::testing::Invoke([](absl::optional<std::string> service,
                               const mojom::BlockchainTokenPtr& token)
                                -> mojom::TokenPinStatusPtr {
            mojom::TokenPinStatusPtr status = mojom::TokenPinStatus::New();
            if ("0x1" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_PINNING_FAILED;
            } else if ("0x2" == token->token_id) {
              status->code =
                  mojom::TokenPinStatusCode::STATUS_PINNING_IN_PROGRESS;
            } else if ("0x3" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_PINNING_PENDING;
            }
            return status;
          }));
  ON_CALL(*GetBraveWalletPinService(), GetTokens(_))
      .WillByDefault(::testing::Return(known_tokens));
  ON_CALL(*GetBraveWalletService(), GetAllUserAssets(_))
      .WillByDefault(::testing::Invoke([](BraveWalletService::
                                              GetUserAssetsCallback callback) {
        std::vector<mojom::BlockchainTokenPtr> result;
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3"));
        std::move(callback).Run(std::move(result));
      }));

  ON_CALL(*GetBraveWalletPinService(), AddPin(_, _, _))
      .WillByDefault(
          ::testing::Invoke([](BlockchainTokenPtr token,
                               const absl::optional<std::string>& service,
                               BraveWalletPinService::AddPinCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));

  EXPECT_CALL(
      *GetBraveWalletPinService(),
      AddPin(TokenPathMatches("nft.local.60.0x1."
                              "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"),
             testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(
      *GetBraveWalletPinService(),
      AddPin(TokenPathMatches("nft.local.60.0x1."
                              "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"),
             testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(
      *GetBraveWalletPinService(),
      AddPin(TokenPathMatches("nft.local.60.0x1."
                              "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3"),
             testing::Eq(absl::nullopt), _))
      .Times(1);

  BraveWalletAutoPinService auto_pin_service(
      GetPrefs(), GetBraveWalletService(), GetBraveWalletPinService());
}

TEST_F(BraveWalletAutoPinServiceTest, UnpinContinue_WhenRestore) {
  std::set<std::string> known_tokens;

  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3");

  ON_CALL(*GetBraveWalletPinService(),
          GetTokenStatus(testing::Eq(absl::nullopt), _))
      .WillByDefault(
          ::testing::Invoke([](absl::optional<std::string> service,
                               const mojom::BlockchainTokenPtr& token)
                                -> mojom::TokenPinStatusPtr {
            mojom::TokenPinStatusPtr status = mojom::TokenPinStatus::New();
            if ("0x1" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_UNPINNING_FAILED;
            } else if ("0x2" == token->token_id) {
              status->code =
                  mojom::TokenPinStatusCode::STATUS_UNPINNING_IN_PROGRESS;
            } else if ("0x3" == token->token_id) {
              status->code =
                  mojom::TokenPinStatusCode::STATUS_UNPINNING_PENDING;
            }
            return status;
          }));
  ON_CALL(*GetBraveWalletPinService(), GetTokens(_))
      .WillByDefault(::testing::Return(known_tokens));
  ON_CALL(*GetBraveWalletService(), GetAllUserAssets(_))
      .WillByDefault(::testing::Invoke([](BraveWalletService::
                                              GetUserAssetsCallback callback) {
        std::vector<mojom::BlockchainTokenPtr> result;
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"));
        std::move(callback).Run(std::move(result));
      }));

  ON_CALL(*GetBraveWalletPinService(), RemovePin(_, _, _))
      .WillByDefault(::testing::Invoke(
          [](BlockchainTokenPtr token,
             const absl::optional<std::string>& service,
             BraveWalletPinService::RemovePinCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));

  EXPECT_CALL(*GetBraveWalletPinService(),
              RemovePin(TokenPathMatches(
                            "nft.local.60.0x1."
                            "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"),
                        testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(*GetBraveWalletPinService(),
              RemovePin(TokenPathMatches(
                            "nft.local.60.0x1."
                            "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"),
                        testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(*GetBraveWalletPinService(),
              RemovePin(TokenPathMatches(
                            "nft.local.60.0x1."
                            "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x3"),
                        testing::Eq(absl::nullopt), _))
      .Times(1);

  BraveWalletAutoPinService auto_pin_service(
      GetPrefs(), GetBraveWalletService(), GetBraveWalletPinService());
}

TEST_F(BraveWalletAutoPinServiceTest, DoNotAutoPin_WhenAutoPinDisabled) {
  service()->SetAutoPinEnabled(false);

  ON_CALL(*GetBraveWalletPinService(), AddPin(_, _, _))
      .WillByDefault(
          ::testing::Invoke([](BlockchainTokenPtr token,
                               const absl::optional<std::string>& service,
                               BraveWalletPinService::AddPinCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));
  EXPECT_CALL(*GetBraveWalletPinService(), AddPin(_, _, _)).Times(0);

  {
    mojom::BlockchainTokenPtr token = BraveWalletPinService::TokenFromPath(
        "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
    service()->OnTokenAdded(std::move(token));
  }
}

TEST_F(BraveWalletAutoPinServiceTest, PinOldTokens_WhenAutoPinEnabled) {
  std::set<std::string> known_tokens;

  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1");
  known_tokens.insert(
      "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2");

  ON_CALL(*GetBraveWalletPinService(),
          GetTokenStatus(testing::Eq(absl::nullopt), _))
      .WillByDefault(
          ::testing::Invoke([](absl::optional<std::string> service,
                               const mojom::BlockchainTokenPtr& token)
                                -> mojom::TokenPinStatusPtr {
            mojom::TokenPinStatusPtr status = mojom::TokenPinStatus::New();
            if ("0x1" == token->token_id) {
              return nullptr;
            } else if ("0x2" == token->token_id) {
              status->code = mojom::TokenPinStatusCode::STATUS_NOT_PINNED;
            }
            return status;
          }));
  ON_CALL(*GetBraveWalletPinService(), GetTokens(_))
      .WillByDefault(::testing::Return(known_tokens));
  ON_CALL(*GetBraveWalletService(), GetAllUserAssets(_))
      .WillByDefault(::testing::Invoke([](BraveWalletService::
                                              GetUserAssetsCallback callback) {
        std::vector<mojom::BlockchainTokenPtr> result;
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"));
        result.push_back(BraveWalletPinService::TokenFromPath(
            "nft.local.60.0x1.0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"));
        std::move(callback).Run(std::move(result));
      }));

  ON_CALL(*GetBraveWalletPinService(), AddPin(_, _, _))
      .WillByDefault(
          ::testing::Invoke([](BlockchainTokenPtr token,
                               const absl::optional<std::string>& service,
                               BraveWalletPinService::AddPinCallback callback) {
            std::move(callback).Run(true, nullptr);
          }));

  EXPECT_CALL(
      *GetBraveWalletPinService(),
      AddPin(TokenPathMatches("nft.local.60.0x1."
                              "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x1"),
             testing::Eq(absl::nullopt), _))
      .Times(1);
  EXPECT_CALL(
      *GetBraveWalletPinService(),
      AddPin(TokenPathMatches("nft.local.60.0x1."
                              "0xbc4ca0eda7647a8ab7c2061c2e118a18a936f13d.0x2"),
             testing::Eq(absl::nullopt), _))
      .Times(1);

  BraveWalletAutoPinService auto_pin_service(
      GetPrefs(), GetBraveWalletService(), GetBraveWalletPinService());
}

}  // namespace brave_wallet
