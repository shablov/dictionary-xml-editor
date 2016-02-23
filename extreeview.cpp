#include "extreeview.h"
#include "headerview.h"

#include <QDebug>
#include <QKeyEvent>

ExTreeView::ExTreeView(QWidget *parent) :
	QTreeView(parent), mBeginColumnToTabOrder(0), mEndColumnToTabOrder(0)
{
	setHeader(new HeaderView(Qt::Horizontal, true, this));
	header()->setSectionResizeMode(QHeaderView::Fixed);

	connect(this, SIGNAL(collapsed(QModelIndex)), SLOT(setCurrentIndex(QModelIndex)));
	connect(this, SIGNAL(expanded(QModelIndex)), SLOT(setCurrentIndex(QModelIndex)));
}

void ExTreeView::normalizeColumnsWidth()
{
	if(! model() )
	{
		return;
	}

	QList<int> columns;
	for(int i = 0; i < model()->columnCount(); ++i)
	{
		if(! isColumnHidden(i) )
		{
			columns << i;
		}
	}

	mColumnsPercentWidth.clear();

	int percent = 100 / columns.count();

	foreach(const int &col, columns)
	{
		mColumnsPercentWidth[col] = percent;
	}
	resizeColumnsToPercentWidth();
}

void ExTreeView::resizeColumnsToPercentWidth()
{
	double widthPercent = viewport()->width() / 100.0;
	QMapIterator<int, int> i(mColumnsPercentWidth);
	while (i.hasNext())
	{
		i.next();
		setColumnWidth(i.key(), widthPercent * i.value());
	}
}

void ExTreeView::setColumnPercentWidth(int col, int percent)
{
	if(! model() || col >= model()->columnCount())
	{
		return;
	}
	mColumnsPercentWidth[col] = percent;
	resizeColumnsToPercentWidth();
}

int ExTreeView::beginColumnToTabOrder() const
{
	return mBeginColumnToTabOrder;
}

int ExTreeView::endColumnToTabOrder() const
{
	return mEndColumnToTabOrder;
}

void ExTreeView::setBeginColumnToTabOrder(int beginColumnToTabOrder)
{
	mBeginColumnToTabOrder = (beginColumnToTabOrder >= model()->columnCount() || beginColumnToTabOrder < 0)
			? 0
			: beginColumnToTabOrder;
}

void ExTreeView::setEndColumnToTabOrder(int endColumnToTabOrder)
{
	mEndColumnToTabOrder = (endColumnToTabOrder >= model()->columnCount() || endColumnToTabOrder < 0)
			? model()->rowCount() - 1
			: endColumnToTabOrder;
}

void ExTreeView::resizeEvent(QResizeEvent *event)
{
	QTreeView::resizeEvent(event);
	resizeColumnsToPercentWidth();
}


void ExTreeView::keyReleaseEvent(QKeyEvent *keyEvent)
{
	QTreeView::keyReleaseEvent(keyEvent);
	if (keyEvent->key() == Qt::Key_Up ||
			keyEvent->key() == Qt::Key_Down)
	{
		QModelIndex index = currentIndex();
		if (index.isValid())
		{
			emit pressed(index);
		}
	}
}

QModelIndex ExTreeView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	if (cursorAction == QAbstractItemView::MoveNext ||
		cursorAction == QAbstractItemView::MovePrevious)
	{
		int step = (cursorAction == QAbstractItemView::MoveNext ? 1 : -1);
		int columnForRowStep = (cursorAction == QAbstractItemView::MoveNext ? mBeginColumnToTabOrder
																			: mEndColumnToTabOrder);
		QModelIndex index = currentIndex();
		if (index.isValid())
		{
			QModelIndex stepColumnIndex = index.sibling(index.row(), index.column() + step);
			if (stepColumnIndex.isValid() && stepColumnIndex.column() >= beginColumnToTabOrder())
			{
				return stepColumnIndex;
			}
			QModelIndex stepRowIndex = index.sibling(index.row() + step, columnForRowStep);
			if (stepRowIndex.isValid())
			{
				return stepRowIndex;
			}
		}
	}
	return QTreeView::moveCursor(cursorAction, modifiers);
}


void ExTreeView::setModel(QAbstractItemModel *model)
{
	QTreeView::setModel(model);
	setEndColumnToTabOrder(model->columnCount() - 1);
}
