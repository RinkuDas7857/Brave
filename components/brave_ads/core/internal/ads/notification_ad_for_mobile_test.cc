/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "base/test/mock_callback.h"
#include "base/time/time.h"
#include "brave/components/brave_ads/core/ad_type.h"
#include "brave/components/brave_ads/core/confirmation_type.h"
#include "brave/components/brave_ads/core/internal/account/transactions/transactions_unittest_util.h"
#include "brave/components/brave_ads/core/internal/ads/ad_events/ad_event_unittest_util.h"
#include "brave/components/brave_ads/core/internal/ads/serving/notification_ad_serving_util.h"
#include "brave/components/brave_ads/core/internal/ads/serving/permission_rules/permission_rules_unittest_util.h"
#include "brave/components/brave_ads/core/internal/catalog/catalog_url_request_builder_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_base.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_mock_util.h"
#include "brave/components/brave_ads/core/internal/creatives/notification_ads/notification_ad_manager.h"
#include "brave/components/brave_ads/core/internal/history/history_unittest_util.h"
#include "brave/components/brave_ads/core/notification_ad_info.h"
#include "net/http/http_status_code.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

using ::testing::_;
using ::testing::Invoke;

class BraveAdsNotificationAdForMobileIntegrationTest : public UnitTestBase {
 protected:
  void SetUp() override {
    UnitTestBase::SetUpForTesting(/*is_integration_test*/ true);
  }

  void SetUpMocks() override {
    MockPlatformHelper(platform_helper_mock_, PlatformType::kAndroid);

    const URLResponseMap url_responses = {
        {BuildCatalogUrlPath(),
         {{net::HTTP_OK,
           /*response_body*/ "/catalog_with_notification_ad.json"}}}};
    MockUrlResponses(ads_client_mock_, url_responses);
  }

  void ServeNextAd() {
    ASSERT_TRUE(ShouldServeAdsAtRegularIntervals());
    FastForwardClockTo(ServeAdAt());
  }

  void ServeAd() {
    NotifyUserDidBecomeActive(/*idle_time*/ base::TimeDelta::Min(),
                              /*screen_was_locked*/ false);
  }
};

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest,
       ServeWhenUserBecomesActive) {
  // Arrange
  ForcePermissionRulesForTesting();

  EXPECT_CALL(ads_client_mock_, ShowNotificationAd)
      .WillOnce(Invoke([](const NotificationAdInfo& ad) {
        // Assert
        ASSERT_TRUE(
            NotificationAdManager::GetInstance().Exists(ad.placement_id));
      }));

  EXPECT_CALL(ads_client_mock_, RecordP2AEvents(_));

  EXPECT_CALL(ads_client_mock_, AddTrainingSample).Times(0);

  // Act
  ServeNextAd();
}

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest,
       DoNotServeWhenUserBecomesActive) {
  // Arrange
  EXPECT_CALL(ads_client_mock_, ShowNotificationAd).Times(0);

  EXPECT_CALL(ads_client_mock_, RecordP2AEvents).Times(0);

  EXPECT_CALL(ads_client_mock_, AddTrainingSample).Times(0);

  // Act
  ServeAd();

  // Assert
}

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest,
       ServeAtRegularIntervals) {
  // Arrange

  // Assert

  // Act
  EXPECT_TRUE(ShouldServeAdsAtRegularIntervals());
}

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest, TriggerViewedEvent) {
  // Arrange
  ForcePermissionRulesForTesting();

  EXPECT_CALL(ads_client_mock_, ShowNotificationAd)
      .WillOnce(Invoke([=](const NotificationAdInfo& ad) {
        ASSERT_TRUE(
            NotificationAdManager::GetInstance().Exists(ad.placement_id));

        base::MockCallback<TriggerAdEventCallback> callback;
        EXPECT_CALL(callback, Run).WillOnce([&ad](const bool success) {
          // Assert
          EXPECT_TRUE(success);
          ASSERT_TRUE(
              NotificationAdManager::GetInstance().Exists(ad.placement_id));
          EXPECT_EQ(1U, GetAdEventCount(AdType::kNotificationAd,
                                        ConfirmationType::kServed));
          EXPECT_EQ(1U, GetAdEventCount(AdType::kNotificationAd,
                                        ConfirmationType::kViewed));
          EXPECT_EQ(1U, GetHistoryItemCount());
          EXPECT_EQ(1U, GetTransactionCount());
        });

        EXPECT_CALL(ads_client_mock_, AddTrainingSample).Times(0);

        // Act
        GetAds().TriggerNotificationAdEvent(
            ad.placement_id, mojom::NotificationAdEventType::kViewed,
            callback.Get());
      }));

  ServeNextAd();
}

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest, TriggerClickedEvent) {
  // Arrange
  ForcePermissionRulesForTesting();

  EXPECT_CALL(ads_client_mock_, ShowNotificationAd)
      .WillOnce(Invoke([=](const NotificationAdInfo& ad) {
        ASSERT_TRUE(
            NotificationAdManager::GetInstance().Exists(ad.placement_id));
        EXPECT_CALL(ads_client_mock_, CloseNotificationAd(ad.placement_id));

        base::MockCallback<TriggerAdEventCallback> callback;
        EXPECT_CALL(callback, Run).WillOnce([](const bool success) {
          // Assert
          EXPECT_TRUE(success);
          EXPECT_EQ(1U, GetAdEventCount(AdType::kNotificationAd,
                                        ConfirmationType::kClicked));
          EXPECT_EQ(1U, GetHistoryItemCount());
          EXPECT_EQ(1U, GetTransactionCount());
        });

        EXPECT_CALL(ads_client_mock_, AddTrainingSample);

        // Act
        GetAds().TriggerNotificationAdEvent(
            ad.placement_id, mojom::NotificationAdEventType::kClicked,
            callback.Get());
      }));

  ServeNextAd();
}

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest, TriggerDismissedEvent) {
  // Arrange
  ForcePermissionRulesForTesting();

  EXPECT_CALL(ads_client_mock_, ShowNotificationAd)
      .WillOnce(Invoke([=](const NotificationAdInfo& ad) {
        ASSERT_TRUE(
            NotificationAdManager::GetInstance().Exists(ad.placement_id));

        base::MockCallback<TriggerAdEventCallback> callback;
        EXPECT_CALL(callback, Run).WillOnce([&ad](const bool success) {
          // Assert
          EXPECT_TRUE(success);
          EXPECT_FALSE(
              NotificationAdManager::GetInstance().Exists(ad.placement_id));
          EXPECT_EQ(1U, GetAdEventCount(AdType::kNotificationAd,
                                        ConfirmationType::kDismissed));
          EXPECT_EQ(1U, GetHistoryItemCount());
          EXPECT_EQ(1U, GetTransactionCount());
        });

        EXPECT_CALL(ads_client_mock_, AddTrainingSample);

        // Act
        GetAds().TriggerNotificationAdEvent(
            ad.placement_id, mojom::NotificationAdEventType::kDismissed,
            callback.Get());
      }));

  ServeNextAd();
}

TEST_F(BraveAdsNotificationAdForMobileIntegrationTest, TriggerTimedOutEvent) {
  // Arrange
  ForcePermissionRulesForTesting();

  EXPECT_CALL(ads_client_mock_, ShowNotificationAd)
      .WillOnce(Invoke([=](const NotificationAdInfo& ad) {
        ASSERT_TRUE(
            NotificationAdManager::GetInstance().Exists(ad.placement_id));

        base::MockCallback<TriggerAdEventCallback> callback;
        EXPECT_CALL(callback, Run).WillOnce([&ad](const bool success) {
          // Assert
          EXPECT_TRUE(success);
          EXPECT_FALSE(
              NotificationAdManager::GetInstance().Exists(ad.placement_id));
          EXPECT_EQ(0U, GetHistoryItemCount());
          EXPECT_EQ(0U, GetTransactionCount());
        });

        EXPECT_CALL(ads_client_mock_, AddTrainingSample);

        // Act
        GetAds().TriggerNotificationAdEvent(
            ad.placement_id, mojom::NotificationAdEventType::kTimedOut,
            callback.Get());
      }));

  ServeNextAd();
}

}  // namespace brave_ads