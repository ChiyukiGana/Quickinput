#pragma once
#include <qstring.h>
#include <map>

#define lang_trans(Text) Qi::translate(Text)

#define lang_save_(Obj, Get) Obj->setProperty(Prop::lang_save,Obj->Get())
#define lang_load_(Obj, Set) Obj->Set(lang_trans(Obj->property(Prop::lang_save).toString()))
#define lang_trans_(Obj, Get, Set) Obj->Set(lang_trans(Obj->Get()))

#define lang_save_t(Obj) lang_save_(Obj, text)
#define lang_load_t(Obj) lang_load_(Obj, setText)
#define lang_trans_t(Obj) lang_trans_(Obj, text, setText)

#define lang_save_ti(Obj) lang_save_(Obj, title)
#define lang_load_ti(Obj) lang_load_(Obj, setTitle)
#define lang_trans_ti(Obj) lang_trans_(Obj, title, setTitle)

#define lang_save_tp(Obj) lang_save_(Obj, toolTip)
#define lang_load_tp(Obj) lang_load_(Obj, setToolTip)
#define lang_trans_tp(Obj) lang_trans_(Obj, toolTip, setToolTip)

#define lang_save_pht(Obj) lang_save_(Obj, placeholderText)
#define lang_load_pht(Obj) lang_load_(Obj, setPlaceholderText)
#define lang_trans_pht(Obj) lang_trans_(Obj, placeholderText, setPlaceholderText)

#define lang_save_tb(Obj) Obj->setData(DataRole::lang,Obj->text())
#define lang_load_tb(Obj) Obj->setText(lang_trans(Obj->data(DataRole::lang).toString()))
#define lang_trans_tb(Obj) lang_trans_(Obj, setText, text)

#define lang_save_icmb(Obj, i) Obj->setItemData(i,Obj->itemText(i),DataRole::lang)
#define lang_load_icmb(Obj, i) Obj->setItemText(i,lang_trans(Obj->itemData(i,DataRole::lang).toString()))
#define lang_trans_icmb(Obj, i) Obj->setItemText(i,lang_trans(Obj->itemText(i)))

#define lang_save_cmb(Obj) for(size_t i=0,c=Obj->count();i<c;i++)Obj->setItemData(i,Obj->itemText(i),DataRole::lang)
#define lang_load_cmb(Obj) for(size_t i=0,c=Obj->count();i<c;i++)Obj->setItemText(i,lang_trans(Obj->itemData(i,DataRole::lang).toString()))
#define lang_trans_cmb(Obj) for(size_t i=0,c=Obj->count();i<c;i++)Obj->setItemText(i,lang_trans(Obj->itemText(i)))

#define lang_load_ke(Obj) Obj->setKey(Obj->key());

struct Language
{
	QString name;
	std::map<QString, QString> map;
	QString translate(const QString& text) const
	{
		if (text.isEmpty()) return {};
		if (map.empty()) return text;
		auto i = map.find(text);
		if (i != map.end()) return i->second;
		return text;
	}
	QString translate(const QString& text, const QString& fail) const
	{
		if (text.isEmpty()) return {};
		if (map.empty()) return fail;
		auto i = map.find(text);
		if (i != map.end()) return i->second;
		return fail;
	}
	static QString makePath(const QString& name);
	static Language load(const QString& file);
};

namespace Qi
{
	inline Language lang;
	static QString translate(const QString& text)
	{
		if (text.isEmpty()) return {};
		return lang.translate(text);
	}
	static QString translate(const QString& text, const QString& fail)
	{
		if (text.isEmpty()) return {};
		return lang.translate(text, fail);
	}
	static std::wstring translate(const std::wstring& text)
	{
		if (text.empty()) return {};
		return lang.translate(QString::fromStdWString(text)).toStdWString();
	}
	static std::wstring translate(const std::wstring& text, const std::wstring& fail)
	{
		if (text.empty()) return {};
		return lang.translate(QString::fromStdWString(text), QString::fromStdWString(fail)).toStdWString();
	}
}