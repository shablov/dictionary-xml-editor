#include "itemcommand.h"
#include "dictionarymodel.h"

#include <QAbstractItemView>

ItemCommand::ItemCommand(QAbstractItemView *view, const QModelIndex &index) :
	pView(view), mIndex(index)
{
	mParentIndex = mIndex.parent();
	mGrandParentIndex = mParentIndex.parent();
	mRootParentIndex = mGrandParentIndex.parent();
	pModel = qobject_cast<DictionaryModel*>(view->model());
}

ItemCommand::~ItemCommand()
{

}

void ItemCommand::reinitializeIndexes()
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
