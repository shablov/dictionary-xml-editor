#include "headerview.h"

#include <QPainter>

HeaderView::HeaderView(Qt::Orientation orientation, bool wrap, QWidget *parent) :
	QHeaderView(orientation, parent)
{
	wrapping = wrap;
	setSectionsClickable(true);
	setStretchLastSection(true);
}

void HeaderView::setWordWrap(bool wrap)
{
	wrapping = wrap;
}

bool HeaderView::wordWrap() const
{
	return wrapping;
}

void HeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	if(wrapping)
	{
		if (!rect.isValid())
		{
			return;
		}

		// get the state of the section
		QStyleOptionHeader opt;
		initStyleOption(&opt);
		QStyle::State state = QStyle::State_None;
		if (isEnabled())
		{
			state |= QStyle::State_Enabled;
		}
		if (window()->isActiveWindow())
		{
			state |= QStyle::State_Active;
		}


		if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
		{
			opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
								? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;
		}

		// setup the style options structure
		QVariant textAlignment = model()->headerData(logicalIndex, orientation(),
													 Qt::TextAlignmentRole);
		opt.rect = rect;
		opt.section = logicalIndex;
		opt.state |= state;
		opt.textAlignment = Qt::Alignment(textAlignment.isValid()
										  ? Qt::Alignment(textAlignment.toInt())
										  : defaultAlignment());

		opt.iconAlignment = Qt::AlignVCenter;
		opt.text = model()->headerData(logicalIndex, orientation(),
									   Qt::DisplayRole).toString();
		if (textElideMode() != Qt::ElideNone)
		{
			opt.text = opt.fontMetrics.elidedText(opt.text, textElideMode() , rect.width() - 4);
		}

		QVariant variant = model()->headerData(logicalIndex, orientation(),
											   Qt::DecorationRole);
		opt.icon = qvariant_cast<QIcon>(variant);
		if (opt.icon.isNull())
		{
			opt.icon = qvariant_cast<QPixmap>(variant);
		}
		QVariant foregroundBrush = model()->headerData(logicalIndex, orientation(),
													   Qt::ForegroundRole);
		if (foregroundBrush.canConvert(QMetaType::QBrush))
		{
			opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
		}

		QPointF oldBO = painter->brushOrigin();
		QVariant backgroundBrush = model()->headerData(logicalIndex, orientation(),
													   Qt::BackgroundRole);
		if (backgroundBrush.canConvert(QMetaType::QBrush))
		{
			opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
			opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
			painter->setBrushOrigin(opt.rect.topLeft());
		}

		// the section position
		int visual = visualIndex(logicalIndex);
		Q_ASSERT(visual != -1);
		if (count() == 1)
		{
			opt.position = QStyleOptionHeader::OnlyOneSection;
		}
		else if (visual == 0)
		{
			opt.position = QStyleOptionHeader::Beginning;
		}
		else if (visual == count() - 1)
		{
			opt.position = QStyleOptionHeader::End;
		}
		else
		{
			opt.position = QStyleOptionHeader::Middle;
		}
		opt.orientation = orientation();

		// draw the section
		style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
		// draw the text
		QRect headerLabelRect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
		style()->drawItemText(painter, headerLabelRect, Qt::TextWordWrap | Qt::AlignCenter, style()->standardPalette(), true, opt.text);

		// draw the indicators if need
		if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
		{
			opt.rect = style()->subElementRect(QStyle::SE_HeaderArrow, &opt, this);
			style()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &opt, painter, this);
		}

		painter->setBrushOrigin(oldBO);
	}
	else
	{
		QHeaderView::paintSection(painter, rect, logicalIndex);
	}
}

QSize HeaderView::sectionSizeFromContents(int logicalIndex) const
{
	if(wrapping)
	{
		int width = sectionSize(logicalIndex);
		int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this) *2;

		QString s = model()->headerData(logicalIndex, Qt::Horizontal).toString();

		QFont font = this->font();
		font.setBold(true);

		QFontMetrics fm(font);
		QSize size = fm.boundingRect(0, 0, width, 2000, Qt::TextWordWrap, s).size();
		size.rwidth() += margin;
		size.rheight() += margin;

		return size;
	}

	return QHeaderView::sectionSizeFromContents(logicalIndex);
}
