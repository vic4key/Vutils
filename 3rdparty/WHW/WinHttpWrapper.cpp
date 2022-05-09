// The MIT License (MIT)
// WinHTTP Wrapper 1.0.3
// Copyright (C) 2020 - 2022, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

// version 1.0.3: Set the text regardless the http status, not just for HTTP OK 200

#include "WinHttpWrapper.h"
#include <winhttp.h>
#pragma comment(lib, "Winhttp.lib")

#pragma warning(push)
#pragma warning(disable: 4267)

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif // !WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY

bool WinHttpWrapper::HttpRequest::Get(
	const std::wstring& rest_of_path,
	const std::wstring& requestHeader,
	HttpResponse& response)
{
	static const std::wstring verb = L"GET";
	static std::string body;
	return Request(
		verb,
		rest_of_path,
		requestHeader,
		body,
		response);
}

bool WinHttpWrapper::HttpRequest::Post(
	const std::wstring& rest_of_path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	static const std::wstring verb = L"POST";
	return Request(
		verb,
		rest_of_path,
		requestHeader,
		body,
		response);
}

bool WinHttpWrapper::HttpRequest::Put(
	const std::wstring& rest_of_path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	static const std::wstring verb = L"PUT";
	return Request(
		verb,
		rest_of_path,
		requestHeader,
		body,
		response);
}

bool WinHttpWrapper::HttpRequest::Delete(
	const std::wstring& rest_of_path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	static const std::wstring verb = L"DELETE";
	return Request(
		verb,
		rest_of_path,
		requestHeader,
		body,
		response);
}

bool WinHttpWrapper::HttpRequest::Request(
	const std::wstring& verb,
	const std::wstring& rest_of_path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	return http(verb, m_UserAgent, m_Domain,
		rest_of_path, m_Port, m_Secure,
		requestHeader, body,
		response.text, response.header,
		response.status, response.error,
		m_ProxyUsername, m_ProxyPassword,
		m_ServerUsername, m_ServerPassword);
}


bool WinHttpWrapper::HttpRequest::http(const std::wstring& verb, const std::wstring& user_agent, const std::wstring& domain,
	const std::wstring& rest_of_path, int port, bool secure,
	const std::wstring& requestHeader, const std::string& body,
	std::string& text, std::wstring& responseHeader, DWORD& dwStatusCode, std::wstring& error,
	const std::wstring& szProxyUsername, const std::wstring& szProxyPassword,
	const std::wstring& szServerUsername, const std::wstring& szServerPassword)
{
	DWORD dwSupportedSchemes;
	DWORD dwFirstScheme;
	DWORD dwSelectedScheme;
	DWORD dwTarget;
	DWORD dwLastStatus = 0;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	BOOL  bResults = FALSE;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	BOOL bDone = FALSE;
	DWORD dwProxyAuthScheme = 0;

	dwStatusCode = 0;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(user_agent.c_str(),
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
	{
		hConnect = WinHttpConnect(hSession, domain.c_str(), port, 0);
	}
	else
	{
		error = L"WinHttpOpen fails!";
		return false;
	}

	// Create an HTTP request handle.
	if (hConnect)
	{
		DWORD flag = secure ? WINHTTP_FLAG_SECURE : 0;
		hRequest = WinHttpOpenRequest(hConnect, verb.c_str(), rest_of_path.c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_REFRESH | flag);
	}
	else
	{
		WinHttpCloseHandle(hSession);
		error = L"WinHttpConnect fails!";
		return false;
	}

	if (hRequest == NULL)
		bDone = TRUE;

	while (!bDone)
	{
		//  If a proxy authentication challenge was responded to, reset
		//  those credentials before each SendRequest, because the proxy  
		//  may require re-authentication after responding to a 401 or  
		//  to a redirect. If you don't, you can get into a 
		//  407-401-407-401- loop.
		if (dwProxyAuthScheme != 0 && szProxyUsername != L"")
		{
			bResults = WinHttpSetCredentials(hRequest,
				WINHTTP_AUTH_TARGET_PROXY,
				dwProxyAuthScheme,
				szProxyUsername.c_str(),
				szProxyPassword.c_str(),
				NULL);
			if (!bResults)
			{
				error = L"WinHttpSetCredentials fails!";
			}
		}

		// Send a request.
		if (hRequest)
		{
			if (requestHeader.empty())
			{
				bResults = WinHttpSendRequest(hRequest,
					WINHTTP_NO_ADDITIONAL_HEADERS, 0,
					(LPVOID)body.data(), body.size(),
					body.size(), 0);
			}
			else
			{
				bResults = WinHttpSendRequest(hRequest,
					requestHeader.c_str(), requestHeader.size(),
					(LPVOID)body.data(), body.size(),
					body.size(), 0);
			}
			if (!bResults)
			{
				error = L"WinHttpSendRequest fails!";
			}
		}

		// End the request.
		if (bResults)
		{
			bResults = WinHttpReceiveResponse(hRequest, NULL);
			if (!bResults)
			{
				error = L"WinHttpReceiveResponse fails!";
			}
		}

		// Resend the request in case of 
		// ERROR_WINHTTP_RESEND_REQUEST error.
		if (!bResults && GetLastError() == ERROR_WINHTTP_RESEND_REQUEST)
			continue;

		// Check the status code.
		if (bResults)
		{
			dwSize = sizeof(dwStatusCode);
			bResults = WinHttpQueryHeaders(hRequest,
				WINHTTP_QUERY_STATUS_CODE |
				WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX,
				&dwStatusCode,
				&dwSize,
				WINHTTP_NO_HEADER_INDEX);

			if (!bResults)
			{
				error = L"WinHttpQueryHeaders fails!";
			}

			// Get response header
			WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
				WINHTTP_HEADER_NAME_BY_INDEX, NULL,
				&dwSize, WINHTTP_NO_HEADER_INDEX);

			// Allocate memory for the buffer.
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				responseHeader.resize(dwSize + 1);

				// Now, use WinHttpQueryHeaders to retrieve the header.
				bResults = WinHttpQueryHeaders(hRequest,
					WINHTTP_QUERY_RAW_HEADERS_CRLF,
					WINHTTP_HEADER_NAME_BY_INDEX,
					(LPVOID) responseHeader.data(), &dwSize,
					WINHTTP_NO_HEADER_INDEX);
			}

		}

		// Keep checking for data until there is nothing left.
		if (bResults)
		{
			std::string temp;
			text = "";
			do
			{
				// Check for available data.
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					error = L"Error in WinHttpQueryDataAvailable: ";
				error += std::to_wstring(GetLastError());

				if (dwSize == 0)
					break;

				// Allocate space for the buffer.
				temp = "";
				temp.resize(dwSize);
				// Read the data.
				ZeroMemory((void*)(&temp[0]), dwSize);
				if (!WinHttpReadData(hRequest, (LPVOID)(&temp[0]),
					dwSize, &dwDownloaded))
				{
					error = L"Error in WinHttpReadData: ";
					error += std::to_wstring(GetLastError());
				}
				else
				{
					text += temp;
				}
			} 
			while (dwSize > 0);

			switch (dwStatusCode)
			{
			default:
				bDone = TRUE;
				break;
			case 401:
				// The server requires authentication.
				//printf(" The server requires authentication. Sending credentials...\n");

				// Obtain the supported and preferred schemes.
				bResults = WinHttpQueryAuthSchemes(hRequest,
					&dwSupportedSchemes,
					&dwFirstScheme,
					&dwTarget);

				if (!bResults)
				{
					error = L"WinHttpQueryAuthSchemes in case 401 fails!";
				}

				// Set the credentials before resending the request.
				if (bResults)
				{
					dwSelectedScheme = ChooseAuthScheme(dwSupportedSchemes);

					if (dwSelectedScheme == 0)
						bDone = TRUE;
					else
					{
						bResults = WinHttpSetCredentials(hRequest,
							dwTarget,
							dwSelectedScheme,
							szServerUsername.c_str(),
							szServerPassword.c_str(),
							NULL);
						if (!bResults)
						{
							error = L"WinHttpSetCredentials in case 401 fails!";
						}
					}
				}

				// If the same credentials are requested twice, abort the
				// request.  For simplicity, this sample does not check
				// for a repeated sequence of status codes.
				if (dwLastStatus == 401)
					bDone = TRUE;

				break;

			case 407:
				// The proxy requires authentication.
				//printf("The proxy requires authentication.  Sending credentials...\n");

				// Obtain the supported and preferred schemes.
				bResults = WinHttpQueryAuthSchemes(hRequest,
					&dwSupportedSchemes,
					&dwFirstScheme,
					&dwTarget);

				if (!bResults)
				{
					error = L"WinHttpQueryAuthSchemes in case 407 fails!";
				}


				// Set the credentials before resending the request.
				if (bResults)
					dwProxyAuthScheme = ChooseAuthScheme(dwSupportedSchemes);

				// If the same credentials are requested twice, abort the
				// request.  For simplicity, this sample does not check 
				// for a repeated sequence of status codes.
				if (dwLastStatus == 407)
					bDone = TRUE;
				break;
			}
		}

		// Keep track of the last status code.
		dwLastStatus = dwStatusCode;

		// If there are any errors, break out of the loop.
		if (!bResults)
			bDone = TRUE;
	}

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	// Report any errors.
	if (!bResults)
		return false;

	return true;
}

DWORD WinHttpWrapper::HttpRequest::ChooseAuthScheme(DWORD dwSupportedSchemes)
{
	//  It is the server's responsibility only to accept 
	//  authentication schemes that provide a sufficient
	//  level of security to protect the servers resources.
	//
	//  The client is also obligated only to use an authentication
	//  scheme that adequately protects its username and password.
	//
	//  Thus, this sample code does not use Basic authentication  
	//  because Basic authentication exposes the client's username
	//  and password to anyone monitoring the connection.

	if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE)
		return WINHTTP_AUTH_SCHEME_NEGOTIATE;
	else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NTLM)
		return WINHTTP_AUTH_SCHEME_NTLM;
	else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT)
		return WINHTTP_AUTH_SCHEME_PASSPORT;
	else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST)
		return WINHTTP_AUTH_SCHEME_DIGEST;
	else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_BASIC)
		return WINHTTP_AUTH_SCHEME_BASIC;
	else
		return 0;
}

#pragma warning(pop)