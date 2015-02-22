#ifndef DICTIONARYMODEL_H
#define DICTIONARYMODEL_H

#include <QAbstractItemModel>
#include <QStandardItemModel>

class DictionaryItem;

class DictionaryModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	DictionaryModel(QObject *parent = 0);
	~DictionaryModel();

	void load(const QString &fileName);

private:
	DictionaryItem *rootItem;

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
	virtual bool insertColumns(int column, int count, const QModelIndex &parent);
	virtual bool removeRows(int row, int count, const QModelIndex &parent);
	virtual bool removeColumns(int column, int count, const QModelIndex &parent);
};

#endif // DICTIONARYMODEL_H
