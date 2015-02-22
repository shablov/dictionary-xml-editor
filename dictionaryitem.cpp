#include "dictionaryitem.h"

#include <QDomElement>

DictionaryItem::DictionaryItem(const QDomElement &domElement, DictionaryItem *parent)
	: parentItem(parent)
{
	tagName = domElement.tagName();
	rusName = domElement.attribute("name_rus", QString());
	engName = domElement.attribute("name_eng", QString());
	if (domElement.hasAttribute("value"))
	{
		QString value = domElement.attribute("value");
		enumId = static_cast<quint8>(value.toUInt());
	}

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
	parentItem->childItems.removeAll(this);
	parentItem = 0;
}

