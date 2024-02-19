// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import { useHistory } from 'react-router'
import { useDispatch, useSelector } from 'react-redux'

// redux
import { WalletPageActions } from '../../../actions'
import { PageSelectors } from '../../../selectors'

// utils
import { getLocale } from '../../../../../common/locale'
import {
  useReportOnboardingActionMutation //
} from '../../../../common/slices/api.slice'

// components
import { WelcomeAction } from '../components/welcome-action/welcome-action'

// routes
import { BraveWallet, WalletRoutes } from '../../../../constants/types'

// styles
import * as leo from '@brave/leo/tokens/css'
import { Row, VerticalSpace } from '../../../../components/shared/style'
import {
  BraveIcon,
  Content,
  WelcomePageBackground,
  WelcomePageWrapper,
  Title,
  Heading,
  SubHeading,
  ActionsContainer,
  WatchOnlyWalletButton,
  Footer
} from './onboarding-welcome.style'

const walletIcons = [
  'brave-icon-release-color',
  'phantom-color',
  'metamask-color',
  'coinbase-color',
  'trezor-color',
  'wallet-ledger'
]

export const OnboardingWelcome = () => {
  // routing
  const history = useHistory()

  // redux
  const dispatch = useDispatch()
  const setupStillInProgress = useSelector(PageSelectors.setupStillInProgress)

  // mutations
  const [report] = useReportOnboardingActionMutation()

  // effects
  React.useEffect(() => {
    // start wallet setup
    if (!setupStillInProgress) {
      report(BraveWallet.OnboardingAction.Shown)
      dispatch(WalletPageActions.walletSetupComplete(false))
    }
  }, [setupStillInProgress, report])

  return (
    <>
      <WelcomePageBackground />
      <WelcomePageWrapper>
        <Content>
          <Row
            gap='10px'
            justifyContent='flex-start'
          >
            <BraveIcon />
            <Title>{getLocale('braveWalletTitle')}</Title>
          </Row>
          <VerticalSpace space={leo.spacing['5Xl']} />
          <Heading>{getLocale('braveWalletWelcomeTitle')}</Heading>
          <VerticalSpace space={leo.spacing.xl} />
          <SubHeading></SubHeading>
          <VerticalSpace space={leo.spacing['5Xl']} />

          <ActionsContainer>
            <WelcomeAction
              title={getLocale('braveWalletWelcomeNewWalletTitle')}
              description={getLocale('braveWalletWelcomeNewWalletDescription')}
              iconName='plus-add'
              onSelect={() =>
                history.push(WalletRoutes.OnboardingNewWalletTerms)
              }
            />

            <WelcomeAction
              title={getLocale('braveWalletWelcomeImportWalletTitle')}
              description={getLocale(
                'braveWalletWelcomeImportWalletDescription'
              )}
              iconName='import-arrow'
              walletIcons={walletIcons}
              onSelect={() =>
                history.push(WalletRoutes.OnboardingImportSelectWalletType)
              }
            />
          </ActionsContainer>
          <VerticalSpace space={leo.spacing['2Xl']} />
          <WatchOnlyWalletButton>
            {getLocale('braveWalletWelcomeWatchOnlyTitle')}
          </WatchOnlyWalletButton>
          <VerticalSpace space='72px' />
          <Footer>{getLocale('braveWalletWelcomeCopyright')}</Footer>
        </Content>
      </WelcomePageWrapper>
    </>
  )
}
