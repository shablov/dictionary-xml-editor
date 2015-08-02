#include "dictionaryitem.h"

#include <QDomElement>
#include <QCoreApplication>


DictionaryItem::DictionaryItem() :
	tagName("dict"), engName(QString()), rusName(QString()), mEnumValue(0)
{
}

DictionaryItem::DictionaryItem(ItemType type, DictionaryItem *parent) : parentItem(parent)
{
	tagName = tagNameForType(type);
}

DictionaryItem::DictionaryItem(const QDomElement &domElement, DictionaryItem *parent)
	: parentItem(parent)
{
	tagName = domElement.tagName();
	rusName = domElement.attribute("name_rus", QString());
	engName = domElement.attribute("name_eng", QString());
	mEnumValue = domElement.attribute("value", QString("0")).toUInt();

	QDomNodeList childNodes = domElement.childNodes();
	for (int i = 0; i < childNodes.count(); i++)
	{
		QDomNode childDomNode = childNodes.at(i);
		QDomElement childDomElement = childDomNode.toElement();
		DictionaryItem *childItem = new DictionaryItem(childDomElement, this);
		childItems.append(childItem);
	}
}

DictionaryItem::DictionaryItem(DictionaryItem *item, DictionaryItem *parent) : parentItem(parent)
{
	tagName = item->tagName;
	engName = item->engName;
	rusName = item->rusName;
	mEnumValue = item->mEnumValue;
	foreach (DictionaryItem *childItem, item->childItems)
	{
		childItems << new DictionaryItem(childItem, this);
	}
}

DictionaryItem::~DictionaryItem()
{
	qDeleteAll(childItems);
}

QString DictionaryItem::englishName() const
{
	return engName;
}

QString DictionaryItem::russiaName() const
{
	return rusName;
}

DictionaryItem::ItemType DictionaryItem::type() const
{
	return typeForTagName(tagName);
}

quint32 DictionaryItem::enumValue() const
{
	return mEnumValue;
}

DictionaryItem::ItemType DictionaryItem::typeForTagName(const QString &tagName)
{
	if (tagName == "enum")
	{
		return EnumType;
	}
	if (tagName == "string")
	{
		return StringType;
	}
	if (tagName == "arg")
	{
		return ArgType;
	}
	if (tagName == "context")
	{
		return ContextType;
	}
	return Invalid;
}

QByteArray DictionaryItem::tagNameForType(DictionaryItem::ItemType type)
{
	switch (type)
	{
		case DictionaryItem::ContextType: return "context";
		case DictionaryItem::StringType: return "string";
		case DictionaryItem::EnumType: return "enum";
		case DictionaryItem::ArgType: return "arg";
		default: return QByteArray();
	}
}

QString DictionaryItem::ruTagNameForType(DictionaryItem::ItemType type)
{
	switch (type)
	{
		case DictionaryItem::ContextType: return QCoreApplication::translate("DictionaryItem", "context");
		case DictionaryItem::StringType: return QCoreApplication::translate("DictionaryItem", "string");
		case DictionaryItem::EnumType: return QCoreApplication::translate("DictionaryItem", "enum");
		case DictionaryItem::ArgType: return QCoreApplication::translate("DictionaryItem", "arg");
		default: return QString();
	}
}

void DictionaryItem::setEnglishName(const QString &name)
{
	engName = name;
}

void DictionaryItem::setRussiaName(const QString &name)
{
	rusName = name;
}

void DictionaryItem::setEnumValue(const quint32 &enumValue)
{
	mEnumValue = enumValue;
}

void DictionaryItem::setParent(DictionaryItem *parent)
{
	parentItem = parent;
	if (!parent)
	{
		return;
	}
	changeType(this, parent);
	parent->childItems.insert(parent->childCount(), this);
}

void DictionaryItem::changeType(DictionaryItem *childItem, DictionaryItem *parentItem)
{
	if (parentItem->type() == EnumType)
	{
		childItem->setType(ArgType);
	}
	if (parentItem->type() == ContextType && childItem->type() == ArgType)
	{
		childItem->setType(StringType);
	}
}

void DictionaryItem::setType(DictionaryItem::ItemType type)
{
	tagName = tagNameForType(type);
}

DictionaryItem *DictionaryItem::childAt(const int &i) const
{
	if ((childItems.isEmpty()) || (childItems.count() <= i))
	{
		return 0;
	}
	return childItems.at(i);
}

DictionaryItem *DictionaryItem::takeChild(const int &i)
{
	if ((childItems.isEmpty()) || (childItems.count() <= i))
	{
		return 0;
	}
	DictionaryItem *childItem = childItems.takeAt(i);
	childItem->setParent(0);
	return childItem;
}

int DictionaryItem::rowOfChild(DictionaryItem *child) const
{
	return childItems.indexOf(child);
}

int DictionaryItem::childCount() const
{
	return childItems.count();
}

void DictionaryItem::insertChild(const int &i, DictionaryItem *childItem)
{
	if (!childItem)
	{
		return;
	}
	childItem->parentItem = this;
	changeType(childItem, this);
	childItems.insert(i, childItem);
}

void DictionaryItem::moveChild(const int &from, const int &to)
{
	childItems.move(from, to);
}

DictionaryItem *DictionaryItem::parent() const
{
	return parentItem;
}

void DictionaryItem::setElementAttributes(QDomElement &domElement) const
{
	if (!engName.isEmpty())
	{
		domElement.setAttribute("name_eng", engName);
	}
	if (!rusName.isEmpty())
	{
		domElement.setAttribute("name_rus", rusName);
	}
	if (typeForTagName(tagName) == ArgType)
	{
		domElement.setAttribute("value", mEnumValue);
	}
}

QPixmap DictionaryItem::pixmap()
{
	switch (type())
	{
		case ContextType: return QPixmap(":images/context");
		case ArgType: return QPixmap(":images/argument");
		case EnumType: return QPixmap(":images/enum");
		case StringType: return QPixmap(":images/string");
		default: return QPixmap();
	}
}

QDomElement DictionaryItem::toDomElement(QDomDocument &domDocument) const
{
	QDomElement domElement = domDocument.createElement(tagName);
	setElementAttributes(domElement);
	for (int i = 0; i < childItems.count(); i++)
	{
		const DictionaryItem *childItem = childItems.at(i);
		QDomElement childElement = childItem->toDomElement(domDocument);
		domElement.appendChild(childElement);
	}
	return domElement;
}

QDomDocument DictionaryItem::toDomDocument() const
{
	QDomDocument domDocument("dictionary");
	QDomElement domElement = toDomElement(domDocument);
	setElementAttributes(domElement);
	domDocument.appendChild(domElement);
	return domDocument;
}


QDataStream &operator <<(QDataStream &in, DictionaryItem *item)
{
	in << static_cast<quint8>(item->type());
	in << item->englishName();
	in << item->russiaName();
	in << item->enumValue();
	in << static_cast<quint32>(item->childCount());
	foreach (DictionaryItem *childItem, item->childItems)
	{
		in << childItem;
	}
	return in;
}

QDataStream &operator >>(QDataStream &out, DictionaryItem *item)
{
	quint8 type = DictionaryItem::Invalid;
	QString engName;
	QString rusName;
	quint32 enumValue = 0;
	quint32 childCount = 0;

	out >> type;
	out >> engName;
	out >> rusName;
	out >> enumValue;

	item->engName = engName;
	item->rusName = rusName;
	item->mEnumValue = enumValue;
	item->tagName = DictionaryItem::tagNameForType(static_cast<DictionaryItem::ItemType>(type));

	out >> childCount;
	for (quint32 i = 0; i < childCount; i++)
	{
		DictionaryItem *childItem = new DictionaryItem;
		out >> childItem;
		item->insertChild(i, childItem);
	}
	return out;
}
