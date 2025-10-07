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
	};
	namespace Text
	{
		inline QString acDown = QString("按下") + Symbol::Down;
		inline QString acUp = QString("松开") + Symbol::Up;
		inline QString acClick = QString("点击") + Symbol::Turn;
		inline QString acPos = QString("位置") + Symbol::Left;
		inline QString acMove = QString("移动") + Symbol::Move;
		inline QString acWait = QString("等待") + Symbol::Time;
		inline QString acCopyText = QString("复制") + Symbol::Text;
		inline QString acLoop = QString("循环") + Symbol::Loop;
		inline QString acColor = QString("找色") + Symbol::Color;
		inline QString acEnd = QString("结束") + Symbol::Stop;
		inline QString acEndLoop = QString("结束循环") + Symbol::Stop;
		inline QString acKeyState = QString("按键状态") + Symbol::Stop;
		inline QString acResetPos = QString("恢复位置") + Symbol::Turn;
		inline QString acImage = QString("找图") + Symbol::Image;
		inline QString acPopText = QString("弹出") + Symbol::Text;
		inline QString acSavePos = QString("记录位置") + Symbol::Turn;
		inline QString acTimer = QString("定时") + Symbol::Loop;
		inline QString acJump = QString("跳转") + Symbol::Jump;
		inline QString acJumpPoint = QString("锚点") + Symbol::Point;
		inline QString acDialog = QString("对话框") + Symbol::Text;
		inline QString acBlock = QString("块") + Symbol::Block;
		inline QString acBlockExec = QString("执行") + Symbol::BlockExec;
		inline QString acQuickInput = QString("输入字符") + Symbol::Text;
		inline QString acKeyBlock = QString("屏蔽按键") + Symbol::Stop;
		inline QString acClock = QString("时钟") + Symbol::Time;
		inline QString acOcr = QString("文字识别") + Symbol::Text;
		inline QString acVarOperator = QString("变量运算") + Symbol::Equal;
		inline QString acVarCondition = QString("变量判断") + Symbol::Var;
		inline QString acMouseTrack = QString("鼠标轨迹") + Symbol::Track;
		inline QString acOpen = QString("打开") + Symbol::Link;
		inline QString acTextPad = QString("文本") + Symbol::Text;
		inline QString acEditDialog = QString("编辑框") + Symbol::Text;
		inline QString acVolume = QString("音量检测") + Symbol::Speaker;
		inline QString acSoundPlay = QString("播放音频") + Symbol::Speaker;
		inline QString acMsgView = QString("消息窗口") + Symbol::Text;
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