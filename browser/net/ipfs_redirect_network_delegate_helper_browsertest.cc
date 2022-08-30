/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/test/scoped_feature_list.h"
#include "brave/browser/ipfs/ipfs_service_factory.h"
#include "brave/components/ipfs/features.h"
#include "brave/components/ipfs/ipfs_constants.h"
#include "brave/components/ipfs/ipfs_service.h"
#include "brave/components/ipfs/pref_names.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/web_contents.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/navigation_handle_observer.h"
#include "net/base/net_errors.h"

namespace ipfs {

class IpfsRedirectNetworkDelegateHelperBrowserTest
    : public InProcessBrowserTest {
 public:
  IpfsRedirectNetworkDelegateHelperBrowserTest() {
    feature_list_.InitAndEnableFeature(ipfs::features::kIpfsFeature);
  }

  ~IpfsRedirectNetworkDelegateHelperBrowserTest() override = default;

  void SetUpOnMainThread() override {
    ipfs_url_ = GURL("ipfs://QmbWqxBEKC3P8tqsKc98xmWNzrzDtRLMiMPL8wBuTGsMnR");
    gateway_url_ = GURL(
        "https://dweb.link/ipfs/"
        "QmbWqxBEKC3P8tqsKc98xmWNzrzDtRLMiMPL8wBuTGsMnR");
    ipfs_service_ =
        ::ipfs::IpfsServiceFactory::GetForContext(browser()->profile());
  }

  content::WebContents* web_contents() const {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

  void ResetTestServer(
      const net::EmbeddedTestServer::HandleRequestCallback& callback) {
    test_server_.reset(new net::EmbeddedTestServer(
        net::test_server::EmbeddedTestServer::TYPE_HTTPS));
    test_server_->SetSSLConfig(net::EmbeddedTestServer::CERT_OK);
    test_server_->RegisterRequestHandler(callback);
    ASSERT_TRUE(test_server_->Start());
    ipfs_service_->SetServerEndpointForTest(test_server_->base_url());
  }

  std::unique_ptr<net::test_server::HttpResponse> HandleGetConnectedPeers(
      const net::test_server::HttpRequest& request) {
    if (request.GetURL().path_piece() != kSwarmPeersPath) {
      return nullptr;
    }

    auto http_response =
        std::make_unique<net::test_server::BasicHttpResponse>();
    http_response->set_code(net::HTTP_OK);
    http_response->set_content_type("application/json");
    http_response->set_content(R"({
      "Peers": [
        {
          "Addr": "/ip4/101.101.101.101/tcp/4001",
          "Direction": 0,
          "Peer": "QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ"
        },
        {
          "Addr": "/ip4/102.102.102.102/tcp/4001",
          "Direction": 0,
          "Peer": "QmStjfkGsfQGQQm6Gdxin6DvrZsFTmTNoX5oEFMzYrc1PS"
        }
      ]
    })");

    return http_response;
  }

  PrefService* GetPrefs() const { return browser()->profile()->GetPrefs(); }
  const GURL& ipfs_url() { return ipfs_url_; }
  const GURL& gateway_url() { return gateway_url_; }
  IpfsService* ipfs_service() const { return ipfs_service_; }

 private:
  base::test::ScopedFeatureList feature_list_;
  ipfs::IpfsService* ipfs_service_;
  std::unique_ptr<net::EmbeddedTestServer> test_server_;
  GURL ipfs_url_;
  GURL gateway_url_;
};

IN_PROC_BROWSER_TEST_F(IpfsRedirectNetworkDelegateHelperBrowserTest,
                       IPFSResolveMethodDisabledNoRedirect) {
  GetPrefs()->SetInteger(
      kIPFSResolveMethod,
      static_cast<int>(IPFSResolveMethodTypes::IPFS_DISABLED));

  EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), ipfs_url()));
  EXPECT_EQ(web_contents()->GetURL(), ipfs_url());
}

IN_PROC_BROWSER_TEST_F(IpfsRedirectNetworkDelegateHelperBrowserTest,
                       IPFSResolveMethodGatewayRedirect) {
  GetPrefs()->SetInteger(
      kIPFSResolveMethod,
      static_cast<int>(IPFSResolveMethodTypes::IPFS_GATEWAY));

  EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), ipfs_url()));
  EXPECT_EQ(web_contents()->GetURL(), gateway_url());
}

IN_PROC_BROWSER_TEST_F(IpfsRedirectNetworkDelegateHelperBrowserTest,
                       IPFSResolveRedirectsToErrorPage_Incognito) {
  GetPrefs()->SetInteger(
      kIPFSResolveMethod,
      static_cast<int>(IPFSResolveMethodTypes::IPFS_GATEWAY));

  EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), ipfs_url()));
  EXPECT_EQ(web_contents()->GetURL(), gateway_url());

  Browser* private_browser = CreateIncognitoBrowser(nullptr);
  auto* private_wc = private_browser->tab_strip_model()->GetActiveWebContents();

  content::NavigationHandleObserver observer(private_wc, ipfs_url());

  // Try to navigate to the url. The navigation should be canceled and the
  // NavigationHandle should have the right error code.
  EXPECT_TRUE(ui_test_utils::NavigateToURL(private_browser, ipfs_url()));
  EXPECT_TRUE(private_wc->GetPrimaryMainFrame()->IsErrorDocument());
  EXPECT_EQ(net::ERR_INCOGNITO_IPFS_NOT_ALLOWED, observer.net_error_code());
}

IN_PROC_BROWSER_TEST_F(IpfsRedirectNetworkDelegateHelperBrowserTest,
                       IPFSResolveRedirectsToErrorPage_IpfsDisabled) {
  GetPrefs()->SetInteger(
      kIPFSResolveMethod,
      static_cast<int>(IPFSResolveMethodTypes::IPFS_DISABLED));

  EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), ipfs_url()));
  EXPECT_EQ(web_contents()->GetURL(), ipfs_url());

  auto* wc = browser()->tab_strip_model()->GetActiveWebContents();

  content::NavigationHandleObserver observer(wc, ipfs_url());

  // Try to navigate to the url. The navigation should be canceled and the
  // NavigationHandle should have the right error code.
  EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), ipfs_url()));
  EXPECT_TRUE(wc->GetPrimaryMainFrame()->IsErrorDocument());
  EXPECT_EQ(net::ERR_IPFS_DISABLED, observer.net_error_code());
}

IN_PROC_BROWSER_TEST_F(IpfsRedirectNetworkDelegateHelperBrowserTest,
                       HTTPSSchemeDowngrade_CommonUrl) {
  GetPrefs()->SetInteger(kIPFSResolveMethod,
                         static_cast<int>(IPFSResolveMethodTypes::IPFS_LOCAL));

  EXPECT_TRUE(
      ui_test_utils::NavigateToURL(browser(), GURL("https://brave.com")));
  EXPECT_EQ(web_contents()->GetURL(), GURL("https://brave.com"));
}

IN_PROC_BROWSER_TEST_F(IpfsRedirectNetworkDelegateHelperBrowserTest,
                       HTTPSSchemeDowngrade_IPNSUrl) {
  ResetTestServer(base::BindRepeating(
      &IpfsRedirectNetworkDelegateHelperBrowserTest::HandleGetConnectedPeers,
      base::Unretained(this)));

  ipfs_service()->SetAllowIpfsLaunchForTest(true);

  GetPrefs()->SetInteger(kIPFSResolveMethod,
                         static_cast<int>(IPFSResolveMethodTypes::IPFS_LOCAL));

  EXPECT_TRUE(ui_test_utils::NavigateToURL(
      browser(),
      GURL("https://en-wikipedia--on--ipfs-org.ipns.localhost:48080/wiki/")));
  EXPECT_EQ(
      web_contents()->GetURL(),
      GURL("http://en-wikipedia--on--ipfs-org.ipns.localhost:48080/wiki/"));
}

}  // namespace ipfs
