// The MIT License (MIT)
// WinHTTP Wrapper 1.0.3
// Copyright (C) 2020 - 2022, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

// version 1.0.3: Set the text regardless the http status, not just for HTTP OK 200

#ifndef WinHttpWrapper_H
#define WinHttpWrapper_H

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#define VU_INET_ENABLED
#endif

#ifdef VU_INET_ENABLED

#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "3rdparty/WHW/Types.h"

namespace WinHttpWrapper
{
	class HttpRequest
	{
	public:
		HttpRequest(
			const std::wstring& domain,
			int port,
			bool secure,
			const std::wstring& user_agent = L"WinHttpClient",
			const std::wstring& proxy_username = L"",
			const std::wstring& proxy_password = L"",
			const std::wstring& server_username = L"",
			const std::wstring& server_password = L"");

		virtual ~HttpRequest();

		bool Get(const std::wstring& rest_of_path,
			const std::wstring& requestHeader,
			HttpResponse& response);
		bool Post(const std::wstring& rest_of_path,
			const std::wstring& requestHeader,
			const std::string& body,
			HttpResponse& response);
		bool Put(const std::wstring& rest_of_path,
			const std::wstring& requestHeader,
			const std::string& body,
			HttpResponse& response);
		bool Delete(const std::wstring& rest_of_path,
			const std::wstring& requestHeader,
			const std::string& body,
			HttpResponse& response);

	private:
		// Request is wrapper around http()
		bool Request(
			const std::wstring& verb,
			const std::wstring& rest_of_path,
			const std::wstring& requestHeader,
			const std::string& body,
			HttpResponse& response);
		static bool http(
			const std::wstring& verb, const std::wstring& user_agent, const std::wstring& domain,
			const std::wstring& rest_of_path, int port, bool secure,
			const std::wstring& requestHeader, const std::string& body,
			std::string& text, std::wstring& responseHeader,
			DWORD& statusCode, std::wstring& error,
			const std::wstring& szProxyUsername, const std::wstring& szProxyPassword,
			const std::wstring& szServerUsername, const std::wstring& szServerPassword);

		static DWORD ChooseAuthScheme(DWORD dwSupportedSchemes);

		std::wstring m_Domain;
		int m_Port;
		bool m_Secure;
		std::wstring m_UserAgent;
		std::wstring m_ProxyUsername;
		std::wstring m_ProxyPassword;
		std::wstring m_ServerUsername;
		std::wstring m_ServerPassword;
	};

}

#endif // VU_INET_ENABLED

#endif // WinHttpWrapper_H