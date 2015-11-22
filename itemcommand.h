#ifndef ITEMCOMMAND_H
#define ITEMCOMMAND_H

#include <QUndoCommand>
#include <QModelIndex>

class QAbstractItemView;
class DictionaryModel;
class DictionaryItem;
class QSortFilterProxyModel;

class ItemCommand : public QUndoCommand
{
public:
	ItemCommand(QAbstractItemView *view, const QModelIndex &index);
	virtual ~ItemCommand();

protected:
	virtual void reinitializeIndexes();

	QAbstractItemView *pView;
	DictionaryModel *sourceModel;
	QSortFilterProxyModel *proxyModel;
	QModelIndex mIndex;
	QModelIndex mParentIndex;
	QModelIndex mGrandParentIndex;
	QModelIndex mRootParentIndex;
	QModelIndex insertedIndex;
};

#endif // ITEMCOMMAND_H
