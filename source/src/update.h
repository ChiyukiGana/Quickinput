#pragma once
#ifdef Q_UPDATE
#include <qapplication.h>
#include <qwidget.h>
#include <qevent.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <regex>
#include <thread>
#include <sstream>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib/httplib.h"
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")
class QiUpdate
{
	// format: YYYY-MM-DD_N  2024-1-11_1
	QWidget* parent = nullptr;
	bool once = false;
	int m_current_date{};
	int m_current_count{};
	std::string m_version;
	std::string m_content;
	std::unique_ptr<httplib::Client> http;

	static bool parseVersion(const std::string& version, int& r_date, int& r_count)
	{
		if (version.empty()) return false;
		std::string t;
		std::string date;
		std::string count;
		std::smatch match;
		if (std::regex_match(version, match, std::regex(R"((\d{4})-(\d{1,2})-(\d{1,2})(\D*))")))
		{
			date = match[1].str();
			t = match[2].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
			t = match[3].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
		}
		else if (std::regex_match(version, match, std::regex(R"((\d{4})-(\d{1,2})-(\d{1,2})_(\d+)(\D*))")))
		{
			date = match[1].str();
			t = match[2].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
			t = match[3].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
			count = match[4].str();
		}
		else return false;
		int id = 0, ic = 0;
		if (!date.empty())
		{
			try { id = std::stoi(date); }
			catch (...) { id = 0; }
		}
		if (!count.empty())
		{
			try { ic = std::stoi(count); }
			catch (...) { ic = 0; }
		}
		r_date = id, r_count = ic;
		return true;
	}
	static bool parseResponse(const std::string& response, int& r_date, int& r_count, std::string& r_version, std::string& r_content)
	{
		if (response.empty()) return false;
		QJsonDocument json(QJsonDocument::fromJson(response.c_str()));
		if (json.isNull()) return false;
		QJsonObject obj(json.object());
		if (obj.isEmpty()) return false;
		std::string version = obj.value("tag_name").toString().toStdString();
		if (version.empty()) return false;
		std::string content = obj.value("body").toString().toStdString();
		if (content.empty()) return false;
		if (!parseVersion(version, r_date, r_count)) return false;
		r_version = version;
		r_content = content;
		return true;
	}
	void setCurrent(int current_date, int current_count)
	{
		m_current_date = current_date;
		m_current_count = current_count;
	}
	void setCurrent(const QString& version)
	{
		parseVersion(version.toStdString(), m_current_date, m_current_count);
	}
	void init()
	{
		if (http) return;
		auto* _http = new httplib::Client("https://api.github.com");
		if (_http->is_valid())
		{
			http.reset(_http);
			return;
		}
		else delete _http;
	}
	bool valid() const { return http && m_current_date; }
public:
	QiUpdate(QWidget* parent, int current_date, int current_count)
	{
		this->parent = parent;
		setCurrent(current_date, current_count);
		init();
	}
	QiUpdate(QWidget* parent, const QString& version)
	{
		this->parent = parent;
		setCurrent(version);
		init();
	}

	bool getlatest()
	{
		if (!valid()) return false;
		if (once) return false;
		std::thread([this] {
			httplib::Headers headers{ std::make_pair("user-agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0") };
			auto result = http->Get("https://api.github.com/repos/ChiyukiGana/Quickinput/releases/latest", headers);
			if (result.error() != httplib::Error::Success) return;
			int latest_date{};
			int latest_count{};
			if (!parseResponse(result.value().body, latest_date, latest_count, m_version, m_content)) return;
			if ((latest_date > m_current_date) || (latest_date == m_current_date && latest_count > m_current_count)) QApplication::postEvent(parent, new QEvent(QEvent::Type::User));
			}).detach();
		once = true;
		return true;
	}
	std::string version() const { return m_version; }
	std::string content() const { return m_content; }
};
#endif