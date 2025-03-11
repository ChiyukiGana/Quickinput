#pragma once
#include <src/tools/msgbox.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qevent.h>
#include <curl/ccurl.h>
#include <sstream>
#include <regex>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
class QiUpdate
{
    // format: YYYY-MM-DD_N  2024-1-11_1
    const int current_date, current_count;
    int latest_date, latest_count;
    ccurl curl;
    bool response_state;
    std::string header;
    std::string url;
    std::string response;
    std::string version;
    std::string content;
    std::thread thread;
    QWidget* parent;
public:
    QiUpdate(QWidget* parent, int current_date, int current_count) : parent(parent), current_date(current_date), current_count(current_count), latest_date(0), latest_count(0),
        url(R"(https://api.github.com/repos/ChiyukiGana/Quickinput/releases/latest)"),
        header(R"(user-agent:Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0)")
    {
        response_state = false;
        if (!good()) return;
    }
    bool good() const { return curl.good(); }
    bool response_good() const { return curl.good() && response_state; }
    bool parse(std::string version)
    {
        if (version.empty()) return false;
        std::string t;
        std::string date;
        std::string count;
        std::smatch match;
        if (std::regex_match(version, match, std::regex(R"((\d{4})-(\d{1,2})-(\d{1,2}))")))
        {
            date = match[1].str();
            t = match[2].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
            t = match[3].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
        }
        else if (std::regex_match(version, match, std::regex(R"((\d{4})-(\d{1,2})-(\d{1,2})_(\d+))")))
        {
            date = match[1].str();
            t = match[2].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
            t = match[3].str(); if (t.size() == 1) t = t.insert(0, 1, '0'); date += t;
            count = match[4].str();
        }
        else return false;
        int id = 0, ic = 0;
        std::stringstream sd;
        if (!date.empty())
        {
            sd << date;
            sd >> id;
        }
        sd.clear();
        if (!count.empty())
        {
            sd << count;
            sd >> ic;
        }
        latest_date = id, latest_count = ic;
        return true;
    }
    bool getlatest()
    {
        if (!good()) return false;
        thread = std::thread
        (
            [this]
            {
                bool result = curl.get(url, response, nullptr, 10000, &std::list<std::string>(1, header));
                QApplication::postEvent(parent, new QEvent(QEvent::Type::User));
                response_state = (result && !response.empty());
            }
        );
        return true;
    }
    bool parselatest()
    {
        if (!good()) return false;
        QJsonDocument json(QJsonDocument::fromJson(response.c_str()));
        QJsonObject obj(json.object());
        version = obj.value("tag_name").toString().toUtf8();
        content = obj.value("body").toString().toUtf8();
        return parse(version);
    }
    bool compare()
    {
        if (!good()) false;
        if (latest_date > current_date) return true;
        else if ((latest_date == current_date) && (latest_count > current_count)) return true;
        return false;
    }
    bool check(std::string& version, std::string& content)
    {
        if (!good()) return false;
        if (!parselatest()) return false;
        if (compare())
        {
            version = this->version, content = this->content;
            return true;
        }
        return false;
    }
};