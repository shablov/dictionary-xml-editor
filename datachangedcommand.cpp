#include "datachangedcommand.h"
#include "QAbstractItemView"
#include "mainwindow.h"

#include <QSortFilterProxyModel>

DataChangedCommand::DataChangedCommand(QAbstractItemView *view, const QModelIndex &index) :
	ItemCommand(view, index)
{
	mIndex = proxyModel->mapToSource(index);
	mOldValue = sourceModel->modifiedData();
	mRole = sourceModel->modifiedRole();
	mNewValue = mIndex.data(Qt::DisplayRole);
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
	sourceModel->startUndo();
	reinitializeIndexes();
	sourceModel->setData(mIndex, value, mRole);
	pView->scrollTo(proxyModel->mapFromSource(mIndex));
	sourceModel->stopUndo();
}

