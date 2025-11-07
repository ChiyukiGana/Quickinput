#pragma once
#include <qlist.h>
#include <qcolor.h>
#include <qstring.h>
namespace QiUi
{
	constexpr const char* color_dialog_style = "*{color:black}QPushButton{width:60px;border:2px solid blue}";
	namespace Symbol
	{
		inline QString Entry = "🔜";
		inline QString Pause = "⏸️";
		inline QString Exit = "🔙";
		inline QString Any = "🟡";
		inline QString On = "✅";
		inline QString Off = "⛔";
		inline QString Ok = "⭕";
		inline QString Yes = "✔️";
		inline QString Not = "❌";
		inline QString Stop = "🛑";
		inline QString Show = "🔼";
		inline QString Hide = "🔽";
		inline QString Option = "⚙";
		inline QString Link = "🔗";
		inline QString Edit = "🔧";
		inline QString Up = "⬆️";
		inline QString Down = "⬇️";
		inline QString Turn = "🔃";
		inline QString Left = "🔙";
		inline QString Top = "🔝";
		inline QString Right = "🔜";
		inline QString Move = "🔛";
		inline QString Time = "⏳";
		inline QString Text = "🅰️";
		inline QString Loop = "♾️";
		inline QString Color = "🌈";
		inline QString Image = "🖼";
		inline QString Point = "🪂";
		inline QString Jump = "🛩";
		inline QString Block = "🪂";
		inline QString BlockExec = "🛩";
		inline QString Equal = "🟰";
		inline QString Var = "💠";
		inline QString Track = "➰";
		inline QString Speaker = "🔈️";
		inline QString Range = "🔁";
	};
	namespace Text
	{
		// state
		inline QString trOn = QString("启用") + Symbol::On;
		inline QString trOff = QString("禁用") + Symbol::Off;
		// record
		inline QString rcStart = QString("开始") + Symbol::Ok;
		inline QString rcStop = QString("停止") + Symbol::Ok;
		inline QString rcClose = QString("取消") + Symbol::Not;
		// menu
		inline QString muOn = QString("启用") + Symbol::On;
		inline QString muOff = QString("禁用") + Symbol::Off;
		inline QString muShow = QString("显示") + Symbol::Show;
		inline QString muHide = QString("隐藏") + Symbol::Hide;
		inline QString muExit = QString("退出") + Symbol::Stop;
	};
	struct Theme
	{
		QString name;
		QString style;
	};
	struct PopTextInfo
	{
		QColor c;
		QString t;
		QString s;
	};
	struct PopTextData
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
		int x = 5000;
		int y = 0;
		int size = 100;
		int time = 1000;
	};
	struct QuickInputUi
	{
		QList<Theme> themes;
		QList<QString> sounds;
		PopTextData pop;
	};
}