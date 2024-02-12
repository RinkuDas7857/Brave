// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import { useHistory } from 'react-router'
import { useDispatch } from 'react-redux'

// utils
import { getLocale } from '../../../../../common/locale'
import { WalletPageActions } from '../../../actions'
import {
  useReportOnboardingActionMutation //
} from '../../../../common/slices/api.slice'

// constants
import { BraveWallet, WalletRoutes } from '../../../../constants/types'

// components
import { OnboardingContentLayout } from '../components/onboarding-content-layout/onboarding-content-layout'

// styles
import { VerticalSpace } from '../../../../components/shared/style'
import { IntroImg, Title, SubTitle } from './onboarding-success.style'

export const OnboardingSuccess = () => {
  // routing
  const history = useHistory()

  // redux
  const dispatch = useDispatch()

  // mutations
  const [report] = useReportOnboardingActionMutation()

  // methods
  const onComplete = React.useCallback(() => {
    dispatch(WalletPageActions.walletSetupComplete(true))
    history.push(WalletRoutes.PortfolioAssets)
  }, [])
  console.log(onComplete)

  // effects
  React.useEffect(() => {
    report(BraveWallet.OnboardingAction.Complete)
  }, [report])

  // render
  return (
    <OnboardingContentLayout>
      <IntroImg />
      <VerticalSpace space='36px' />
      <Title>{getLocale('braveWalletOnboardingSuccessTitle')}</Title>
      <VerticalSpace space='16px' />
      <SubTitle>
        {getLocale('braveWalletOnboardingSuccessDescription')}
      </SubTitle>
    </OnboardingContentLayout>
  )
}
