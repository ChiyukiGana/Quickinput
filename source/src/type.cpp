#include "type.h"
HANDLE WndLock::thread = nullptr;
DEFINE_INPUTHOOK();
namespace Qi
{
	// for setStyle
	QApplication* application = nullptr;
	QiUi::QuickInputUi ui;
	// state
	bool state = false;
	bool run = false;
	// record
	bool recordState = false;
	bool recording = false;
	clock_t recordClock = 0;
	HWND recordWindow = 0;
	// macro
	Actions record;
	Actions clipboard;
	Macros macros;
	// data
	FuncData fun;
	SettingsData set;
	Widget widget;
	QPopText* popText = nullptr;
	QWindowSelection* windowSelection = nullptr;
	// input
	bool keyState[keySize];
	QList<byte> blockKeys;
	XBoxPad xboxpad;
	// dir
	const QString dir = QDir::fromNativeSeparators(QString::fromWCharArray(Process::runPath().c_str()));
	const QString folder = dir.mid(dir.lastIndexOf('/') + 1);
	const QString macroDir = dir + "/macro/";
	const QString macroType = ".json";
	const QString configFile = "QuickInput.json";
	// other
	SIZE screen = {};
	MSG msg;
}