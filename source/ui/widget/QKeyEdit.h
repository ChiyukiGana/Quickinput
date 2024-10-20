#pragma execution_character_set("utf-8")
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
	static bool isMouse(int keyCode)
	{
		return ((keyCode >= VK_LBUTTON && keyCode <= VK_XBUTTON2) || keyCode == VK_WHEELUP || keyCode == VK_WHEELDOWN);
	}
	static bool isKeyboard(int keyCode)
	{
		return ((keyCode >= VK_CLEAR && keyCode <= VK_OEM_CLEAR) || keyCode == VK_BACK || keyCode == VK_TAB);
	}
	static bool isPad(int keyCode)
	{
		return ((keyCode >= XBoxPadButton::start) && (keyCode <= XBoxPadButton::r_bottom));
	}

	static QString keyName(int keyCode)
	{
		if (isPad(keyCode))
		{
			return padName(keyCode);
		}
		else
		{
			switch (keyCode) {
			case VK_LBUTTON: return "左键";
			case VK_RBUTTON: return "右键";
			case VK_CANCEL: return "Cancel";
			case VK_MBUTTON: return "中键";
			case VK_XBUTTON1: return "侧键1";
			case VK_XBUTTON2: return "侧键2";
			case VK_WHEELUP: return "滚轮↑";
			case VK_WHEELDOWN: return "滚轮↓";
			case VK_BACK: return "Back";
			case VK_TAB: return "Tab";
			case VK_CLEAR: return "Clear";
			case VK_RETURN: return "Return";
			case VK_SHIFT: return "Shift";
			case VK_CONTROL: return "Ctrl";
			case VK_MENU: return "Alt";
			case VK_PAUSE: return "Pause";
			case VK_CAPITAL: return "CapsLock";
			case VK_KANA: return "Kana";
			case VK_JUNJA: return "Junja";
			case VK_FINAL: return "Final";
			case VK_HANJA: return "Hanja";
			case VK_ESCAPE: return "Esc";
			case VK_CONVERT: return "Convert";
			case VK_NONCONVERT: return "NonConvert";
			case VK_ACCEPT: return "Accept";
			case VK_MODECHANGE: return "ModeChange";
			case VK_SPACE: return "空格";
			case VK_PRIOR: return "PageUp";
			case VK_NEXT: return "PageDown";
			case VK_END: return "End";
			case VK_HOME: return "Home";
			case VK_LEFT: return "←";
			case VK_UP: return "↑";
			case VK_RIGHT: return "→";
			case VK_DOWN: return "↓";
			case VK_SELECT: return "Select";
			case VK_PRINT: return "Print";
			case VK_EXECUTE: return "Execute";
			case VK_SNAPSHOT: return "ScreenShot";
			case VK_INSERT: return "Insert";
			case VK_DELETE: return "Delete";
			case VK_HELP: return "Help";
			case 0x30: return "0";
			case 0x31: return "1";
			case 0x32: return "2";
			case 0x33: return "3";
			case 0x34: return "4";
			case 0x35: return "5";
			case 0x36: return "6";
			case 0x37: return "7";
			case 0x38: return "8";
			case 0x39: return "9";
			case 0x41: return "A";
			case 0x42: return "B";
			case 0x43: return "C";
			case 0x44: return "D";
			case 0x45: return "E";
			case 0x46: return "F";
			case 0x47: return "G";
			case 0x48: return "H";
			case 0x49: return "I";
			case 0x4A: return "J";
			case 0x4B: return "K";
			case 0x4C: return "L";
			case 0x4D: return "M";
			case 0x4E: return "N";
			case 0x4F: return "O";
			case 0x50: return "P";
			case 0x51: return "Q";
			case 0x52: return "R";
			case 0x53: return "S";
			case 0x54: return "T";
			case 0x55: return "U";
			case 0x56: return "V";
			case 0x57: return "W";
			case 0x58: return "X";
			case 0x59: return "Y";
			case 0x5A: return "Z";
			case VK_LWIN: return "LWin";
			case VK_RWIN: return "RWin";
			case VK_APPS: return "Apps";
			case VK_SLEEP: return "Sleep";
			case VK_NUMPAD0: return "Num0";
			case VK_NUMPAD1: return "Num1";
			case VK_NUMPAD2: return "Num2";
			case VK_NUMPAD3: return "Num3";
			case VK_NUMPAD4: return "Num4";
			case VK_NUMPAD5: return "Num5";
			case VK_NUMPAD6: return "Num6";
			case VK_NUMPAD7: return "Num7";
			case VK_NUMPAD8: return "Num8";
			case VK_NUMPAD9: return "Num9";
			case VK_MULTIPLY: return "Num*";
			case VK_ADD: return "Num+";
			case VK_SEPARATOR: return "NumEnter";
			case VK_SUBTRACT: return "Num-";
			case VK_DECIMAL: return "Num.";
			case VK_DIVIDE: return "Num/";
			case VK_F1: return "F1";
			case VK_F2: return "F2";
			case VK_F3: return "F3";
			case VK_F4: return "F4";
			case VK_F5: return "F5";
			case VK_F6: return "F6";
			case VK_F7: return "F7";
			case VK_F8: return "F8";
			case VK_F9: return "F9";
			case VK_F10: return "F10";
			case VK_F11: return "F11";
			case VK_F12: return "F12";
			case VK_F13: return "F13";
			case VK_F14: return "F14";
			case VK_F15: return "F15";
			case VK_F16: return "F16";
			case VK_F17: return "F17";
			case VK_F18: return "F18";
			case VK_F19: return "F19";
			case VK_F20: return "F20";
			case VK_F21: return "F21";
			case VK_F22: return "F22";
			case VK_F23: return "F23";
			case VK_F24: return "F24";
			case VK_NUMLOCK: return "NumLock";
			case VK_SCROLL: return "ScrollLock";
			case VK_LSHIFT: return "LShift";
			case VK_RSHIFT: return "RShift";
			case VK_LCONTROL: return "LCtrl";
			case VK_RCONTROL: return "RCtrl";
			case VK_LMENU: return "LAlt";
			case VK_RMENU: return "RAlt";
			case VK_BROWSER_BACK: return "BrowserBack";
			case VK_BROWSER_FORWARD: return "BrowserForward";
			case VK_BROWSER_REFRESH: return "BrowserRefresh";
			case VK_BROWSER_STOP: return "BrowserStop";
			case VK_BROWSER_SEARCH: return "BrowserSearch";
			case VK_BROWSER_FAVORITES: return "BrowserFavorites";
			case VK_BROWSER_HOME: return "BrowserHome";
			case VK_VOLUME_MUTE: return "VolumeMute";
			case VK_VOLUME_DOWN: return "VolumeDown";
			case VK_VOLUME_UP: return "VolumeUp";
			case VK_MEDIA_NEXT_TRACK: return "MediaNext";
			case VK_MEDIA_PREV_TRACK: return "MediaPrev";
			case VK_MEDIA_STOP: return "MediaStop";
			case VK_MEDIA_PLAY_PAUSE: return "MediaPlay";
			case VK_LAUNCH_MAIL: return "Mail";
			case VK_LAUNCH_MEDIA_SELECT: return "MediaSelect";
			case VK_LAUNCH_APP1: return "RunApp1";
			case VK_LAUNCH_APP2: return "RunApp2";
			case VK_OEM_1: return ";:";
			case VK_OEM_PLUS: return "=+";
			case VK_OEM_COMMA: return ",<";
			case VK_OEM_MINUS: return "-_";
			case VK_OEM_PERIOD: return ".>";
			case VK_OEM_2: return "/?";
			case VK_OEM_3: return "`~";
			case VK_OEM_4: return "[{";
			case VK_OEM_5: return "\\|";
			case VK_OEM_6: return "]}";
			case VK_OEM_7: return "\'\"";
			case VK_OEM_8: return "Oem8";
			case VK_OEM_102: return "Oem102";
			case VK_PROCESSKEY: return "Process";
			case VK_PACKET: return "Packet";
			case VK_ATTN: return "Attn";
			case VK_CRSEL: return "CrSel";
			case VK_EXSEL: return "ExSel";
			case VK_EREOF: return "ErEof";
			case VK_PLAY: return "Play";
			case VK_ZOOM: return "Zoom";
			case VK_NONAME: return "NoName";
			case VK_PA1: return "Pa1";
			case VK_OEM_CLEAR: return "OemClear";
			}
		}
		return "";
	}
	static QString padName(short keyCode)
	{
		switch (keyCode)
		{
		case XBoxPadButton::a: return "PadA";
		case XBoxPadButton::b: return "PadB";
		case XBoxPadButton::x: return "PadX";
		case XBoxPadButton::y: return "PadY";
		case XBoxPadButton::up: return "Pad↑";
		case XBoxPadButton::down: return "Pad↓";
		case XBoxPadButton::left: return "Pad←";
		case XBoxPadButton::right: return "Pad→";
		case XBoxPadButton::l_joy_up: return "LJoy↑";
		case XBoxPadButton::l_joy_down: return "LJoy↓";
		case XBoxPadButton::l_joy_left: return "LJoy←";
		case XBoxPadButton::l_joy_right: return "LJoy→";
		case XBoxPadButton::r_joy_up: return "RJoy↑";
		case XBoxPadButton::r_joy_down: return "RJoy↓";
		case XBoxPadButton::r_joy_left: return "RJoy←";
		case XBoxPadButton::r_joy_right: return "RJoy→";
		case XBoxPadButton::l_top: return "PadLT";
		case XBoxPadButton::l_bottom: return "PadLB";
		case XBoxPadButton::r_top: return "PadRT";
		case XBoxPadButton::r_bottom: return "PadRB";
		}
		return "None";
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
		connect(padTimer, SIGNAL(timeout()), this, SLOT(XBoxPadStateTimer()));
	}

	void setKeyboardEnable(bool enable)
	{
		this->keyboard = enable;
	}
	void setMouseEnable(bool enable)
	{
		this->mouse = enable;
	}
	void setWheelEnable(bool enable)
	{
		this->wheel = enable;
	}
	void setPadEnable(bool enable)
	{
		this->pad = enable;
	}
	void setMaxKeys(int maxKeys)
	{
		this->key_max = maxKeys;
	}

	int mode() const
	{
		return this->edit_mode;
	}
	void setMode(int mode = Mode::combination)
	{
		this->edit_mode = mode;
	}

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
	int KeyboardToKeyCode(int key, Qt::KeyboardModifiers mod) const
	{
		switch (key)
		{
		case Qt::Key_Escape: return VK_ESCAPE;
		case Qt::Key_F1: return VK_F1;
		case Qt::Key_F2: return VK_F2;
		case Qt::Key_F3: return VK_F3;
		case Qt::Key_F4: return VK_F4;
		case Qt::Key_F5: return VK_F5;
		case Qt::Key_F6: return VK_F6;
		case Qt::Key_F7: return VK_F7;
		case Qt::Key_F8: return VK_F8;
		case Qt::Key_F9: return VK_F9;
		case Qt::Key_F10: return VK_F10;
		case Qt::Key_F11: return VK_F11;
		case Qt::Key_F12: return VK_F12;
		case Qt::Key_F13: return VK_F13;
		case Qt::Key_F14: return VK_F14;
		case Qt::Key_F15: return VK_F15;
		case Qt::Key_F16: return VK_F16;
		case Qt::Key_F17: return VK_F17;
		case Qt::Key_F18: return VK_F18;
		case Qt::Key_F19: return VK_F19;
		case Qt::Key_F20: return VK_F20;
		case Qt::Key_F21: return VK_F21;
		case Qt::Key_F22: return VK_F22;
		case Qt::Key_F23: return VK_F23;
		case Qt::Key_F24: return VK_F24;

		case Qt::Key_QuoteLeft: return VK_OEM_3;
		case Qt::Key_AsciiTilde: return VK_OEM_3;
		case Qt::Key_1: if (mod & Qt::KeypadModifier) return VK_NUMPAD1; return '1';
		case Qt::Key_Exclam: return '1';
		case Qt::Key_2: if (mod & Qt::KeypadModifier) return VK_NUMPAD2; return '2';
		case Qt::Key_At: return '2';
		case Qt::Key_3: if (mod & Qt::KeypadModifier) return VK_NUMPAD3; return '3';
		case Qt::Key_NumberSign: return '3';
		case Qt::Key_4: if (mod & Qt::KeypadModifier) return VK_NUMPAD4; return '4';
		case Qt::Key_Dollar: return '4';
		case Qt::Key_5: if (mod & Qt::KeypadModifier) return VK_NUMPAD5; return '5';
		case Qt::Key_Percent: return '5';
		case Qt::Key_6: if (mod & Qt::KeypadModifier) return VK_NUMPAD6; return '6';
		case Qt::Key_AsciiCircum: return '6';
		case Qt::Key_7: if (mod & Qt::KeypadModifier) return VK_NUMPAD7; return '7';
		case Qt::Key_Ampersand: return '7';
		case Qt::Key_8: if (mod & Qt::KeypadModifier) return VK_NUMPAD8; return '8';
		case Qt::Key_Asterisk: if (mod & Qt::KeypadModifier) return VK_MULTIPLY; return '8';
		case Qt::Key_9: if (mod & Qt::KeypadModifier) return VK_NUMPAD9; return '9';
		case Qt::Key_ParenLeft: return '9';
		case Qt::Key_0: if (mod & Qt::KeypadModifier) return VK_NUMPAD0; return '0';
		case Qt::Key_ParenRight: return '0';
		case Qt::Key_Minus: if (mod & Qt::KeypadModifier) return VK_SUBTRACT; return VK_OEM_MINUS;
		case Qt::Key_Underscore: if (mod & Qt::KeypadModifier) return VK_SUBTRACT; return VK_OEM_MINUS;
		case Qt::Key_Plus: if (mod & Qt::KeypadModifier) return VK_ADD; return VK_OEM_PLUS;
		case Qt::Key_Equal: if (mod & Qt::KeypadModifier) return VK_ADD; return VK_OEM_PLUS;
		case Qt::Key_Backspace: return VK_BACK;


		case Qt::Key_Tab: return VK_TAB;
		case Qt::Key_Q: return 'Q';
		case Qt::Key_W: return 'W';
		case Qt::Key_E: return 'E';
		case Qt::Key_R: return 'R';
		case Qt::Key_T: return 'T';
		case Qt::Key_Y: return 'Y';
		case Qt::Key_U: return 'U';
		case Qt::Key_I: return 'I';
		case Qt::Key_O: return 'O';
		case Qt::Key_P: return 'P';
		case Qt::Key_BracketLeft: return VK_OEM_4;
		case Qt::Key_BraceLeft: return VK_OEM_4;
		case Qt::Key_BracketRight: return VK_OEM_6;
		case Qt::Key_BraceRight: return VK_OEM_6;
		case Qt::Key_Backslash: return VK_OEM_5;
		case Qt::Key_Bar: return VK_OEM_5;

		case Qt::Key_CapsLock: return VK_CAPITAL;
		case Qt::Key_A: return 'A';
		case Qt::Key_S: return 'S';
		case Qt::Key_D: return 'D';
		case Qt::Key_F: return 'F';
		case Qt::Key_G: return 'G';
		case Qt::Key_H: return 'H';
		case Qt::Key_J: return 'J';
		case Qt::Key_K: return 'K';
		case Qt::Key_L: return 'L';
		case Qt::Key_Colon: return VK_OEM_1;
		case Qt::Key_Semicolon: return VK_OEM_1;
		case Qt::Key_QuoteDbl: return VK_OEM_7;
		case Qt::Key_Apostrophe: return VK_OEM_7;
		case Qt::Key_Return: return VK_RETURN;

		case Qt::Key_Shift: return VK_SHIFT;
		case Qt::Key_Z: return 'Z';
		case Qt::Key_X: return 'X';
		case Qt::Key_C: return 'C';
		case Qt::Key_V: return 'V';
		case Qt::Key_B: return 'B';
		case Qt::Key_N: return 'N';
		case Qt::Key_M: return 'M';
		case Qt::Key_Comma: return VK_OEM_COMMA;
		case Qt::Key_Less: return VK_OEM_COMMA;
		case Qt::Key_Period: if (mod & Qt::KeypadModifier) return VK_DECIMAL; return VK_OEM_PERIOD;
		case Qt::Key_Greater: if (mod & Qt::KeypadModifier) return VK_DECIMAL; return VK_OEM_PERIOD;
		case Qt::Key_Slash: if (mod & Qt::KeypadModifier) return VK_DIVIDE; return VK_OEM_2;
		case Qt::Key_Question: if (mod & Qt::KeypadModifier) return VK_DIVIDE; return VK_OEM_2;

		case Qt::Key_Control: return VK_CONTROL;
		case Qt::Key_Alt: return VK_MENU;
		case Qt::Key_Space: return VK_SPACE;
		case Qt::Key_Menu: return VK_APPS;

		case Qt::Key_ScrollLock: return VK_SCROLL;
		case Qt::Key_Pause: return VK_PAUSE;
		case Qt::Key_Insert: return VK_INSERT;
		case Qt::Key_Home: return VK_HOME;
		case Qt::Key_PageUp: return VK_PRIOR;
		case Qt::Key_Delete: return VK_DELETE;
		case Qt::Key_End: return VK_END;
		case Qt::Key_PageDown: return VK_NEXT;

		case Qt::Key_Up: return VK_UP;
		case Qt::Key_Left: return VK_LEFT;
		case Qt::Key_Down: return VK_DOWN;
		case Qt::Key_Right: return VK_RIGHT;

		case Qt::Key_NumLock: return VK_NUMLOCK;
		case Qt::Key_Enter: return VK_SEPARATOR;
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
				int keyCode = KeyboardToKeyCode(key->key(), key->modifiers());
				if (edit_mode == Mode::combination)
				{
					bool isMod = false;
					if (keyCode == VK_CONTROL) s_mod_control = mod_control = true, isMod = true;
					if (keyCode == VK_SHIFT) s_mod_shift = mod_shift = true, isMod = true;
					if (keyCode == VK_MENU) s_mod_alt = mod_alt = true, isMod = true;
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
				int keyCode = KeyboardToKeyCode(key->key(), key->modifiers());
				if (edit_mode == Mode::combination)
				{
					bool isMod = false;
					if (keyCode == VK_CONTROL) s_mod_control = false, isMod = true;
					if (keyCode == VK_SHIFT) s_mod_shift = false, isMod = true;
					if (keyCode == VK_MENU) s_mod_alt = false, isMod = true;
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
				Sleep(32);
			}
		}
	}
};

QT_END_NAMESPACE