#ifndef EXTREEVIEW_H
#define EXTREEVIEW_H

#include <QTreeView>

class ExTreeView : public QTreeView
{
	Q_OBJECT
	Q_PROPERTY(int beginColumnToTabOrder READ beginColumnToTabOrder WRITE setBeginColumnToTabOrder)
	Q_PROPERTY(int endColumnToTabOrder READ endColumnToTabOrder WRITE setEndColumnToTabOrder)
public:
	explicit ExTreeView(QWidget *parent = 0);

	void normalizeColumnsWidth();
	void setColumnPercentWidth(int col, int percent);
	int beginColumnToTabOrder() const;
	int endColumnToTabOrder() const;

public slots:
	void setBeginColumnToTabOrder(int beginColumnToTabOrder);
	void setEndColumnToTabOrder(int endColumnToTabOrder);

private:
	void resizeColumnsToPercentWidth();

	QMap<int, int> mColumnsPercentWidth;
	quint32 mBeginColumnToTabOrder;
	quint32 mEndColumnToTabOrder;


	// QWidget interface
protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *keyEvent);

	// QAbstractItemView interface
public:
	virtual void setModel(QAbstractItemModel *model);

	// QAbstractItemView interface
protected:
	virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
};


#endif // EXTREEVIEW_H
