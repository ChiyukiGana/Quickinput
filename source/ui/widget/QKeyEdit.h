#pragma once
#include <string>
#include <windows.h>
#include <Xinput.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qevent.h>
#pragma comment(lib,"xinput.lib")

#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B

QT_BEGIN_NAMESPACE

class QKeyEdit : public QLineEdit
{
	Q_OBJECT;
	using This = QKeyEdit;

public:
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
	struct Mode
	{
		enum
		{
			solid,
			combination
		};
	};
	struct Key
	{
		enum Type
		{
			keyboard,
			mouse,
			pad
		};
		int keyCode = 0;
		int modifiers = 0;
		Key() {}
		Key(int _keyCode, int _modifiers = 0) : keyCode(_keyCode), modifiers(_modifiers) {}
	};

private:
	struct XBoxPadState
	{
		bool start;
		bool back;
		bool a;
		bool b;
		bool x;
		bool y;
		bool up;
		bool down;
		bool left;
		bool right;
		bool l_joy_press;
		bool l_joy_up;
		bool l_joy_down;
		bool l_joy_left;
		bool l_joy_right;
		bool r_joy_press;
		bool r_joy_up;
		bool r_joy_down;
		bool r_joy_left;
		bool r_joy_right;
		bool l_top;
		bool r_top;
		bool l_bottom;
		bool r_bottom;
	};
	struct KeyState
	{
		int keyCode = 0;
		bool press = false;
	};

private:
	QTimer* padTimer = new QTimer(this);;

	int edit_mode = Mode::combination;
	int key_max = 0;
	bool editing = false;
	bool keyboard = false;
	bool mouse = false;
	bool wheel = false;
	bool pad = false;

	bool mod_control = false;
	bool mod_shift = false;
	bool mod_alt = false;
	bool s_mod_control = false;
	bool s_mod_shift = false;
	bool s_mod_alt = false;

	QList<KeyState> keystates;

	XBoxPadState padPrev = {};
	XBoxPadState padNext = {};

Q_SIGNALS:
	void changed() const;

public:
	static bool isMouse(int keyCode) { return ((keyCode >= VK_LBUTTON && keyCode <= VK_XBUTTON2) || keyCode == VK_WHEELUP || keyCode == VK_WHEELDOWN); }
	static bool isKeyboard(int keyCode) { return ((keyCode >= VK_CLEAR && keyCode <= VK_OEM_CLEAR) || keyCode == VK_BACK || keyCode == VK_TAB); }
	static bool isPad(int keyCode) { return ((keyCode >= XBoxPadButton::start) && (keyCode <= XBoxPadButton::r_bottom)); }

	static QString keyName(int keyCode)
	{
		if (isPad(keyCode))
		{
			return padName(keyCode);
		}
		else
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
		return u8"";
	}
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
		case XBoxPadButton::l_joy_up: return u8"LJoy↑";
		case XBoxPadButton::l_joy_down: return u8"LJoy↓";
		case XBoxPadButton::l_joy_left: return u8"LJoy←";
		case XBoxPadButton::l_joy_right: return u8"LJoy→";
		case XBoxPadButton::r_joy_up: return u8"RJoy↑";
		case XBoxPadButton::r_joy_down: return u8"RJoy↓";
		case XBoxPadButton::r_joy_left: return u8"RJoy←";
		case XBoxPadButton::r_joy_right: return u8"RJoy→";
		case XBoxPadButton::l_top: return u8"PadLT";
		case XBoxPadButton::l_bottom: return u8"PadLB";
		case XBoxPadButton::r_top: return u8"PadRT";
		case XBoxPadButton::r_bottom: return u8"PadRB";
		}
		return u8"None";
	}

public:
	QKeyEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{
		setReadOnly(true);
		setAttribute(Qt::WA_InputMethodEnabled, false);
		setAlignment(Qt::AlignCenter);
		setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
		setKeyboardEnable(true);
		setMode(Mode::combination);
		setMaxKeys(1);
		connect(padTimer, &QTimer::timeout, this, &This::XBoxPadStateTimer);
	}

	void setKeyboardEnable(bool enable) { this->keyboard = enable; }
	void setMouseEnable(bool enable) { this->mouse = enable; }
	void setWheelEnable(bool enable) { this->wheel = enable; }
	void setPadEnable(bool enable) { this->pad = enable; }
	void setMaxKeys(int maxKeys) { this->key_max = maxKeys; }
	void setMode(int mode = Mode::combination) { this->edit_mode = mode; }
	int mode() const { return this->edit_mode; }
	Key key() const
	{
		Key key;
		if (keystates.size())
		{
			key.keyCode = keystates[0].keyCode;
			key.modifiers = ModifiersToMod();
		}
		return key;
	}
	QList<Key> keys() const
	{
		QList<Key> keys;
		for (size_t i = 0; i < keystates.size(); i++)
		{
			Key key;
			key.keyCode = keystates[i].keyCode;
			key.modifiers = ModifiersToMod();
			keys.push_back(key);
		}
		return keys;
	}
	void setKey(const Key& key)
	{
		clear();
		if (key.keyCode)
		{
			QList<Key> keys;
			keys.push_back(key);
			setKeys(keys);
		}
	}
	void setKeys(const QList<Key>& keys)
	{
		clear();
		if (keys.size())
		{
			for (size_t i = 0; i < keys.size(); i++)
			{
				if (keys[i].keyCode)
				{
					KeyState keyState;
					keyState.keyCode = keys[i].keyCode;
					mod_control = keys[i].modifiers & MOD_CONTROL;
					mod_shift = keys[i].modifiers & MOD_SHIFT;
					mod_alt = keys[i].modifiers & MOD_ALT;
					this->keystates.push_back(keyState);
				}
			}
			update();
		}
	}

	void clear()
	{
		keystates.clear();
		mod_control = mod_shift = mod_alt = false;
		s_mod_control = s_mod_shift = s_mod_alt = false;
		update();
	}

private:
	bool padState(XBoxPadState* pPadState) const
	{
		memset(pPadState, 0, sizeof(XBoxPadState));
		XINPUT_STATE state = {};
		if (XInputGetState(0, &state) == NO_ERROR)
		{
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) pPadState->start = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) pPadState->back = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) pPadState->a = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) pPadState->b = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) pPadState->x = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) pPadState->y = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) pPadState->up = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) pPadState->down = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) pPadState->left = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) pPadState->right = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) pPadState->l_joy_press = true;
			if (state.Gamepad.sThumbLY > 0) pPadState->l_joy_up = true;
			if (state.Gamepad.sThumbLY < 0) pPadState->l_joy_down = true;
			if (state.Gamepad.sThumbLX < 0) pPadState->l_joy_left = true;
			if (state.Gamepad.sThumbLX > 0) pPadState->l_joy_right = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) pPadState->r_joy_press = true;
			if (state.Gamepad.sThumbRY > 0) pPadState->r_joy_up = true;
			if (state.Gamepad.sThumbRY < 0) pPadState->r_joy_down = true;
			if (state.Gamepad.sThumbRX < 0) pPadState->r_joy_left = true;
			if (state.Gamepad.sThumbRX > 0) pPadState->r_joy_right = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) pPadState->l_top = true;
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) pPadState->r_top = true;
			pPadState->l_bottom = state.Gamepad.bLeftTrigger;
			pPadState->r_bottom = state.Gamepad.bRightTrigger;
			return true;
		}
		return false;
	}

	QString modName() const
	{
		QString text;
		if (mod_control) text += L"Ctrl + ";
		if (mod_shift) text += L"Shift + ";
		if (mod_alt) text += L"Alt + ";
		return text;
	}

	KeyState* find(int keyCode)
	{
		for (size_t i = 0; i < keystates.size(); i++)
		{
			if (keystates[i].keyCode == keyCode) return &keystates[i];
		}
		return nullptr;
	}

	int MouseToKeyCode(Qt::MouseButton key) const
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
	int ModifiersToMod() const
	{
		int mod = 0;
		if (mod_control) mod |= MOD_CONTROL;
		if (mod_shift) mod |= MOD_SHIFT;
		if (mod_alt) mod |= MOD_ALT;
		return mod;
	}

	void update()
	{
		QString text;
		int size = keystates.size();
		if (size)
		{
			if (edit_mode == Mode::combination)
			{
				if (mod_control || mod_shift || mod_alt) text = modName();
				text += keyName(keystates[0].keyCode);
			}
			else
			{
				for (size_t i = 0; i < size - 1; i++) text += keyName(keystates[i].keyCode) + " + ";
				text += keyName(keystates[size - 1].keyCode);
			}
		}
		else if (edit_mode == Mode::combination && (mod_control || mod_shift || mod_alt))
		{
			text = modName();
		}
		else if (editing) text = "...";
		else text = "None";
		setText(text);
	}

	void beginEdit()
	{
		clear();
		grabMouse();
		grabKeyboard();
		padState(&padNext);
		padPrev = padNext;
		padTimer->start(32);
		editing = true;
		update();
	}
	void endEdit()
	{
		editing = false;
		padTimer->stop();
		releaseMouse();
		releaseKeyboard();
		if (edit_mode == Mode::combination)
		{
			int count = mod_control + mod_shift + mod_alt;
			if (!keystates.size())
			{
				if (count == 1)
				{
					if (mod_control)
					{
						mod_control = mod_shift = mod_alt = false;
						press(VK_CONTROL), release(VK_CONTROL);
					}
					if (mod_shift)
					{
						mod_control = mod_shift = mod_alt = false;
						press(VK_SHIFT), release(VK_SHIFT);
					}
					if (mod_alt)
					{
						mod_control = mod_shift = mod_alt = false;
						press(VK_MENU), release(VK_MENU);
					}
					return;
				}
				else mod_control = mod_shift = mod_alt = false;
			}
		}
		update();
		changed();
	}

	bool isReleased() const
	{
		for (size_t i = 0; i < keystates.size(); i++) if (keystates[i].press) return false;
		if (s_mod_control || s_mod_shift || s_mod_alt) return false;
		return true;
	}
	void release(int keyCode)
	{
		for (size_t i = 0; i < keystates.size(); i++) if (keystates[i].keyCode == keyCode) { keystates[i].press = false; return; }
	}
	void press(int keyCode)
	{
		if (edit_mode == Mode::combination)
		{
			if (keystates.size() == 1)
			{
				keystates[0].keyCode = keyCode;
				keystates[0].press = true;
				update();
				return;
			}
		}
		else if (edit_mode == Mode::solid)
		{
			if (keystates.size() >= key_max) return;
			if ("reset state")
			{
				KeyState* pKeyState = find(keyCode);
				if (pKeyState)
				{
					pKeyState->press = true;
					update();
					return;
				}
			}
		}
		if ("add new")
		{
			KeyState keyState;
			keyState.keyCode = keyCode;
			keyState.press = true;
			keystates.push_back(keyState);
			update();
		}
	}

private:
	bool event(QEvent* e)
	{
		if (e->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouse = (QMouseEvent*)e;
			if (editing && mouse)
			{
				if (edit_mode != Mode::combination) press(MouseToKeyCode(mouse->button()));
			}
			return true;
		}
		else if (e->type() == QEvent::MouseButtonDblClick)
		{
			QMouseEvent* mouse = (QMouseEvent*)e;
			if (editing && mouse)
			{
				if (edit_mode != Mode::combination) press(MouseToKeyCode(mouse->button()));
			}
			return true;
		}
		else if (e->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouse = (QMouseEvent*)e;
			if (editing && mouse)
			{
				if (edit_mode != Mode::combination) release(MouseToKeyCode(mouse->button()));
				if (isReleased()) endEdit();
			}
			else if (mouse->button() == Qt::LeftButton)
			{
				beginEdit();
			}
			return true;
		}
		else if (e->type() == QEvent::Wheel)
		{
			QWheelEvent* mouseWheel = (QWheelEvent*)e;
			if (editing && wheel)
			{
				if (edit_mode != Mode::combination)
				{
					if (mouseWheel->angleDelta().y() > 0) press(VK_WHEELUP), release(VK_WHEELUP);
					else press(VK_WHEELDOWN), release(VK_WHEELDOWN);
				}
				if (isReleased()) endEdit();
			}
			return true;
		}
		else if (e->type() == QEvent::KeyPress)
		{
			QKeyEvent* key = (QKeyEvent*)e;
			if (editing && keyboard && !key->isAutoRepeat())
			{
				int keyCode = MapVirtualKeyW(key->nativeScanCode(), MAPVK_VSC_TO_VK_EX);
				if (edit_mode == Mode::combination)
				{
					bool isMod = false;
					if (keyCode == VK_CONTROL || keyCode == VK_LCONTROL || keyCode == VK_RCONTROL) s_mod_control = mod_control = true, isMod = true;
					if (keyCode == VK_SHIFT || keyCode == VK_LSHIFT || keyCode == VK_RSHIFT) s_mod_shift = mod_shift = true, isMod = true;
					if (keyCode == VK_MENU || keyCode == VK_LMENU || keyCode == VK_RMENU) s_mod_alt = mod_alt = true, isMod = true;
					if (isMod) update();
					else press(keyCode);
				}
				else press(keyCode);
			}
			return true;
		}
		else if (e->type() == QEvent::KeyRelease)
		{
			QKeyEvent* key = (QKeyEvent*)e;
			if (editing && keyboard && !key->isAutoRepeat())
			{
				int keyCode = MapVirtualKeyW(key->nativeScanCode(), MAPVK_VSC_TO_VK_EX);
				if (edit_mode == Mode::combination)
				{
					bool isMod = false;
					if (keyCode == VK_CONTROL || keyCode == VK_LCONTROL || keyCode == VK_RCONTROL) s_mod_control = mod_control = false, isMod = true;
					if (keyCode == VK_SHIFT || keyCode == VK_LSHIFT || keyCode == VK_RSHIFT) s_mod_shift = mod_shift = false, isMod = true;
					if (keyCode == VK_MENU || keyCode == VK_LMENU || keyCode == VK_RMENU) s_mod_alt = mod_alt = false, isMod = true;
					if (!isMod) release(keyCode);
				}
				else release(keyCode);
				if (isReleased()) endEdit();
			}
			return true;
		}
		return QLineEdit::event(e);
	}

	void padEvent(short keyCode, bool state)
	{
		if (editing && pad)
		{
			if (edit_mode != Mode::combination)
			{
				if (state) press(keyCode);
				else release(keyCode);
			}
			if (isReleased()) endEdit();
		}
	}

private Q_SLOTS:
	void XBoxPadStateTimer()
	{
		if (editing && pad)
		{
			if (edit_mode != Mode::combination)
			{
				if (padState(&padNext))
				{
					if (padNext.start != padPrev.start) padEvent(XBoxPadButton::start, padNext.start);
					if (padNext.back != padPrev.back) padEvent(XBoxPadButton::back, padNext.back);
					if (padNext.a != padPrev.a) padEvent(XBoxPadButton::a, padNext.a);
					if (padNext.b != padPrev.b) padEvent(XBoxPadButton::b, padNext.b);
					if (padNext.x != padPrev.x) padEvent(XBoxPadButton::x, padNext.x);
					if (padNext.y != padPrev.y) padEvent(XBoxPadButton::y, padNext.y);
					if (padNext.up != padPrev.up) padEvent(XBoxPadButton::up, padNext.up);
					if (padNext.down != padPrev.down) padEvent(XBoxPadButton::down, padNext.down);
					if (padNext.left != padPrev.left) padEvent(XBoxPadButton::left, padNext.left);
					if (padNext.right != padPrev.right) padEvent(XBoxPadButton::right, padNext.right);
					if (padNext.l_joy_press != padPrev.l_joy_press) padEvent(XBoxPadButton::l_joy_press, padNext.l_joy_press);
					if (padNext.l_joy_up != padPrev.l_joy_up) padEvent(XBoxPadButton::l_joy_up, padNext.l_joy_up);
					if (padNext.l_joy_down != padPrev.l_joy_down) padEvent(XBoxPadButton::l_joy_down, padNext.l_joy_down);
					if (padNext.l_joy_left != padPrev.l_joy_left) padEvent(XBoxPadButton::l_joy_left, padNext.l_joy_left);
					if (padNext.l_joy_right != padPrev.l_joy_right) padEvent(XBoxPadButton::l_joy_right, padNext.l_joy_right);
					if (padNext.r_joy_press != padPrev.r_joy_press) padEvent(XBoxPadButton::r_joy_press, padNext.r_joy_press);
					if (padNext.r_joy_up != padPrev.r_joy_up) padEvent(XBoxPadButton::r_joy_up, padNext.r_joy_up);
					if (padNext.r_joy_down != padPrev.r_joy_down) padEvent(XBoxPadButton::r_joy_down, padNext.r_joy_down);
					if (padNext.r_joy_left != padPrev.r_joy_left) padEvent(XBoxPadButton::r_joy_left, padNext.r_joy_left);
					if (padNext.r_joy_right != padPrev.r_joy_right) padEvent(XBoxPadButton::r_joy_right, padNext.r_joy_right);
					if (padNext.l_top != padPrev.l_top) padEvent(XBoxPadButton::l_top, padNext.l_top);
					if (padNext.r_top != padPrev.r_top) padEvent(XBoxPadButton::r_top, padNext.r_top);
					if (padNext.l_bottom != padPrev.l_bottom) padEvent(XBoxPadButton::l_bottom, padNext.l_bottom);
					if (padNext.r_bottom != padPrev.r_bottom) padEvent(XBoxPadButton::r_bottom, padNext.r_bottom);
					padPrev = padNext;
				}
			}
		}
	}
};

QT_END_NAMESPACE