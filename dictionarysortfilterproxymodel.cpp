#include "dictionarysortfilterproxymodel.h"

#include "dictionarymodel.h"

DictionarySortFilterProxyModel::DictionarySortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent), mFilterType(DictionaryItem::Invalid)
{
}

QString DictionarySortFilterProxyModel::filterText() const
{
	return mFilterText;
}

DictionaryItem::ItemType DictionarySortFilterProxyModel::filterType() const
{
	return mFilterType;
}

void DictionarySortFilterProxyModel::setFilterText(QString filterText)
{
	if (mFilterText == filterText)
		return;

	mFilterText = filterText;
	invalidateFilter();
}

void DictionarySortFilterProxyModel::setFilterType(DictionaryItem::ItemType filterType)
{
	if (mFilterType == filterType)
		return;

	mFilterType = filterType;
	invalidateFilter();
}

bool DictionarySortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if (!source_parent.isValid())
	{
		return true;
	}
	QModelIndex child = source_parent.child(source_row, DictionaryModel::PixmapColumn);
	DictionaryItem::ItemType type = static_cast<DictionaryItem::ItemType>(child.data(Qt::UserRole).toInt());
	if ((type == DictionaryItem::StringType || type == DictionaryItem::EnumType) &&
		(mFilterType != DictionaryItem::Invalid && type != mFilterType))
	{
		return false;
	}

	if (mFilterText.isEmpty())
	{
		return true;
	}
	QModelIndex index = child.sibling(child.row(), DictionaryModel::RussiaColumn);
	QString text = index.data(Qt::DisplayRole).toString();
	bool containsRussianText = text.contains(mFilterText, Qt::CaseInsensitive);
	if (containsRussianText)
	{
		return true;
	}
	switch (type)
	{
		case DictionaryItem::EnumType:
		case DictionaryItem::ContextType:
		{
			for (int i = 0; i < sourceModel()->rowCount(child); ++i)
			{
				if (filterAcceptsRow(i, child))
				{
					return true;
				}
			}
		}
		case DictionaryItem::StringType:
		case DictionaryItem::ArgType: return false;
		case DictionaryItem::Invalid:
		default: return true;
	}
}

bool DictionarySortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
	if ((source_right.column() == DictionaryModel::PixmapColumn) &&
		(source_left.column() == DictionaryModel::PixmapColumn) &&
		!source_left.data(Qt::DisplayRole).isValid())
	{
		return source_left.data(Qt::UserRole).toInt() > source_right.data(Qt::UserRole).toInt();
	}
	return QSortFilterProxyModel::lessThan(source_left, source_right);
}

