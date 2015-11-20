#include "extreeview.h"
#include "headerview.h"

#include <QKeyEvent>

ExTreeView::ExTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setHeader(new HeaderView(Qt::Horizontal, true, this));
#if QT_VERSION >= 0x050000
	header()->setSectionResizeMode(QHeaderView::Fixed);
#else
	header()->setResizeMode(QHeaderView::Fixed);
#endif

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

	QMapIterator<int, int> i(mColumnsPercentWidth);
	while (i.hasNext())
	{
		i.next();
		setColumnWidth(i.key(), widthPercent * i.value());
	}
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
