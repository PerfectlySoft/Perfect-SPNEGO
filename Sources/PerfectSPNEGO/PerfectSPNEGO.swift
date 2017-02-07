//
//  PerfectSPNEGO.swift
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
import cspnego

/// Errors
public enum Exception : Error {
  /// invalid ticket - null or expired.
  case INVALID_TICKET
  /// invalid token - malformed or expired.
  case INVALID_TOKEN
  /// invalid user - unexpected null user
  case INVALID_USER
  /// other reasons.
  case UNKNOWN
}//end Exception

/// Base64 encoder / decoder
public class Base64 {

  /// encode a token to a string
  /// - parameters:
  ///   - token: a token with binary data
  /// - returns:
  ///   a base 64 encoded string
  public static func encode(token: TOKEN) throws -> String {
    var output = TOKEN()
    base64_encode(token, &output)
    guard let r = String(validatingUTF8: unsafeBitCast(output.value, to: UnsafePointer<CChar>.self)) else {
        throw Exception.INVALID_TOKEN
    }//end guard
    kfree(output)
    return r
  }//end encode

  /// decode a string to a token, note that the token MUST be released manually
  /// - parameters:
  ///   - string: the base64 string to decode
  /// - returns:
  ///   a token decoded from the string.
  public static func decode(string: String) -> TOKEN {
    return string.withCString { ptr -> TOKEN in
      let inp = TOKEN(length: string.utf8.count, value: unsafeBitCast(ptr, to: UnsafeMutableRawPointer.self))
      var oup = TOKEN()
      base64_decode(inp, &oup)
      return oup
    }
  }
}

/// generic server side protocol implementation class Spnego
public class Spnego {

  /// print debug info or note
  public static var debug: Bool = false

  /// gss credential
  internal var ticket: UnsafeMutableRawPointer?

  /// constructor of spnego
  /// - parameters:
  ///   service: service name listed in the application keytab file, MUST in form of `protocol@host`, such as `HTTP@host.where`
  public init(_ service: String) throws {
      ticket = service.withCString { kinit($0, Spnego.debug ? 1 : 0) }
      guard ticket != nil else {
        throw Exception.INVALID_TICKET
      }//end guard
  }//end init

  /// accept a challenge from client
  /// - parameters:
  ///   - base64Token: challenge string from client
  /// - returns:
  ///   (username, response) - a turple of authentication. username is the client name, response is the token to send back to the client
  public func accept(base64Token: String) throws -> (String, String?) {
    let input = Base64.decode(string: base64Token)
    var usr = TOKEN()
    var reply = TOKEN()
    let r = krequest(ticket, input, &usr, &reply, Spnego.debug ? 1 : 0)
    kfree(input)
    switch r {
    case -1: throw Exception.INVALID_TICKET
    case -2: throw Exception.INVALID_TOKEN
    case -3: throw Exception.INVALID_USER
    default:
      guard usr.length > 0 && usr.value != nil else {
        throw Exception.UNKNOWN
      }//end usr
    }
    guard let username = String(validatingUTF8: unsafeBitCast(usr.value, to: UnsafePointer<CChar>.self)) else {
      throw Exception.UNKNOWN
    }//end guard
    kfree(usr)
    if reply.length > 0 && reply.value != nil {
      let token = String(validatingUTF8: unsafeBitCast(reply.value, to: UnsafePointer<CChar>.self))
      kfree(reply)
      return (username, token)
    }//end if
    return (username, nil)
  }//end func

  deinit {
      kdestroy(ticket)
  }
}
