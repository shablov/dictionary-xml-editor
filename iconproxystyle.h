#ifndef ICONPROXYSTYLE_H
#define ICONPROXYSTYLE_H

#include <QProxyStyle>

class IconProxyStyle : public QProxyStyle
{
public:

	IconProxyStyle(QStyle *style);

	// QStyle interface
public:
	QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = 0, const QWidget *widget = 0) const;

private:
	QPixmap colorizedPixmap(const QPixmap &pixmap, QColor color = Qt::transparent) const;
};

#endif // ICONPROXYSTYLE_H
