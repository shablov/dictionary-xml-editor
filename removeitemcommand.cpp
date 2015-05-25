#include "dictionarymodel.h"
#include "removeitemcommand.h"

#include <QTreeView>

RemoveItemCommand::RemoveItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index), isExpanded(false)
{
	pItem = new DictionaryItem(pModel->itemForIndex(mIndex));
}

RemoveItemCommand::~RemoveItemCommand()
{
	delete pItem;
}

void RemoveItemCommand::undo()
{
	reinitializeIndexes();
	mIndex = pModel->insertItem(new DictionaryItem(pItem), mIndex);
	mIndex = pModel->upItem(mIndex.row(), mParentIndex);
	pView->scrollTo(mIndex);
	if (isExpanded)
	{
		qobject_cast<QTreeView*>(pView)->expand(mIndex);
	}
}

void RemoveItemCommand::redo()
{
	reinitializeIndexes();
	isExpanded = qobject_cast<QTreeView*>(pView)->isExpanded(mIndex);
	pModel->removeRow(mIndex.row(), mParentIndex);
}
