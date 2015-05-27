#include "datachangedcommand.h"
#include "QAbstractItemView"
#include "mainwindow.h"

DataChangedCommand::DataChangedCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index)
{
	mOldValue = pModel->modifiedData();
	mRole = pModel->modifiedRole();
	mNewValue = mIndex.data(mRole);
}

void DataChangedCommand::undo()
{
	changeData(mOldValue);
}

void DataChangedCommand::redo()
{
	changeData(mNewValue);
}

void DataChangedCommand::changeData(const QVariant &value)
{
	pModel->startUndo();
	reinitializeIndexes();
	pModel->setData(mIndex, value, mRole);
	pView->scrollTo(mIndex);
	pModel->stopUndo();
}

