#pragma message("======== 设置编译条件在config.props ========")

#include <ui/MainUi.h>
#include <src/inc_header.h>

#if !defined(DEBUG) && defined(Q_VERIFY)
#include <verify/verify.h>
#endif

int main(int argc, char* argv[])
{
	puts("QUICKINPUT_(C)CHIYUKIGANA");
	std::locale::global(std::locale(".UTF8")); // set utf8 for all std streams

	// recode ascii to unicode
	auto u_argv = new char*[argc];
	for (size_t i{};i < argc; i++)
	{
		auto s = String::recode(argv[i], String::systemCode(), CP_UTF8);
		u_argv[i] = new char[s.size() + 1];
		strcpy_s(u_argv[i], s.size() + 1, s.c_str());
	}

	Process::RunPath(); // reset work path to exe path

	std::wstring mutex = Path::toSlash(Process::runPath()); // mutex name, the current directory is only running one
	if (Process::isRunning(mutex.c_str()))
	{
		MsgBox::Warning(L"当前文件夹的程序已经运行，点击任务栏小图标显示窗口\n\n若运行更多程序请复制此文件夹", L"提示");
		return -1;
	}
	else CreateMutexW(0, 0, mutex.c_str());

#if !defined(DEBUG) && defined(Q_VERIFY)
	verify();
#endif

	Qi::init();

	qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
	QApplication application(argc, u_argv);
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