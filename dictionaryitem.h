#ifndef DICTIONARYITEM_H
#define DICTIONARYITEM_H

#include <QList>
#include <QString>

#include <QDomDocument>
#include <QDomElement>

class DictionaryItem
{
public:
	enum ItemType
	{
		ContextType,
		StringType,
		EnumType,
		ArgType,
		Invalid
	};

	explicit DictionaryItem();
	explicit DictionaryItem(ItemType typeForTagName, DictionaryItem *parent = 0);
	explicit DictionaryItem(const QDomElement &domElement, DictionaryItem *parent = 0);
	explicit DictionaryItem(DictionaryItem *item, DictionaryItem *parent = 0);
	~DictionaryItem();

	QString englishName() const;
	QString russiaName() const;
	ItemType type() const;
	static ItemType typeForTagName(const QString &tagName);
	static QByteArray tagNameForType(ItemType typeForTagName);

	void setEnglishName(const QString &name);
	void setRussiaName(const QString &name);
	void setParent(DictionaryItem *parent);

	DictionaryItem *childAt(const int &i) const;
	DictionaryItem *takeChild(const int &i);
	int rowOfChild(DictionaryItem *child) const;
	int childCount() const;
	void insertChild(const int &i, DictionaryItem *childItem);

	DictionaryItem *parent() const;

	QDomElement toDomElement(QDomDocument &domDocument) const;
	QDomDocument toDomDocument() const;
	void setElementAttributes(QDomElement &domElement) const;

private:
	QString tagName;

	DictionaryItem *parentItem;
	QList<DictionaryItem*> childItems;

	QString engName;
	QString rusName;
};

#endif // DICTIONARYITEM_H
