#pragma once
#define CURL_STATICLIB
#include <list>
#include <vector>
#include <string>
#include "curl.h"
#include "easy.h"
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib,"msvcrt.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "normaliz.lib")

class ccurl
{
	bool m_init = false;

	static size_t writedata(void* _data, size_t _count, size_t _size, void* _buffer)
	{
		char* data = reinterpret_cast<char*>(_data);
		size_t length = _count * _size;
		std::vector<char>* buffer = reinterpret_cast<std::vector<char>*>(_buffer);
		buffer->insert(buffer->end(), data, data + length);
		return length;
	}
	static size_t writestring(void* _data, size_t _count, size_t _size, void* _buffer)
	{
		char* data = reinterpret_cast<char*>(_data);
		size_t length = _count * _size;
		std::string* buffer = reinterpret_cast<std::string*>(_buffer);
		buffer->insert(buffer->end(), data, data + length);
		return length;
	}

public:
	using PFWriteData = size_t(*)(void* _data, size_t _count, size_t _size, void* _buffer);

	ccurl() { m_init = curl_global_init(CURL_GLOBAL_WIN32) == CURLE_OK; }

	bool isInit() const { return m_init; }

	bool get(const std::string& url, void* response, PFWriteData response_call, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		if (!m_init) { if (status) *status = CURLE_FAILED_INIT; return false; }
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
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_call);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, connectTimeout);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);

		code = curl_easy_perform(curl);
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		curl_easy_cleanup(curl);
		return true;
	}
	bool get(const std::string& url, std::vector<char>& response, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		response.clear();
		return get(url, &response, writedata, status, connectTimeout, header);
	}
	bool get(const std::string& url, std::string& response, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		response.clear();
		return get(url, &response, writestring, status, connectTimeout, header);
	}

	bool post(const std::string& url, const std::string postdata, void* response, PFWriteData response_call, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		if (!m_init) { if (status) *status = CURLE_FAILED_INIT; return false; }
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
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_call);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, connectTimeout);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);

		code = curl_easy_perform(curl);
		if (code != CURLE_OK) { if (status) *status = code; return false; }

		curl_easy_cleanup(curl);
		return true;
	}
	bool post(const std::string& url, const std::string postdata, std::vector<char>& response, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		return post(url, postdata, &response, writedata, status, connectTimeout, header);
	}
	bool post(const std::string& url, const std::string postdata, std::string& response, CURLcode* status = nullptr, int connectTimeout = 5000, const std::list<std::string>* header = nullptr)
	{
		return post(url, postdata, &response, writestring, status, connectTimeout, header);
	}

	~ccurl()
	{
		if (m_init) curl_global_cleanup();
	}
};