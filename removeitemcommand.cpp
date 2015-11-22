#include "dictionarymodel.h"
#include "removeitemcommand.h"

#include <QTreeView>
#include <QSortFilterProxyModel>

RemoveItemCommand::RemoveItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index), isExpanded(false)
{
	pItem = new DictionaryItem(sourceModel->itemForIndex(mIndex));
}

RemoveItemCommand::~RemoveItemCommand()
{
	delete pItem;
}

void RemoveItemCommand::undo()
{
	reinitializeIndexes();
	bool lastItem = !mIndex.isValid();
	mIndex = mIndex.isValid() ? mIndex : mParentIndex;
	mIndex = sourceModel->insertItem(new DictionaryItem(pItem), mIndex);
	if (!lastItem)
	{
		mIndex = sourceModel->upItem(mIndex.row(), mParentIndex);
	}
	pView->scrollTo(proxyModel->mapFromSource(mIndex));
	if (isExpanded)
	{
		qobject_cast<QTreeView*>(pView)->expand(proxyModel->mapFromSource(mIndex));
	}
}

void RemoveItemCommand::redo()
{
	reinitializeIndexes();
	isExpanded = qobject_cast<QTreeView*>(pView)->isExpanded(proxyModel->mapFromSource(mIndex));
	sourceModel->removeRow(mIndex.row(), mParentIndex);
}
