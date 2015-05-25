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
}

void CutItemCommand::redo()
{
	reinitializeIndexes();
	if (!pCutItem)
	{
		pCutItem = new DictionaryItem(pModel->itemForIndex(mIndex));
	}
	pModel->cutItem(mIndex.row(), mParentIndex);
}


PasteItemCommand::PasteItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index), pPasteItem(0)
{
}

PasteItemCommand::~PasteItemCommand()
{
	delete pPasteItem;
	pPasteItem = 0;
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
	insertedIndex = pModel->pasteItem(mIndex);
	if (insertedIndex.isValid() && !pPasteItem)
	{
		pPasteItem = new DictionaryItem(pModel->itemForIndex(insertedIndex));
	}
}
