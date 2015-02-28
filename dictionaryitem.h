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
		ArgType
	};

	explicit DictionaryItem();
	explicit DictionaryItem(QString engName, QString rusName);
	explicit DictionaryItem(const QDomElement &domElement, DictionaryItem *parent);
	~DictionaryItem();

	QString englishName() const;
	QString russiaName() const;
	ItemType type();

	void setEnglishName(const QString &name);
	void setRussiaName(const QString &name);

	DictionaryItem *childAt(int i) const;
	int rowOfChild(DictionaryItem *child) const;
	int childCount() const;
	void insertChild(const int &i, DictionaryItem *childItem);

	DictionaryItem *parent() const;

	QDomElement toDomElement(QDomDocument &domDocument) const;
	QDomDocument toDomDocument() const;

private:
	QString tagName;

	DictionaryItem *parentItem;
	QList<DictionaryItem*> childItems;

	QString engName;
	QString rusName;
	qint8 enumId;
};

#endif // DICTIONARYITEM_H
