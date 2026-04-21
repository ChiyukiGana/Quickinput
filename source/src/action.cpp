#include "action.h"
#include "typepack/typepack.cpp"

bool find_index(const Actions& actions, QiIndex& index, QiType type, int id)
{
	for (size_t i = 0; i < actions.size(); i++)
	{
		const Action& a = actions[i];
		if ((type == QiType::none || type == a.type()) && a.base().id == id)
		{
			index.index = i;
			index.next1 = false;
			index.next2 = false;
			index.next.reset();
			return true;
		}
		if (a.base().next.not_empty())
		{
			auto temp_index = std::make_unique<QiIndex>();
			if (find_index(a.base().next, *temp_index, type, id))
			{
				index.index = i;
				index.next1 = true;
				index.next2 = false;
				index.next = std::move(temp_index);
				return true;
			}
		}
		if (a.base().next2.not_empty())
		{
			auto temp_index = std::make_unique<QiIndex>();
			if (find_index(a.base().next2, *temp_index, type, id))
			{
				index.index = i;
				index.next1 = false;
				index.next2 = true;
				index.next = std::move(temp_index);
				return true;
			}
		}
	}
	index.index = QiIndex::npos;
	index.next1 = false;
	index.next2 = false;
	index.next.reset();
	return false;
}
bool find_index(const Actions& actions, QiIndex& index, QiTypeProperty property)
{
	for (size_t i = 0; i < actions.size(); i++)
	{
		const Action& a = actions[i];
		if (
			(property == QiTypeProperty::disable && a.base().disable) ||
			(property == QiTypeProperty::debug_entry && a.base().debug_entry) ||
			(property == QiTypeProperty::debug_break && a.base().debug_break) ||
			(property == QiTypeProperty::debug_exit && a.base().debug_exit)
			)
		{
			index.index = i;
			index.next1 = false;
			index.next2 = false;
			index.next.reset();
			return true;
		}
		if (a.base().next.not_empty())
		{
			auto temp_index = std::make_unique<QiIndex>();
			if (find_index(a.base().next, *temp_index, property))
			{
				index.index = i;
				index.next1 = true;
				index.next2 = false;
				index.next = std::move(temp_index);
				return true;
			}
		}
		if (a.base().next2.not_empty())
		{
			auto temp_index = std::make_unique<QiIndex>();
			if (find_index(a.base().next2, *temp_index, property))
			{
				index.index = i;
				index.next1 = false;
				index.next2 = true;
				index.next = std::move(temp_index);
				return true;
			}
		}
	}
	index.index = QiIndex::npos;
	index.next1 = false;
	index.next2 = false;
	index.next.reset();
	return false;
}

bool Action::fullEquals(const Action& other) const
{
	if (type() != other.type()) return false;
	switch (other.type())
	{
	case QiType::none: return true;
	case QiType::end: return to<QiEnd>().fullEquals(other.to<QiEnd>());
	case QiType::delay: return to<QiDelay>().fullEquals(other.to<QiDelay>());
	case QiType::key: return to<QiKey>().fullEquals(other.to<QiKey>());
	case QiType::mouse: return to<QiMouse>().fullEquals(other.to<QiMouse>());
	case QiType::copyText: return to<QiCopyText>().fullEquals(other.to<QiCopyText>());
	case QiType::color: return to<QiColor>().fullEquals(other.to<QiColor>());
	case QiType::loop: return to<QiLoop>().fullEquals(other.to<QiLoop>());
	case QiType::loopEnd: return to<QiLoopEnd>().fullEquals(other.to<QiLoopEnd>());
	case QiType::keyState: return to<QiKeyState>().fullEquals(other.to<QiKeyState>());
	case QiType::resetPos: return to<QiResetPos>().fullEquals(other.to<QiResetPos>());
	case QiType::image: return to<QiImage>().fullEquals(other.to<QiImage>());
	case QiType::popText: return to<QiPopText>().fullEquals(other.to<QiPopText>());
	case QiType::savePos: return to<QiSavePos>().fullEquals(other.to<QiSavePos>());
	case QiType::timer: return to<QiTimer>().fullEquals(other.to<QiTimer>());
	case QiType::jump: return to<QiJump>().fullEquals(other.to<QiJump>());
	case QiType::jumpPoint: return to<QiJumpPoint>().fullEquals(other.to<QiJumpPoint>());
	case QiType::dialog: return to<QiDialog>().fullEquals(other.to<QiDialog>());
	case QiType::block: return to<QiBlock>().fullEquals(other.to<QiBlock>());
	case QiType::blockExec: return to<QiBlockExec>().fullEquals(other.to<QiBlockExec>());
	case QiType::quickInput: return to<QiQuickInput>().fullEquals(other.to<QiQuickInput>());
	case QiType::keyBlock: return to<QiKeyBlock>().fullEquals(other.to<QiKeyBlock>());
	case QiType::clock: return to<QiClock>().fullEquals(other.to<QiClock>());
	case QiType::ocr: return to<QiOcr>().fullEquals(other.to<QiOcr>());
	case QiType::varOperator: return to<QiVarOperator>().fullEquals(other.to<QiVarOperator>());
	case QiType::varCondition: return to<QiVarCondition>().fullEquals(other.to<QiVarCondition>());
	case QiType::mouseTrack: return to<QiMouseTrack>().fullEquals(other.to<QiMouseTrack>());
	case QiType::open: return to<QiOpen>().fullEquals(other.to<QiOpen>());
	case QiType::textPad: return to<QiTextPad>().fullEquals(other.to<QiTextPad>());
	case QiType::editDialog: return to<QiEditDialog>().fullEquals(other.to<QiEditDialog>());
	case QiType::volume: return to<QiVolume>().fullEquals(other.to<QiVolume>());
	case QiType::soundPlay: return to<QiSoundPlay>().fullEquals(other.to<QiSoundPlay>());
	case QiType::msgView: return to<QiMsgView>().fullEquals(other.to<QiMsgView>());
	case QiType::range: return to<QiRangeSet>().fullEquals(other.to<QiRangeSet>());
	}
	return false;
}
bool Action::paramEquals(const Action& other) const
{
	if (type() != other.type()) return false;
	switch (other.type())
	{
	case QiType::none: return true;
	case QiType::end: return to<QiEnd>().paramEquals(other.to<QiEnd>());
	case QiType::delay: return to<QiDelay>().paramEquals(other.to<QiDelay>());
	case QiType::key: return to<QiKey>().paramEquals(other.to<QiKey>());
	case QiType::mouse: return to<QiMouse>().paramEquals(other.to<QiMouse>());
	case QiType::copyText: return to<QiCopyText>().paramEquals(other.to<QiCopyText>());
	case QiType::color: return to<QiColor>().paramEquals(other.to<QiColor>());
	case QiType::loop: return to<QiLoop>().paramEquals(other.to<QiLoop>());
	case QiType::loopEnd: return to<QiLoopEnd>().paramEquals(other.to<QiLoopEnd>());
	case QiType::keyState: return to<QiKeyState>().paramEquals(other.to<QiKeyState>());
	case QiType::resetPos: return to<QiResetPos>().paramEquals(other.to<QiResetPos>());
	case QiType::image: return to<QiImage>().paramEquals(other.to<QiImage>());
	case QiType::popText: return to<QiPopText>().paramEquals(other.to<QiPopText>());
	case QiType::savePos: return to<QiSavePos>().paramEquals(other.to<QiSavePos>());
	case QiType::timer: return to<QiTimer>().paramEquals(other.to<QiTimer>());
	case QiType::jump: return to<QiJump>().paramEquals(other.to<QiJump>());
	case QiType::jumpPoint: return to<QiJumpPoint>().paramEquals(other.to<QiJumpPoint>());
	case QiType::dialog: return to<QiDialog>().paramEquals(other.to<QiDialog>());
	case QiType::block: return to<QiBlock>().paramEquals(other.to<QiBlock>());
	case QiType::blockExec: return to<QiBlockExec>().paramEquals(other.to<QiBlockExec>());
	case QiType::quickInput: return to<QiQuickInput>().paramEquals(other.to<QiQuickInput>());
	case QiType::keyBlock: return to<QiKeyBlock>().paramEquals(other.to<QiKeyBlock>());
	case QiType::clock: return to<QiClock>().paramEquals(other.to<QiClock>());
	case QiType::ocr: return to<QiOcr>().paramEquals(other.to<QiOcr>());
	case QiType::varOperator: return to<QiVarOperator>().paramEquals(other.to<QiVarOperator>());
	case QiType::varCondition: return to<QiVarCondition>().paramEquals(other.to<QiVarCondition>());
	case QiType::mouseTrack: return to<QiMouseTrack>().paramEquals(other.to<QiMouseTrack>());
	case QiType::open: return to<QiOpen>().paramEquals(other.to<QiOpen>());
	case QiType::textPad: return to<QiTextPad>().paramEquals(other.to<QiTextPad>());
	case QiType::editDialog: return to<QiEditDialog>().paramEquals(other.to<QiEditDialog>());
	case QiType::volume: return to<QiVolume>().paramEquals(other.to<QiVolume>());
	case QiType::soundPlay: return to<QiSoundPlay>().paramEquals(other.to<QiSoundPlay>());
	case QiType::msgView: return to<QiMsgView>().paramEquals(other.to<QiMsgView>());
	case QiType::range: return to<QiRangeSet>().paramEquals(other.to<QiRangeSet>());
	}
	return false;
}
QJsonObject Action::toJson() const
{
	return base().toJson();
}
void Action::fromJson(const QJsonObject& json)
{
	switch (static_cast<QiType>(json.value("type").toInt()))
	{
	case QiType::end:
	{
		QiEnd a;
		a.fromJson(json);
		emplace<QiEnd>(a);
	} break;
	case QiType::delay:
	{
		QiDelay a;
		a.fromJson(json);
		emplace<QiDelay>(a);
	} break;
	case QiType::key:
	{
		QiKey a;
		a.fromJson(json);
		emplace<QiKey>(a);
	} break;
	case QiType::mouse:
	{
		QiMouse a;
		a.fromJson(json);
		emplace<QiMouse>(a);
	} break;
	case QiType::copyText:
	{
		QiCopyText a;
		a.fromJson(json);
		emplace<QiCopyText>(a);
	} break;
	case QiType::color:
	{
		QiColor a;
		a.fromJson(json);
		emplace<QiColor>(a);
	} break;
	case QiType::loop:
	{
		QiLoop a;
		a.fromJson(json);
		emplace<QiLoop>(a);
	} break;
	case QiType::loopEnd:
	{
		QiLoopEnd a;
		a.fromJson(json);
		emplace<QiLoopEnd>(a);
	} break;
	case QiType::keyState:
	{
		QiKeyState a;
		a.fromJson(json);
		emplace<QiKeyState>(a);
	} break;
	case QiType::resetPos:
	{
		QiResetPos a;
		a.fromJson(json);
		emplace<QiResetPos>(a);
	} break;
	case QiType::image:
	{
		QiImage a;
		a.fromJson(json);
		emplace<QiImage>(a);
	} break;
	case QiType::popText:
	{
		QiPopText a;
		a.fromJson(json);
		emplace<QiPopText>(a);
	} break;
	case QiType::savePos:
	{
		QiSavePos a;
		a.fromJson(json);
		emplace<QiSavePos>(a);
	} break;
	case QiType::timer:
	{
		QiTimer a;
		a.fromJson(json);
		emplace<QiTimer>(a);
	} break;
	case QiType::jump:
	{
		QiJump a;
		a.fromJson(json);
		emplace<QiJump>(a);
	} break;
	case QiType::jumpPoint:
	{
		QiJumpPoint a;
		a.fromJson(json);
		emplace<QiJumpPoint>(a);
	} break;
	case QiType::dialog:
	{
		QiDialog a;
		a.fromJson(json);
		emplace<QiDialog>(a);
	} break;
	case QiType::block:
	{
		QiBlock a;
		a.fromJson(json);
		emplace<QiBlock>(a);
	} break;
	case QiType::blockExec:
	{
		QiBlockExec a;
		a.fromJson(json);
		emplace<QiBlockExec>(a);
	} break;
	case QiType::quickInput:
	{
		QiQuickInput a;
		a.fromJson(json);
		emplace<QiQuickInput>(a);
	} break;
	case QiType::keyBlock:
	{
		QiKeyBlock a;
		a.fromJson(json);
		emplace<QiKeyBlock>(a);
	} break;
	case QiType::clock:
	{
		QiClock a;
		a.fromJson(json);
		emplace<QiClock>(a);
	} break;
	case QiType::ocr:
	{
		QiOcr a;
		a.fromJson(json);
		emplace<QiOcr>(a);
	} break;
	case QiType::varOperator:
	{
		QiVarOperator a;
		a.fromJson(json);
		emplace<QiVarOperator>(a);
	} break;
	case QiType::varCondition:
	{
		QiVarCondition a;
		a.fromJson(json);
		emplace<QiVarCondition>(a);
	} break;
	case QiType::mouseTrack:
	{
		QiMouseTrack a;
		a.fromJson(json);
		emplace<QiMouseTrack>(a);
	} break;
	case QiType::open:
	{
		QiOpen a;
		a.fromJson(json);
		emplace<QiOpen>(a);
	} break;
	case QiType::textPad:
	{
		QiTextPad a;
		a.fromJson(json);
		emplace<QiTextPad>(a);
	} break;
	case QiType::editDialog:
	{
		QiEditDialog a;
		a.fromJson(json);
		emplace<QiEditDialog>(a);
	} break;
	case QiType::volume:
	{
		QiVolume a;
		a.fromJson(json);
		emplace<QiVolume>(a);
	} break;
	case QiType::soundPlay:
	{
		QiSoundPlay a;
		a.fromJson(json);
		emplace<QiSoundPlay>(a);
	} break;
	case QiType::msgView:
	{
		QiMsgView a;
		a.fromJson(json);
		emplace<QiMsgView>(a);
	} break;
	case QiType::range:
	{
		QiRangeSet a;
		a.fromJson(json);
		emplace<QiRangeSet>(a);
	} break;
	default:
	{
		QiBase a;
		a.fromJson(json);
		emplace<QiBase>(a);
	}
	}
}
typepack::object Action::toPack() const
{
	return base().toPack();
}
void Action::fromPack(const typepack::object& pack)
{
	switch (static_cast<QiType>(pack.get("t").toInt()))
	{
	case QiType::end:
	{
		QiEnd a;
		a.fromPack(pack);
		emplace<QiEnd>(a);
	} break;
	case QiType::delay:
	{
		QiDelay a;
		a.fromPack(pack);
		emplace<QiDelay>(a);
	} break;
	case QiType::key:
	{
		QiKey a;
		a.fromPack(pack);
		emplace<QiKey>(a);
	} break;
	case QiType::mouse:
	{
		QiMouse a;
		a.fromPack(pack);
		emplace<QiMouse>(a);
	} break;
	case QiType::copyText:
	{
		QiCopyText a;
		a.fromPack(pack);
		emplace<QiCopyText>(a);
	} break;
	case QiType::color:
	{
		QiColor a;
		a.fromPack(pack);
		emplace<QiColor>(a);
	} break;
	case QiType::loop:
	{
		QiLoop a;
		a.fromPack(pack);
		emplace<QiLoop>(a);
	} break;
	case QiType::loopEnd:
	{
		QiLoopEnd a;
		a.fromPack(pack);
		emplace<QiLoopEnd>(a);
	} break;
	case QiType::keyState:
	{
		QiKeyState a;
		a.fromPack(pack);
		emplace<QiKeyState>(a);
	} break;
	case QiType::resetPos:
	{
		QiResetPos a;
		a.fromPack(pack);
		emplace<QiResetPos>(a);
	} break;
	case QiType::image:
	{
		QiImage a;
		a.fromPack(pack);
		emplace<QiImage>(a);
	} break;
	case QiType::popText:
	{
		QiPopText a;
		a.fromPack(pack);
		emplace<QiPopText>(a);
	} break;
	case QiType::savePos:
	{
		QiSavePos a;
		a.fromPack(pack);
		emplace<QiSavePos>(a);
	} break;
	case QiType::timer:
	{
		QiTimer a;
		a.fromPack(pack);
		emplace<QiTimer>(a);
	} break;
	case QiType::jump:
	{
		QiJump a;
		a.fromPack(pack);
		emplace<QiJump>(a);
	} break;
	case QiType::jumpPoint:
	{
		QiJumpPoint a;
		a.fromPack(pack);
		emplace<QiJumpPoint>(a);
	} break;
	case QiType::dialog:
	{
		QiDialog a;
		a.fromPack(pack);
		emplace<QiDialog>(a);
	} break;
	case QiType::block:
	{
		QiBlock a;
		a.fromPack(pack);
		emplace<QiBlock>(a);
	} break;
	case QiType::blockExec:
	{
		QiBlockExec a;
		a.fromPack(pack);
		emplace<QiBlockExec>(a);
	} break;
	case QiType::quickInput:
	{
		QiQuickInput a;
		a.fromPack(pack);
		emplace<QiQuickInput>(a);
	} break;
	case QiType::keyBlock:
	{
		QiKeyBlock a;
		a.fromPack(pack);
		emplace<QiKeyBlock>(a);
	} break;
	case QiType::clock:
	{
		QiClock a;
		a.fromPack(pack);
		emplace<QiClock>(a);
	} break;
	case QiType::ocr:
	{
		QiOcr a;
		a.fromPack(pack);
		emplace<QiOcr>(a);
	} break;
	case QiType::varOperator:
	{
		QiVarOperator a;
		a.fromPack(pack);
		emplace<QiVarOperator>(a);
	} break;
	case QiType::varCondition:
	{
		QiVarCondition a;
		a.fromPack(pack);
		emplace<QiVarCondition>(a);
	} break;
	case QiType::mouseTrack:
	{
		QiMouseTrack a;
		a.fromPack(pack);
		emplace<QiMouseTrack>(a);
	} break;
	case QiType::open:
	{
		QiOpen a;
		a.fromPack(pack);
		emplace<QiOpen>(a);
	} break;
	case QiType::textPad:
	{
		QiTextPad a;
		a.fromPack(pack);
		emplace<QiTextPad>(a);
	} break;
	case QiType::editDialog:
	{
		QiEditDialog a;
		a.fromPack(pack);
		emplace<QiEditDialog>(a);
	} break;
	case QiType::volume:
	{
		QiVolume a;
		a.fromPack(pack);
		emplace<QiVolume>(a);
	} break;
	case QiType::soundPlay:
	{
		QiSoundPlay a;
		a.fromPack(pack);
		emplace<QiSoundPlay>(a);
	} break;
	case QiType::msgView:
	{
		QiMsgView a;
		a.fromPack(pack);
		emplace<QiMsgView>(a);
	} break;
	case QiType::range:
	{
		QiRangeSet a;
		a.fromPack(pack);
		emplace<QiRangeSet>(a);
	} break;
	default:
	{
		QiBase a;
		a.fromPack(pack);
		emplace<QiBase>(a);
	}
	}
}
Action* Action::iter(std::function<bool(Action&)> callBack, QiType type)
{
	Action* r = nullptr;
	if (base().next.not_empty()) r = base().next.iter(callBack, type);
	if (r) return r;
	if (base().next2.not_empty()) r = base().next2.iter(callBack, type);
	if (r) return r;
	return nullptr;
}
const Action* Action::iter(std::function<bool(const Action&)> callBack, QiType type) const
{
	const Action* r = nullptr;
	if (base().next.not_empty()) r = base().next.iter(callBack, type);
	if (r) return r;
	if (base().next2.not_empty()) r = base().next2.iter(callBack, type);
	if (r) return r;
	return nullptr;
}
Action* Action::find(QiType type, int id)
{
	return iter([id](Action& action) { if (action.base().id == id) return true; return false; }, type);
}
const Action* Action::find(QiType type, int id) const
{
	return iter([id](const Action& action) { if (action.base().id == id) return true; return false; }, type);
}
Actions Action::loadType(QiType type) const
{
	Actions actions;
	iter([&actions, type](const Action& action) { if (action.type() == type) actions.append(action); return false; }, type);
	return actions;
}
QiIdList Action::loadId(QiType type) const
{
	QiIdList id;
	iter([&id](const Action& action) { id.append(action.base().id); return false; }, type);
	return id;
}

bool Actions::equals(const Actions& other) const
{
	if (size() != other.size()) return false;
	for (size_t i = 0; i < size(); i++) if (!at(i).fullEquals(other.at(i))) return false;
	return true;
}
QJsonArray Actions::toJson() const
{
	QJsonArray array;
	for (const auto& i : *this) array.append(i.toJson());
	return array;
}
void Actions::fromJson(const QJsonArray& json)
{
	clear();
	for (const auto& i : json)
	{
		Action a;
		a.fromJson(i.toObject());
		append(std::move(a));
	}
}
typepack::array Actions::toPack() const
{
	typepack::array array;
	array.resize(size());
	for (size_t i = 0; i < array.size(); i++) array[i] = operator[](i).toPack();
	return array;
}
void Actions::fromPack(const typepack::array& pack)
{
	clear();
	for (const auto& i : pack)
	{
		Action a;
		a.fromPack(i.toObject());
		append(std::move(a));
	}
}
Action* Actions::iter(std::function<bool(Action&)> callBack, QiType type)
{
	for (Action& a : *this)
	{
		Action* r = nullptr;
		if (type == QiType::none || type == a.type()) if (callBack(a)) r = &a;
		if (r) return r;
		if (a.base().next.not_empty()) r = a.base().next.iter(callBack, type);
		if (r) return r;
		if (a.base().next2.not_empty()) r = a.base().next2.iter(callBack, type);
		if (r) return r;
	}
	return nullptr;
}
const Action* Actions::iter(std::function<bool(const Action&)> callBack, QiType type) const
{
	for (const Action& a : *this)
	{
		const Action* r = nullptr;
		if (type == QiType::none || type == a.type()) if (callBack(a)) r = &a;
		if (r) return r;
		if (a.base().next.not_empty()) r = a.base().next.iter(callBack, type);
		if (r) return r;
		if (a.base().next2.not_empty()) r = a.base().next2.iter(callBack, type);
		if (r) return r;
	}
	return nullptr;
}
Action* Actions::find(QiType type, int id)
{
	return iter([id](Action& action) { if (action.base().id == id) return true; return false; }, type);
}
const Action* Actions::find(QiType type, int id) const
{
	return iter([id](const Action& action) { if (action.base().id == id) return true; return false; }, type);
}

QiIndex Actions::find_index(QiType type, int id) const
{
	QiIndex index;
	::find_index(*this, index, type, id);
	return index;
}
QiIndex Actions::find_index(QiTypeProperty property) const
{
	QiIndex index;
	::find_index(*this, index, property);
	return index;
}
Actions Actions::loadType(QiType type) const
{
	Actions actions;
	iter([&actions, type](const Action& action) { if (action.type() == type) actions.append(action); return false; }, type);
	return actions;
}
QiIdList Actions::loadId(QiType type) const
{
	QiIdList id;
	iter([&id](const Action& action) { id.append(action.base().id); return false; }, type);
	return id;
}