#include "iconproxystyle.h"

#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>

IconProxyStyle::IconProxyStyle(QStyle *style) : QProxyStyle(style)
{
}

QIcon IconProxyStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
{
	switch (standardIcon)
	{
		case QStyle::SP_MessageBoxWarning:
		{
			return QIcon(colorizedPixmap(QPixmap(":/images/warning")));
		}
		case QStyle::SP_DialogCancelButton:
		{
			return QIcon(colorizedPixmap(QPixmap(":/images/cancel")));
		}
		case QStyle::SP_DialogSaveButton:
		{
			return QIcon(colorizedPixmap(QPixmap(":/images/save_file")));
		}
		case QStyle::SP_DialogDiscardButton:
		{
			return QIcon(colorizedPixmap(QPixmap(":/icons/material/discard_changes.svg")));
		}
		default: break;
	}
	return QProxyStyle::standardIcon(standardIcon, option, widget);
}

QPixmap IconProxyStyle::colorizedPixmap(const QPixmap &pixmap, QColor color) const
{
	QImage image = pixmap.toImage().alphaChannel();
	if (color == Qt::transparent)
	{
		color.setRgb(qrand() % 256, qrand() % 256, qrand() % 256);
	}

	for (int i = 0; i < image.colorCount(); i++)
	{
		color.setAlpha(qGray(image.color(i)));
		image.setColor(i, color.rgba());
	}

	return QPixmap::fromImage(image);
}
