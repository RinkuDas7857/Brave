/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/ads/serving/eligible_ads/eligible_ads_feature_util.h"

#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForEmptyParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights =
      ToAdPredictorWeights(/*param_value*/ {});

  // Assert
  EXPECT_TRUE(weights.empty());
}

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForNonNumericParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights =
      ToAdPredictorWeights("1.0, foobar, 2.2");

  // Assert
  EXPECT_TRUE(weights.empty());
}

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForAllZeroParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights = ToAdPredictorWeights("0.0, 0.0, 0.0");

  // Assert
  EXPECT_TRUE(weights.empty());
}

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForSomeZeroParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights = ToAdPredictorWeights("0.0, 0.1, 0.0");

  // Assert
  const AdPredictorWeightList expected_weights = {0.0, 0.1, 0.0};
  EXPECT_EQ(expected_weights, weights);
}

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForNegativeParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights = ToAdPredictorWeights("1.0, 3.0, -2.0");

  // Assert
  EXPECT_TRUE(weights.empty());
}

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForSingleParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights = ToAdPredictorWeights("1.0");

  // Assert
  const AdPredictorWeightList expected_weights = {1.0};
  EXPECT_EQ(expected_weights, weights);
}

TEST(BraveAdsEligibleAdFeatureUtilTest, ToAdPredictorWeightsForParamValue) {
  // Arrange

  // Act
  const AdPredictorWeightList weights = ToAdPredictorWeights("1.1, 3.3, 2.2");

  // Assert
  const AdPredictorWeightList expected_weights = {1.1, 3.3, 2.2};
  EXPECT_EQ(expected_weights, weights);
}

TEST(BraveAdsEligibleAdFeatureUtilTest,
     ToAdPredictorWeightsForParamValueWithMixedTypes) {
  // Arrange

  // Act
  const AdPredictorWeightList weights = ToAdPredictorWeights("1, 3, 2.2");

  // Assert
  const AdPredictorWeightList expected_weights = {1.0, 3.0, 2.2};
  EXPECT_EQ(expected_weights, weights);
}

}  // namespace brave_ads