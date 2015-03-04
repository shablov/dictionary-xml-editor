#include "dictionaryitem.h"

#include <QDomElement>


DictionaryItem::DictionaryItem() :
	tagName("dict"), engName(QString()), rusName(QString())
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

	QDomNodeList childNodes = domElement.childNodes();
	for (int i = 0; i < childNodes.count(); i++)
	{
		QDomNode childDomNode = childNodes.at(i);
		QDomElement childDomElement = childDomNode.toElement();
		DictionaryItem *childItem = new DictionaryItem(childDomElement, this);
		childItems.append(childItem);
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

void DictionaryItem::setEnglishName(const QString &name)
{
	engName = name;
}

void DictionaryItem::setRussiaName(const QString &name)
{
	rusName = name;
}

void DictionaryItem::setParent(DictionaryItem *parent)
{
	parentItem = parent;
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
	return childItems.takeAt(i);
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
	childItem->setParent(this);
	childItems.insert(i, childItem);
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
}

QDomElement DictionaryItem::toDomElement(QDomDocument &domDocument) const
{
	QDomElement domElement = domDocument.createElement(tagName);
	setElementAttributes(domElement);
	for (int i = 0; i < childItems.count(); i++)
	{
		const DictionaryItem *childItem = childItems.at(i);
		QDomElement childElement = childItem->toDomElement(domDocument);
		if (childItem->type() == ArgType)
		{
			childElement.setAttribute("value", i);
		}
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
