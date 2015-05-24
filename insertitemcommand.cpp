#include "insertitemcommand.h"

#include "dictionarymodel.h"

#include <QAbstractItemView>

InsertItemCommand::InsertItemCommand(QAbstractItemView *view, int type, const QModelIndex &index) :
	view(view), mIndex(index)
{
	mParentIndex = mIndex.parent();
	mGrandParentIndex = mParentIndex.parent();
	mRootParentIndex = mGrandParentIndex.parent();
	pModel = qobject_cast<DictionaryModel*>(view->model());
	pItem = new DictionaryItem(static_cast<DictionaryItem::ItemType>(type));
}

InsertItemCommand::~InsertItemCommand()
{
	delete pItem;
}

void InsertItemCommand::undo()
{
	reinitializeIndexes();
	insertedIndex = pModel->index(insertedIndex.row(), insertedIndex.column(), mParentIndex);
	pModel->removeRow(insertedIndex.row(), mParentIndex);
}

void InsertItemCommand::redo()
{
	reinitializeIndexes();
	insertedIndex = pModel->insertItem(new DictionaryItem(pItem), mIndex);
	view->scrollTo(insertedIndex);

}

void InsertItemCommand::reinitializeIndexes()
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

