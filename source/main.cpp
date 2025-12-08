// 通过config.props配置预处理宏控制编译条件
#pragma message("编译配置：")

#ifdef Q_COPYRIGHT_HIDE
#pragma message("版权信息=0")
#else
#pragma message("版权信息=1")
#endif

#ifdef Q_URL_HIDE
#pragma message("显示网址=0")
#else
#pragma message("显示网址=1")
#endif

#ifdef Q_KEYEDIT_PAD_ENABLED
#pragma message("手柄识别=1")
#else
#pragma message("手柄识别=0")
#endif

#ifdef Q_INTEGRITY_VERIFY
#pragma message("修改验证=1")
#else
#pragma message("修改验证=0")
#endif

#ifdef Q_UPDATE
#pragma message("更新检查=1")
#else
#pragma message("更新检查=0")
#endif

#ifdef Q_RAWINPUT
#pragma message("驱动输入=1")
#else
#pragma message("驱动输入=0")
#endif

#ifdef Q_ENCRYPT
#pragma message("文件加密=1")
#else
#pragma message("文件加密=0")
#endif

#ifdef Q_VERIFY
#pragma message("网络验证=1")
#else
#pragma message("网络验证=0")
#endif

#include <ui/MainUi.h>
#include <src/inc_header.h>

#ifndef DEBUG

#ifdef Q_INTEGRITY_VERIFY
#define INTEGRITY_VERIFY_EXEC
#include <src/integrity_verify.h>
#endif

#ifdef Q_VERIFY
#define VERIFY_EXEC
#include <verify/verify.h>
#endif

#endif

void Init();
int main(int argc, char* argv[])
{
    puts("QUICKINPUT_(C)CHIYUKIGANA");
	std::locale::global(std::locale(".UTF8")); // set utf8 for all std streams
	Process::RunPath(); // reset work path to exe path

	std::wstring mutex = Path::toSlash(Process::runPath()); // mutex name, the current directory is only running one
	if (Process::isRunning(mutex.c_str()))
	{
		MsgBox::Warning(L"当前文件夹的程序已经运行，点击任务栏小图标显示窗口\n\n若运行更多程序请复制此文件夹", L"提示");
		return -1;
	}
	else CreateMutexW(0, 0, mutex.c_str());

#ifdef INTEGRITY_VERIFY_EXEC
	integrity_verify();
#endif
#ifdef VERIFY_EXEC
	verify();
#endif

    Init();

    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
	QApplication application(argc, argv);
	QApplication::setWindowIcon(QIcon(":/icon.png"));
	Qi::application = &application;
	Qi::popText = new QPopText;
	Qi::popText->setPosition(Qi::ui.pop.x, Qi::ui.pop.y);
	Qi::popText->setSize(Qi::ui.pop.size);
	Qi::windowSelection = new QWindowSelection;

	MainUi mainWindow;
	Qi::version = mainWindow.Version();
	application.exec();
	return 0;
}