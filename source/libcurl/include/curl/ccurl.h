#pragma once
#define CURL_STATICLIB
#include <list>
#include <string>
#include "curl.h"
#include "easy.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "normaliz.lib")
#ifdef _DEBUG
#pragma comment(lib, "msvcrtd.lib")
#else
#pragma comment(lib, "msvcrt.lib")
#endif
#pragma comment(lib, "source/libcurl/lib/libcurl.lib")

class ccurl
{
	bool failed;

	static size_t writedata(void* data, size_t count, size_t size, void* buffer)
	{
		if (data && buffer && count && size)
		{
			size_t datasize = count * size;
			std::string* pstring = (std::string*)buffer;
			pstring->append((const char*)data, datasize);
			return datasize;
		}
		return 0;
	}

public:
	ccurl()
	{
		failed = curl_global_init(CURL_GLOBAL_WIN32) == CURLE_FAILED_INIT;
	}

	bool good() const { return !failed; }

	bool get(const std::string& url, std::string& response, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		if (failed) { if (status) *status = CURLE_FAILED_INIT; return false; }
		CURLcode code = CURLE_OK;

		CURL* curl = curl_easy_init();
		if (!curl) { if (status) *status = CURLE_FAILED_INIT; return false; }

		code = curl_easy_setopt(curl, CURLOPT_POST, false);
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		if (code != CURLE_OK) { if (status) *status = code; return false; }
		
		if (header && header->size())
		{
			curl_slist* headerlist = nullptr;
			for (auto i = header->begin(); i != header->end(); i++)
			{
				headerlist = curl_slist_append(headerlist, (*i).c_str());
			}
			if (headerlist)
			{
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
				if (code != CURLE_OK) { if (status) *status = code; return false; }
			}
		}

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, true);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, connectTimeout);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);

		code = curl_easy_perform(curl);
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		curl_easy_cleanup(curl);
		return true;
	}

	bool post(const std::string& url, const std::string postdata, std::string& response, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		if (failed) { if (status) *status = CURLE_FAILED_INIT; return false; }
		CURLcode code = CURLE_OK;

		CURL* curl = curl_easy_init();
		if (!curl) { if (status) *status = CURLE_FAILED_INIT; return false; }

		code = curl_easy_setopt(curl, CURLOPT_POST, true);
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		if (header && header->size())
		{
			curl_slist* headerlist = nullptr;
			for (auto i = header->begin(); i != header->end(); i++)
			{
				headerlist = curl_slist_append(headerlist, (*i).c_str());
			}
			if (headerlist)
			{
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
				if (code != CURLE_OK) { if (status) *status = code; return false; }
			}
		}

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, true);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, connectTimeout);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);

		code = curl_easy_perform(curl);
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		curl_easy_cleanup(curl);
		return true;
	}

	~ccurl()
	{
		if (!failed) curl_global_cleanup();
	}
};