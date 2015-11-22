#include "cutpasteitemcommand.h"
#include "dictionarymodel.h"
#include <QAbstractItemView>
#include <QSortFilterProxyModel>

CutItemCommand::CutItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index), pCutItem(0)
{
}

CutItemCommand::~CutItemCommand()
{
	delete pCutItem;
	pCutItem = 0;
}



void CutItemCommand::undo()
{
	reinitializeIndexes();
	if (mIndex.isValid())
	{
		mIndex = sourceModel->insertItem(new DictionaryItem(pCutItem), mIndex);
		mIndex = sourceModel->upItem(mIndex.row(), mParentIndex);
		pView->scrollTo(proxyModel->mapFromSource(mIndex));
	}
	else
	{
		mIndex = sourceModel->insertItem(new DictionaryItem(pCutItem), mIndex);
		pView->scrollTo(mIndex);
	}
}

void CutItemCommand::redo()
{
	reinitializeIndexes();
	sourceModel->cutItem(mIndex.row(), mParentIndex);
	if (!pCutItem)
	{
		pCutItem = new DictionaryItem(sourceModel->cuttedItem());
	}
}


PasteItemCommand::PasteItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index), pItem(0)
{
	if (sourceModel->cuttedItem())
	{
		pItem = new DictionaryItem(sourceModel->cuttedItem());
	}
}

PasteItemCommand::~PasteItemCommand()
{
}

void PasteItemCommand::undo()
{
	reinitializeIndexes();
	insertedIndex = sourceModel->index(insertedIndex.row(), insertedIndex.column(), mParentIndex);
	sourceModel->removeRow(insertedIndex.row(), mParentIndex);
}

void PasteItemCommand::redo()
{
	reinitializeIndexes();
	insertedIndex = sourceModel->insertItem(new DictionaryItem(pItem), mIndex);
	if (insertedIndex.parent() == mIndex)
	{
		mParentIndex = insertedIndex.parent();
		mGrandParentIndex = mParentIndex.parent();
		mRootParentIndex = mGrandParentIndex.parent();
	}
	pView->scrollTo(proxyModel->mapFromSource(insertedIndex));
}

void PasteItemCommand::reinitializeIndexes()
{
	bool indexIsParent = (mParentIndex == mIndex);
	ItemCommand::reinitializeIndexes();
	if (indexIsParent)
	{
		mIndex = mParentIndex;
	}
}
