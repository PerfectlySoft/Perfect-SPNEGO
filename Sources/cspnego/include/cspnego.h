//
//  cspnego.h
//  PerfectSPNEGO
//
//  Created by Rocky Wei on 2017-02-07.
//	Copyright (C) 2017 PerfectlySoft, Inc.
//
//===----------------------------------------------------------------------===//
//
// This source file is part of the Perfect.org open source project
//
// Copyright (c) 2017 - 2018 PerfectlySoft Inc. and the Perfect project authors
// Licensed under Apache License v2.0
//
// See http://perfect.org/licensing.html for license information
//
//===----------------------------------------------------------------------===//
//
#ifndef __PERFECT_SPNEGO__
#define __PERFECT_SPNEGO__

#ifdef __APPLE__
#include <GSS/GSS.h>
#else
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_generic.h>
#include <gssapi/gssapi_krb5.h>
#define GSS_C_NT_USER_NAME gss_nt_user_name
#define GSS_C_NT_HOSTBASED_SERVICE gss_nt_service_name
#define GSS_KRB5_NT_PRINCIPAL_NAME gss_nt_krb5_name
#endif

/// TOKEN: (length: size_t, value: void *)
typedef gss_buffer_desc TOKEN;

/// connect application server to KDC
/// - parameters:
///   - service_name: service name listed in keytab, such as `HTTP@host.somewhere`
///   - debug: non-zero for printing debug info
/// - returns:
///   a ticket for application server
void * kinit(const char * service_name, int debug);

/// disconnect application server from KDC
/// - parameters:
///   - ticket: a ticket returned from kinit()
void kdestroy(void * ticket);

/// accept a challenge
/// - parameters:
///   - ticket: a ticket returned from kinit()
///   - token: client token
///   - usr: if success, krequest() will return the user name in this token
///   - reply: if need, the server should send back to the client with this reply. Could be null.
///   - debug: non-zero for printing debug info
int krequest(void * ticket, TOKEN token, TOKEN * usr, TOKEN * reply, int debug);

/// for testing purposes, ignored for production server
int klogin(const char * service_name, TOKEN * output_token, int debug);

/// base 64 decoder, please use your own base64 decoder if need
/// - parameters:
///   - token: input buffer
///   - output_token: output buffer
void base64_decode(TOKEN token, TOKEN * output_token);

/// base 64 encoder, please use your own base64 encoder if need
/// - parameters:
///   - token: input buffer
///   - output_token: output buffer
void base64_encode(TOKEN token, TOKEN * output_token);

/// release the token allocated.
/// - parameters:
///   - token: token to release
void kfree(TOKEN token);
#endif
