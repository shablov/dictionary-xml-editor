#include "extreeview.h"
#include "headerview.h"


ExTreeView::ExTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setHeader(new HeaderView(Qt::Horizontal, true, this));
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

	foreach(int col, columns)
	{
		mColumnsPercentWidth[col] = percent;
	}
}

void ExTreeView::setColumnPercentWidth(int col, int percent)
{
	if(! model() || col >= model()->columnCount())
	{
		return;
	}

	mColumnsPercentWidth[col] = percent;
}

void ExTreeView::resizeEvent(QResizeEvent *event)
{
	QTreeView::resizeEvent(event);

	double widthPercent = viewport()->width() / 100.0;

	foreach(int col, mColumnsPercentWidth.keys())
	{
		int colWidth = widthPercent * mColumnsPercentWidth[col];
		setColumnWidth(col, colWidth);
	}
}
