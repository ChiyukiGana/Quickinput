#pragma once
#include <windows.h>
#include <thread>
#include <mutex>
#include <qapplication.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qevent.h>
#ifdef Q_KEYEDIT_PAD_ENABLED
#include <xinput.h>
#pragma comment(lib,"xinput.lib")
#endif
#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B
class QKeyEdit;
using QKeyEditKey = short;
using QKeyEditKeys = QList<QKeyEditKey>;
struct QKeyEditCombinationKey { short key; short mod; };
struct XBoxPadButton
{
	enum
	{
		start = 0x0100,
		back,
		a,
		b,
		x,
		y,
		up,
		down,
		left,
		right,
		l_joy_press,
		l_joy_up,
		l_joy_down,
		l_joy_left,
		l_joy_right,
		r_joy_press,
		r_joy_up,
		r_joy_down,
		r_joy_left,
		r_joy_right,
		l_top,
		r_top,
		l_bottom,
		r_bottom,
	};
};
class QKeyEditEvent : public QEvent
{
	bool m_press;
	short m_key;
public:
	QKeyEditEvent(bool press, short key) : QEvent((QEvent::Type)(QEvent::User + 1)), m_press(press), m_key(key) {}
	bool press() const { return m_press; }
	short key() const { return m_key; }
};
class QKeyEditHookManager : public QObject
{
	Q_OBJECT
private:
	HHOOK m_hook = nullptr;
	DWORD m_threadId = 0;
	std::recursive_mutex m_mutex;
	QMap<QKeyEdit*, bool> m_instances;
	static QKeyEditHookManager* s_instance;
	QKeyEditHookManager(QObject* parent = nullptr) : QObject(parent) {}
	~QKeyEditHookManager() { uninstallHook(); }
public:
	static QKeyEditHookManager* instance()
	{
		static QKeyEditHookManager inst;
		return &inst;
	}
	void registerClient(QKeyEdit* client)
	{
		std::unique_lock lock(m_mutex);
		m_instances[client] = true;
		installHook();
	}
	void unregisterClient(QKeyEdit* client)
	{
		std::unique_lock lock(m_mutex);
		m_instances.remove(client);
		if (m_instances.isEmpty()) uninstallHook();
	}
private:
	void installHook()
	{
		if (m_hook) return;
		std::thread([this] {
			m_hook = SetWindowsHookExW(WH_KEYBOARD_LL, [](int code, WPARAM wParam, LPARAM lParam) {
				short key = ((PKBDLLHOOKSTRUCT)lParam)->vkCode;
				auto mgr = QKeyEditHookManager::instance();
				if (code == HC_ACTION) {
					if (((PKBDLLHOOKSTRUCT)lParam)->flags & 1)
					{
						switch (key)
						{
						case VK_RETURN: key = VK_SEPARATOR; break;
						}
					}
					else
					{
						switch (key)
						{
						case VK_CLEAR: key = VK_NUMPAD5; break;
						case VK_PRIOR: key = VK_NUMPAD9; break;
						case VK_NEXT: key = VK_NUMPAD3; break;
						case VK_END: key = VK_NUMPAD1; break;
						case VK_HOME: key = VK_NUMPAD7; break;
						case VK_INSERT: key = VK_NUMPAD0; break;
						case VK_DELETE: key = VK_DECIMAL; break;
						case VK_LEFT: key = VK_NUMPAD4; break;
						case VK_UP: key = VK_NUMPAD8; break;
						case VK_RIGHT: key = VK_NUMPAD6; break;
						case VK_DOWN: key = VK_NUMPAD2; break;
						}
					}
					bool press = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
					mgr->dispatchKeyEvent(key, press);
				}
				return CallNextHookEx(nullptr, code, wParam, lParam);
			}, nullptr, 0);
			MSG msg;
			while (GetMessageW(&msg, nullptr, 0, 0)) {}
		}).detach();
	}
	void uninstallHook()
	{
		if (m_hook)
		{
			UnhookWindowsHookEx(m_hook);
			m_hook = nullptr;
		}
	}
	void dispatchKeyEvent(short key, bool press)
	{
		std::unique_lock lock(m_mutex);
		for (auto it = m_instances.keyBegin(); it != m_instances.keyEnd(); ++it)
		{
			QApplication::postEvent((QObject*)*it, new QKeyEditEvent(press, key));
		}
	}
};
class QKeyEdit : public QLineEdit
{
	Q_OBJECT
		Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
		Q_PROPERTY(bool combinationMode READ isCombinationMode WRITE setCombinationMode)
		Q_PROPERTY(bool keybdEnabled READ isKeybdEnabled WRITE setKeybdEnabled)
		Q_PROPERTY(bool mouseEnabled READ isMouseEnabled WRITE setMouseEnabled)
		Q_PROPERTY(bool wheelEnabled READ isWheelEnabled WRITE setWheelEnabled)
#ifdef Q_KEYEDIT_PAD_ENABLED
		Q_PROPERTY(bool padEnabled READ isPadEnabled WRITE setPadEnabled)
#endif
		Q_PROPERTY(int maximumKeys READ maximumKeys WRITE setMaximumKeys)
		Q_PROPERTY(QKeyEditCombinationKey combinationKey READ combinationKey WRITE setCombinationKey)
		Q_PROPERTY(QKeyEditKey key READ key WRITE setKey)
		Q_PROPERTY(QKeyEditKeys keys READ keys WRITE setKeys)
private: /* propertys */
	using This = QKeyEdit;
#ifdef Q_KEYEDIT_PAD_ENABLED
	QTimer m_timer = QTimer(this);
	bool m_pad = false;
#endif
	bool m_readonly = false;
	bool m_keybd = true;
	bool m_mouse = false;
	bool m_wheel = false;
	bool m_combination = true;
	int m_maximumKeys = 1;
private:
	bool m_editable = false;
	QKeyEditKeys m_keys;
	QKeyEditKey m_mod = 0;
#ifdef Q_KEYEDIT_PAD_ENABLED
	static constexpr int s_state_size = XBoxPadButton::r_bottom + 1;
#else
	static constexpr int s_state_size = 256;
#endif
	bool m_state[s_state_size] = {};

public:
	QKeyEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{
		QKeyEditHookManager::instance()->registerClient(this);
		QLineEdit::setReadOnly(true);
		QLineEdit::setAttribute(Qt::WA_InputMethodEnabled, false);
		QLineEdit::setAlignment(Qt::AlignCenter);
		QLineEdit::setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
		QLineEdit::setText("none");
#ifdef Q_KEYEDIT_PAD_ENABLED
		m_timer.setInterval(100);
		connect(&m_timer, &QTimer::timeout, this, &This::XBoxPadStateTimer);
#endif
	}
	~QKeyEdit()
	{
		QKeyEditHookManager::instance()->unregisterClient(this);
	}
public: /* propertys */
	bool isReadOnly() const { return m_readonly; }
	void setReadOnly(bool readonly) { m_readonly = readonly; }
	bool isKeybdEnabled() const { return m_keybd; }
	void setKeybdEnabled(bool enabled) { m_keybd = enabled; }
	bool isMouseEnabled() const { return m_mouse; }
	void setMouseEnabled(bool enabled) { m_mouse = enabled; }
	bool isWheelEnabled() const { return m_wheel; }
	void setWheelEnabled(bool enabled) { m_wheel = enabled; }
#ifdef Q_KEYEDIT_PAD_ENABLED
	bool isPadEnabled() const { return m_pad; }
	void setPadEnabled(bool enabled) { m_pad = enabled; }
	void setDeviceEnabled(bool keybd, bool mouse = false, bool wheel = false, bool pad = false) { m_keybd = keybd, m_mouse = mouse, m_wheel = wheel, m_pad = pad; };
#else
	void setDeviceEnabled(bool keybd, bool mouse = false, bool wheel = false) { m_keybd = keybd, m_mouse = mouse, m_wheel = wheel; };
#endif
	bool isCombinationMode() const { return m_combination; }
	void setCombinationMode(bool combinationMode) { m_combination = combinationMode; }
	int maximumKeys() const { return m_maximumKeys; }
	void setMaximumKeys(int maximumKeys) { m_maximumKeys = maximumKeys; }
	QKeyEditCombinationKey combinationKey() const { if (m_keys.isEmpty()) return QKeyEditCombinationKey(); return { m_keys.first(), m_mod }; }
	void setCombinationKey(const QKeyEditCombinationKey& combinationKey) { m_keys.clear(); m_keys.append(combinationKey.key); m_mod = combinationKey.mod; draw(); }
	QKeyEditKey key() const { if (m_keys.isEmpty()) return QKeyEditKey(); return m_keys.first(); }
	void setKey(const QKeyEditKey& key) { m_keys.clear(); m_keys.append(key); draw(); }
	QKeyEditKeys keys() const { return m_keys; }
	void setKeys(const QKeyEditKeys& keys) { m_keys = keys; draw(); }
Q_SIGNALS:
	void changed();
private:
	bool isPressed(short key)
	{
		for (auto i : m_keys) if (i == key) return true;
		return false;
	}
	bool isAllReleased()
	{
		for (size_t i = 0; i < s_state_size; i++) if (m_state[i]) return false;
		return true;
	}
	void draw()
	{
		QString text;
		if (m_combination)
		{
			if (!m_keys.isEmpty() || m_mod)
			{
				if (m_mod & MOD_CONTROL) text = "Ctrl + ";
				if (m_mod & MOD_SHIFT) text += "Shift + ";
				if (m_mod & MOD_ALT) text += "Alt + ";
				if (!m_keys.isEmpty()) text += keyName(m_keys.first());
				if (m_editable) text += "...";
			}
			else text = "none";
		}
		else
		{
			if (!m_keys.isEmpty())
			{
				for (size_t i = 0; i < m_keys.size() - 1; i++) text += keyName(m_keys[i]) + QString(" + ");
				text += keyName(m_keys.last());
				if (m_editable)
				{
					if (m_keys.size() < m_maximumKeys) text += " + ...";
					else text += " ...";
				}
			}
			else text = "none";
		}
		QLineEdit::setText(text);
	}
	void press(short key)
	{
		if (m_editable)
		{
			m_state[key] = true;
			if (!isPressed(key))
			{
				if (m_combination)
				{
					if (key == VK_LCONTROL || key == VK_RCONTROL) key = VK_CONTROL;
					if (key == VK_LSHIFT || key == VK_RSHIFT) key = VK_SHIFT;
					if (key == VK_LMENU || key == VK_RMENU) key = VK_MENU;

					if (key == VK_CONTROL) m_mod |= MOD_CONTROL;
					else if (key == VK_SHIFT) m_mod |= MOD_SHIFT;
					else if (key == VK_MENU) m_mod |= MOD_ALT;
					else
					{
						m_keys.clear();
						m_keys.append(key);
					}
				}
				else if (m_keys.size() < m_maximumKeys) m_keys.append(key);
				draw();
			}
		}
	}
	void release(short key)
	{
		if (m_editable)
		{
			m_state[key] = false;
			if (isAllReleased()) end();
		}
	}
	void begin()
	{
		if (m_editable) return;
		grabMouse(), grabKeyboard();
		QLineEdit::setText("...");
		memset(m_state, 0, s_state_size);
		m_keys.clear();
		m_mod = 0;
		m_editable = true;
#ifdef Q_KEYEDIT_PAD_ENABLED
		if (m_pad) m_timer.start();
#endif
	}
	void end()
	{
#ifdef Q_KEYEDIT_PAD_ENABLED
		m_timer.stop();
#endif
		m_editable = false;
		releaseMouse(), releaseKeyboard();
		if (m_combination && m_keys.isEmpty()) m_mod = 0;
		draw();
		changed();
	}
protected:
	bool event(QEvent* e)
	{
		if (e->type() == QEvent::MouseButtonPress)
		{
			if (m_editable && m_mouse && !m_combination)
			{
				QMouseEvent* mouse = (QMouseEvent*)e;
				press(toKeyCode(mouse->button()));
			}
			return true;
		}
		else if (e->type() == QEvent::MouseButtonDblClick)
		{
			if (m_editable && m_mouse && !m_combination)
			{
				QMouseEvent* mouse = (QMouseEvent*)e;
				press(toKeyCode(mouse->button()));
			}
			return true;
		}
		else if (e->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouse = (QMouseEvent*)e;
			if (m_editable)
			{
				if (m_mouse && !m_combination) release(toKeyCode(mouse->button()));
			}
			else if (mouse->button() == Qt::LeftButton && !m_readonly)
			{
				begin();
			}
			return true;
		}
		else if (e->type() == QEvent::Wheel)
		{
			QWheelEvent* mouseWheel = (QWheelEvent*)e;
			if (m_editable && m_wheel && !m_combination)
			{
				if (mouseWheel->angleDelta().y() > 0) press(VK_WHEELUP), release(VK_WHEELUP);
				else press(VK_WHEELDOWN), release(VK_WHEELDOWN);
			}
			return true;
		}
		return QLineEdit::event(e);
	}
	void customEvent(QEvent* e)
	{
		QKeyEditEvent* key = (QKeyEditEvent*)e;
		if (key->press()) press(key->key());
		else release(key->key());
	}
private:
#ifdef Q_KEYEDIT_PAD_ENABLED
private Q_SLOTS:
	void XBoxPadStateTimer()
	{
		if (m_editable && m_pad && !m_combination)
		{
			XINPUT_STATE state = {};
			if (XInputGetState(0, &state) == NO_ERROR)
			{
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) > m_state[XBoxPadButton::start])
					press(XBoxPadButton::start);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) < m_state[XBoxPadButton::start])
					release(XBoxPadButton::start);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) > m_state[XBoxPadButton::back])
					press(XBoxPadButton::back);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) < m_state[XBoxPadButton::back])
					release(XBoxPadButton::back);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) > m_state[XBoxPadButton::a])
					press(XBoxPadButton::a);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) < m_state[XBoxPadButton::a])
					release(XBoxPadButton::a);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) > m_state[XBoxPadButton::b])
					press(XBoxPadButton::b);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) < m_state[XBoxPadButton::b])
					release(XBoxPadButton::b);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) > m_state[XBoxPadButton::x])
					press(XBoxPadButton::x);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) < m_state[XBoxPadButton::x])
					release(XBoxPadButton::x);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) > m_state[XBoxPadButton::y])
					press(XBoxPadButton::y);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) < m_state[XBoxPadButton::y])
					release(XBoxPadButton::y);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) > m_state[XBoxPadButton::up])
					press(XBoxPadButton::up);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) < m_state[XBoxPadButton::up])
					release(XBoxPadButton::up);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) > m_state[XBoxPadButton::down])
					press(XBoxPadButton::down);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) < m_state[XBoxPadButton::down])
					release(XBoxPadButton::down);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) > m_state[XBoxPadButton::left])
					press(XBoxPadButton::left);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) < m_state[XBoxPadButton::left])
					release(XBoxPadButton::left);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) > m_state[XBoxPadButton::right])
					press(XBoxPadButton::right);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) < m_state[XBoxPadButton::right])
					release(XBoxPadButton::right);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) > m_state[XBoxPadButton::l_joy_press])
					press(XBoxPadButton::l_joy_press);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) < m_state[XBoxPadButton::l_joy_press])
					release(XBoxPadButton::l_joy_press);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) > m_state[XBoxPadButton::r_joy_press])
					press(XBoxPadButton::r_joy_press);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) < m_state[XBoxPadButton::r_joy_press])
					release(XBoxPadButton::r_joy_press);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) > m_state[XBoxPadButton::l_top])
					press(XBoxPadButton::l_top);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) < m_state[XBoxPadButton::l_top])
					release(XBoxPadButton::l_top);
				if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) > m_state[XBoxPadButton::r_top])
					press(XBoxPadButton::r_top);
				else if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) < m_state[XBoxPadButton::r_top])
					release(XBoxPadButton::r_top);
				if (state.Gamepad.sThumbLY > 0 && !m_state[XBoxPadButton::l_joy_up])
					press(XBoxPadButton::l_joy_up);
				else if (state.Gamepad.sThumbLY <= 0 && m_state[XBoxPadButton::l_joy_up])
					release(XBoxPadButton::l_joy_up);
				if (state.Gamepad.sThumbLY < 0 && !m_state[XBoxPadButton::l_joy_down])
					press(XBoxPadButton::l_joy_down);
				else if (state.Gamepad.sThumbLY >= 0 && m_state[XBoxPadButton::l_joy_down])
					release(XBoxPadButton::l_joy_down);
				if (state.Gamepad.sThumbLX < 0 && !m_state[XBoxPadButton::l_joy_left])
					press(XBoxPadButton::l_joy_left);
				else if (state.Gamepad.sThumbLX >= 0 && m_state[XBoxPadButton::l_joy_left])
					release(XBoxPadButton::l_joy_left);
				if (state.Gamepad.sThumbLX > 0 && !m_state[XBoxPadButton::l_joy_right])
					press(XBoxPadButton::l_joy_right);
				else if (state.Gamepad.sThumbLX <= 0 && m_state[XBoxPadButton::l_joy_right])
					release(XBoxPadButton::l_joy_right);
				if (state.Gamepad.sThumbRY > 0 && !m_state[XBoxPadButton::r_joy_up])
					press(XBoxPadButton::r_joy_up);
				else if (state.Gamepad.sThumbRY <= 0 && m_state[XBoxPadButton::r_joy_up])
					release(XBoxPadButton::r_joy_up);
				if (state.Gamepad.sThumbRY < 0 && !m_state[XBoxPadButton::r_joy_down])
					press(XBoxPadButton::r_joy_down);
				else if (state.Gamepad.sThumbRY >= 0 && m_state[XBoxPadButton::r_joy_down])
					release(XBoxPadButton::r_joy_down);
				if (state.Gamepad.sThumbRX < 0 && !m_state[XBoxPadButton::r_joy_left])
					press(XBoxPadButton::r_joy_left);
				else if (state.Gamepad.sThumbRX >= 0 && m_state[XBoxPadButton::r_joy_left])
					release(XBoxPadButton::r_joy_left);
				if (state.Gamepad.sThumbRX > 0 && !m_state[XBoxPadButton::r_joy_right])
					press(XBoxPadButton::r_joy_right);
				else if (state.Gamepad.sThumbRX <= 0 && m_state[XBoxPadButton::r_joy_right])
					release(XBoxPadButton::r_joy_right);
				if (state.Gamepad.bLeftTrigger > 0 && !m_state[XBoxPadButton::l_bottom])
					press(XBoxPadButton::l_bottom);
				else if (state.Gamepad.bLeftTrigger <= 0 && m_state[XBoxPadButton::l_bottom])
					release(XBoxPadButton::l_bottom);
				if (state.Gamepad.bRightTrigger > 0 && !m_state[XBoxPadButton::r_bottom])
					press(XBoxPadButton::r_bottom);
				else if (state.Gamepad.bRightTrigger <= 0 && m_state[XBoxPadButton::r_bottom])
					release(XBoxPadButton::r_bottom);
			}
		}
	}
#endif
public:
	static bool isMouse(short keyCode)
	{
		return ((keyCode >= VK_LBUTTON && keyCode <= VK_XBUTTON2) || keyCode == VK_WHEELUP || keyCode == VK_WHEELDOWN);
	}
	static bool isKeyboard(short keyCode)
	{
		return ((keyCode >= VK_CLEAR && keyCode <= VK_OEM_CLEAR) || keyCode == VK_BACK || keyCode == VK_TAB);
	}
#ifdef Q_KEYEDIT_PAD_ENABLED
	static bool isPad(short keyCode)
	{
		return ((keyCode >= XBoxPadButton::start) && (keyCode <= XBoxPadButton::r_bottom));
	}
#endif
	static short toKeyCode(Qt::MouseButton key)
	{
		switch (key)
		{
		case Qt::LeftButton: return VK_LBUTTON;
		case Qt::RightButton: return VK_RBUTTON;
		case Qt::MiddleButton: return VK_MBUTTON;
		case Qt::XButton1: return VK_XBUTTON1;
		case Qt::XButton2: return VK_XBUTTON2;
		}
		return 0;
	}
#ifdef Q_KEYEDIT_PAD_ENABLED
	static QString padName(short keyCode)
	{
		switch (keyCode)
		{
		case XBoxPadButton::a: return u8"PadA";
		case XBoxPadButton::b: return u8"PadB";
		case XBoxPadButton::x: return u8"PadX";
		case XBoxPadButton::y: return u8"PadY";
		case XBoxPadButton::up: return u8"Pad↑";
		case XBoxPadButton::down: return u8"Pad↓";
		case XBoxPadButton::left: return u8"Pad←";
		case XBoxPadButton::right: return u8"Pad→";
		case XBoxPadButton::l_joy_press: return u8"LJoy";
		case XBoxPadButton::l_joy_up: return u8"LJoy↑";
		case XBoxPadButton::l_joy_down: return u8"LJoy↓";
		case XBoxPadButton::l_joy_left: return u8"LJoy←";
		case XBoxPadButton::l_joy_right: return u8"LJoy→";
		case XBoxPadButton::r_joy_press: return u8"RJoy";
		case XBoxPadButton::r_joy_up: return u8"RJoy↑";
		case XBoxPadButton::r_joy_down: return u8"RJoy↓";
		case XBoxPadButton::r_joy_left: return u8"RJoy←";
		case XBoxPadButton::r_joy_right: return u8"RJoy→";
		case XBoxPadButton::l_top: return u8"PadLT";
		case XBoxPadButton::l_bottom: return u8"PadLB";
		case XBoxPadButton::r_top: return u8"PadRT";
		case XBoxPadButton::r_bottom: return u8"PadRB";
		}
		return u8"none";
	}
#endif
	static QString keyName(short keyCode)
	{
#ifdef Q_KEYEDIT_PAD_ENABLED
		if (isPad(keyCode)) return padName(keyCode);
		else
#endif
		{
			switch (keyCode) {
			case VK_LBUTTON: return u8"左键";
			case VK_RBUTTON: return u8"右键";
			case VK_CANCEL: return u8"Cancel";
			case VK_MBUTTON: return u8"中键";
			case VK_XBUTTON1: return u8"侧键1";
			case VK_XBUTTON2: return u8"侧键2";
			case VK_WHEELUP: return u8"滚轮↑";
			case VK_WHEELDOWN: return u8"滚轮↓";
			case VK_BACK: return u8"Back";
			case VK_TAB: return u8"Tab";
			case VK_CLEAR: return u8"Clear";
			case VK_RETURN: return u8"Return";
			case VK_SHIFT: return u8"Shift";
			case VK_CONTROL: return u8"Ctrl";
			case VK_MENU: return u8"Alt";
			case VK_PAUSE: return u8"Pause";
			case VK_CAPITAL: return u8"CapsLock";
			case VK_KANA: return u8"Kana";
			case VK_JUNJA: return u8"Junja";
			case VK_FINAL: return u8"Final";
			case VK_HANJA: return u8"Hanja";
			case VK_ESCAPE: return u8"Esc";
			case VK_CONVERT: return u8"Convert";
			case VK_NONCONVERT: return u8"NonConvert";
			case VK_ACCEPT: return u8"Accept";
			case VK_MODECHANGE: return u8"ModeChange";
			case VK_SPACE: return u8"空格";
			case VK_PRIOR: return u8"PageUp";
			case VK_NEXT: return u8"PageDown";
			case VK_END: return u8"End";
			case VK_HOME: return u8"Home";
			case VK_LEFT: return u8"←";
			case VK_UP: return u8"↑";
			case VK_RIGHT: return u8"→";
			case VK_DOWN: return u8"↓";
			case VK_SELECT: return u8"Select";
			case VK_PRINT: return u8"Print";
			case VK_EXECUTE: return u8"Execute";
			case VK_SNAPSHOT: return u8"ScreenShot";
			case VK_INSERT: return u8"Insert";
			case VK_DELETE: return u8"Delete";
			case VK_HELP: return u8"Help";
			case 0x30: return u8"0";
			case 0x31: return u8"1";
			case 0x32: return u8"2";
			case 0x33: return u8"3";
			case 0x34: return u8"4";
			case 0x35: return u8"5";
			case 0x36: return u8"6";
			case 0x37: return u8"7";
			case 0x38: return u8"8";
			case 0x39: return u8"9";
			case 0x41: return u8"A";
			case 0x42: return u8"B";
			case 0x43: return u8"C";
			case 0x44: return u8"D";
			case 0x45: return u8"E";
			case 0x46: return u8"F";
			case 0x47: return u8"G";
			case 0x48: return u8"H";
			case 0x49: return u8"I";
			case 0x4A: return u8"J";
			case 0x4B: return u8"K";
			case 0x4C: return u8"L";
			case 0x4D: return u8"M";
			case 0x4E: return u8"N";
			case 0x4F: return u8"O";
			case 0x50: return u8"P";
			case 0x51: return u8"Q";
			case 0x52: return u8"R";
			case 0x53: return u8"S";
			case 0x54: return u8"T";
			case 0x55: return u8"U";
			case 0x56: return u8"V";
			case 0x57: return u8"W";
			case 0x58: return u8"X";
			case 0x59: return u8"Y";
			case 0x5A: return u8"Z";
			case VK_LWIN: return u8"LWin";
			case VK_RWIN: return u8"RWin";
			case VK_APPS: return u8"Apps";
			case VK_SLEEP: return u8"Sleep";
			case VK_NUMPAD0: return u8"Num0";
			case VK_NUMPAD1: return u8"Num1";
			case VK_NUMPAD2: return u8"Num2";
			case VK_NUMPAD3: return u8"Num3";
			case VK_NUMPAD4: return u8"Num4";
			case VK_NUMPAD5: return u8"Num5";
			case VK_NUMPAD6: return u8"Num6";
			case VK_NUMPAD7: return u8"Num7";
			case VK_NUMPAD8: return u8"Num8";
			case VK_NUMPAD9: return u8"Num9";
			case VK_MULTIPLY: return u8"Num*";
			case VK_ADD: return u8"Num+";
			case VK_SEPARATOR: return u8"NumEnter";
			case VK_SUBTRACT: return u8"Num-";
			case VK_DECIMAL: return u8"Num.";
			case VK_DIVIDE: return u8"Num/";
			case VK_F1: return u8"F1";
			case VK_F2: return u8"F2";
			case VK_F3: return u8"F3";
			case VK_F4: return u8"F4";
			case VK_F5: return u8"F5";
			case VK_F6: return u8"F6";
			case VK_F7: return u8"F7";
			case VK_F8: return u8"F8";
			case VK_F9: return u8"F9";
			case VK_F10: return u8"F10";
			case VK_F11: return u8"F11";
			case VK_F12: return u8"F12";
			case VK_F13: return u8"F13";
			case VK_F14: return u8"F14";
			case VK_F15: return u8"F15";
			case VK_F16: return u8"F16";
			case VK_F17: return u8"F17";
			case VK_F18: return u8"F18";
			case VK_F19: return u8"F19";
			case VK_F20: return u8"F20";
			case VK_F21: return u8"F21";
			case VK_F22: return u8"F22";
			case VK_F23: return u8"F23";
			case VK_F24: return u8"F24";
			case VK_NUMLOCK: return u8"NumLock";
			case VK_SCROLL: return u8"ScrollLock";
			case VK_LSHIFT: return u8"LShift";
			case VK_RSHIFT: return u8"RShift";
			case VK_LCONTROL: return u8"LCtrl";
			case VK_RCONTROL: return u8"RCtrl";
			case VK_LMENU: return u8"LAlt";
			case VK_RMENU: return u8"RAlt";
			case VK_BROWSER_BACK: return u8"BrowserBack";
			case VK_BROWSER_FORWARD: return u8"BrowserForward";
			case VK_BROWSER_REFRESH: return u8"BrowserRefresh";
			case VK_BROWSER_STOP: return u8"BrowserStop";
			case VK_BROWSER_SEARCH: return u8"BrowserSearch";
			case VK_BROWSER_FAVORITES: return u8"BrowserFavorites";
			case VK_BROWSER_HOME: return u8"BrowserHome";
			case VK_VOLUME_MUTE: return u8"VolumeMute";
			case VK_VOLUME_DOWN: return u8"VolumeDown";
			case VK_VOLUME_UP: return u8"VolumeUp";
			case VK_MEDIA_NEXT_TRACK: return u8"MediaNext";
			case VK_MEDIA_PREV_TRACK: return u8"MediaPrev";
			case VK_MEDIA_STOP: return u8"MediaStop";
			case VK_MEDIA_PLAY_PAUSE: return u8"MediaPlay";
			case VK_LAUNCH_MAIL: return u8"Mail";
			case VK_LAUNCH_MEDIA_SELECT: return u8"MediaSelect";
			case VK_LAUNCH_APP1: return u8"RunApp1";
			case VK_LAUNCH_APP2: return u8"RunApp2";
			case VK_OEM_1: return u8";:";
			case VK_OEM_PLUS: return u8"=+";
			case VK_OEM_COMMA: return u8",<";
			case VK_OEM_MINUS: return u8"-_";
			case VK_OEM_PERIOD: return u8".>";
			case VK_OEM_2: return u8"/?";
			case VK_OEM_3: return u8"`~";
			case VK_OEM_4: return u8"[{";
			case VK_OEM_5: return u8"\\|";
			case VK_OEM_6: return u8"]}";
			case VK_OEM_7: return u8"\'\"";
			case VK_OEM_8: return u8"Oem8";
			case VK_OEM_102: return u8"Oem102";
			case VK_PROCESSKEY: return u8"Process";
			case VK_PACKET: return u8"Packet";
			case VK_ATTN: return u8"Attn";
			case VK_CRSEL: return u8"CrSel";
			case VK_EXSEL: return u8"ExSel";
			case VK_EREOF: return u8"ErEof";
			case VK_PLAY: return u8"Play";
			case VK_ZOOM: return u8"Zoom";
			case VK_NONAME: return u8"NoName";
			case VK_PA1: return u8"Pa1";
			case VK_OEM_CLEAR: return u8"OemClear";
			}
		}
		return u8"none";
	}
};