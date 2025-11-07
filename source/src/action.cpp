#include "action.h"
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