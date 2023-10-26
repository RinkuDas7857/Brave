// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import ButtonMenu from '@brave/leo/react/buttonMenu'
import Icon from '@brave/leo/react/icon'
import Checkbox from '@brave/leo/react/checkbox'
import { getLocale } from '$web-common/locale'
import getPageHandlerInstance, * as mojom from '../../api/page_handler'
import DataContext from '../../state/context'
import styles from './style.module.scss'
import classnames from '$web-common/classnames'

export default function FeatureMenu() {
  const context = React.useContext(DataContext)

  const handleSettingsClick = () => {
    getPageHandlerInstance().pageHandler.openBraveLeoSettings()
  }

  // TODO(petemill): Whilst this may be accurate, if the default (i.e. Unset)
  // value changes to True, this will be wrong.
  const isAutoGeneratedQuestionsEnabled =
  (context.userAutoGeneratePref === mojom.AutoGenerateQuestionsPref.Enabled)

  const handleAutoGenerateQuestionsClick = () => {
    context.setUserAllowsAutoGenerating(!isAutoGeneratedQuestionsEnabled)
  }

  const handleNewConversationClick = () => {
    getPageHandlerInstance().pageHandler.clearConversationHistory()
  }

  return (
    <ButtonMenu className={styles.featureMenu}>
      <div slot='anchor-content'>
        <Icon name='more-horizontal' />
      </div>
      <div className={styles.menuSectionTitle}>
        {getLocale('menuTitleModels')}
      </div>
      <div className={styles.menuSubtitle}>
        <Icon name='speech-bubble-comments' />
        <span>{getLocale('modelCategory-chat')}</span>
      </div>

      {context.allModels.map((model) => (
        <leo-menu-item
          key={model.key}
          aria-selected={model.key === context.currentModel?.key || undefined}
          onClick={() => context.setCurrentModel(model)}
        >
          <div className={styles.menuItemWithIcon}>
            <div className={styles.menuText}>
              <div>{model.name}</div>
              <p className={styles.modelSubtitle}>
                {getLocale(`braveLeoModelSubtitle-${model.key}`)}
              </p>
            </div>
            {model.isPremium && (
              <Icon
                className={classnames({
                  [styles.lockOpen]: context.isPremiumUser
                })}
                name={context.isPremiumUser ? 'lock-open' : 'lock-plain'}
              />
            )}
          </div>
        </leo-menu-item>
      ))}

      <div className={styles.menuSeparator} />

      <leo-menu-item onClick={handleNewConversationClick}>
        <div className={classnames(styles.menuItemWithIcon, styles.menuItemMainItem)}>
          <Icon name='erase' />
          <span className={styles.menuText}>{getLocale('menuNewChat')}</span>
        </div>
      </leo-menu-item>

      <leo-menu-item
        onClick={handleAutoGenerateQuestionsClick}
        data-is-interactive={true}
      >
        <div className={classnames(styles.menuItemWithIcon, styles.menuItemMainItem)}>
          <Icon name='message-comments' />
          <span className={styles.menuText}>{getLocale('menuSuggestedQuestions')}</span>
          <Checkbox
            size='normal'
            checked={isAutoGeneratedQuestionsEnabled}
            onChange={handleAutoGenerateQuestionsClick}
          />
        </div>
      </leo-menu-item>

      <leo-menu-item onClick={context.goPremium}>
        <div className={classnames(styles.menuItemWithIcon, styles.menuItemMainItem)}>
          <Icon name='lock-open' />
          <span className={styles.menuText}>{getLocale('menuGoPremium')}</span>
        </div>
      </leo-menu-item>

      <leo-menu-item onClick={handleSettingsClick}>
        <div className={classnames(styles.menuItemWithIcon, styles.menuItemMainItem)}>
          <Icon name='settings' />
          <span className={styles.menuText}>{getLocale('menuSettings')}</span>
        </div>
      </leo-menu-item>
    </ButtonMenu>
  )
}