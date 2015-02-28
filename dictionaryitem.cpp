#include "dictionaryitem.h"

#include <QDomElement>


DictionaryItem::DictionaryItem() :
	tagName("context"), engName("default"), rusName("cтандартный"), enumId(-1)
{
}

DictionaryItem::DictionaryItem(QString engName, QString rusName) :
	tagName("string"), engName(engName), rusName(rusName), enumId(-1)
{

}

DictionaryItem::DictionaryItem(const QDomElement &domElement, DictionaryItem *parent)
	: parentItem(parent)
{
	tagName = domElement.tagName();
	rusName = domElement.attribute("name_rus", QString());
	engName = domElement.attribute("name_eng", QString());
	QString value = domElement.attribute("value", QString("-1"));
	enumId = static_cast<qint8>(value.toInt());

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

DictionaryItem::ItemType DictionaryItem::type()
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
	return ContextType;
}

void DictionaryItem::setEnglishName(const QString &name)
{
	engName = name;
}

void DictionaryItem::setRussiaName(const QString &name)
{
	rusName = name;
}

DictionaryItem *DictionaryItem::childAt(int i) const
{
	if ((childItems.isEmpty()) || (childItems.count() < i))
	{
		return 0;
	}
	return childItems.at(i);
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
	childItems.insert(i, childItem);
}

DictionaryItem *DictionaryItem::parent() const
{
	return parentItem;
}

QDomElement DictionaryItem::toDomElement(QDomDocument &domDocument) const
{
	QDomElement domElement = domDocument.createElement(tagName);
	if (!engName.isEmpty())
	{
		domElement.setAttribute("name_eng", engName);
	}
	if (!rusName.isEmpty())
	{
		domElement.setAttribute("name_rus", rusName);
	}
	if (enumId != -1)
	{
		domElement.setAttribute("value", enumId);
	}
	for (int i = 0; i < childItems.count(); i++)
	{
		const DictionaryItem *childItem = childItems.at(i);
		domElement.appendChild(childItem->toDomElement(domDocument));
	}
	return domElement;
}

QDomDocument DictionaryItem::toDomDocument() const
{
	QDomDocument domDocument("dictionary");
	QDomElement domElement = domDocument.createElement(tagName);
	domDocument.appendChild(domElement);
	if (!engName.isEmpty())
	{
		domElement.setAttribute("name_eng", engName);
	}
	if (!rusName.isEmpty())
	{
		domElement.setAttribute("name_rus", rusName);
	}
	if (enumId != -1)
	{
		domElement.setAttribute("value", enumId);
	}
	for (int i = 0; i < childItems.count(); i++)
	{
		const DictionaryItem *childItem = childItems.at(i);
		domElement.appendChild(childItem->toDomElement(domDocument));
	}
	return domDocument;
}
