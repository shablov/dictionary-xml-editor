#include "updownitemcommand.h"
#include "dictionarymodel.h"

#include <QAbstractItemView>
#include <QSortFilterProxyModel>

UpDownItemCommand::UpDownItemCommand(QAbstractItemView *view, const QModelIndex &index, MoveAction action) :
	ItemCommand(view, index), successMove(true), action(action)
{

}

void UpDownItemCommand::undo()
{
	moveItem(false);
}

void UpDownItemCommand::redo()
{
	moveItem(true);
}

void UpDownItemCommand::moveItem(bool isRedo)
{
	if (!successMove)
	{
		return;
	}
	reinitializeIndexes();
	QModelIndex index = ((action == UpMove) ^ !isRedo) ? sourceModel->upItem(mIndex.row(), mParentIndex)
													   : sourceModel->downItem(mIndex.row(), mParentIndex);
	successMove = (index.row() != mIndex.row());
	mIndex = index;
	pView->scrollTo(proxyModel->mapFromSource(mIndex));
}
