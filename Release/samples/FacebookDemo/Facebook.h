/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Facebook.h - Simple client for connecting to facebook. See blog post 
* at http://aka.ms/FacebookCppRest for a detailed walkthrough of this sample
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#pragma once
#include <string>
#include <cpprest/http_client.h>

class facebook_client {
public:
	static facebook_client& instance(); // Singleton
	pplx::task<void> login(utility::string_t scopes);
	pplx::task<web::json::value> get(utility::string_t path);
	web::http::uri_builder base_uri(bool absolute = false);

private:
	facebook_client(): 
	raw_client(U("https://graph.facebook.com/")),
	signed_in(false) {}

	pplx::task<void> full_login(utility::string_t scopes);

	utility::string_t token_;
	bool signed_in;
	web::http::client::http_client raw_client;
};
