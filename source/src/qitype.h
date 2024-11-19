#pragma execution_character_set("utf-8")
#pragma once
#include "minc.h"

////////////////// Actions
struct QiType
{
	enum
	{
		none,
		end,
		delay,
		key,
		mouse,
		text,
		color,
		loop,
		loopEnd,
		keyState,
		recoverPos,
		image,
		popText,
		rememberPos,
		timer,
		jump,
		jumpPoint,
		dialog,
		block,
		blockExec
	};
};

class Action;
using Actions = List<Action>;

class QiBase
{
public:
	uint32 type;
	QString mark;
	Actions next;
	Actions next2;
	QiBase(uint32 qiType = QiType::none) noexcept : type(qiType), mark(QString()), next(Actions()), next2(Actions()) {}
	QiBase(const QiBase& r) noexcept { operator=(r); }
	QiBase(QiBase&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBase& r) noexcept { type = r.type; mark = r.mark; next = r.next; next2 = r.next2; }
	void operator=(QiBase&& r) noexcept { type = r.type; mark = std::move(r.mark); next = std::move(r.next); next2 = std::move(r.next2); }
};
class QiEnd : public QiBase
{
public:
	QiEnd() noexcept : QiBase(QiType::end) {}
	QiEnd(const QiEnd& r) noexcept { operator=(r); }
	QiEnd(QiEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiEnd& r) noexcept { QiBase::operator=(r); }
	void operator=(QiEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiDelay : public QiBase
{
public:
	uint32 min; uint32 max;
	QiDelay() noexcept : QiBase(QiType::delay) {}
	QiDelay(const QiDelay& r) noexcept { operator=(r); }
	QiDelay(QiDelay&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiDelay& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiDelay&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiKey : public QiBase
{
public:
	enum { up, down, click };
	uint32 vk = 0; uint32 state = down;
	QiKey() noexcept : QiBase(QiType::key) {}
	QiKey(const QiKey& r) noexcept { operator=(r); }
	QiKey(QiKey&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKey& r) noexcept { QiBase::operator=(r); vk = r.vk; state = r.state; }
	void operator=(QiKey&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; state = r.state; }
};
class QiMouse : public QiBase
{
public:
	int32 x = 0; int32 y = 0; uint32 ex = 0; uint32 speed = 0; bool move = false; bool track = false;
	QiMouse() noexcept : QiBase(QiType::mouse) {}
	QiMouse(const QiMouse& r) noexcept { operator=(r); }
	QiMouse(QiMouse&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiMouse& r) noexcept { QiBase::operator=(r); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
	void operator=(QiMouse&& r) noexcept { QiBase::operator=(std::move(r)); x = r.x; y = r.y; ex = r.ex; speed = r.speed; move = r.move; track = r.track; }
};
class QiText : public QiBase
{
public:
	QString text;
	QiText() noexcept : QiBase(QiType::text) {}
	QiText(const QiText& r) noexcept { operator=(r); }
	QiText(QiText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiText& r) noexcept { QiBase::operator=(r); text = r.text; }
	void operator=(QiText&& r) noexcept { QiBase::operator=(std::move(r)); text = std::move(r.text); }
};
class QiColor : public QiBase
{
public:
	Rgba rgbe = 0; RECT rect = {}; bool move = false;
	QiColor() noexcept : QiBase(QiType::color) {}
	QiColor(const QiColor& r) noexcept { operator=(r); }
	QiColor(QiColor&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiColor& r) noexcept { QiBase::operator=(r); rgbe = r.rgbe; rect = r.rect; move = r.move; }
	void operator=(QiColor&& r) noexcept { QiBase::operator=(std::move(r)); rgbe = std::move(r.rgbe); rect = r.rect; move = r.move; }
};
class QiLoop : public QiBase
{
public:
	uint32 min = 0; uint32 max = 0;
	QiLoop() noexcept : QiBase(QiType::loop) {}
	QiLoop(const QiLoop& r) noexcept { operator=(r); }
	QiLoop(QiLoop&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoop& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiLoop&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiLoopEnd : public QiBase
{
public:
	QiLoopEnd() noexcept : QiBase(QiType::loopEnd) {}
	QiLoopEnd(const QiLoopEnd& r) noexcept { operator=(r); }
	QiLoopEnd(QiLoopEnd&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiLoopEnd& r) noexcept { QiBase::operator=(r); }
	void operator=(QiLoopEnd&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiKeyState : public QiBase
{
public:
	uint32 vk = 0;
	QiKeyState() noexcept : QiBase(QiType::keyState) {}
	QiKeyState(const QiKeyState& r) noexcept { operator=(r); }
	QiKeyState(QiKeyState&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiKeyState& r) noexcept { QiBase::operator=(r); vk = r.vk; }
	void operator=(QiKeyState&& r) noexcept { QiBase::operator=(std::move(r)); vk = r.vk; }
};
class QiRecoverPos : public QiBase
{
public:
	QiRecoverPos() noexcept : QiBase(QiType::recoverPos) {}
	QiRecoverPos(const QiRecoverPos& r) noexcept { operator=(r); }
	QiRecoverPos(QiRecoverPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRecoverPos& r) noexcept { QiBase::operator=(r); }
	void operator=(QiRecoverPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiImage : public QiBase
{
public:
	RgbMap map; uint32 sim; RECT rect = {}; bool move = false;
	QiImage() noexcept : QiBase(QiType::image) {}
	QiImage(const QiImage& r) noexcept { operator=(r); }
	QiImage(QiImage&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiImage& r) noexcept { QiBase::operator=(r); map = r.map; sim = r.sim; rect = r.rect; move = r.move; }
	void operator=(QiImage&& r) noexcept { QiBase::operator=(std::move(r)); map = std::move(r.map); sim = r.sim; rect = r.rect; move = r.move; }
};
class QiPopText : public QiBase
{
public:
	QString text; uint32 time = 0; bool sync = false;
	QiPopText() noexcept : QiBase(QiType::popText) {}
	QiPopText(const QiPopText& r) noexcept { operator=(r); }
	QiPopText(QiPopText&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiPopText& r) noexcept { QiBase::operator=(r); time = r.time; text = r.text; sync = r.sync; }
	void operator=(QiPopText&& r) noexcept { QiBase::operator=(std::move(r)); time = r.time; text = std::move(r.text); sync = r.sync; }
};
class QiRememberPos : public QiBase
{
public:
	QiRememberPos() noexcept : QiBase(QiType::rememberPos) {}
	QiRememberPos(const QiRememberPos& r) noexcept { operator=(r); }
	QiRememberPos(QiRememberPos&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiRememberPos& r) noexcept { QiBase::operator=(r); }
	void operator=(QiRememberPos&& r) noexcept { QiBase::operator=(std::move(r)); }
};
class QiTimer : public QiBase
{
public:
	uint32 min = 0; uint32 max = 0;
	QiTimer() noexcept : QiBase(QiType::loop) {}
	QiTimer(const QiTimer& r) noexcept { operator=(r); }
	QiTimer(QiTimer&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiTimer& r) noexcept { QiBase::operator=(r); min = r.min; max = r.max; }
	void operator=(QiTimer&& r) noexcept { QiBase::operator=(std::move(r)); min = r.min; max = r.max; }
};
class QiJump : public QiBase
{
public:
	int32 id = 0;
	QiJump() noexcept : QiBase(QiType::jump) {}
	QiJump(const QiJump& r) noexcept { operator=(r); }
	QiJump(QiJump&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiJump& r) noexcept { QiBase::operator=(r); id = r.id; }
	void operator=(QiJump&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiJumpPoint : public QiBase
{
public:
	int32 id = 0;
	QiJumpPoint() noexcept : QiBase(QiType::jumpPoint) {}
	QiJumpPoint(const QiJumpPoint& r) noexcept { operator=(r); }
	QiJumpPoint(QiJumpPoint&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiJumpPoint& r) noexcept { QiBase::operator=(r); id = r.id; }
	void operator=(QiJumpPoint&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiDialog : public QiBase
{
public:
	enum Style
	{
		none,
		warning,
		error
	};
	uint32 style;
	QString title;
	QString text;
	QiDialog() noexcept : QiBase(QiType::dialog) {}
	QiDialog(const QiDialog& r) noexcept { operator=(r); }
	QiDialog(QiDialog&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiDialog& r) noexcept { QiBase::operator=(r); style = r.style; title = r.title; text = r.text; }
	void operator=(QiDialog&& r) noexcept { QiBase::operator=(std::move(r)); style = r.style; title = std::move(r.title); text = std::move(r.text); }
};
class QiBlock : public QiBase
{
public:
	int32 id;
	QiBlock() noexcept : QiBase(QiType::block) {}
	QiBlock(const QiBlock& r) noexcept { operator=(r); }
	QiBlock(QiBlock&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBlock& r) noexcept { QiBase::operator=(r); id = r.id; }
	void operator=(QiBlock&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};
class QiBlockExec : public QiBase
{
public:
	int32 id;
	QiBlockExec() noexcept : QiBase(QiType::blockExec) {}
	QiBlockExec(const QiBlockExec& r) noexcept { operator=(r); }
	QiBlockExec(QiBlockExec&& r) noexcept { operator=(std::move(r)); }
	void operator=(const QiBlockExec& r) noexcept { QiBase::operator=(r); id = r.id; }
	void operator=(QiBlockExec&& r) noexcept { QiBase::operator=(std::move(r)); id = r.id; }
};

using ActionVariant = std::variant
<
	QiBase,
	QiEnd,
	QiDelay,
	QiKey,
	QiMouse,
	QiText,
	QiColor,
	QiLoop,
	QiLoopEnd,
	QiKeyState,
	QiRecoverPos,
	QiImage,
	QiPopText,
	QiRememberPos,
	QiTimer,
	QiJump,
	QiJumpPoint,
	QiDialog,
	QiBlock,
	QiBlockExec
>;

class Action : public ActionVariant
{
public:
	using ActionVariant::ActionVariant;

	Action() : ActionVariant(QiBase()) {}

	QiBase& base()
	{
		QiBase* base;
		std::visit([&base](auto&& var)
			{
				base = &var;
			}, *this
		);
		return *base;
	}
	const QiBase& base() const
	{
		const QiBase* base;
		std::visit([&base](auto&& const var)
			{
				base = &var;
			}, *this
		);
		return *base;
	}
};

// action visit
/*
const Action& var = actions->at(i);
switch (var.index())
{
case QiType::end:
{
	QiEnd& end = std::get<QiEnd>(var);
} break;
case QiType::delay:
{
	QiDelay& delay = std::get<QiDelay>(var);
} break;
case QiType::key:
{
	QiKey& key = std::get<QiKey>(var);
} break;
case QiType::mouse:
{
	QiMouse& mouse = std::get<QiMouse>(var);
} break;
case QiType::text:
{
	QiText& text = std::get<QiText>(var);
} break;
case QiType::color:
{
	QiColor& color = std::get<QiColor>(var);
} break;
case QiType::loop:
{
	QiLoop& loop = std::get<QiLoop>(var);
} break;
case QiType::loopEnd:
{
	QiLoopEnd& loopEnd = std::get<QiLoopEnd>(var);
} break;
case QiType::keyState:
{
	QiKeyState& keyState = std::get<QiKeyState>(var);
} break;
case QiType::recoverPos:
{
	QiRecoverPos& recoverPos = std::get<QiRecoverPos>(var);
} break;
case QiType::image:
{
	QiImage& image = std::get<QiImage>(var);
} break;
case QiType::popText:
{
	QiPopText& popText = std::get<QiPopText>(var);
} break;
case QiType::rememberPos:
{
	QiRememberPos& rememberPos = std::get<QiRememberPos>(var);
} break;
case QiType::timer:
{
	QiTimer& timer = std::get<QiTimer>(var);
} break;
case QiType::jump:
{
	QiJump& jump = std::get<QiJump>(var);
} break;
case QiType::jumpPoint:
{
	QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
} break;
case QiType::dialog:
{
	QiDialog& dialog = std::get<QiDialog>(var);
} break;
case QiType::block:
{
	QiBlock& block = std::get<QiBlock>(var);
} break;
case QiType::blockExec:
{
	QiBlockExec& blockExec = std::get<QiBlockExec>(var);
} break;
*/

// action visit, const
/*
const Action& var = actions->at(i);
switch (var.index())
{
case QiType::end:
{
	const QiEnd& end = std::get<QiEnd>(var);
} break;
case QiType::delay:
{
	const QiDelay& delay = std::get<QiDelay>(var);
} break;
case QiType::key:
{
	const QiKey& key = std::get<QiKey>(var);
} break;
case QiType::mouse:
{
	const QiMouse& mouse = std::get<QiMouse>(var);
} break;
case QiType::text:
{
	const QiText& text = std::get<QiText>(var);
} break;
case QiType::color:
{
	const QiColor& color = std::get<QiColor>(var);
} break;
case QiType::loop:
{
	const QiLoop& loop = std::get<QiLoop>(var);
} break;
case QiType::loopEnd:
{
	const QiLoopEnd& loopEnd = std::get<QiLoopEnd>(var);
} break;
case QiType::keyState:
{
	const QiKeyState& keyState = std::get<QiKeyState>(var);
} break;
case QiType::recoverPos:
{
	const QiRecoverPos& recoverPos = std::get<QiRecoverPos>(var);
} break;
case QiType::image:
{
	const QiImage& image = std::get<QiImage>(var);
} break;
case QiType::popText:
{
	const QiPopText& popText = std::get<QiPopText>(var);
} break;
case QiType::rememberPos:
{
	const QiRememberPos& rememberPos = std::get<QiRememberPos>(var);
} break;
case QiType::timer:
{
	const QiTimer& timer = std::get<QiTimer>(var);
} break;
case QiType::jump:
{
	const QiJump& jump = std::get<QiJump>(var);
} break;
case QiType::jumpPoint:
{
	const QiJumpPoint& jumpPoint = std::get<QiJumpPoint>(var);
} break;
case QiType::dialog:
{
	const QiDialog& dialog = std::get<QiDialog>(var);
} break;
case QiType::block:
{
	const QiBlock& block = std::get<QiBlock>(var);
} break;
case QiType::blockExec:
{
	const QiBlockExec& blockExec = std::get<QiBlockExec>(var);
} break;
}
*/

////////////////// ~Actions