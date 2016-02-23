#include "dropmimedatacommand.h"

#include "dictionarymodel.h"

#include <QSortFilterProxyModel>

DropMimeDataCommand::DropMimeDataCommand(QAbstractItemView *view, const QByteArray &data, const int &row, const QModelIndex &proxyParentIndex)
	: ItemCommand(view, proxyParentIndex), mEncodingData(data), mRow(row)
{
	mIndex = QModelIndex();
	mParentIndex = proxyModel->mapToSource(proxyParentIndex.sibling(proxyParentIndex.row(), 0));
	QModelIndex proxyGrandParentIndex = proxyParentIndex.parent();
	mGrandParentIndex = proxyModel->mapToSource(proxyGrandParentIndex);
	QModelIndex proxyRootParentIndex = proxyGrandParentIndex.parent();
	mRootParentIndex = proxyModel->mapToSource(proxyRootParentIndex);
}

void DropMimeDataCommand::undo()
{
	reinitializeIndexes();
	sourceModel->submitDropMimeData(mEncodingData, mRow, mParentIndex);
}

void DropMimeDataCommand::redo()
{
	reinitializeIndexes();
	sourceModel->revertDropMimeData(mEncodingData, mRow, mParentIndex);
}
