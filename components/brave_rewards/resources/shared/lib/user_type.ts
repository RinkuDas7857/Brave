/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as mojom from '../../shared/lib/mojom'

export type UserType = 'unconnected' | 'connected'

export function userTypeFromMojo (type: number): UserType {
  switch (type) {
    case mojom.UserType.kConnected:
      return 'connected'
    default:
      return 'unconnected'
  }
}

export function userTypeFromString (type: string): UserType {
  switch (type) {
    case 'unconnected':
    case 'connected':
      return type
  }
  return 'unconnected'
}
