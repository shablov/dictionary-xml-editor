#include "itemcommand.h"
#include "dictionarymodel.h"

#include <QAbstractItemView>
#include <QSortFilterProxyModel>

ItemCommand::ItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	pView(view)
{
	proxyModel = qobject_cast<QSortFilterProxyModel*>(view->model());
	Q_ASSERT(proxyModel);
	sourceModel = qobject_cast<DictionaryModel*>(proxyModel->sourceModel());
	Q_ASSERT(sourceModel);
	mIndex = proxyModel->mapToSource(index.sibling(index.row(), 0));
	QModelIndex proxyParentIndex = index.parent();
	mParentIndex = proxyModel->mapToSource(proxyParentIndex);
	QModelIndex proxyGrandParentIndex = proxyParentIndex.parent();
	mGrandParentIndex = proxyModel->mapToSource(proxyGrandParentIndex);
	QModelIndex proxyRootParentIndex = proxyGrandParentIndex.parent();
	mRootParentIndex = proxyModel->mapToSource(proxyRootParentIndex);
}

ItemCommand::~ItemCommand()
{

}

void ItemCommand::reinitializeIndexes()
{
	if (mGrandParentIndex.isValid())
	{
		mGrandParentIndex = sourceModel->index(mGrandParentIndex.row(), mGrandParentIndex.column(), mRootParentIndex);
	}
	if (mParentIndex.isValid())
	{
		mParentIndex = sourceModel->index(mParentIndex.row(), mParentIndex.column(), mGrandParentIndex);
	}
	mIndex = sourceModel->index(mIndex.row(), mIndex.column(), mParentIndex);
}
