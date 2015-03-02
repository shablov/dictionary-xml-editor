#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>

class HeaderView : public QHeaderView
{
	Q_OBJECT
public:
	explicit HeaderView(Qt::Orientation orientation = Qt::Horizontal,
						bool wrap = true, QWidget *parent = 0);
	void setWordWrap(bool wrap);
	bool wordWrap() const;

private:
	bool wrapping;

protected:
	virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
	virtual QSize sectionSizeFromContents(int logicalIndex) const;
};

#endif // HEADERVIEW_H
