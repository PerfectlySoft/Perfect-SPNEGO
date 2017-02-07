//
//  cspnego.c
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
#include "cspnego.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#define szSTR 256

/// base64 encode table, Acknowledgement: Sean Starkey <sean@seanstarkey.com>
const char encodeCharacterTable[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// base64 decode table, Acknowledgement: Sean Starkey <sean@seanstarkey.com>
const char decodeCharacterTable[256] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
	,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1
};

/// release the token allocated.
/// - parameters:
///   - token: token to release
void kfree(TOKEN token) { free(token.value); }

/// base 64 encoder, please use your own base64 encoder if need. Acknowledgement: Sean Starkey <sean@seanstarkey.com>
/// - parameters:
///   - token: input buffer
///   - output_token: output buffer
void base64_encode(TOKEN token, TOKEN * output_token)
{
	char buff1[3];
	char buff2[4];
	unsigned char i=0, j;
	unsigned input_cnt=0;
	unsigned output_cnt=0;
  char * input = token.value;
  size_t size = token.length * 2;
  char * output = (char*)malloc(size);
  memset(output, 0, size);
	while(input_cnt<token.length)
	{
		buff1[i++] = input[input_cnt++];
		if (i==3)
		{
			output[output_cnt++] = encodeCharacterTable[(buff1[0] & 0xfc) >> 2];
			output[output_cnt++] = encodeCharacterTable[((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4)];
			output[output_cnt++] = encodeCharacterTable[((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6)];
			output[output_cnt++] = encodeCharacterTable[buff1[2] & 0x3f];
			i=0;
		}
	}
	if (i)
	{
		for(j=i;j<3;j++)
		{
			buff1[j] = '\0';
		}
		buff2[0] = (buff1[0] & 0xfc) >> 2;
		buff2[1] = ((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4);
		buff2[2] = ((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6);
		buff2[3] = buff1[2] & 0x3f;
		for (j=0;j<(i+1);j++)
		{
			output[output_cnt++] = encodeCharacterTable[buff2[j]];
		}
		while(i++<3)
		{
			output[output_cnt++] = '=';
		}
	}
	output[output_cnt] = 0;
  if (output_token) {
    output_token->value = output;
    output_token->length = output_cnt;
  }//end if
}

/// base 64 decoder, please use your own base64 decoder if need.  Acknowledgement: Sean Starkey <sean@seanstarkey.com>
/// - parameters:
///   - token: input buffer
///   - output_token: output buffer
void base64_decode(TOKEN token, TOKEN * output_token)
{
	char buff1[4];
	char buff2[4];
	unsigned char i=0, j;
	unsigned input_cnt=0;
	unsigned output_cnt=0;
  unsigned char * input = token.value;
  unsigned char * output = (unsigned char *)malloc(token.length);
	while(input_cnt<token.length)
	{
		buff2[i] = input[input_cnt++];
		if (buff2[i] == '=')
		{
			break;
		}
		if (++i==4)
		{
			for (i=0;i!=4;i++)
			{
				buff2[i] = decodeCharacterTable[buff2[i]];
			}
			output[output_cnt++] = (char)((buff2[0] << 2) + ((buff2[1] & 0x30) >> 4));
			output[output_cnt++] = (char)(((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2));
			output[output_cnt++] = (char)(((buff2[2] & 0x3) << 6) + buff2[3]);
			i=0;
		}
	}
	if (i)
	{
		for (j=i;j<4;j++)
		{
			buff2[j] = '\0';
		}
		for (j=0;j<4;j++)
		{
			buff2[j] = decodeCharacterTable[buff2[j]];
		}
		buff1[0] = (buff2[0] << 2) + ((buff2[1] & 0x30) >> 4);
		buff1[1] = ((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2);
		buff1[2] = ((buff2[2] & 0x3) << 6) + buff2[3];
		for (j=0;j<(i-1); j++)
		{
			output[output_cnt++] = (char)buff1[j];
		}
	}
	output[output_cnt] = 0;
  if (output_token) {
    output_token->value = output;
    output_token->length = output_cnt;
  }//end if
}

/// print the gss debug info
void gss_debug(OM_uint32 err_maj, OM_uint32 err_min)
{
  OM_uint32 maj_stat, min_stat;
  OM_uint32 msg_ctx = 0;
  gss_buffer_desc status_string;
  char *err_msg;

  fprintf(stderr,
          "GSS-API major_status:%8.8x, minor_status:%8.8x",
          err_maj, err_min);

  do {
    maj_stat = gss_display_status (&min_stat,
                                   err_maj,
                                   GSS_C_GSS_CODE,
                                   GSS_C_NO_OID,
                                   &msg_ctx,
                                   &status_string);
    if (!GSS_ERROR(maj_stat)) {
      fprintf(stderr, "\nTYPE ERR: %s\n", (char*) status_string.value, NULL);
      gss_release_buffer(&min_stat, &status_string);
    }
  } while (!GSS_ERROR(maj_stat) && msg_ctx != 0);

  msg_ctx = 0;
  do {
    maj_stat = gss_display_status (&min_stat,
                                   err_min,
                                   GSS_C_MECH_CODE,
                                   GSS_C_NULL_OID,
                                   &msg_ctx,
                                   &status_string);
    if (!GSS_ERROR(maj_stat)) {
      fprintf(stderr, "\nMECH ERR: %s\n", (char*) status_string.value, NULL);
      gss_release_buffer(&min_stat, &status_string);
    }
  } while (!GSS_ERROR(maj_stat) && msg_ctx != 0);
}

/// connect application server to KDC
/// - parameters:
///   - service_name: service name listed in keytab, such as `HTTP@host.somewhere`
///   - debug: non-zero for printing debug info
/// - returns:
///   a ticket for application server
void * kinit(const char * service_name, int debug) {
	if (!service_name) return 0;
  OM_uint32 mi, mj;
  gss_buffer_desc token = GSS_C_EMPTY_BUFFER;
  token.value = strdup(service_name);
  token.length = strlen(token.value) + 1;
  gss_name_t name = GSS_C_NO_NAME;

	// import the service name
  mj = gss_import_name(&mi, &token, GSS_C_NT_HOSTBASED_SERVICE, &name);
  if (mj != GSS_S_COMPLETE) {
    if (debug) gss_debug(mj, mi);
    gss_release_buffer(&mi, &token);
    return 0;
  }//end if
  gss_release_buffer(&mi, &token);

  gss_cred_id_t cred;

	// get credential from KDC
  mj = gss_acquire_cred(&mi, name, GSS_C_INDEFINITE, GSS_C_NO_OID_SET, GSS_C_ACCEPT, &cred, 0, 0);
  gss_release_name(&mi, &name);
  if (mj != GSS_S_COMPLETE) {
    if (debug) gss_debug(mj, mi);
    gss_release_name(&mi, &name);
    return 0;
  }//end if
  return (void *)cred;
}

/// disconnect application server from KDC
/// - parameters:
///   - ticket: a ticket returned from kinit()
void kdestroy(void * ticket) {
	if (!ticket) return;
  OM_uint32 mi = 0;
  gss_cred_id_t cred = (gss_cred_id_t)ticket;
  gss_release_cred(&mi, &cred);
}//end kdestroy

/// accept a challenge
/// - parameters:
///   - ticket: a ticket returned from kinit()
///   - token: client token
///   - usr: if success, krequest() will return the user name in this token
///   - reply: if need, the server should send back to the client with this reply. Could be null.
///   - debug: non-zero for printing debug info
int krequest(void * ticket, TOKEN token, TOKEN * usr, TOKEN * reply, int debug) {

	if (!ticket || !token.value || token.length < 1) return -1;

  if(debug) fprintf(stderr, "\nREQ: %zu\n", token.length);
  gss_ctx_id_t context = GSS_C_NO_CONTEXT;
  OM_uint32 mi, mj, flags = 0;
  gss_cred_id_t server_creds = (gss_cred_id_t)ticket;
	gss_buffer_desc output_token;
	memset(&output_token, 0, sizeof(sizeof(gss_buffer_desc)));
  gss_name_t client_name = GSS_C_NO_NAME;

	// authenticate challenge by KDC
  mj = gss_accept_sec_context(&mi, &context, server_creds, &token,
    GSS_C_NO_CHANNEL_BINDINGS, &client_name, NULL, &output_token,
		&flags, NULL, NULL);
	if (mj != GSS_S_COMPLETE) {
		if (debug) gss_debug(mj, mi);
		return -2;
	}//end if

	// get client name
	mj = gss_display_name(&mi, client_name, usr, NULL);
	if (mj != GSS_S_COMPLETE) {
		if (debug) gss_debug(mj, mi);
		return -3;
	}//end if
	gss_release_name(&mi, &client_name);

	// prepare token back to client
	if (output_token.length > 0) {
		base64_encode(output_token, reply);
		gss_release_buffer(&mi, &output_token);
	}//end if
  return 0;
}//end krequest

/// for testing purposes, ignored for production server
int klogin(const char * service_name, TOKEN * output_token, int debug) {

	if (!service_name || !output_token) return -1;

  OM_uint32 mi, mj;
  gss_buffer_desc token = GSS_C_EMPTY_BUFFER;
  token.value = strdup(service_name);
  token.length = strlen(token.value) + 1;
  gss_name_t name = GSS_C_NO_NAME;
  mj = gss_import_name(&mi, &token, GSS_C_NT_HOSTBASED_SERVICE, &name);
	//free(token.value);
  gss_ctx_id_t context = GSS_C_NO_CONTEXT;
  mj = gss_init_sec_context(&mi, GSS_C_NO_CREDENTIAL, &context, name, GSS_C_NO_OID, 0, 0,
     GSS_C_NO_CHANNEL_BINDINGS, GSS_C_NO_BUFFER, NULL, output_token, 0, 0);
  if (mj != GSS_S_COMPLETE || output_token->length < 1) {
   if (debug) gss_debug(mj, mi);
   return -2;
  }//end if
  gss_release_name(&mi, &name);
  return 0;
}//end klogin
