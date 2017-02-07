# Perfect-SPNEGO [English](README.md)

<p align="center">
    <a href="http://perfect.org/get-involved.html" target="_blank">
        <img src="http://perfect.org/assets/github/perfect_github_2_0_0.jpg" alt="Get Involed with Perfect!" width="854" />
    </a>
</p>

<p align="center">
    <a href="https://github.com/PerfectlySoft/Perfect" target="_blank">
        <img src="http://www.perfect.org/github/Perfect_GH_button_1_Star.jpg" alt="Star Perfect On Github" />
    </a>  
    <a href="https://gitter.im/PerfectlySoft/Perfect" target="_blank">
        <img src="http://www.perfect.org/github/Perfect_GH_button_2_Git.jpg" alt="Chat on Gitter" />
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
    <a href="https://gitter.im/PerfectlySoft/Perfect?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge" target="_blank">
        <img src="https://img.shields.io/badge/Gitter-Join%20Chat-brightgreen.svg" alt="Join the chat at https://gitter.im/PerfectlySoft/Perfect">
    </a>
    <a href="http://perfect.ly" target="_blank">
        <img src="http://perfect.ly/badge.svg" alt="Slack Status">
    </a>
</p>

该项目实现了一个用于 Swift 服务器编程的 SPNEGO 安全机制函数库

该软件使用SPM进行编译和测试，本软件也是[Perfect](https://github.com/PerfectlySoft/Perfect)项目的一部分。

请确保您已经安装并激活了最新版本的 Swift 3.0 tool chain 工具链。

### 开始之前

Perfect SPNEGO 瞄准的是所有用Swift 编程的通用服务器组件，因此能够应用于⚠️任何⚠️服务器，比如 HTTP / FTP / SSH，等等。

尽管该服务器组件能够原生支持 Perfect 自己的 HTTP 服务器，但是实际上也适用于任何一种其他互联网协议的服务器。

在安装到目标服务器软件之前，请确认您的服务器已经完成了Kerberos V的配置。

### Linux 编译指南

如果您的服务器是一个KDC（安全控制中心），您可以忽略这一步，否则请安装 Kerberos V5 工具组件，以及开发函数库：

```
$ sudo apt-get install krb5-user libkrb5-dev
```

### KDC配置

请配置好您的应用服务器/etc/krb5.conf，以便于该服务器能够正常连接到目标的KDC。请参考下面的例子，在这个例子中，应用服务器希望连接到控制区域`KRB5.CA`，并且该控制区域的KDC控制中心域名为`nut.krb5.ca`:

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

### 准备Kerberos 授权钥匙

下一步需要联系您的KDC 管理员，获得应用服务器将使用的keytab钥匙文件。

参考下面的例子的示范配置，⚠️下列所有主机必须注册到同一个DNS⚠️

- KDC 安全控制中心服务器: nut.krb5.ca
- 应用服务器: coco.krb5.ca
- 应用服务器计划安装的协议类型: HTTP

如果处于上述环境，则KDC管理员需要登录`nut.krb5.ca` 控制区域并采取下列操作：

```
kadmin.local: addprinc -randkey HTTP/coco.krb5.ca@KRB5.CA
kadmin.local: ktadd -k /tmp/krb5.keytab HTTP/coco.krb5.ca@KRB5.CA
```

生成钥匙文件krb5.keytab后，请将该钥匙安全地传输到您的应用服务器`coco.krb5.ca`并将文件移动到目录`/etc`下，然后赋予其适当权限，以便于您的服务器可以访问到。

## 快速开始

首先在您的软件工程配置文件Package.swift中增加下列依存关系：

``` swift
.Package(url: "https://github.com/PerfectlySoft/Perfect-SPNEGO.git", majorVersion: 1)
```

然后将 Perfect-SPNEGO 引入源代码：

``` swift
import PerfectSPNEGO
```

### 在应用程序中连接到 KDC

请使用默认钥匙文件 `/etc/krb5.keytab` 中列出的服务器名称，然后在程序中注册到KDC：

``` swift
let spnego = try Spnego("HTTP@coco.krb5.ca")
```

请注意主机名和协议⚠️务必⚠️与钥匙文件中的内容保持完全一致。

### 响应 SPNEGO 挑战

初始化成功之后，您的应用程序即可使用`spnego`对象响应此类挑战。比如，如果用户正尝试访问应用服务器：

```
$ kinit rocky@KRB5.CA
$ curl --negotiate -u : http://coco.krb5.ca
```

这种情况下，curl命令行将在其HTTP请求头数据中发送一个 base64 编码的申请表：

```
> Authorization: Negotiate YIICnQYGKwYBBQUCoIICkTCCAo2gJzAlBgkqhkiG9xIBAgI ...
```

您的服务器一旦收到这个申请表，即可使用 `spnego` 对象进行解码分析：
```
let (user, response) = try spnego.accept(base64Token: "YIICnQYGKwYBBQUCoIICkTCCAo2gJzAlBgkqhkiG9xIBAgI...")
```

如果成功的话，用户名称将会是"rocky@KRB5.CA"。而变量`response`有可能为空，表示这种申请不需要回复多余的数据；否则请将这个同样是base64的字符串返回给客户。

到此为止，您的程序已经获得了用户信息，您的应用程序可以根据这个验证过的用户名称，查看ACL（安全控制表）来决定是否允许该用户访问其期待的资源。

### 问题报告、内容贡献和客户支持

我们目前正在过渡到使用JIRA来处理所有源代码资源合并申请、修复漏洞以及其它有关问题。因此，GitHub 的“issues”问题报告功能已经被禁用了。

如果您发现了问题，或者希望为改进本文提供意见和建议，[请在这里指出](http://jira.perfect.org:8080/servicedesk/customer/portal/1).

在您开始之前，请参阅[目前待解决的问题清单](http://jira.perfect.org:8080/projects/ISS/issues).

## 更多信息
关于本项目更多内容，请参考[perfect.org](http://perfect.org).

## 致谢

因快速开发需要，该函数库使用了部分base64功能，源于 Sean Starkey <sean@seanstarkey.com> 项目，但后期可能会根据 Perfect 未来函数库的开发而移除。详见：
[https://github.com/SeanStarkey/base64](https://github.com/SeanStarkey/base64)
