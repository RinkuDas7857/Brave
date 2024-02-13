// Copyright 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

import AuthenticationServices
import BraveCore
import BraveUI
import DesignSystem
import Strings
import SwiftUI

private struct Favicon: View {
  var attributes: FaviconAttributes

  var body: some View {
    if let image = attributes.faviconImage {
      Image(uiImage: image)
        .resizable()
        .aspectRatio(contentMode: .fit)
    } else if let monogramString = attributes.monogramString {
      Text(monogramString)
        .bold()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .foregroundColor(Color(uiColor: attributes.textColor ?? .label))
        .background(Color(uiColor: attributes.backgroundColor ?? .clear))
    }
  }
}

public struct CredentialListView: View {
  @ObservedObject public var model: CredentialListModel
  @State private var filter: String = ""
  @State private var credDetails: (any Credential)?

  public init(model: CredentialListModel) {
    self.model = model

    UIView.applyAppearanceDefaults()
  }

  private struct CredentialButton: View {
    @ObservedObject public var model: CredentialListModel
    var credential: any Credential
    var tappedInfoAction: () -> Void

    var body: some View {
      HStack {
        Button {
          model.actionHandler?(.selectedCredential(credential))
        } label: {
          HStack(spacing: 12) {
            ZStack {
              if let attributes = model.faviconAttributes[credential.serviceIdentifier] {
                Favicon(attributes: attributes)
              }
            }
            .onAppear {
              model.loadFavicon(for: credential)
            }
            .frame(width: 28, height: 28)
            .background(Color(braveSystemName: .containerBackground))
            .overlay {
              RoundedRectangle(cornerRadius: 6, style: .continuous).strokeBorder(
                Color(braveSystemName: .dividerStrong),
                lineWidth: 1
              )
            }
            .clipShape(RoundedRectangle(cornerRadius: 6, style: .continuous))
            .padding(.vertical, 4)
            VStack(alignment: .leading, spacing: 0) {
              Text(credential.serviceName)
              if !credential.user.isEmpty {
                Text(credential.user)
                  .foregroundStyle(.secondary)
              }
            }
          }
        }
        .tint(Color(braveSystemName: .textPrimary))
        .frame(maxWidth: .infinity, alignment: .leading)
        Button {
          tappedInfoAction()
        } label: {
          Image(braveSystemName: "leo.info.outline")
            .foregroundColor(Color(braveSystemName: .iconInteractive))
        }
        .buttonStyle(.plain)
      }
    }
  }

  private var allCredentials: [any Credential] {
    if filter.isEmpty {
      return model.allCredentials
    }
    return model.allCredentials.filter({ $0.serviceName.localizedStandardContains(filter) })
  }

  public var body: some View {
    NavigationView {
      List {
        if let origin = model.originHost {
          Section {
            if model.suggestedCredentials.isEmpty {
              Text(Strings.CredentialProvider.emptySuggestions)
                .font(.footnote)
                .foregroundStyle(Color(braveSystemName: .textTertiary))
                .listRowBackground(Color(uiColor: .secondaryBraveGroupedBackground))
            } else {
              ForEach(
                model.suggestedCredentials.sorted(by: { $0.rank < $1.rank }),
                id: \.recordIdentifier
              ) { cred in
                CredentialButton(model: model, credential: cred) {
                  credDetails = cred
                }
                .listRowBackground(Color(uiColor: .secondaryBraveGroupedBackground))
              }
            }
          } header: {
            Text(
              String.localizedStringWithFormat(Strings.CredentialProvider.loginsForWebsite, origin)
            )
            .foregroundStyle(Color(braveSystemName: .textTertiary))
          }
        }
        Section {
          ForEach(allCredentials, id: \.recordIdentifier) { cred in
            CredentialButton(model: model, credential: cred) {
              credDetails = cred
            }
            .listRowBackground(Color(uiColor: .secondaryBraveGroupedBackground))
          }
          if allCredentials.isEmpty && !filter.isEmpty {
            Text(
              String.localizedStringWithFormat(
                Strings.CredentialProvider.searchEmptyResults,
                filter
              )
            )
            .font(.footnote)
            .foregroundStyle(Color(braveSystemName: .textTertiary))
            .listRowBackground(Color(uiColor: .secondaryBraveGroupedBackground))
          }
        } header: {
          Text(Strings.CredentialProvider.otherLogins)
            .foregroundStyle(Color(braveSystemName: .textTertiary))
        }
      }
      .background {
        NavigationLink(
          isActive: Binding(
            get: { credDetails != nil },
            set: { if !$0 { credDetails = nil } }
          )
        ) {
          if let credDetails {
            CredentialDetailView(model: model, credential: credDetails)
          }
        } label: {
          EmptyView()
        }
      }
      .navigationTitle(Strings.CredentialProvider.credentialListTitle)
      .navigationBarTitleDisplayMode(.inline)
      .animation(.default, value: filter)
      .searchable(
        text: $filter,
        placement: .navigationBarDrawer(displayMode: .always),
        prompt: Text(Strings.CredentialProvider.searchBarPrompt)
      )
      .listStyle(.insetGrouped)
      .listBackgroundColor(Color(uiColor: .braveGroupedBackground))
      .toolbar {
        ToolbarItemGroup(placement: .cancellationAction) {
          Button(Strings.CredentialProvider.cancelButtonTitle) {
            model.actionHandler?(.cancelled)
          }
          .tint(Color(braveSystemName: .textInteractive))
        }
      }
    }
    .navigationViewStyle(.stack)
    .tint(Color(braveSystemName: .textInteractive))
  }
}

#if DEBUG
extension CredentialListModel {
  private class MockCredential: NSObject, Credential {
    var favicon: String!
    var password: String!
    var rank: Int64 = 0
    var recordIdentifier: String!
    var serviceIdentifier: String!
    var serviceName: String!
    var user: String!
    var note: String!

    init(
      favicon: FaviconAttributes?,
      rank: Int64,
      serviceName: String,
      user: String,
      password: String,
      note: String
    ) {
      if let favicon {
        self.favicon = {
          if let data = try? NSKeyedArchiver.archivedData(
            withRootObject: favicon,
            requiringSecureCoding: false
          ) {
            return String(data: data, encoding: .utf8)
          }
          return ""
        }()
      } else {
        self.favicon = ""
      }
      self.password = password
      self.rank = rank
      self.recordIdentifier = UUID().uuidString
      self.serviceIdentifier = UUID().uuidString
      self.serviceName = serviceName
      self.user = user
      self.note = note
      super.init()
    }
  }
  static let mock = CredentialListModel(
    allCredentials: [MockCredential](
      arrayLiteral:
        .init(
          favicon: nil,
          rank: 1,
          serviceName: "github.com",
          user: "user",
          password: "test",
          note: ""
        ),
      .init(favicon: nil, rank: 2, serviceName: "github.com", user: "", password: "test", note: "")
    ),
    originHost: "github.com"
  )
}

#Preview {
  CredentialListView(model: .mock)
}
#endif
