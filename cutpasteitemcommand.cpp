#include "cutpasteitemcommand.h"
#include "dictionarymodel.h"
#include <QAbstractItemView>

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
		mIndex = pModel->insertItem(new DictionaryItem(pCutItem), mIndex);
		mIndex = pModel->upItem(mIndex.row(), mParentIndex);
		pView->scrollTo(mIndex);
	}
	else
	{
		mIndex = pModel->insertItem(new DictionaryItem(pCutItem), mIndex);
		pView->scrollTo(mIndex);
	}
}

void CutItemCommand::redo()
{
	reinitializeIndexes();
	pModel->cutItem(mIndex.row(), mParentIndex);
	if (!pCutItem)
	{
		pCutItem = new DictionaryItem(pModel->cuttedItem());
	}
}


PasteItemCommand::PasteItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index)
{
	pPasteItem = new DictionaryItem(pModel->cuttedItem());
}

PasteItemCommand::~PasteItemCommand()
{
}

void PasteItemCommand::undo()
{
	reinitializeIndexes();
	insertedIndex = pModel->index(insertedIndex.row(), insertedIndex.column(), mParentIndex);
	pModel->removeRow(insertedIndex.row(), mParentIndex);
}

void PasteItemCommand::redo()
{
	reinitializeIndexes();
	insertedIndex = pModel->insertItem(new DictionaryItem(pPasteItem), mIndex);
	if (insertedIndex.parent() == mIndex)
	{
		mParentIndex = insertedIndex.parent();
		mGrandParentIndex = mParentIndex.parent();
		mRootParentIndex = mGrandParentIndex.parent();
	}
	pView->scrollTo(insertedIndex);
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
