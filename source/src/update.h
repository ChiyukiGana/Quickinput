﻿#pragma once
#include <src/tools/msgbox.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qevent.h>
#include <curl/ccurl.h>
#include <sstream>
#include <regex>
#include <thread>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
class QiUpdate
{
    // format: YYYY-MM-DD_N  2024-1-11_1
    int current_date = 0;
    int current_count = 0;
    int latest_date = 0;
    int latest_count = 0;
    ccurl curl;
    bool response_state = false;
    const std::string header = R"(user-agent:Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0)";
    const std::string url = R"(https://api.github.com/repos/ChiyukiGana/Quickinput/releases/latest)";
    std::string response;
    std::string version;
    std::string content;
    std::thread thread;
    QWidget* parent;
public:
    QiUpdate(QWidget* parent, int current_date, int current_count)
    {
        this->parent = parent;
        this->current_date = current_date;
        this->current_count = current_count;
    }
    QiUpdate(QWidget* parent, const QString version)
    {
        this->parent = parent;
        parse(version.toStdString(), current_date, current_count);
    }
    static bool parse(const std::string& version, int& r_date, int& r_count)
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
    bool good() const { return curl.good() && current_date; }
    bool response_good() const { return curl.good() && response_state; }
    bool getlatest()
    {
        if (!good()) return false;
        thread = std::thread
        (
            [this]
            {
                std::list<std::string> headerList(1, header);
                bool result = curl.get(url, response, nullptr, 10000, &headerList);
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
        version = obj.value("tag_name").toString().toStdString();
        content = obj.value("body").toString().toStdString();
        return parse(version, latest_date, latest_count);
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