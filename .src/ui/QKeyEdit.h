﻿#pragma once

#include <windows.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <qpainter.h>
#include <qevent.h>

#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B

class  QKeyEdit : public QLabel
{
	Q_OBJECT;

	struct Keys
	{
		quint8 type = 0; // 0: keybd, 1: mouse, 2: wheel
		bool wheel = 0; // 0: up, 1: dwon
		Qt::MouseButton mouse = Qt::NoButton;
		Qt::KeyboardModifiers mod = Qt::NoModifier;
		int key = 0;
		bool state = 0;
	};

	QString text = u8"None";
	quint8 mode = 2; // 0: solid, 1: mod + key, 2: key + key
	bool input = 0;
	bool disable = 0;
	Keys keys[2];

public:
	struct QKeybdStruct { int key = 0; Qt::KeyboardModifiers mod = Qt::NoModifier; };

	QKeyEdit(QWidget* parent = nullptr) : QLabel(parent) {
		setAttribute(Qt::WA_StyledBackground);
		setMinimumSize(QSize(48, 24));
		setMaximumSize(QSize(16777215, 24));
		setWindowFlags(Qt::FramelessWindowHint);
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		setStyleSheet(QString::fromUtf8(u8"background-color: white; border: 1px solid gray;"));
		setAlignment(Qt::AlignCenter);
		setText(text);
	}

	// 0: keybd, 1: mouse
	bool Type(BYTE vk)
	{
		if (vk == VK_LBUTTON || vk == VK_RBUTTON || vk == VK_MBUTTON || vk == VK_XBUTTON1 || vk == VK_XBUTTON2 || vk == VK_WHEELUP || vk == VK_WHEELDOWN) return 1;
		return 0;
	}

	// 0: solid, 1: mod + key, 2: key + key
	void Mode(quint8 mode) {
		this->mode = mode;
	}

	QString Name()
	{
		return text;
	}

	void VirtualKey(BYTE k1, USHORT k2 = 0)
	{
		keys[0].state = 0, keys[1].state = 0;

		if (k1 == 0) Paint();
		if (mode == 0)
		{
			if (Type(k1))
			{
				if (k1 == VK_WHEELUP || k1 == VK_WHEELDOWN)
				{
					keys[0].type = 2;
					keys[0].state = 1;
					if (k1 == VK_WHEELUP) keys[0].wheel = 0;
					else keys[0].wheel = 1;
				}
				keys[0].type = 1;
				keys[0].state = 1;
				keys[0].mouse = MouseToQk(k1);
			}
			else
			{
				QKeybdStruct ks = KeybdToQk(k1);
				keys[0].type = 0;
				keys[0].state = 1;
				keys[0].key = ks.key;
				keys[0].mod = ks.mod;
			}
		}
		else if (mode == 1)
		{
			QKeybdStruct ks = KeybdToQk(k1);
			keys[0].type = 0;
			keys[0].state = 1;
			keys[0].key = ks.key;
			keys[0].mod |= ks.mod;
			if (k2 & MOD_CONTROL) keys[0].mod |= Qt::ControlModifier;
			if (k2 & MOD_SHIFT) keys[0].mod |= Qt::ShiftModifier;
			if (k2 & MOD_ALT) keys[0].mod |= Qt::AltModifier;
		}
		else if (mode == 2)
		{
			if (Type(k1))
			{
				if (k1 != VK_WHEELUP && k1 != VK_WHEELDOWN)
				{
					keys[0].type = 1;
					keys[0].state = 1;
					keys[0].mouse = MouseToQk(k1);
				}
			}
			else
			{
				QKeybdStruct ks = KeybdToQk(k1);
				keys[0].state = 1;
				keys[0].type = 0;
				keys[0].key = ks.key;
				keys[0].mod |= ks.mod;
			}
			if (Type(k2) && k2)
			{
				if (k2 != VK_WHEELUP && k2 != VK_WHEELDOWN)
				{
					keys[1].type = 1;
					keys[1].state = 1;
					keys[1].mouse = MouseToQk(k2);
				}
			}
			else if (k2)
			{
				QKeybdStruct ks = KeybdToQk(k2);
				keys[1].type = 0;
				keys[1].state = 1;
				keys[1].key = ks.key;
				keys[1].mod |= ks.mod;
			}
		}
		Paint();
	}

	DWORD virtualKey()
	{
		USHORT k1 = 0, k2 = 0;
		if (mode == 0)
		{
			if (keys[0].state)
			{
				if (keys[0].type == 0) k1 = KeybdToVk(keys[0].key, keys[0].mod);
				else if (keys[0].type == 1) k1 = MouseToVk(keys[0].mouse);
				else if (keys[0].type == 2)
				{
					if (keys[0].wheel) k1 = VK_WHEELDOWN;
					else k1 = VK_WHEELUP;
				}
			}
		}
		else if (mode == 1)
		{
			if (keys[0].state && keys[0].type == 0)
			{
				k1 = KeybdToVk(keys[0].key, keys[0].mod);
				if (keys[0].mod & Qt::ControlModifier) k2 |= MOD_CONTROL;
				if (keys[0].mod & Qt::ShiftModifier) k2 |= MOD_SHIFT;
				if (keys[0].mod & Qt::AltModifier) k2 |= MOD_ALT;
			}
		}
		else if (mode == 2)
		{
			if (keys[0].state)
			{
				if (keys[0].type == 0) k1 = KeybdToVk(keys[0].key, keys[0].mod);
				else if (keys[0].type == 1) k1 = MouseToVk(keys[0].mouse);
				else if (keys[0].type == 2)
				{
					if (keys[0].wheel) k1 = VK_WHEELDOWN;
					else k1 = VK_WHEELUP;
				}
			}
			if (keys[1].state)
			{
				if (keys[1].type == 0) k2 = KeybdToVk(keys[1].key, keys[1].mod);
				else if (keys[1].type == 1) k2 = MouseToVk(keys[1].mouse);
				else if (keys[1].type == 2)
				{
					if (keys[1].wheel) k2 = VK_WHEELDOWN;
					else k2 = VK_WHEELUP;
				}
			}
		}
		return (k2 << 16) | k1;
	}

signals:

	void changed();

private:
	QString Name(BYTE vk) {
		switch (vk) {
		case VK_LBUTTON: return u8"左键";
		case VK_RBUTTON: return u8"右键";
		case VK_CANCEL: return u8"Cancel";
		case VK_MBUTTON: return u8"中键";
		case VK_XBUTTON1: return u8"侧键1";
		case VK_XBUTTON2: return u8"侧键2";
		case VK_WHEELUP: return u8"滚轮上";
		case VK_WHEELDOWN: return u8"滚轮下";
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
		return u8"None";
	}

	BYTE MouseToVk(Qt::MouseButton b)
	{
		switch (b)
		{
		case Qt::LeftButton: return VK_LBUTTON;
		case Qt::RightButton: return VK_RBUTTON;
		case Qt::MiddleButton: return VK_MBUTTON;
		case Qt::XButton1: return VK_XBUTTON1;
		case Qt::XButton2: return VK_XBUTTON2;
		}
		return 0;
	}

	BYTE KeybdToVk(int k, Qt::KeyboardModifiers m)
	{
		switch (k)
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
		case Qt::Key_1: if (m & Qt::KeypadModifier) return VK_NUMPAD1; return '1';
		case Qt::Key_Exclam: return '1';
		case Qt::Key_2: if (m & Qt::KeypadModifier) return VK_NUMPAD2; return '2';
		case Qt::Key_At: return '2';
		case Qt::Key_3: if (m & Qt::KeypadModifier) return VK_NUMPAD3; return '3';
		case Qt::Key_NumberSign: return '3';
		case Qt::Key_4: if (m & Qt::KeypadModifier) return VK_NUMPAD4; return '4';
		case Qt::Key_Dollar: return '4';
		case Qt::Key_5: if (m & Qt::KeypadModifier) return VK_NUMPAD5; return '5';
		case Qt::Key_Percent: return '5';
		case Qt::Key_6: if (m & Qt::KeypadModifier) return VK_NUMPAD6; return '6';
		case Qt::Key_AsciiCircum: return '6';
		case Qt::Key_7: if (m & Qt::KeypadModifier) return VK_NUMPAD7; return '7';
		case Qt::Key_Ampersand: return '7';
		case Qt::Key_8: if (m & Qt::KeypadModifier) return VK_NUMPAD8; return '8';
		case Qt::Key_Asterisk: if (m & Qt::KeypadModifier) return VK_MULTIPLY; return '8';
		case Qt::Key_9: if (m & Qt::KeypadModifier) return VK_NUMPAD9; return '9';
		case Qt::Key_ParenLeft: return '9';
		case Qt::Key_0: if (m & Qt::KeypadModifier) return VK_NUMPAD0; return '0';
		case Qt::Key_ParenRight: return '0';
		case Qt::Key_Minus: if (m & Qt::KeypadModifier) return VK_SUBTRACT; return VK_OEM_MINUS;
		case Qt::Key_Underscore: if (m & Qt::KeypadModifier) return VK_SUBTRACT; return VK_OEM_MINUS;
		case Qt::Key_Plus: if (m & Qt::KeypadModifier) return VK_ADD; return VK_OEM_PLUS;
		case Qt::Key_Equal: if (m & Qt::KeypadModifier) return VK_ADD; return VK_OEM_PLUS;
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
		case Qt::Key_Period: if (m & Qt::KeypadModifier) return VK_DECIMAL; return VK_OEM_PERIOD;
		case Qt::Key_Greater: if (m & Qt::KeypadModifier) return VK_DECIMAL; return VK_OEM_PERIOD;
		case Qt::Key_Slash: if (m & Qt::KeypadModifier) return VK_DIVIDE; return VK_OEM_2;
		case Qt::Key_Question: if (m & Qt::KeypadModifier) return VK_DIVIDE; return VK_OEM_2;

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

	Qt::MouseButton MouseToQk(BYTE b)
	{
		switch (b)
		{
		case VK_LBUTTON: return Qt::LeftButton;
		case VK_RBUTTON: return Qt::RightButton;
		case VK_MBUTTON: return  Qt::MiddleButton;
		case VK_XBUTTON1: return  Qt::XButton1;
		case VK_XBUTTON2: return  Qt::XButton2;
		}
		return Qt::NoButton;
	}

	QKeybdStruct KeybdToQk(BYTE k)
	{
		QKeybdStruct qk;
		switch (k)
		{
		case VK_ESCAPE: qk.key = Qt::Key_Escape; break;
		case VK_F1: qk.key = Qt::Key_F1; break;
		case VK_F2: qk.key = Qt::Key_F2; break;
		case VK_F3: qk.key = Qt::Key_F3; break;
		case VK_F4: qk.key = Qt::Key_F4; break;
		case VK_F5: qk.key = Qt::Key_F5; break;
		case VK_F6: qk.key = Qt::Key_F6; break;
		case VK_F7: qk.key = Qt::Key_F7; break;
		case VK_F8: qk.key = Qt::Key_F8; break;
		case VK_F9: qk.key = Qt::Key_F9; break;
		case VK_F10: qk.key = Qt::Key_F10; break;
		case VK_F11: qk.key = Qt::Key_F11; break;
		case VK_F12: qk.key = Qt::Key_F12; break;
		case VK_F13: qk.key = Qt::Key_F13; break;
		case VK_F14: qk.key = Qt::Key_F14; break;
		case VK_F15: qk.key = Qt::Key_F15; break;
		case VK_F16: qk.key = Qt::Key_F16; break;
		case VK_F17: qk.key = Qt::Key_F17; break;
		case VK_F18: qk.key = Qt::Key_F18; break;
		case VK_F19: qk.key = Qt::Key_F19; break;
		case VK_F20: qk.key = Qt::Key_F20; break;
		case VK_F21: qk.key = Qt::Key_F21; break;
		case VK_F22: qk.key = Qt::Key_F22; break;
		case VK_F23: qk.key = Qt::Key_F23; break;
		case VK_F24: qk.key = Qt::Key_F24; break;

		case VK_OEM_3: qk.key = Qt::Key_QuoteLeft; break;
		case '1': qk.key = Qt::Key_1; break;
		case '2': qk.key = Qt::Key_2; break;
		case '3': qk.key = Qt::Key_3; break;
		case '4': qk.key = Qt::Key_4; break;
		case '5': qk.key = Qt::Key_5; break;
		case '6': qk.key = Qt::Key_6; break;
		case '7': qk.key = Qt::Key_7; break;
		case '8': qk.key = Qt::Key_8; break;
		case '9': qk.key = Qt::Key_9; break;
		case '0': qk.key = Qt::Key_0; break;
		case VK_OEM_MINUS: qk.key = Qt::Key_Minus; break;
		case VK_OEM_PLUS: qk.key = Qt::Key_Equal; break;
		case VK_BACK: qk.key = Qt::Key_Backspace; break;


		case VK_TAB: qk.key = Qt::Key_Tab; break;
		case 'Q': qk.key = Qt::Key_Q; break;
		case 'W': qk.key = Qt::Key_W; break;
		case 'E': qk.key = Qt::Key_E; break;
		case 'R': qk.key = Qt::Key_R; break;
		case 'T': qk.key = Qt::Key_T; break;
		case 'Y': qk.key = Qt::Key_Y; break;
		case 'U': qk.key = Qt::Key_U; break;
		case 'I': qk.key = Qt::Key_I; break;
		case 'O': qk.key = Qt::Key_O; break;
		case 'P': qk.key = Qt::Key_P; break;
		case VK_OEM_4: qk.key = Qt::Key_BracketLeft; break;
		case VK_OEM_6: qk.key = Qt::Key_BracketRight; break;
		case VK_OEM_5: qk.key = Qt::Key_Backslash; break;

		case VK_CAPITAL: qk.key = Qt::Key_CapsLock; break;
		case 'A': qk.key = Qt::Key_A; break;
		case 'S': qk.key = Qt::Key_S; break;
		case 'D': qk.key = Qt::Key_D; break;
		case 'F': qk.key = Qt::Key_F; break;
		case 'G': qk.key = Qt::Key_G; break;
		case 'H': qk.key = Qt::Key_H; break;
		case 'J': qk.key = Qt::Key_J; break;
		case 'K': qk.key = Qt::Key_K; break;
		case 'L': qk.key = Qt::Key_L; break;
		case VK_OEM_1: qk.key = Qt::Key_Semicolon; break;
		case VK_OEM_7: qk.key = Qt::Key_Apostrophe; break;
		case VK_RETURN: qk.key = Qt::Key_Return; break;

		case VK_SHIFT: qk.key = Qt::Key_Shift; break;
		case 'Z': qk.key = Qt::Key_Z; break;
		case 'X': qk.key = Qt::Key_X; break;
		case 'C': qk.key = Qt::Key_C; break;
		case 'V': qk.key = Qt::Key_V; break;
		case 'B': qk.key = Qt::Key_B; break;
		case 'N': qk.key = Qt::Key_N; break;
		case 'M': qk.key = Qt::Key_M; break;
		case VK_OEM_COMMA: qk.key = Qt::Key_Comma; break;
		case VK_OEM_PERIOD: qk.key = Qt::Key_Period; break;
		case VK_OEM_2: qk.key = Qt::Key_Slash; break;

		case VK_CONTROL: qk.key = Qt::Key_Control; break;
		case VK_MENU: qk.key = Qt::Key_Alt; break;
		case VK_SPACE: qk.key = Qt::Key_Space; break;
		case VK_APPS: qk.key = Qt::Key_Menu; break;

		case VK_SCROLL: qk.key = Qt::Key_ScrollLock; break;
		case VK_PAUSE: qk.key = Qt::Key_Pause; break;
		case VK_INSERT: qk.key = Qt::Key_Insert; break;
		case VK_HOME: qk.key = Qt::Key_Home; break;
		case VK_PRIOR: qk.key = Qt::Key_PageUp; break;
		case VK_DELETE: qk.key = Qt::Key_Delete; break;
		case VK_END: qk.key = Qt::Key_End; break;
		case VK_NEXT: qk.key = Qt::Key_PageDown; break;

		case VK_UP: qk.key = Qt::Key_Up; break;
		case VK_LEFT: qk.key = Qt::Key_Left; break;
		case VK_DOWN: qk.key = Qt::Key_Down; break;
		case VK_RIGHT: qk.key = Qt::Key_Right; break;

		case VK_NUMLOCK: qk.key = Qt::Key_NumLock; qk.mod = Qt::KeypadModifier; break;
		case VK_SEPARATOR: qk.key = Qt::Key_Enter; qk.mod = Qt::KeypadModifier; break;
		case VK_DIVIDE: qk.key = Qt::Key_Slash; qk.mod = Qt::KeypadModifier; break;
		case VK_MULTIPLY: qk.key = Qt::Key_Asterisk; qk.mod = Qt::KeypadModifier; break;
		case VK_SUBTRACT: qk.key = Qt::Key_Minus; qk.mod = Qt::KeypadModifier; break;
		case VK_ADD: qk.key = Qt::Key_Plus; qk.mod = Qt::KeypadModifier; break;
		case VK_DECIMAL: qk.key = Qt::Key_Period; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD0: qk.key = Qt::Key_0; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD1: qk.key = Qt::Key_1; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD2: qk.key = Qt::Key_2; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD3: qk.key = Qt::Key_3; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD4: qk.key = Qt::Key_4; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD5: qk.key = Qt::Key_5; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD6: qk.key = Qt::Key_6; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD7: qk.key = Qt::Key_7; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD8: qk.key = Qt::Key_8; qk.mod = Qt::KeypadModifier; break;
		case VK_NUMPAD9: qk.key = Qt::Key_9; qk.mod = Qt::KeypadModifier; break;
		}

		return qk;
	}

private slots:

	void Paint()
	{
		if (mode == 0)
		{
			if (keys[0].state)
			{
				text = "";
				if (keys[0].type == 0) text += Name(KeybdToVk(keys[0].key, keys[0].mod));
				if (keys[0].type == 1) text += Name(MouseToVk(keys[0].mouse));
				if (keys[0].type == 2)
				{
					if (keys[0].wheel == 0) text += Name(VK_WHEELUP);
					if (keys[0].wheel == 1) text += Name(VK_WHEELDOWN);
				}
				setText(text);
			}
		}
		else if (mode == 1)
		{
			if (keys[0].state)
			{
				text = "";
				if (keys[0].type == 0)
				{
					if (keys[0].mod & Qt::ControlModifier) text += "Ctrl + ";
					if (keys[0].mod & Qt::ShiftModifier) text += "Shift + ";
					if (keys[0].mod & Qt::AltModifier) text += "Alt + ";
					if (keys[0].key != Qt::Key_Control && keys[0].key != Qt::Key_Shift && keys[0].key != Qt::Key_Alt) text += Name(KeybdToVk(keys[0].key, keys[0].mod));
				}
				setText(text);
			}
		}
		else if (mode == 2)
		{
			if (keys[0].state)
			{
				text = "";
				if (keys[0].type == 0)
				{
					text += Name(KeybdToVk(keys[0].key, keys[0].mod));
				}
				else if (keys[0].type == 1)
				{
					text += Name(MouseToVk(keys[0].mouse));
				}
				else if (keys[0].type == 2)
				{
					if (keys[0].wheel == 0) text += Name(VK_WHEELUP);
					else text += Name(VK_WHEELDOWN);
				}
				if (keys[1].state)
				{
					text += " + ";
					if (keys[1].type == 0)
					{
						text += Name(KeybdToVk(keys[1].key, keys[1].mod));
					}
					else if (keys[1].type == 1)
					{
						text += Name(MouseToVk(keys[1].mouse));
					}
					else if (keys[1].type == 2)
					{
						if (keys[1].wheel == 0) text += Name(VK_WHEELUP);
						else text += Name(VK_WHEELDOWN);
					}
				}
				setText(text);
			}
		}
	}

	void keyPressEvent(QKeyEvent* et) {
		if (disable) return;
		if (input)
		{
			if (mode == 0)
			{
				keys[0].type = 0;
				keys[0].state = 1;
				keys[0].key = et->key();
				keys[0].mod = et->modifiers();
			}
			else if (mode == 1)
			{
				keys[0].type = 0;
				keys[0].state = 1;
				keys[0].key = et->key();
				keys[0].mod = et->modifiers();
			}
			else if (mode == 2)
			{
				if (keys[0].state)
				{
					if (!keys[1].state && keys[0].key != et->key())
					{
						keys[1].type = 0;
						keys[1].state = 1;
						keys[1].key = et->key();
						keys[1].mod = et->modifiers();
					}
				}
				else
				{
					keys[0].type = 0;
					keys[0].state = 1;
					keys[0].key = et->key();
					keys[0].mod = et->modifiers();
				}
			}

			Paint();
		}
	}

	void keyReleaseEvent(QKeyEvent* et) {
		if (disable) return;
		if (input)
		{
			if (keys[0].state && keys[0].key == et->key())
			{
				input = 0;
				releaseKeyboard();
				releaseMouse();
				emit changed();
			}
		}
	}

	void mousePressEvent(QMouseEvent* et) {
		if (disable) return;
		if (input)
		{
			if (mode == 0)
			{
				keys[0].type = 1;
				keys[0].state = 1;
				keys[0].mouse = et->button();
			}
			else if (mode == 2)
			{
				if (keys[0].state)
				{
					if (!keys[1].state && keys[0].mouse != et->button())
					{
						keys[1].type = 1;
						keys[1].state = 1;
						keys[1].mouse = et->button();
					}
				}
				else
				{
					keys[0].type = 1;
					keys[0].state = 1;
					keys[0].mouse = et->button();
				}
			}

			Paint();
		}
	}

	void mouseReleaseEvent(QMouseEvent* et) {
		if (disable) return;
		if (input)
		{
			if (keys[0].state && keys[0].mouse == et->button())
			{
				input = 0;
				releaseKeyboard();
				releaseMouse();
				emit changed();
			}
		}
		else {
			if (et->button() == Qt::LeftButton)
			{
				keys[0].state = 0;
				keys[0].key = 0;
				keys[0].mod = 0;
				keys[0].mouse = Qt::NoButton;
				keys[1].state = 0;
				keys[1].key = 0;
				keys[1].mod = 0;
				keys[1].mouse = Qt::NoButton;
				input = 1;
				text = "...";
				setText(text);
				grabKeyboard();
				grabMouse();
			}
		}
	}

	void wheelEvent(QWheelEvent* et)
	{
		if (disable) return;
		if (input)
		{
			if (!keys[0].state)
			{
				if (et->angleDelta().y() > 0)
				{
					keys[0].type = 2;
					keys[0].state = 1;
					keys[0].wheel = 0;
				}
				else
				{
					keys[0].type = 2;
					keys[0].state = 1;
					keys[0].wheel = 1;
				}
			}

			input = 0;
			Paint();
			releaseKeyboard();
			releaseMouse();
			emit changed();
		}
	}
};