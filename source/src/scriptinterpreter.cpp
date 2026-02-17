#include "scriptinterpreter.h"
#include "inc_header.h"

time_t sys_last_time = 0;
time_t sys_last_clock = 0;
std::mutex sys_last_mutex;

bool SetSystemTime_UnixMs(time_t msec)
{
	ULONGLONG totalTicks = 0x019DB1DED53E8000ULL + msec * 10000;
	FILETIME ft = {};
	ft.dwLowDateTime = (DWORD)(totalTicks & 0xFFFFFFFF);
	ft.dwHighDateTime = (DWORD)(totalTicks >> 32);
	SYSTEMTIME st;
	if (FileTimeToSystemTime(&ft, &st)) return SetSystemTime(&st);
	return false;
}

time_t TimeToS(time_t hour, time_t min, time_t sec, time_t msec)
{
	return (hour * 3600) + (min * 60) + sec + (msec / 1000);
}

time_t TimeToMs(time_t hour, time_t min, time_t sec, time_t msec)
{
	return (hour * 3600000) + (min * 60000) + (sec * 1000) + msec;
}

time_t DateTimeToUnixMs(time_t hour, time_t min, time_t sec, time_t msec, time_t year, time_t mon, time_t day)
{
	std::time_t now = std::time(nullptr);
	std::tm* timeinfo = std::localtime(&now);
	timeinfo->tm_hour = hour;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;
	if (year != 0) timeinfo->tm_year = year - 1900;
	if (mon != 0) timeinfo->tm_mon = mon - 1;
	if (day != 0) timeinfo->tm_mday = day;
	return std::mktime(timeinfo) * 1000 + msec;
}

struct QiFunc_date : public QiFunc
{
	QiFunc_date() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return QiVar::toString(m.tm_year + 1900) + std::string("-") + QiVar::toString(m.tm_mon + 1) + std::string("-") + QiVar::toString(m.tm_mday);
	}
};
struct QiFunc_time : public QiFunc
{
	QiFunc_time() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return QiVar::toString(m.tm_hour) + std::string(":") + QiVar::toString(m.tm_min) + std::string(":") + QiVar::toString(m.tm_sec);
	}
};
struct QiFunc_datetime : public QiFunc
{
	QiFunc_datetime() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return QiVar::toString(m.tm_year + 1900) + std::string("-") + QiVar::toString(m.tm_mon + 1) + std::string("-") + QiVar::toString(m.tm_mday) + std::string(" ") + QiVar::toString(m.tm_hour) + std::string(":") + QiVar::toString(m.tm_min) + std::string(":") + QiVar::toString(m.tm_sec);
	}
};
struct QiFunc_hourtime : public QiFunc
{
	QiFunc_hourtime() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.empty())
		{
			time_t s = time(nullptr);
			tm m;
			localtime_s(&m, &s);
			return QiVar::toString(m.tm_hour) + std::string(":") + QiVar::toString(m.tm_min) + std::string(":") + QiVar::toString(m.tm_sec);
		}
		time_t s = args[0].toInteger();
		return QiVar::toString(s / 3600) + std::string(":") + QiVar::toString((s % 3600) / 60) + std::string(":") + QiVar::toString(s % 60);
	}
};
struct QiFunc_mintime : public QiFunc
{
	QiFunc_mintime() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.empty())
		{
			time_t s = time(nullptr);
			tm m;
			localtime_s(&m, &s);
			return QiVar::toString(m.tm_min) + std::string(":") + QiVar::toString(m.tm_sec);
		}
		time_t s = args[0].toInteger();
		return QiVar::toString(s / 60) + std::string(":") + QiVar::toString(s % 60);
	}
};
struct QiFunc_time_s : public QiFunc
{
	QiFunc_time_s() : QiFunc(0, 4) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.empty()) return time(nullptr);
		return TimeToS(args[0].toInteger(),
			args.size() > 1 ? args[1].toInteger() : 0,
			args.size() > 2 ? args[2].toInteger() : 0,
			args.size() > 3 ? args[3].toInteger() : 0
		);
	}
};
struct QiFunc_time_ms : public QiFunc
{
	QiFunc_time_ms() : QiFunc(0, 4) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.empty()) return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		return TimeToMs(args[0].toInteger(),
			args.size() > 1 ? args[1].toInteger() : 0,
			args.size() > 2 ? args[2].toInteger() : 0,
			args.size() > 3 ? args[3].toInteger() : 0
		);
	}
};
struct QiFunc_time_y : public QiFunc
{
	QiFunc_time_y() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_year + 1900;
	}
};
struct QiFunc_time_yd : public QiFunc
{
	QiFunc_time_yd() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_yday;
	}
};
struct QiFunc_time_m : public QiFunc
{
	QiFunc_time_m() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_mon + 1;
	}
};
struct QiFunc_time_w : public QiFunc
{
	QiFunc_time_w() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		if (m.tm_wday == 0) m.tm_wday = 7;
		return m.tm_wday;
	}
};
struct QiFunc_time_d : public QiFunc
{
	QiFunc_time_d() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_mday;
	}
};
struct QiFunc_time_dh : public QiFunc
{
	QiFunc_time_dh() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_hour;
	}
};
struct QiFunc_time_dm : public QiFunc
{
	QiFunc_time_dm() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_min;
	}
};
struct QiFunc_time_ds : public QiFunc
{
	QiFunc_time_ds() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		time_t s = args.empty() ? time(nullptr) : static_cast<time_t>(args[0].toInteger());
		tm m;
		localtime_s(&m, &s);
		return m.tm_sec;
	}
};
struct QiFunc_time_rms : public QiFunc
{
	QiFunc_time_rms() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return clock();
	}
};
struct QiFunc_time_set_s : public QiFunc
{
	QiFunc_time_set_s() : QiFunc(0, 6) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		std::unique_lock<std::mutex> lock(sys_last_mutex);
		time_t msec;
		if (args.empty())
		{
			if (sys_last_time) msec = sys_last_time + clock() - sys_last_clock;
			else return true;
		}
		else
		{
			if (args.size() > 1)
			{
				msec = DateTimeToUnixMs(args[0].toInteger(),
					args.size() > 1 ? args[1].toInteger() : 0,
					args.size() > 2 ? args[2].toInteger() : 0,
					0,
					args.size() > 3 ? args[3].toInteger() : 0,
					args.size() > 4 ? args[4].toInteger() : 0,
					args.size() > 5 ? args[5].toInteger() : 0
				);
			}
			else
			{
				msec = args[0].toInteger() * 1000;
			}
			if (!sys_last_time) sys_last_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), sys_last_clock = clock();
		}
		return SetSystemTime_UnixMs(msec);
	}
};
struct QiFunc_time_set_ms : public QiFunc
{
	QiFunc_time_set_ms() : QiFunc(0, 7) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		std::unique_lock<std::mutex> lock(sys_last_mutex);
		time_t msec;
		if (args.empty())
		{
			if (sys_last_time) msec = sys_last_time + clock() - sys_last_clock;
			else return true;
		}
		else
		{
			if (args.size() > 1)
			{
				msec = DateTimeToUnixMs(args[0].toInteger(),
					args.size() > 1 ? args[1].toInteger() : 0,
					args.size() > 2 ? args[2].toInteger() : 0,
					args.size() > 3 ? args[3].toInteger() : 0,
					args.size() > 4 ? args[4].toInteger() : 0,
					args.size() > 5 ? args[5].toInteger() : 0,
					args.size() > 6 ? args[6].toInteger() : 0
				);
			}
			else
			{
				msec = args[0].toInteger();
			}
			if (!sys_last_time) sys_last_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), sys_last_clock = clock();
		}
		return SetSystemTime_UnixMs(msec);
	}
};

const char* ntp_servers[] = { "ntp.aliyun.com", "ntp.tencent.com", "ntp.ntsc.ac.cn", "pool.ntp.org" };
struct QiFunc_time_ntp_s : public QiFunc
{
	QiFunc_time_ntp_s() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		NtpClient ntp;
		if (args.empty())
		{
			for (auto& s : ntp_servers)
			{
				auto t = ntp.unix(s, 123, 500);
				if (t) return *t;
			}
		}
		else
		{
			auto t = ntp.unix(args[0].toString(), 123, 500);
			if (t) return *t;
		}
		return QiVar();
	}
};
struct QiFunc_time_ntp_ms : public QiFunc
{
	QiFunc_time_ntp_ms() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		NtpClient ntp;
		if (args.empty())
		{
			for (auto& s : ntp_servers)
			{
				auto t = ntp.unix64(s, 123, 500);
				if (t) return *t;
			}
		}
		else
		{
			auto t = ntp.unix64(args[0].toString(), 123, 500);
			if (t) return *t;
		}
		return QiVar();
	}
};

struct QiFunc_exist : public QiFunc
{
	QiFunc_exist() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter* inter, QiVarMap* global, QiVarMap* local, QiWorker*) const override
	{
		return inter->exist(args[0].toString(), local);
	}
};
struct QiFunc_value : public QiFunc
{
	QiFunc_value() : QiFunc(1, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter* inter, QiVarMap* global, QiVarMap* local, QiWorker*) const override
	{
		if (args.size() > 1)
		{
			inter->setValue(args[0].toString(), args[1]);
			return args[1];
		}
		return inter->value(args[0].toString());
	}
};
struct QiFunc_type : public QiFunc
{
	QiFunc_type() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].type_name();
	}
};

struct QiFunc_is_null : public QiFunc
{
	QiFunc_is_null() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].isNull();
	}
};
struct QiFunc_is_int : public QiFunc
{
	QiFunc_is_int() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].isInteger();
	}
};
struct QiFunc_is_num : public QiFunc
{
	QiFunc_is_num() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].isNumber();
	}
};
struct QiFunc_is_str : public QiFunc
{
	QiFunc_is_str() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].isString();
	}
};

struct QiFunc_str : public QiFunc
{
	QiFunc_str() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].toString();
	}
};
struct QiFunc_num : public QiFunc
{
	QiFunc_num() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].toNumber();
	}
};
struct QiFunc_int : public QiFunc
{
	QiFunc_int() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].toInteger();
	}
};
struct QiFunc_len : public QiFunc
{
	QiFunc_len() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return args[0].len();
	}
};

struct QiFunc_char : public QiFunc
{
	QiFunc_char() : QiFunc(2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar::ch(args[0].toString(), args[1].toInteger());
	}
};
struct QiFunc_rmn : public QiFunc
{
	QiFunc_rmn() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar::removeNums(args[0].toString());
	}
};
struct QiFunc_rmc : public QiFunc
{
	QiFunc_rmc() : QiFunc(1, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.size() == 2) return QiVar::removeChars(args[0].toString(), args[1].toString());
		return QiVar::removeChars(args[0].toString());
	}
};
struct QiFunc_rms : public QiFunc
{
	QiFunc_rms() : QiFunc(1, ~size_t(0)) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.size() == 1) return QiVar::removeChars(args[0].toString());
		std::string result = args[0].toString();
		for (size_t i = 1; i < args.size(); i++) result = QiVar::removeStr(result, args[i].toString());
		return result;
	}
};
struct QiFunc_sub : public QiFunc
{
	QiFunc_sub() : QiFunc(2, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar::sub(args[0].toString(), args[1].toInteger(), args.size() > 2 ? args[2].toInteger() : ~size_t(0));
	}
};
struct QiFunc_subx : public QiFunc
{
	QiFunc_subx() : QiFunc(2, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar::subx(args[0].toString(), args[1].toInteger(), args.size() > 2 ? args[2].toInteger() : ~size_t(0));
	}
};
struct QiFunc_find : public QiFunc
{
	QiFunc_find() : QiFunc(2, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		size_t i = args[0].toString().find(args[1].toString(), args.size() > 2 ? args[2].toInteger() : 0);
		return i == std::string::npos ? -1ll : static_cast<long long>(i);
	}
};
struct QiFunc_replace : public QiFunc
{
	QiFunc_replace() : QiFunc(2, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar::replace_u8(args[0].toString(), args[1].toString(), args.size() > 2 ? args[2].toString() : std::string(""));
	}
};

struct QiFunc_sleep : public QiFunc
{
	QiFunc_sleep() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker* worker) const override
	{
		long long ms = args[0].toInteger();
		worker ? worker->sleep(ms) : Sleep(ms);
		return QiVar();
	}
};
struct QiFunc_exit : public QiFunc
{
	QiFunc_exit() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker* worker) const override
	{
		if (worker) worker->m_stop = true;
		return bool(worker);
	}
};
struct QiFunc_rand : public QiFunc
{
	QiFunc_rand() : QiFunc(1, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[std::string("rand_last")] = random((args[0].toInteger()), args.size() > 1 ? (args[1].toInteger()) : 0);
	}
	static long long random(long long max, long long min = 0)
	{
		if (min > max) std::swap(min, max);
		return min + (rand() % (max - min + 1));
	}
};
struct QiFunc_rand_last : public QiFunc
{
	QiFunc_rand_last() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_rand_last];
	}
};
struct QiFunc_count : public QiFunc
{
	QiFunc_count() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_count];
	}
};
struct QiFunc_index : public QiFunc
{
	QiFunc_index() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_index];
	}
};

struct QiFunc_cur_to : public QiFunc
{
	QiFunc_cur_to() : QiFunc(2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		SIZE screen = System::screenSize();
		double x = args[0].toNumber();
		double y = args[1].toNumber();

		bool x_int = QiVar::isInteger(x);
		bool y_int = QiVar::isInteger(y);

		if (x_int && y_int)
		{
			SetCursorPos(int(x), int(y));
		}
		else
		{
			Input::MoveToA(x_int ? ((float)x / (float)screen.cx * 65535.0f) : (x * 65535), y_int ? ((float)y / (float)screen.cy * 65535.0f) : (y * 65535), Qi::key_info);
		}
		return QiVar();
	}
};
struct QiFunc_cur_move : public QiFunc
{
	QiFunc_cur_move() : QiFunc(2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		SIZE screen = System::screenSize();
		double x = args[0].toNumber();
		double y = args[1].toNumber();
		int ix;
		int iy;
		if (QiVar::isInteger(x)) ix = x;
		else ix = x * (double)screen.cx;
		if (QiVar::isInteger(y)) iy = y;
		else iy = y * (double)screen.cx;
		Input::Move(ix, iy, Qi::key_info);
		return QiVar();
	}
};
struct QiFunc_cur_x : public QiFunc
{
	QiFunc_cur_x() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		POINT pt;
		GetCursorPos(&pt);
		return QiVar(pt.x);
	}
};
struct QiFunc_cur_y : public QiFunc
{
	QiFunc_cur_y() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		POINT pt;
		GetCursorPos(&pt);
		return QiVar(pt.y);
	}
};
struct QiFunc_cur_last_x : public QiFunc
{
	QiFunc_cur_last_x() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_cur_last_x];
	}
};
struct QiFunc_cur_last_y : public QiFunc
{
	QiFunc_cur_last_y() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_cur_last_y];
	}
};
struct QiFunc_cur_last_ax : public QiFunc
{
	QiFunc_cur_last_ax() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_cur_last_ax];
	}
};
struct QiFunc_cur_last_ay : public QiFunc
{
	QiFunc_cur_last_ay() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_cur_last_ay];
	}
};

struct QiFunc_scr_cx : public QiFunc
{
	QiFunc_scr_cx() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar(static_cast<long long>(System::screenSize().cx));
	}
};
struct QiFunc_scr_cy : public QiFunc
{
	QiFunc_scr_cy() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar(static_cast<long long>(System::screenSize().cy));
	}
};

struct QiFunc_pop : public QiFunc
{
	QiFunc_pop() : QiFunc(1, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		Qi::popText->Popup(args.size() > 1 ? args[1].toInteger() : 1000, QString::fromStdString(args[0].toString()));
		if (args.size() > 2 && args[2].toBool()) Sleep(args[1].toInteger());
		else Sleep(1);
		return args[0];
	}
};
struct QiFunc_text_box : public QiFunc
{
	enum
	{
		normal,
		warning,
		error
	};

	QiFunc_text_box() : QiFunc(1, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		std::string text(args[0].toString());
		std::string title("MessageBox"); if (args.size() > 1) title = args[1].toString();
		DWORD style = MB_YESNO | MB_TOPMOST;
		if (args.size() > 2)
		{
			if (args[2].toInteger() == warning) style |= MB_ICONWARNING;
			else if (args[2].toInteger() == error) style |= MB_ICONERROR;
		}
		return MessageBoxW(nullptr, String::toWString(text).c_str(), String::toWString(title).c_str(), style) == IDYES;
	}
};
struct QiFunc_edit_box : public QiFunc
{
	QiFunc_edit_box() : QiFunc(0, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		std::string title("TextEditBox"); if (args.size() > 0) title = args[0].toString();
		std::string text; if (args.size() > 1) text = args[1].toString();
		return TextEditBox(nullptr, String::toWString(title).c_str(), String::toWString(text).c_str(), args.size() > 2 ? args[2].toBool() : false, WS_EX_TOPMOST, Qi::ico);
	}
};

struct QiFunc_wnd_find : public QiFunc
{
	QiFunc_wnd_find() : QiFunc(1, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		struct EnumParam
		{
			std::wstring name;
			std::wstring clas;
			std::vector<HWND> wnd;
		} param;

		param.name = String::toWString(args[0].toString());
		size_t i = 0;

		if (args.size() > 1)
		{
			if (args[1].isString())
			{
				param.clas = String::toWString(args[1].toString());
				if (args.size() > 2) i = args[2].toInteger();
			}
			else i = args[1].toInteger();
		}

		EnumWindows([](HWND wnd, LPARAM lp) -> BOOL {
			EnumParam& param = *((EnumParam*)lp);

			wchar_t name[256];
			GetWindowTextW(wnd, name, 256);
			if (!param.name.empty() && param.name != name) return TRUE;

			GetClassNameW(wnd, name, 256);
			if (!param.clas.empty() && param.clas != name) return TRUE;

			param.wnd.push_back(wnd);
			return TRUE;
			}, (LPARAM)&param);

		if (param.wnd.size() <= i) return QiVar();

		std::sort(param.wnd.begin(), param.wnd.end());
		return QiVar::ptr_t(param.wnd[i]);
	}
};
struct QiFunc_wnd_find_other : public QiFunc
{
	QiFunc_wnd_find_other() : QiFunc(1, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		struct EnumParam
		{
			std::wstring name;
			std::wstring clas;
			std::vector<HWND> wnd;
			HWND current;
		} param;

		param.name = String::toWString(args[0].toString());
		size_t i = 0;

		if (args.size() > 1)
		{
			if (args[1].isString())
			{
				param.clas = String::toWString(args[1].toString());
				if (args.size() > 2) i = args[2].toInteger();
			}
			else i = args[1].toInteger();
		}

		param.current = GetForegroundWindow();
		EnumWindows([](HWND wnd, LPARAM lp) -> BOOL {
			EnumParam& param = *((EnumParam*)lp);
			if (wnd == param.current) return TRUE;

			wchar_t name[256];
			GetWindowTextW(wnd, name, 256);
			if (!param.name.empty() && param.name != name) return TRUE;

			GetClassNameW(wnd, name, 256);
			if (!param.clas.empty() && param.clas != name) return TRUE;

			param.wnd.push_back(wnd);
			return TRUE;
			}, (LPARAM)&param);

		if (param.wnd.size() <= i) return QiVar();

		std::sort(param.wnd.begin(), param.wnd.end());
		return QiVar::ptr_t(param.wnd[i]);
	}
};
struct QiFunc_wnd_search : public QiFunc
{
	QiFunc_wnd_search() : QiFunc(1, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		struct EnumParam
		{
			std::wstring name;
			std::wstring clas;
			std::vector<HWND> wnd;
		} param;

		param.name = String::toWString(args[0].toString());
		size_t i = 0;

		if (args.size() > 1)
		{
			if (args[1].isString())
			{
				param.clas = String::toWString(args[1].toString());
				if (args.size() > 2) i = args[2].toInteger();
			}
			else i = args[1].toInteger();
		}

		EnumWindows([](HWND wnd, LPARAM lp) -> BOOL {
			EnumParam& param = *((EnumParam*)lp);

			wchar_t name[256];
			GetWindowTextW(wnd, name, 256);
			if (!param.name.empty() && (std::wstring(name).find(param.name) == std::wstring::npos)) return TRUE;

			GetClassNameW(wnd, name, 256);
			if (!param.clas.empty() && (std::wstring(name).find(param.clas) == std::wstring::npos)) return TRUE;

			param.wnd.push_back(wnd);
			return TRUE;
			}, (LPARAM)&param);

		if (param.wnd.size() <= i) return QiVar();

		std::sort(param.wnd.begin(), param.wnd.end());
		return QiVar::ptr_t(param.wnd[i]);
	}
};
struct QiFunc_wnd_search_other : public QiFunc
{
	QiFunc_wnd_search_other() : QiFunc(1, 3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		struct EnumParam
		{
			std::wstring name;
			std::wstring clas;
			std::vector<HWND> wnd;
			HWND current;
		} param;

		param.name = String::toWString(args[0].toString());
		size_t i = 0;

		if (args.size() > 1)
		{
			if (args[1].isString())
			{
				param.clas = String::toWString(args[1].toString());
				if (args.size() > 2) i = args[2].toInteger();
			}
			else i = args[1].toInteger();
		}

		param.current = GetForegroundWindow();
		EnumWindows([](HWND wnd, LPARAM lp) -> BOOL {
			EnumParam& param = *((EnumParam*)lp);
			if (wnd == param.current) return TRUE;

			wchar_t name[256];
			GetWindowTextW(wnd, name, 256);
			if (!param.name.empty() && (std::wstring(name).find(param.name) == std::wstring::npos)) return TRUE;

			GetClassNameW(wnd, name, 256);
			if (!param.clas.empty() && (std::wstring(name).find(param.clas) == std::wstring::npos)) return TRUE;

			param.wnd.push_back(wnd);
			return TRUE;
			}, (LPARAM)&param);

		if (param.wnd.size() <= i) return QiVar();

		std::sort(param.wnd.begin(), param.wnd.end());
		QiVar::ptr_t(param.wnd[i]);
	}
};
struct QiFunc_wnd_open : public QiFunc
{
	QiFunc_wnd_open() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);

		ShowWindow(wnd, SW_SHOWNORMAL);
		SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetForegroundWindow(wnd);
		SetFocus(wnd);

		return QiVar(true);
	}
};
struct QiFunc_wnd_close : public QiFunc
{
	QiFunc_wnd_close() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		return QiVar(CloseWindow(wnd));
	}
};
struct QiFunc_wnd_show : public QiFunc
{
	QiFunc_wnd_show() : QiFunc(1, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		return QiVar(ShowWindow(wnd, args.size() > 1 ? args[1].toBool() : SW_SHOW));
	}
};
struct QiFunc_wnd_top : public QiFunc
{
	QiFunc_wnd_top() : QiFunc(1, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		bool top = args.size() > 1 ? args[1].toBool() : true;
		if (top) return QiVar(SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) && SetWindowPos(wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
		return QiVar(SetWindowPos(wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
	}
};
struct QiFunc_wnd_opacity : public QiFunc
{
	QiFunc_wnd_opacity() : QiFunc(1, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		size_t opacty = args.size() > 1 ? args[1].toNumber() * 255 : 255;
		if (opacty > 255) opacty = 255;

		DWORD style = GetWindowLongW(wnd, GWL_EXSTYLE);
		if (!(style & WS_EX_LAYERED))
		{
			if (opacty == 255) return QiVar(true);
			SetWindowLongW(wnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
		}
		return QiVar(SetLayeredWindowAttributes(wnd, 0, opacty, LWA_ALPHA));
	}
};
struct QiFunc_wnd_pos : public QiFunc
{
	QiFunc_wnd_pos() : QiFunc(3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		return QiVar(SetWindowPos(wnd, nullptr, args[1].toInteger(), args[2].toInteger(), 0, 0, SWP_NOSIZE));
	}
};
struct QiFunc_wnd_size : public QiFunc
{
	QiFunc_wnd_size() : QiFunc(3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		return QiVar(SetWindowPos(wnd, nullptr, 0, 0, args[1].toInteger(), args[2].toInteger(), SWP_NOMOVE));
	}
};
struct QiFunc_wnd_exist : public QiFunc
{
	QiFunc_wnd_exist() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		return QiVar((bool)IsWindow(wnd));
	}
};
struct QiFunc_wnd_current : public QiFunc
{
	QiFunc_wnd_current() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar::ptr_t(GetForegroundWindow());
	}
};
struct QiFunc_wnd_visible : public QiFunc
{
	QiFunc_wnd_visible() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		HWND wnd = (HWND)args[0].toPointer();
		if (!IsWindow(wnd)) return QiVar(false);
		return QiVar((bool)IsWindowVisible(wnd));
	}
};

struct QiFunc_dir_create : public QiFunc
{
	QiFunc_dir_create() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return CreateDirectoryW(args[0].toWString().c_str(), NULL) != FALSE;
	}
};
struct QiFunc_dir_remove : public QiFunc
{
	QiFunc_dir_remove() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return std::filesystem::remove_all(args[0].toString());
	}
};
struct QiFunc_dir_exist : public QiFunc
{
	QiFunc_dir_exist() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return File::FolderState(String::toWString(args[0].toString()));
	}
};

struct QiFunc_file_read : public QiFunc
{
	QiFunc_file_read() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		QByteArray data;
		if (File::LoadText(args[0].toString().c_str(), data)) return QiVar(QString::fromUtf8(data).toStdString());
		return QiVar();
	}
};
struct QiFunc_file_write : public QiFunc
{
	QiFunc_file_write() : QiFunc(2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return File::SaveText(args[0].toString().c_str(), args[1].toString().c_str());
	}
};
struct QiFunc_file_exist : public QiFunc
{
	QiFunc_file_exist() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return File::FileState(String::toWString(args[0].toString()));
	}
};
struct QiFunc_file_remove : public QiFunc
{
	QiFunc_file_remove() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return bool(DeleteFileW(String::toWString(args[0].toString()).c_str()));
	}
};
struct QiFunc_file_open : public QiFunc
{
	QiFunc_file_open() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return Process::open(String::toWString(args[0].toString()));
	}
};

struct QiFunc_csv_rows : public QiFunc
{
	QiFunc_csv_rows() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return (long long)CsvTool::rows(args[0].toString());
	}
};
struct QiFunc_csv_cols : public QiFunc
{
	QiFunc_csv_cols() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return (long long)CsvTool::cols(args[0].toString());
	}
};
struct QiFunc_csv_read : public QiFunc
{
	QiFunc_csv_read() : QiFunc(3) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return CsvTool::read(args[0].toString(), args[1].toInteger(), args[2].toInteger());
	}
};
struct QiFunc_csv_write : public QiFunc
{
	QiFunc_csv_write() : QiFunc(4) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		long long row = args[1].toInteger();
		long long col = args[2].toInteger();
		if (row < 0 || col < 0) return std::string();
		return CsvTool::write(args[0].toString(), row, col, args[3].toString());
	}
};

struct QiFunc_clip_read : public QiFunc
{
	QiFunc_clip_read() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar(String::toString(System::ClipBoardText()));
	}
};
struct QiFunc_clip_write : public QiFunc
{
	QiFunc_clip_write() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return QiVar(System::ClipBoardText(String::toWString(args[0].toString()).c_str()));
	}
};

struct QiFunc_macro_name : public QiFunc
{
	QiFunc_macro_name() : QiFunc(0) {}
	QiVar exec(const std::vector<QiVar>&, QiScriptInterpreter*, QiVarMap*, QiVarMap* local, QiWorker*) const override
	{
		return (*local)[QiScriptInterpreter::var_macro_name];
	}
};
struct QiFunc_macro_active : public QiFunc
{
	QiFunc_macro_active() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		Macro* macro = QiFn::FindMacro(QString::fromStdString(args[0].toString()));
		if (macro) return macro->thread.active();
		return false;
	}
};
struct QiFunc_macro_start : public QiFunc
{
	QiFunc_macro_start() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		Macro* macro = QiFn::FindMacro(QString::fromStdString(args[0].toString()));
		if (macro)
		{
			macro->thread.run_start(macro);
			return true;
		}
		return false;
	}
};
struct QiFunc_macro_stop : public QiFunc
{
	QiFunc_macro_stop() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		Macro* macro = QiFn::FindMacro(QString::fromStdString(args[0].toString()));
		if (macro)
		{
			macro->thread.stop();
			return true;
		}
		return false;
	}
};

struct QiFunc_proc_find : public QiFunc
{
	QiFunc_proc_find() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return static_cast<long long>(Process::find(args[0].toWString()));
	}
};
struct QiFunc_proc_close : public QiFunc
{
	QiFunc_proc_close() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		if (args.empty()) ExitProcess(0);
		return static_cast<long long>(Process::close(args[0].toWString()));
	}
};

struct QiFunc_volume : public QiFunc
{
	QiFunc_volume() : QiFunc(0, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		return (double)Sound::SpeakerVolume(args.size() > 0 ? args[0].toInteger() : 10, args.size() > 1 ? args[1].toBool() : false);
	}
};
struct QiFunc_power : public QiFunc
{
	enum
	{
		lock,
		signout,
		shutdown,
		reboot,
	};

	QiFunc_power() : QiFunc(0, 1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		BOOL result = FALSE;
		int op = lock;
		if (args.empty());
		else if (args[0].isInteger()) op = args[0].toInteger();
		else
		{
			if (args[0].toString() == "lock") op = lock;
			else if (args[0].toString() == "sign") op = signout;
			else if (args[0].toString() == "shutdown") op = shutdown;
			else if (args[0].toString() == "reboot") op = reboot;
		}

		if (op > lock && op <= reboot)
		{
			HANDLE hToken;
			TOKEN_PRIVILEGES tkp = {};

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return QiVar(false);

			LookupPrivilegeValueW(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

			if (GetLastError() != ERROR_SUCCESS) return QiVar(false);
		}

		switch (op)
		{
		case lock: result = LockWorkStation(); break;
		case signout: result = ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_MAJOR_APPLICATION); break;
		case shutdown: result = ExitWindowsEx(EWX_POWEROFF, SHTDN_REASON_MAJOR_APPLICATION); break;
		case reboot: result = ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION); break;
		}
		return result;
	}
};
struct QiFunc_cmd : public QiFunc
{
	QiFunc_cmd() : QiFunc(1) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		std::wstring output;
		System::ExecuteCmd(String::toWString(args[0].toString()), output);
		return output;
	}
};

/*
0: use time file name
1: 1: use file path
2: 1: file(use time file name for null), 2: folder
*/
struct QiFunc_capture_screen : public QiFunc
{
	QiFunc_capture_screen() : QiFunc(0, 2) {}
	QiVar exec(const std::vector<QiVar>& args, QiScriptInterpreter*, QiVarMap*, QiVarMap*, QiWorker*) const override
	{
		std::wstring file;
		if (args.empty())
		{
			file = File::Unique("capture", QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"), ".png").toStdWString();
			CreateDirectoryW(L"capture", NULL);
		}
		else if (args.size() == 1)
		{
			file = args[0].toWString();
		}
		else if (args.size() == 2)
		{
			if (args[0].isNull()) file = File::Unique(QString::fromStdString(args[1].toString()), QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"), ".png").toStdWString();
			else file = args[1].toWString() + L"\\" + args[0].toWString();
		}

		if (file.empty()) return false;

		CImage image = Image::ScreenCImage24();
		if (SUCCEEDED(image.Save(file.c_str())))
		{
			image.ReleaseDC();
			return true;
		}
		image.ReleaseDC();
		return false;
	}
};

QiFuncMap::QiFuncMap()
{
	insert({ "date", std::make_unique<QiFunc_date>() });
	insert({ "time", std::make_unique<QiFunc_time>() });
	insert({ "datetime", std::make_unique<QiFunc_datetime>() });
	insert({ "hourtime", std::make_unique<QiFunc_hourtime>() });
	insert({ "mintime", std::make_unique<QiFunc_mintime>() });
	insert({ "time_s", std::make_unique<QiFunc_time_s>() });
	insert({ "time_ms", std::make_unique<QiFunc_time_ms>() });
	insert({ "time_ys", std::make_unique<QiFunc_time_s>() });
	insert({ "time_y", std::make_unique<QiFunc_time_y>() });
	insert({ "time_yd", std::make_unique<QiFunc_time_yd>() });
	insert({ "time_m", std::make_unique<QiFunc_time_m>() });
	insert({ "time_w", std::make_unique<QiFunc_time_w>() });
	insert({ "time_d", std::make_unique<QiFunc_time_d>() });
	insert({ "time_dh", std::make_unique<QiFunc_time_dh>() });
	insert({ "time_dm", std::make_unique<QiFunc_time_dm>() });
	insert({ "time_ds", std::make_unique<QiFunc_time_ds>() });
	insert({ "time_rms", std::make_unique<QiFunc_time_rms>() });
	insert({ "time_set_s", std::make_unique<QiFunc_time_set_s>() });
	insert({ "time_set_ms", std::make_unique<QiFunc_time_set_ms>() });
	insert({ "time_ntp_s", std::make_unique<QiFunc_time_ntp_s>() });
	insert({ "time_ntp_ms", std::make_unique<QiFunc_time_ntp_ms>() });

	insert({ "exist", std::make_unique<QiFunc_exist>() });
	insert({ "value", std::make_unique<QiFunc_value>() });
	insert({ "type", std::make_unique<QiFunc_type>() });

	insert({ "is_null", std::make_unique<QiFunc_is_null>() });
	insert({ "is_int", std::make_unique<QiFunc_is_int>() });
	insert({ "is_num", std::make_unique<QiFunc_is_num>() });
	insert({ "is_str", std::make_unique<QiFunc_is_str>() });

	insert({ "str", std::make_unique<QiFunc_str>() });
	insert({ "num", std::make_unique<QiFunc_num>() });
	insert({ "int", std::make_unique<QiFunc_int>() });
	insert({ "len", std::make_unique<QiFunc_len>() });

	insert({ "char", std::make_unique<QiFunc_char>() });
	insert({ "rmn", std::make_unique<QiFunc_rmn>() });
	insert({ "rmc", std::make_unique<QiFunc_rmc>() });
	insert({ "rms", std::make_unique<QiFunc_rms>() });
	insert({ "sub", std::make_unique<QiFunc_sub>() });
	insert({ "subx", std::make_unique<QiFunc_subx>() });
	insert({ "find", std::make_unique<QiFunc_find>() });
	insert({ "replace", std::make_unique<QiFunc_replace>() });

	insert({ "sleep", std::make_unique<QiFunc_sleep>() });
	insert({ "exit", std::make_unique<QiFunc_exit>() });
	insert({ "close", std::make_unique<QiFunc_proc_close>() });
	insert({ "rand", std::make_unique<QiFunc_rand>() });
	insert({ "rand_last", std::make_unique<QiFunc_rand_last>() });
	insert({ "count", std::make_unique<QiFunc_count>() });
	insert({ "index", std::make_unique<QiFunc_index>() });

	insert({ "cur_to", std::make_unique<QiFunc_cur_to>() });
	insert({ "cur_move", std::make_unique<QiFunc_cur_move>() });
	insert({ "cur_x", std::make_unique<QiFunc_cur_x>() });
	insert({ "cur_y", std::make_unique<QiFunc_cur_y>() });
	insert({ "cur_last_x", std::make_unique<QiFunc_cur_last_x>() });
	insert({ "cur_last_y", std::make_unique<QiFunc_cur_last_y>() });
	insert({ "cur_last_ax", std::make_unique<QiFunc_cur_last_ax>() });
	insert({ "cur_last_ay", std::make_unique<QiFunc_cur_last_ay>() });

	insert({ "scr_cx", std::make_unique<QiFunc_scr_cx>() });
	insert({ "scr_cy", std::make_unique<QiFunc_scr_cy>() });

	insert({ "pop", std::make_unique<QiFunc_pop>() });
	insert({ "text_box", std::make_unique<QiFunc_text_box>() });
	insert({ "edit_box", std::make_unique<QiFunc_edit_box>() });

	insert({ "wnd_find", std::make_unique<QiFunc_wnd_find>() });
	insert({ "wnd_find_other", std::make_unique<QiFunc_wnd_find_other>() });
	insert({ "wnd_search", std::make_unique<QiFunc_wnd_search>() });
	insert({ "wnd_search_other", std::make_unique<QiFunc_wnd_search_other>() });
	insert({ "wnd_open", std::make_unique<QiFunc_wnd_open>() });
	insert({ "wnd_close", std::make_unique<QiFunc_wnd_close>() });
	insert({ "wnd_show", std::make_unique<QiFunc_wnd_show>() });
	insert({ "wnd_top", std::make_unique<QiFunc_wnd_top>() });
	insert({ "wnd_opacty", std::make_unique<QiFunc_wnd_opacity>() });
	insert({ "wnd_opacity", std::make_unique<QiFunc_wnd_opacity>() });
	insert({ "wnd_alpha", std::make_unique<QiFunc_wnd_opacity>() });
	insert({ "wnd_pos", std::make_unique<QiFunc_wnd_pos>() });
	insert({ "wnd_size", std::make_unique<QiFunc_wnd_size>() });
	insert({ "wnd_exist", std::make_unique<QiFunc_wnd_exist>() });
	insert({ "wnd_current", std::make_unique<QiFunc_wnd_current>() });
	insert({ "wnd_visible", std::make_unique<QiFunc_wnd_visible>() });

	insert({ "dir_create", std::make_unique<QiFunc_dir_create>() });
	insert({ "dir_remove", std::make_unique<QiFunc_dir_remove>() });
	insert({ "dir_exist", std::make_unique<QiFunc_dir_remove>() });

	insert({ "file_read", std::make_unique<QiFunc_file_read>() });
	insert({ "file_write", std::make_unique<QiFunc_file_write>() });
	insert({ "file_exist", std::make_unique<QiFunc_file_exist>() });
	insert({ "file_remove", std::make_unique<QiFunc_file_remove>() });
	insert({ "file_open", std::make_unique<QiFunc_file_open>() });

	insert({ "csv_rows", std::make_unique<QiFunc_csv_rows>() });
	insert({ "csv_cols", std::make_unique<QiFunc_csv_cols>() });
	insert({ "csv_read", std::make_unique<QiFunc_csv_read>() });
	insert({ "csv_write", std::make_unique<QiFunc_csv_write>() });

	insert({ "clip_read", std::make_unique<QiFunc_clip_read>() });
	insert({ "clip_write", std::make_unique<QiFunc_clip_write>() });

	insert({ "macro_name", std::make_unique<QiFunc_macro_name>() });
	insert({ "macro_active", std::make_unique<QiFunc_macro_active>() });
	insert({ "macro_start", std::make_unique<QiFunc_macro_start>() });
	insert({ "macro_stop", std::make_unique<QiFunc_macro_stop>() });

	insert({ "proc_find", std::make_unique<QiFunc_proc_find>() });
	insert({ "proc_close", std::make_unique<QiFunc_proc_close>() });

	insert({ "volume", std::make_unique<QiFunc_volume>() });
	insert({ "power", std::make_unique<QiFunc_power>() });
	insert({ "cmd", std::make_unique<QiFunc_cmd>() });

	insert({ "capture_screen", std::make_unique<QiFunc_capture_screen>() });
}