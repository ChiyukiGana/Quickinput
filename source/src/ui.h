#pragma once
#include "inc_external.h"

namespace QiUi
{
	constexpr int event_restyle = QEvent::User;
	constexpr const char* color_dialog_style = "*{color:black}QPushButton{width:60px;border:2px solid blue}";

	struct Symbol
	{
		static inline QString Entry = "🔜";
		static inline QString Pause = "⏸️";
		static inline QString Exit = "🔙";
		static inline QString Any = "🟡";
		static inline QString On = "✅";
		static inline QString Off = "⛔";
		static inline QString Ok = "⭕";
		static inline QString Yes = "✔️";
		static inline QString Not = "❌";
		static inline QString Stop = "🛑";
		static inline QString Show = "🔼";
		static inline QString Hide = "🔽";
		static inline QString Option = "⚙";
		static inline QString Link = "🔗";
		static inline QString Edit = "🔧";
		static inline QString Up = "⬆️";
		static inline QString Down = "⬇️";
		static inline QString Turn = "🔃";
		static inline QString Left = "🔙";
		static inline QString Top = "🔝";
		static inline QString Right = "🔜";
		static inline QString Move = "🔛";
		static inline QString Time = "⏳";
		static inline QString Text = "🅰️";
		static inline QString Loop = "♾️";
		static inline QString Color = "🌈";
		static inline QString Image = "🖼";
		static inline QString Point = "🪂";
		static inline QString Jump = "🛩";
		static inline QString Block = "🪂";
		static inline QString BlockExec = "🛩";
		static inline QString Equal = "🟰";
		static inline QString Var = "💠";
		static inline QString Track = "➰";
		static inline QString Speaker = "🔈️";
	};
	struct Text
	{
		static inline QString acDown = QString("按下") + Symbol::Down;
		static inline QString acUp = QString("松开") + Symbol::Up;
		static inline QString acClick = QString("点击") + Symbol::Turn;
		static inline QString acPos = QString("位置") + Symbol::Left;
		static inline QString acMove = QString("移动") + Symbol::Move;
		static inline QString acWait = QString("等待") + Symbol::Time;
		static inline QString acCopyText = QString("复制") + Symbol::Text;
		static inline QString acLoop = QString("循环") + Symbol::Loop;
		static inline QString acColor = QString("找色") + Symbol::Color;
		static inline QString acEnd = QString("结束") + Symbol::Stop;
		static inline QString acEndLoop = QString("结束循环") + Symbol::Stop;
		static inline QString acKeyState = QString("按键状态") + Symbol::Stop;
		static inline QString acResetPos = QString("恢复位置") + Symbol::Turn;
		static inline QString acImage = QString("找图") + Symbol::Image;
		static inline QString acPopText = QString("弹出") + Symbol::Text;
		static inline QString acSavePos = QString("记录位置") + Symbol::Turn;
		static inline QString acTimer = QString("定时") + Symbol::Loop;
		static inline QString acJump = QString("跳转") + Symbol::Jump;
		static inline QString acJumpPoint = QString("锚点") + Symbol::Point;
		static inline QString acDialog = QString("对话框") + Symbol::Text;
		static inline QString acBlock = QString("块") + Symbol::Block;
		static inline QString acBlockExec = QString("执行") + Symbol::BlockExec;
		static inline QString acQuickInput = QString("输入字符") + Symbol::Text;
		static inline QString acKeyBlock = QString("屏蔽按键") + Symbol::Stop;
		static inline QString acClock = QString("时钟") + Symbol::Time;
		static inline QString acOcr = QString("文字识别") + Symbol::Text;
		static inline QString acVarOperator = QString("变量运算") + Symbol::Equal;
		static inline QString acVarCondition = QString("变量判断") + Symbol::Var;
		static inline QString acMouseTrack = QString("鼠标轨迹") + Symbol::Track;
		static inline QString acOpen = QString("打开") + Symbol::Link;
		static inline QString acTextPad = QString("文本") + Symbol::Text;
		static inline QString acEditDialog = QString("编辑框") + Symbol::Text;
		static inline QString acVolume = QString("音量检测") + Symbol::Speaker;
		static inline QString acSoundPlay = QString("播放音频") + Symbol::Speaker;
		// state
		static inline QString trOn = QString("启用") + Symbol::On;
		static inline QString trOff = QString("禁用") + Symbol::Off;
		// record
		static inline QString rcStart = QString("开始") + Symbol::Ok;
		static inline QString rcStop = QString("停止") + Symbol::Ok;
		static inline QString rcClose = QString("取消") + Symbol::Not;
		// menu
		static inline QString muOn = QString("启用") + Symbol::On;
		static inline QString muOff = QString("禁用") + Symbol::Off;
		static inline QString muShow = QString("显示") + Symbol::Show;
		static inline QString muHide = QString("隐藏") + Symbol::Hide;
		static inline QString muExit = QString("退出") + Symbol::Stop;
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
		int size = 100;
		int time = 1000;
	};
	struct QuickInputUi
	{
		QList<Theme> themes;
		QList<QString> sounds;
		PopText pop;
	};
}