#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

inline const QString configFile = "QuickInput.json";
inline const QString macroPath = "macro\\";
inline const QString macroType = ".json";

struct DataRole
{
	enum
	{
		id = Qt::UserRole
	};
};

struct PopTextInfo
{
	QString t;
	QColor c;
};

// theme and text
namespace QiUi
{
	constexpr int event_restyle = QEvent::User + 1;

	struct Theme
	{
		QString name;
		QString style;
	};
	using Themes = List<Theme>;

	struct Text
	{
		QString syOn;
		QString syOff;
		QString syOk;
		QString syYes;
		QString syNot;
		QString syStop;
		QString syShow;
		QString syHide;
		QString syOption;
		QString syLink;
		QString syEdit;
		QString syUp;
		QString syDown;
		QString syTurn;
		QString syLeft;
		QString syTop;
		QString syRight;
		QString syMove;
		QString syTime;
		QString syText;
		QString syLoop;
		QString syColor;
		QString syImage;
		QString syJump;
		QString syPoint;

		QString muOn;
		QString muOff;
		QString muShow;
		QString muHide;
		QString muExit;
		QString acWait;
		QString acDown;
		QString acUp;
		QString acClick;
		QString acPos;
		QString acMove;
		QString acLoop;
		QString acText;
		QString acColor;
		QString acEnd;
		QString acEndLoop;
		QString acKeyState;
		QString acRecoverPos;
		QString acImage;
		QString acPopText;
		QString acRememberPos;
		QString acTimer;
		QString acJump;
		QString acJumpPoint;
		QString acDialog;
		QString acBlock;
		QString acBlockExec;

		QString trOn;
		QString trOff;
		QString etChange;
		QString etAdd;
		QString etDel;
		QString etEdit;
		QString etFunc;
		QString etParam;
		QString rcStart;
		QString rcStop;
		QString rcClose;
	};

	struct PopText
	{
		PopTextInfo qe;
		PopTextInfo qd;
		PopTextInfo we;
		PopTextInfo wd;
		PopTextInfo qce;
		PopTextInfo qcd;
		PopTextInfo swe;
		PopTextInfo swd;
		PopTextInfo dwe;
		PopTextInfo dwd;
		PopTextInfo upe;
		PopTextInfo upd;
		POINT p = {};
		uint32 size = 100;
		uint32 time = 1000;
	};

	struct QuickInputUi
	{
		QString dialogStyle = R"(*{color:black}QPushButton{width:60px;border:2px solid blue})";
		Themes themes;
		Text text;
		PopText pop;
	};
}



////////////////// Window
using HWNDS = List<HWND>;
struct WndInput
{
	HWND wnd = nullptr; // parent
	HWNDS children;
	bool child = false;
	HWND current = nullptr;

	POINT pt = {}; // prev point
	WORD mk = 0; // prev key
	bool active()
	{
		return IsWindow(wnd);
	}
	HWND find(const POINT& pt, POINT& ptrel, bool child)
	{
		if (!active()) return nullptr;

		if (child)
		{
			Window::FindChild(wnd, children);
			for (uint32 i = 0; i < children.size(); i++) if (!IsWindowVisible(children[i])) children.Del(i);
			
			HWNDS inrect;
			for (int32 u = (int32)children.size() - 1; u > -1; u--) if (InRect(Window::childRect(wnd, children[u]), pt)) inrect.Add(children[u]);

			if (inrect.size())
			{
				// select minimum window
				HWND min = nullptr;
				uint64 minArea = uint64_max;
				for (uint32 i = 0; i < inrect.size(); i++)
				{
					uint64 area = RectArea(Window::rect(inrect[i]));
					if (area < minArea) min = inrect[i], minArea = area;
				}

				if (min)
				{
					ptrel = InRectPos(Window::childRect(wnd, min), pt);
					return min;
				}
			}
		}
		ptrel = InRectPos(Window::sizeRect(wnd), pt);
		return wnd;
	}
};
struct WndInfo
{
	HWND wnd = nullptr;
	bool child = false;
	QString wndName;
	QString wndClass;
	bool update() {
		if (!IsWindow(wnd))
		{
			return wnd = FindWindowW(QStringToW(wndClass), QStringToW(wndName));
		}
		return true;
	}
};
struct WndLock
{
	static inline HANDLE thread;
	static DWORD _stdcall LockThread(PVOID wnd)
	{
		while (IsWindowVisible((HWND)wnd)) {
			RECT rect = Window::rect((HWND)wnd);
			ClipCursor(&rect);
			Thread::Sleep(10);
		}
		thread = 0;
		return 0;
	}
	static void Lock(HWND wnd)
	{
		UnLock();
		thread = Thread::Start(LockThread, wnd);
	}
	static void UnLock()
	{
		if (thread)
		{
			TerminateThread(thread, 0);
			thread = 0;
		}
		ClipCursor(0);
	}
};
////////////////// ~Window



////////////////// Datas
struct QuickClick
{
	bool state = 0;
	uint32 mode = 0;
	uint32 key = 0;
	uint32 delay = 10;
	HANDLE thread = 0;
};
struct ShowClock
{
	bool state = false;
	uint32 key = 0;
};
struct WndActive
{
	bool state = false;
	WndInfo wi;
	HANDLE thread = 0;
};
struct SettingsData
{
	uint32 theme = 0;
	uint32 key = 0;
	uint32 recKey = 0;
	bool recTrack = false;
	bool defOn = false;
	bool showTips = false;
	bool audFx = false;
	bool minMode = false;
	bool scaleBlock = false;
};
struct FuncData
{
	QuickClick quickClick;
	ShowClock showClock;
	WndActive wndActive;
};
struct Widget
{
	bool dialogActive = false;
	bool mainActive = false;
	bool moreActive = false;
	bool versionActive = false;
	bool licenseActive = false;
	QWidget* main = nullptr;
	QWidget* record = nullptr;
};
////////////////// ~Datas




////////////////// Macros
struct Macro
{
	enum { sw, down, up };
	bool state = false; // enable | disable
	bool block = false; // block this trigger key
	bool wndState = false; // window mode enable | disable
	bool active = false; // state of release trigger
	uint32 key = 0;
	uint32 mode = 0;
	uint32 count = 0;
	QString name;

	POINT cursor = {};

	WndInfo wi;
	WndInput wp;

	Actions acRun;
	Actions acEnd;

	HANDLE thRun = nullptr;
	HANDLE thEnd = nullptr;
};
using Macros = List<Macro>;
////////////////// ~Macros




namespace Qi
{
	constexpr size_t keySize = XBoxPad::key_end;

	inline QApplication* application = nullptr;

	inline bool state = false;
	inline bool run = false;

	inline bool recordState = false;
	inline bool recording = false;
	inline clock_t recordClock = 0;
	inline HWND recordWindow = 0;

	inline Actions record;
	inline Actions clipboard;
	inline Macros macros;

	inline FuncData fun;
	inline SettingsData set;
	inline Widget widget;
	inline QPopText* popText = nullptr;

	inline bool keyState[keySize];
	inline List<byte> blockKeys;

	inline XBoxPad xboxpad;

	inline SIZE screen = {};

	inline std::wstring dir = Path::Last(Process::runPath());

	inline MSG msg;

	inline QiUi::QuickInputUi ui;
}