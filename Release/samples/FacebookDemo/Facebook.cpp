/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Facebook.cpp - Implementation file for simple facebook client. 
* Note: this implementation will not work until you replace the placeholder 
* strings below with tokens obtained from facebook.
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "pch.h"
#include "facebook.h"

using namespace pplx;
using namespace web;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::Storage;

// Placeholder strings for app tokens
// see blog post at http://aka.ms/FacebookCppRest
// for information on obtaining these values
const utility::string_t application_id(U("insert your application ID"));
const utility::string_t application_token(U("insert your application token"));

facebook_client& facebook_client::instance()
{
	static facebook_client c;
	return c;
}

pplx::task<void> facebook_client::login(utility::string_t scopes)
{
	// Look in the Local Settings for previously-stored login credentials
	auto ls = ApplicationData::Current->LocalSettings->CreateContainer("LoginDetailsCache",
		ApplicationDataCreateDisposition::Always);

	if(ls->Values->HasKey("facebookToken")) {
		utf16string tokenUtf16 = dynamic_cast<String^>(ls->Values->Lookup("facebookToken"))->Data();
		token_ = utility::conversions::to_string_t(token);
	}

	if(!token_.empty()) {
		// Check if the token is still valid
		using namespace http;

		uri_builder tokendbg_uri(U("/debug_token"));
		tokendbg_uri.append_query(U("input_token"), token_);
		tokendbg_uri.append_query(U("access_token"), application_token);

		http_request request(methods::GET);
		request.set_request_uri(tokendbg_uri.to_string());
		request.headers().add(header_names::accept, L"application/json");

		return raw_client.request(request)
		.then([](http_response response){
			return response.extract_json();
		}).then([=](json::value v) -> task<void> {
			auto obj = v[U("data")];

			if(obj[U("is_valid")].as_bool()) {
				// Currently cached access token is valid
				signed_in = true;
				return create_task([](){}); // Return an empty task to match the function's return value
			}

			// If the token was invalid, go through full login
			return full_login(scopes);
		}, task_continuation_context::use_current());
	}

	// If no token was found, go through full login
	return full_login(scopes);
}

pplx::task<void> facebook_client::full_login(utility::string_t scopes)
{
	// Create uri for OAuth login on Facebook
	http::uri_builder login_uri(U("https://www.facebook.com/dialog/oauth"));
	login_uri.append_query(U("client_id"), application_id); // App id
	login_uri.append_query(U("redirect_uri"), U("https://www.facebook.com/connect/login_success.html"));
	login_uri.append_query(U("scope"), scopes);
	login_uri.append_query(U("display"), U("popup"));
	login_uri.append_query(U("response_type"), U("token"));

	return create_task(WebAuthenticationBroker::AuthenticateAsync(
		WebAuthenticationOptions::None, ref new Uri(ref new String(utility::conversions::to_utf16string(login_uri.to_string()).c_str())),
		ref new Uri("https://www.facebook.com/connect/login_success.html")))
	.then([=](WebAuthenticationResult^ result) {
		if(result->ResponseStatus == WebAuthenticationStatus::Success)
		{
			signed_in = true;

			std::wstring response(result->ResponseData->Data());  // Save authentication token
			auto start = response.find(L"access_token=");
			start += 13;
			auto end = response.find('&');
			std::wstring tokenUtf16 = response.substr(start, end-start);

			token_ = utility::conversions::to_string_t(tokenUtf16);

			// Add token to Local Settings for future login attempts
			auto ls = ApplicationData::Current->LocalSettings->CreateContainer("LoginDetailsCache",
				ApplicationDataCreateDisposition::Always);

			ls->Values->Insert("facebookToken", ref new String(tokenUtf16.c_str()));
		}
	});
}

task<json::value> facebook_client::get(utility::string_t path)
{
	using namespace http;
	
	http_request request(methods::GET);

	request.set_request_uri(base_uri().append_path(path).to_uri());
	request.headers().add(header_names::accept, L"application/json");

	return raw_client.request(request)
	.then([](http_response response) {
		return response.extract_json();
	});
}

http::uri_builder facebook_client::base_uri(bool absolute)
{
	http::uri_builder ret;

	if(absolute)
		ret.set_path(U("https://graph.facebook.com"));

	ret.append_query(U("access_token"), token_);
	return ret;
}
