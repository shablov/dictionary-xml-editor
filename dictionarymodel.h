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

	enum ModelError
	{
		FileOpenedError,
		XsdSchemaOpenedError,
		InvalidXsdSchema,
		InvalidXmlFile
	};

	DictionaryItem::ItemType typeForIndex(const QModelIndex &index) const;

	QModelIndex insertDictionaryItem(DictionaryItem::ItemType type, const QModelIndex &index);
	void cutItem(const int &row, const QModelIndex &parent);
	DictionaryItem *cuttedItem();
	void copyItem(const int &row, const QModelIndex &parent);
	DictionaryItem::ItemType typeForCutItem();
	QModelIndex pasteItem(const QModelIndex &index);

	QModelIndex upItem(int itemRow, const QModelIndex &parent);
	QModelIndex downItem(int itemRow, const QModelIndex &parent);

	enum Columns
	{
		PixmapColumn,
		EnglishColumn,
		RussiaColumn,
		ColumnCount
	};

	QVariant modifiedData() const;
	int modifiedRole() const;
	DictionaryItem *itemForIndex(const QModelIndex &index) const;
	QModelIndex insertItem(DictionaryItem *itemForInsert, const QModelIndex &index);

	void startUndo();
	void stopUndo();

signals:
	void error(DictionaryModel::ModelError code, const QString &description);
	void modified(bool);
	void modifiedData(const QModelIndex &index);

private:
	DictionaryItem *pRootItem;
	DictionaryItem *pCutItem;

	QVariant mModifiedData;
	int mModifiedRole;
	bool isUndoStop;

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
	virtual QStringList mimeTypes() const;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
	virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	virtual Qt::DropActions supportedDropActions() const;
	virtual Qt::DropActions supportedDragActions() const;
	virtual bool insertRows(int row, int count, const QModelIndex &parent);
	virtual bool removeRows(int row, int count, const QModelIndex &parent);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // DICTIONARYMODEL_H
