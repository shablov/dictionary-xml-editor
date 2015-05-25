#include "datachangedcommand.h"
#include "QAbstractItemView"
#include "mainwindow.h"

DataChangedCommand::DataChangedCommand(QAbstractItemView *view, const QModelIndex &index, MainWindow *w) :
	ItemCommand(view, index), w(w)
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
	reinitializeIndexes();
	QObject::disconnect(pModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), w, SLOT(onDataChanged(QModelIndex)));
	pModel->setData(mIndex, value, mRole);
	QObject::connect(pModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), w, SLOT(onDataChanged(QModelIndex)));
	pView->scrollTo(mIndex);
}

