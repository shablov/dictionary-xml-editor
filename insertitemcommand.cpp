#include "insertitemcommand.h"
#include "dictionarymodel.h"

#include <QAbstractItemView>

InsertItemCommand::InsertItemCommand(QAbstractItemView *view, const QModelIndex &index, int type) :
	ItemCommand(view, index)
{
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
	if (insertedIndex.parent() == mIndex)
	{
		mParentIndex = insertedIndex.parent();
		mGrandParentIndex = mParentIndex.parent();
		mRootParentIndex = mGrandParentIndex.parent();
	}
	pView->scrollTo(insertedIndex);
}


void InsertItemCommand::reinitializeIndexes()
{
	bool indexIsParent = (mParentIndex == mIndex);
	ItemCommand::reinitializeIndexes();
	if (indexIsParent)
	{
		mIndex = mParentIndex;
	}
}
