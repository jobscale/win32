///////////////////////////////////////////////////////////////////////////////
// By default the ActiveX support for FireFox is a WHITELIST.
// Only a pre-installed Windows Media Player control is enabled by default. If
// you want to open up the plugin to other controls, read the instructions
// and sample prefs below to learn how to do it.

pref("general.useragent.vendorComment", "ax");
pref("security.xpconnect.activex.global.hosting_flags", 9);
pref("security.classID.allowByDefault", false);
pref("capability.policy.default.ClassID.CID6BF52A52-394A-11D3-B153-00C04F79FAA6", "AllAccess");
pref("capability.policy.default.ClassID.CID22D6F312-B0F6-11D0-94AB-0080C74C7E95", "AllAccess");


///////////////////////////////////////////////////////////////////////////////
// General hosting flags settings. Read nsIActiveXSecurityPolicy.idl in
// http://lxr.mozilla.org/seamonkey/find?string=nsIActiveXSecurityPolicy.idl
// for more combinations.
//
// Briefly,
//
//   0 means no hosting of activex controls whatsoever
//   13 means medium settings (safe for scripting controls and download / install)
//   31 means host anything (extremely dangerous!)
//

// pref("security.xpconnect.activex.global.hosting_flags", 13);


///////////////////////////////////////////////////////////////////////////////
// Whitelist / Blacklist capabilities
//
// The whitelist and blacklist settings define what controls Gecko will host
// and the default allow / deny behavior.
//
//   Note 1:
//
//   The hosting flags pref value above takes priority over settings below.
//   Therefore if the hosting flags are set to 0 (i.e. host nothing) then
//   no control will be hosted no matter what controls are enabled. Likewise, 
//   If safe for scripting checks are (wisely) enabled, no unsafe control
//   will be hosted even if it is explicitly enabled below.
//
//
//   Note 2:
//
//   Gecko always reads the IE browser's control blacklist if one is defined
//   in the registry. This is to ensure any control identified by Microsoft
//   or others as unsafe is not hosted without requiring it to be explicitly
//   listed here also.
//   


///////////////////////////////////////////////////////////////////////////////
// This pref sets the default policy to allow all controls or deny them all
// default. If the value is false, only controls explicitly enabled by their
// classid will be allowed. Otherwise all controls are allowed except those
// explicitly disabled by their classid.
//
// If you are writing an embedding application that only needs to run
// certain known controls, (e.g. an intranet control of some kind) you are
// advised to use the false value and enable the control explicitly.

// pref("security.classID.allowByDefault", true);


///////////////////////////////////////////////////////////////////////////////
// Specify below the controls that should be explicitly enabled or disabled.
// This is achieved by writing a policy rule, specifiying the classid of the
// control and giving the control "AllAccess" or "NoAccess".
//
// CIDaaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
//
// You could explicitly ban a control (using the appropriate classid) like this
//
// pref("capability.policy.default.ClassID.CID039ef260-2a0d-11d5-90a7-0010a4e73d9a", "NoAccess");
//
// If you want to explicity enable a control then do this:
//
// pref("capability.policy.default.ClassID.CID039ef260-2a0d-11d5-90a7-0010a4e73d9a", "AllAccess");
//
// If you want to explicitly ban or allow a control for one or more sites then
// you can create a policy for those sites. This example creates a domain
// called 'trustable' containing sites where you allow an additional control
// to be hosted.:
//
// user_pref("capability.policy.policynames", "trustable");
// user_pref("capability.policy.trustable.sites", "http://www.site1.net http://www.site2.net");
// user_pref("capability.policy.trustable.ClassID.CID039ef260-2a0d-11d5-90a7-0010a4e73d9a", "AllAccess");
//


