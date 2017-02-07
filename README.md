# Perfect-SPNEGO [简体中文](README.zh_CN.md)

<p align="center">
    <a href="http://perfect.org/get-involved.html" target="_blank">
        <img src="http://perfect.org/assets/github/perfect_github_2_0_0.jpg" alt="Get Involed with Perfect!" width="854" />
    </a>
</p>

<p align="center">
    <a href="https://github.com/PerfectlySoft/Perfect" target="_blank">
        <img src="http://www.perfect.org/github/Perfect_GH_button_1_Star.jpg" alt="Star Perfect On Github" />
    </a>  
    <a href="http://stackoverflow.com/questions/tagged/perfect" target="_blank">
        <img src="http://www.perfect.org/github/perfect_gh_button_2_SO.jpg" alt="Stack Overflow" />
    </a>  
    <a href="https://twitter.com/perfectlysoft" target="_blank">
        <img src="http://www.perfect.org/github/Perfect_GH_button_3_twit.jpg" alt="Follow Perfect on Twitter" />
    </a>  
    <a href="http://perfect.ly" target="_blank">
        <img src="http://www.perfect.org/github/Perfect_GH_button_4_slack.jpg" alt="Join the Perfect Slack" />
    </a>
</p>

<p align="center">
    <a href="https://developer.apple.com/swift/" target="_blank">
        <img src="https://img.shields.io/badge/Swift-3.0-orange.svg?style=flat" alt="Swift 3.0">
    </a>
    <a href="https://developer.apple.com/swift/" target="_blank">
        <img src="https://img.shields.io/badge/Platforms-OS%20X%20%7C%20Linux%20-lightgray.svg?style=flat" alt="Platforms OS X | Linux">
    </a>
    <a href="http://perfect.org/licensing.html" target="_blank">
        <img src="https://img.shields.io/badge/License-Apache-lightgrey.svg?style=flat" alt="License Apache">
    </a>
    <a href="http://twitter.com/PerfectlySoft" target="_blank">
        <img src="https://img.shields.io/badge/Twitter-@PerfectlySoft-blue.svg?style=flat" alt="PerfectlySoft Twitter">
    </a>
    <a href="http://perfect.ly" target="_blank">
        <img src="http://perfect.ly/badge.svg" alt="Slack Status">
    </a>
</p>

This project provides a general server library which provides SPNEGO mechanism.

This package builds with Swift Package Manager and is part of the [Perfect](https://github.com/PerfectlySoft/Perfect) project.

Ensure you have installed and activated the latest Swift 3.0 tool chain.

### Before Start

Perfect SPNEGO is aiming on a general application of Server Side Swift, so it could be plugged into *ANY* servers, such as HTTP / FTP / SSH, etc.

Although it supports Perfect HTTP server natively, it could be applied to any other Swift servers as well.

Before attaching to any actual server applications, please make sure your server has been already configured with Kerberos V5.

### Linux Build Note

If your server is a KDC, then you can skip this step, otherwise please install Kerberos V5 utilities:

```
$ sudo apt-get install krb5-user libkrb5-dev
```

### KDC Configuration

Configure the application server's /etc/krb5.conf to your KDC. The following sample configuration shows how to connect your application server to realm `KRB5.CA` under control of a KDC named `nut.krb5.ca`:

```
[realms]
KRB5.CA = {
	kdc = nut.krb5.ca
	admin_server = nut.krb5.ca
}
[domain_realm]
.krb5.ca = KRB5.CA
krb5.ca = KRB5.CA
```

### Prepare Kerberos Keys for Server
Contact to your KDC administrator to assign a `keytab` file to your application server.

Take example, *SUPPOSE ALL HOSTS BELOW REGISTERED ON THE SAME DNS SERVER*:

- KDC server: nut.krb5.ca
- Application server: coco.krb5.ca
- Application server type: HTTP

In such a case, KDC administrator shall login on `nut.krb5.ca` then perform following operation:

```
kadmin.local: addprinc -randkey HTTP/coco.krb5.ca@KRB5.CA
kadmin.local: ktadd -k /tmp/krb5.keytab HTTP/coco.krb5.ca@KRB5.CA
```

Then please ship this krb5.keytab file securely and install on your application server `coco.krb5.ca` and move to folder `/etc`, then grant sufficient permissions to your swift application to access it.

## Quick Start

Add the following dependency to your project's Package.swift file:

``` swift
.Package(url: "https://github.com/PerfectlySoft/Perfect-SPNEGO.git", majorVersion: 1)
```

Then import Perfect-SPNEGO to your source code:

``` swift
import PerfectSPNEGO
```

### Connect to KDC

Use the key in your default keytab `/etc/krb5.keytab` file on application server to register your application server to the KDC.

``` swift
let spnego = try Spnego("HTTP@coco.krb5.ca")
```

Please note that the host name and protocol type *MUST* match the records listed in the keytab file.

### Respond to A Spnego Challenge

Once initialized, object `spnego` could respond to the challenges. Take example, if a user is trying to connect to the application server as:

```
$ kinit rocky@KRB5.CA
$ curl --negotiate -u : http://coco.krb5.ca
```

In this case, the curl command would possibly send a base64 encoded challenge in the HTTP header:

```
> Authorization: Negotiate YIICnQYGKwYBBQUCoIICkTCCAo2gJzAlBgkqhkiG9xIBAgI ...
```

Once received such a challenge, you could apply this base64 string to the `spnego` object:
```
let (user, response) = try spnego.accept(base64Token: "YIICnQYGKwYBBQUCoIICkTCCAo2gJzAlBgkqhkiG9xIBAgI...")
```

If succeeded, the user would be "rocky@KRB5.CA". The variable `response` might be `nil` which indicates nothing is required to reply such a token, otherwise you should send this `response` back to the client.

Up till now, your application had already got the user information and request, then the application server might decide if this user could access the objective resource or not, according to your ACL (access control list) configuration.

## Issues

We are transitioning to using JIRA for all bugs and support related issues, therefore the GitHub issues has been disabled.

If you find a mistake, bug, or any other helpful suggestion you'd like to make on the docs please head over to [http://jira.perfect.org:8080/servicedesk/customer/portal/1](http://jira.perfect.org:8080/servicedesk/customer/portal/1) and raise it.

A comprehensive list of open issues can be found at [http://jira.perfect.org:8080/projects/ISS/issues](http://jira.perfect.org:8080/projects/ISS/issues)

## Further Information
For more information on the Perfect project, please visit [perfect.org](http://perfect.org).

## Acknowledgement

For rapid development purposes, this repo includes an base64 implementation developed by Sean Starkey <sean@seanstarkey.com> but it is subject to Perfect's future development.
[https://github.com/SeanStarkey/base64](https://github.com/SeanStarkey/base64)
