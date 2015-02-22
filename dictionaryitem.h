#ifndef DICTIONARYITEM_H
#define DICTIONARYITEM_H

#include <QList>
#include <QString>

#include <QStandardItem>

class QDomElement;

enum DictionaryItemType
{
	ContextItem,
	StringItem,
	EnumItem,
	ArgumentItem
};

class DictionaryItem : public QStandardItem
{
public:
	explicit DictionaryItem(const QDomElement &domElement, DictionaryItem *parent);
	~DictionaryItem();

	QString tagName;

	DictionaryItem *parentItem;
	QList<DictionaryItem*> childItems;

	QString engName;
	QString rusName;
	quint8 enumId;
};

#endif // DICTIONARYITEM_H
