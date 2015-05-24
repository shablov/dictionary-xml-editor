#include "datachangedcommand.h"
#include "mainwindow.h"

DataChangedCommand::DataChangedCommand(QAbstractItemModel *model, const QModelIndex &index, MainWindow *w) : w(w)
{
	mIndex = index;
	mParentIndex = mIndex.parent();
	mGrandParentIndex = mParentIndex.parent();
	mRootParentIndex = mGrandParentIndex.parent();

	pModel = qobject_cast<DictionaryModel*>(model);
	mOldValue = pModel->modifiedData();
	mRole = pModel->modifiedRole();
	mNewValue = index.data(mRole);
}

void DataChangedCommand::undo()
{
	command(mOldValue);
}

void DataChangedCommand::redo()
{
	command(mNewValue);
}

void DataChangedCommand::command(const QVariant &value)
{
	reinitializeIndexes();
	QObject::disconnect(pModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), w, SLOT(onDataChanged(QModelIndex)));
	pModel->setData(mIndex, value, mRole);
	QObject::connect(pModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), w, SLOT(onDataChanged(QModelIndex)));
}

void DataChangedCommand::reinitializeIndexes()
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

