#ifndef EXTREEVIEW_H
#define EXTREEVIEW_H

#include <QTreeView>

class ExTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit ExTreeView(QWidget *parent = 0);

	void normalizeColumnsWidth();
	void setColumnPercentWidth(int col, int percent);
private:
	QMap<int, int> mColumnsPercentWidth;

	// QWidget interface
protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *keyEvent);
};


#endif // EXTREEVIEW_H
