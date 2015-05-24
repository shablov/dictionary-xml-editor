#include "dictionarymodel.h"
#include "removeitemcommand.h"

#include <QAbstractItemView>

RemoveItemCommand::RemoveItemCommand(QAbstractItemView *view, QModelIndex &index) :
	pView(view), pItem(0), mIndex(index)
{
	mParentIndex = mIndex.parent();
	mGrandParentIndex = mParentIndex.parent();
	mRootParentIndex = mGrandParentIndex.parent();
	pModel = qobject_cast<DictionaryModel*>(pView->model());
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
	if (pModel->upItem(mIndex.row(), mParentIndex))
	{
		mIndex = pModel->index(mIndex.row() - 1, mIndex.column(), mParentIndex);
	}
	pView->scrollTo(mIndex);
}

void RemoveItemCommand::redo()
{
	reinitializeIndexes();
	pModel->removeRow(mIndex.row(), mParentIndex);
}

void RemoveItemCommand::reinitializeIndexes()
{
	if (mGrandParentIndex.isValid())
	{
		mGrandParentIndex = pModel->index(mGrandParentIndex.row(), mGrandParentIndex.column(), mRootParentIndex);
	}
	if (mParentIndex.isValid())
	{
		mParentIndex = pModel->index(mParentIndex.row(), mParentIndex.column(), mGrandParentIndex);
	}
	mIndex = pModel->index(mIndex.row(), mIndex.column(), mParentIndex);
}
