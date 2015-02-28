#include "dictionaryitem.h"
#include "dictionarymodel.h"

#include <QDomElement>

#include <QFile>

#include <QDebug>

DictionaryModel::DictionaryModel(QObject *parent) :
	QAbstractItemModel(parent), rootItem(0)
{
}

DictionaryModel::~DictionaryModel()
{
	delete rootItem;
}

void DictionaryModel::load(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QDomDocument doc("dictionary");
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement rootElement = doc.documentElement();
	rootItem = new DictionaryItem(rootElement, 0);
	reset();
}

bool DictionaryModel::save(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly) || !rootItem)
	{
		return false;
	}
	QDomDocument doc = rootItem->toDomDocument();
	QByteArray data = doc.toByteArray();
	file.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" + data);
	return true;
}

void DictionaryModel::clear()
{
	delete rootItem;
	rootItem = 0;
	reset();
}

DictionaryItem *DictionaryModel::itemForIndex(const QModelIndex &index) const
{
	if (index.isValid())
	{
		DictionaryItem *item = static_cast<DictionaryItem*>(index.internalPointer());
		if (item)
		{
			return item;
		}
	}
	return rootItem;
}


QModelIndex DictionaryModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!rootItem || row < 0 || column < 0 || column >= ColumnCount
			|| (parent.isValid() && parent.column() != 0))
	{
		return QModelIndex();
	}
	DictionaryItem *parentItem = itemForIndex(parent);
	Q_ASSERT(parentItem);
	if (DictionaryItem *item = parentItem->childAt(row))
	{
		return createIndex(row, column, item);
	}
	return QModelIndex();
}

QModelIndex DictionaryModel::parent(const QModelIndex &child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}
	DictionaryItem *childItem = itemForIndex(child);
	if (childItem)
	{
		DictionaryItem *parentItem = childItem->parent();
		if (parentItem)
		{
			if (parentItem == rootItem)
			{
				return QModelIndex();
			}
			DictionaryItem *grandParentItem = parentItem->parent();
			if (grandParentItem)
			{
				int row = grandParentItem->rowOfChild(parentItem);
				return createIndex(row, 0, parentItem);
			}
		}
	}
	return QModelIndex();
}

int DictionaryModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
	{
		return 0;
	}
	DictionaryItem *parentItem = itemForIndex(parent);
	return parentItem ? parentItem->childCount() : 0;
}

int DictionaryModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() && parent.column() != 0 ? 0 : ColumnCount;
}

QVariant DictionaryModel::data(const QModelIndex &index, int role) const
{
	if (!rootItem || !index.isValid() || index.column() < 0 ||
			index.column() >= ColumnCount)
	{
		return QVariant();
	}
	DictionaryItem *item = itemForIndex(index);
	if (item)
	{
		if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
		{
			switch (index.column())
			{
			case EnglishColumn:
				return item->englishName();
			case RussiaColumn:
				return item->russiaName();
			}
		}

		if (role == Qt::TextAlignmentRole)
		{
			return static_cast<int>(Qt::AlignCenter);
		}

		if ((role == Qt::DecorationRole) && (index.column() == EnglishColumn))
		{
			switch (item->type())
			{
			case DictionaryItem::ContextType:
			{
				QPixmap pixmap(":images/context");
				return pixmap.scaledToHeight(16, Qt::SmoothTransformation);
			}
			case DictionaryItem::ArgType:
			{
				QPixmap pixmap(":images/argument");
				return pixmap.scaledToHeight(16, Qt::SmoothTransformation);
			}
			case DictionaryItem::EnumType:
			{
				QPixmap pixmap(":images/enum");
				return pixmap.scaledToHeight(16, Qt::SmoothTransformation);
			}
			case DictionaryItem::StringType:
			{
				QPixmap pixmap(":images/string");
				return pixmap.scaledToHeight(16, Qt::SmoothTransformation);
			}
			}
		}
	}
	return QVariant();
}

bool DictionaryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if ((!index.isValid()) || (role != Qt::EditRole))
	{
		return false;
	}
	DictionaryItem *item = itemForIndex(index);
	if (item)
	{
		switch (static_cast<Columns>(index.column()))
		{
			case DictionaryModel::EnglishColumn:
			{
				item->setEnglishName(value.toString());
				break;
			}
			case DictionaryModel::RussiaColumn:
			{
				item->setRussiaName(value.toString());
				break;
			}
		default:
			return false;
		}
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

QVariant DictionaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole))
	{
		return QVariant();
	}
	switch (section)
	{
	case EnglishColumn:
		return QString(tr("English"));
	case RussiaColumn:
		return QString(tr("Russia"));
	default:
		return QVariant();
	}
}


Qt::ItemFlags DictionaryModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
	if (index.isValid())
	{
		theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}
	return theFlags;
}
