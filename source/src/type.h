#pragma once
#include "macro.h"
#include "ui.h"

struct RECTF { float left; float top; float right; float bottom; };

enum class InterpreterResult
{
	r_exit,
	r_continue,
	r_break,
	r_top
};
enum class DataRole
{
	id = Qt::UserRole,
	type,
	group,
	macro
};
enum class QiEvent
{
	wid_close = QEvent::User + 1,
	rec_start,
	rec_stop,
	rec_close,
	mac_edit_enter,
	mac_edit_exit,
	mac_edit_exit_d,
	mac_load,
	edt_debug_pause,
	edt_varop_stop,
	var_reload,
	key_reset
};

struct MsgViewInfo
{
	enum
	{
		_msg,
		_war,
		_err
	};
	size_t level = _msg;
	time_t time = ::time(nullptr);
	QString text;
	QString macro;
};
using MsgViewInfos = QiVector<MsgViewInfo>;
class MsgViewEvent : public QEvent
{
	const MsgViewInfo _info;
public:
	enum Type
	{
		setText = QEvent::User + 1,
		newLine,
		clear,
		show,
		hide
	};
	MsgViewEvent(int type) : QEvent(static_cast<QEvent::Type>(type)) {}
	MsgViewEvent(int type, const MsgViewInfo& info) : QEvent(static_cast<QEvent::Type>(type)), _info(info) {}
	MsgViewInfo info() const { return _info; }
};

////////////////// Datas
struct FuncData
{
	struct QuickClick
	{
		bool state = 0;
		int mode = 0;
		int key = 0;
		int delay = 10;
		QiQuickClickThread thread;
	} quickClick;
	struct ShowClock
	{
		bool state = false;
		int key = 0;
	} showClock;
	struct WndActive
	{
		bool state = false;
		WndInfo wndInfo;
		QiWindowBindThread thread;
	} wndActive;
};
struct SettingsData
{
	QString ocr_current;
	int ocr_thread = 0;
	bool rawInput = false;
	int theme = 0;
	short key1 = 0;
	short key2 = 0;
	short recKey = 0;
	bool recTrack = false;
	bool defOn = false;
	bool showTips = false;
	bool audFx = false;
	bool minMode = false;
	bool pad = false;
	// eidt
	bool tabLock = false;
	bool tabHideTip = false;
	bool markPoint = false;
	QSize editSize;
};
struct GroupData
{
	std::map<QString, bool> fold;
	QiVector<QString> sort;
};
struct Widget
{
	bool dialogActive = false;
	bool mainActive = false;
	bool moreActive = false;
	QWidget* main = nullptr;
	QWidget* macro = nullptr;
	QWidget* trigger = nullptr;
	QWidget* func = nullptr;
	QWidget* settings = nullptr;
	QWidget* record = nullptr;
	QWidget* edit = nullptr;
	QWidget* varView = nullptr;
	QWidget* msgView = nullptr;
	Macro editMacro;
	bool active() const
	{
		return !(mainActive || dialogActive || moreActive);
	}
	void recordStart() const
	{
		QApplication::postEvent(record, new QEvent(static_cast<QEvent::Type>(QiEvent::rec_start)));
	}
	void recordStop() const
	{
		QApplication::postEvent(record, new QEvent(static_cast<QEvent::Type>(QiEvent::rec_stop)));
	}
	void recordClose() const
	{
		QApplication::postEvent(record, new QEvent(static_cast<QEvent::Type>(QiEvent::rec_close)));
	}
	void macroLoad() const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>(QiEvent::mac_load)));
	}
	void macroEdit() const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>(QiEvent::mac_edit_enter)));
	}
	void macroEdited(bool save = true) const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>((save ? QiEvent::mac_edit_exit : QiEvent::mac_edit_exit_d))));
	}
	void editClose() const
	{
		QApplication::postEvent(edit, new QEvent(static_cast<QEvent::Type>(QiEvent::wid_close)));
	}
	void editDebugPause() const
	{
		QApplication::postEvent(edit, new QEvent(static_cast<QEvent::Type>(QiEvent::edt_debug_pause)));
	}
	void editVaropStop() const
	{
		QApplication::postEvent(edit, new QEvent(static_cast<QEvent::Type>(QiEvent::edt_varop_stop)));
	}
	void varViewReload() const
	{
		if (!varView->isHidden()) QApplication::postEvent(varView, new QEvent(static_cast<QEvent::Type>(QiEvent::var_reload)));
	}
	void msgViewSet(const QString& text) const
	{
		MsgViewInfo info; info.text = text;
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::setText, info));
	}
	void msgViewAdd(const MsgViewInfo& info) const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::newLine, info));
	}
	void msgViewClear() const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::clear));
	}
	void msgViewShow() const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::show));
	}
	void msgViewHide() const
	{
		QApplication::postEvent(msgView, new MsgViewEvent(MsgViewEvent::Type::hide));
	}
	void keyEditReload() const
	{
		QApplication::postEvent(macro, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
		QApplication::postEvent(trigger, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
		QApplication::postEvent(func, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
		QApplication::postEvent(settings, new QEvent(static_cast<QEvent::Type>(QiEvent::key_reset)));
	}
};

namespace Qi
{
	inline size_t ocr_ver = 0;
	inline QiOcrModule ocr;
#ifdef Q_RAWINPUT
	inline QiRawInputModule rawInput;
#endif
	// for setStyle
	inline QApplication* application = nullptr;
	inline QiUi::QuickInputUi ui;
	// state
	inline bool state = false;
	// record
	inline bool recordState = false;
	inline bool recording = false;
	inline clock_t recordClock = 0;
	inline HWND recordWindow = 0;
	// macro
	inline Actions record;
	inline Actions clipboard;
	inline MacroGroups macroGroups;
	inline MacroPointers macroActive;
	// data
	inline FuncData fun;
	inline SettingsData set;
	inline GroupData group;
	inline Widget widget;
	inline QPopText* popText = nullptr;
	inline QWindowSelection* windowSelection = nullptr;
	// input
	inline bool keyState[key_size];
	inline bool keyBlock[key_size];
	inline int curBlock = 0;
#ifdef Q_KEYEDIT_PAD_ENABLED
	inline XBoxPad xboxpad;
#endif
	// other
	inline QiDisplayUpdateThread display_update;
	inline SIZE screen = {};

	QJsonObject toJson();
	void fromJson(const QJsonObject& json);
}