#ifndef DICTIONARYMODEL_H
#define DICTIONARYMODEL_H

#include "dictionaryitem.h"

#include <QAbstractItemModel>

class DictionaryItem;

class DictionaryModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	DictionaryModel(QObject *parent = 0);
	~DictionaryModel();

	bool load(const QString &fileName);
	bool save(const QString &fileName);
	void clear();
	void createNew();

	bool validate(const QByteArray &data);
	bool isModified();

	enum ModelError
	{
		FileOpenedError,
		XsdSchemaOpenedError,
		InvalidXsdSchema,
		InvalidXmlFile
	};

	DictionaryItem::ItemType typeForIndex(const QModelIndex &index) const;

	void insertDictionaryItem(DictionaryItem::ItemType type, const QModelIndex &index);

private:
	DictionaryItem *itemForIndex(const QModelIndex &index) const;

signals:
	void error(DictionaryModel::ModelError code, const QString &description);

private:
	enum Columns
	{
		EnglishColumn,
		RussiaColumn,
		ColumnCount
	};
	DictionaryItem *rootItem;

	bool mIsModified;

protected:
	void reset();

	// QAbstractItemModel interface
public:
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	virtual bool insertRows(int row, int count, const QModelIndex &parent);
	virtual bool removeRows(int row, int count, const QModelIndex &parent);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // DICTIONARYMODEL_H
