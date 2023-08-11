/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_APP_BRAVE_COMMAND_IDS_H_
#define BRAVE_APP_BRAVE_COMMAND_IDS_H_

// First brave id must be higher than last chrome command.
// Check chrome/app/chrome_command_ids.h when rebase.
// ID of IDC_BRAVE_COMANDS_START and first brave command should be same.

// If new main menu items are added, please consider adding a case for it
// in the switch statement of BraveAppMenu::RecordMenuUsage for P3A purposes
#define IDC_BRAVE_COMMANDS_START 56000
#define IDC_SHOW_BRAVE_REWARDS 56000
#define IDC_NEW_TOR_CONNECTION_FOR_SITE     56002
#define IDC_NEW_OFFTHERECORD_WINDOW_TOR 56003
#define IDC_CONTENT_CONTEXT_OPENLINKTOR 56004
#define IDC_SHOW_BRAVE_SYNC      56005
#define IDC_SHOW_BRAVE_WALLET 56006
#define IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER   56009
#define IDC_SPEEDREADER_ICON_ONCLICK 56010
#define IDC_SIDEBAR_SHOW_OPTION_MENU 56011
#define IDC_SIDEBAR_SHOW_OPTION_ALWAYS 56012
#define IDC_SIDEBAR_SHOW_OPTION_MOUSEOVER 56013
#define IDC_SIDEBAR_SHOW_OPTION_NEVER 56015
#define IDC_CONTENT_CONTEXT_IMPORT_IPFS 56016
#define IDC_CONTENT_CONTEXT_IMPORT_LINK_IPFS 56017
#define IDC_CONTENT_CONTEXT_IMPORT_IPFS_PAGE 56018
#define IDC_CONTENT_CONTEXT_IMPORT_IMAGE_IPFS 56019
#define IDC_CONTENT_CONTEXT_IMPORT_AUDIO_IPFS 56020
#define IDC_CONTENT_CONTEXT_IMPORT_VIDEO_IPFS 56021
#define IDC_CONTENT_CONTEXT_IMPORT_SELECTED_TEXT_IPFS 56022
#define IDC_APP_MENU_IPFS 56023
#define IDC_APP_MENU_IPFS_PUBLISH_LOCAL_FILE 56024
#define IDC_APP_MENU_IPFS_PUBLISH_LOCAL_FOLDER 56025
#define IDC_SHOW_BRAVE_WALLET_PANEL 56026
#define IDC_CLOSE_BRAVE_WALLET_PANEL 56027
#define IDC_SHOW_BRAVE_VPN_PANEL 56028
#define IDC_BRAVE_VPN_MENU 56029
#define IDC_TOGGLE_BRAVE_VPN_TOOLBAR_BUTTON 56030
#define IDC_SEND_BRAVE_VPN_FEEDBACK 56031
#define IDC_ABOUT_BRAVE_VPN 56032
#define IDC_MANAGE_BRAVE_VPN_PLAN 56033
#define IDC_TOGGLE_BRAVE_VPN 56034
#define IDC_APP_MENU_IPFS_SHARE_LOCAL_FILE 56035
#define IDC_APP_MENU_IPFS_SHARE_LOCAL_FOLDER 56036
#define IDC_APP_MENU_IPFS_OPEN_FILES 56037
#define IDC_APP_MENU_IPFS_UPDATE_IPNS 56038
#define IDC_CONTENT_CONTEXT_FORCE_PASTE 56039
#define IDC_COPY_CLEAN_LINK 56040
#define IDC_TOGGLE_TAB_MUTE 56041
#define IDC_SIDEBAR_TOGGLE_POSITION 56042
#define IDC_CONTENT_CONTEXT_COPY_TEXT_FROM_IMAGE 56043
#define IDC_TOGGLE_SIDEBAR 56044

#define IDC_TOGGLE_VERTICAL_TABS 56045
#define IDC_TOGGLE_VERTICAL_TABS_WINDOW_TITLE 56046

#define IDC_CONFIGURE_BRAVE_NEWS 56047

#define IDC_CONFIGURE_SHORTCUTS 56048

#define IDC_SHOW_BRAVE_TALK 56049
#define IDC_TOGGLE_BRAVE_VPN_TRAY_ICON 56050
#define IDC_CONTENT_CONTEXT_IMPORT_IPNS_KEYS_START 56100
#define IDC_CONTENT_CONTEXT_IMPORT_IPNS_KEYS_END 56199

#define IDC_BRAVE_BOOKMARK_BAR_SUBMENU 56200
#define IDC_BRAVE_BOOKMARK_BAR_ALWAYS 56201
#define IDC_BRAVE_BOOKMARK_BAR_NTP 56202
#define IDC_BRAVE_BOOKMARK_BAR_NEVER 56203

#define IDC_TOGGLE_SHIELDS 56204
#define IDC_TOGGLE_JAVASCRIPT 56205

#define IDC_SHOW_PLAYLIST_BUBBLE 56206

#define IDC_GROUP_TABS_ON_CURRENT_ORIGIN 56207
#define IDC_MOVE_GROUP_TO_NEW_WINDOW 56208
#define IDC_CLOSE_DUPLICATE_TABS 56209

#define IDC_TOGGLE_VERTICAL_TABS_EXPANDED 56215

#define IDC_BRAVE_COMMANDS_LAST  57000

#endif  // BRAVE_APP_BRAVE_COMMAND_IDS_H_