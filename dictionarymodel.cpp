#include "dictionaryitem.h"
#include "dictionarymodel.h"

#include <QDomElement>

#include <QFile>

#include <QDebug>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

DictionaryModel::DictionaryModel(QObject *parent) :
	QAbstractItemModel(parent), rootItem(0)
{
}

DictionaryModel::~DictionaryModel()
{
	delete rootItem;
}

bool DictionaryModel::load(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray data = file.readAll();
	file.close();

	if (!validate(data))
	{
		return false;
	}
	QDomDocument doc("dictionary");
	if (!doc.setContent(data))
	{
		return false;
	}

	QDomElement rootElement = doc.documentElement();
	rootItem = new DictionaryItem(rootElement, 0);
	reset();
	return true;
}

bool DictionaryModel::save(const QString &fileName)
{
	if (!rootItem)
	{
		return false;
	}
	QDomDocument doc = rootItem->toDomDocument();
	QByteArray data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" + doc.toByteArray();
	if (!validate(data))
	{
		return false;
	}
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		return false;
	}
	file.write(data);
	return true;
}

void DictionaryModel::clear()
{
	delete rootItem;
	rootItem = 0;
	reset();
}

void DictionaryModel::createNew()
{
	if (rootItem)
	{
		clear();
	}
	rootItem = new DictionaryItem;
}

bool DictionaryModel::validate(const QByteArray &data)
{
	QFile file(":/files/xsd_schema");
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Can't open schema file";
		/// TODO: сделать обработчик ошибок
		emit schemaNotExists();
		return false;
	}
	QXmlSchema schema;
	if (!schema.load(file.readAll()))
	{
		qDebug() << "Schema is invalid";
		/// TODO: сделать обработчик ошибок
		emit invalidSchema();
		return false;
	}

	QXmlSchemaValidator validator(schema);
	bool isValid = validator.validate(data);
	if (!isValid)
	{
		/// TODO: сделать обработчик ошибок
		emit invalidXml();
	}
	return isValid;
}

bool DictionaryModel::isModified()
{
	return false;
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

void DictionaryModel::reset()
{
#if QT_VERSION >= 050000
	beginResetModel();
	endResetModel();
#else
	QAbstractItemModel::reset();
#endif
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

bool DictionaryModel::insertRows(int row, int count, const QModelIndex &parent)
{
	return QAbstractItemModel::insertRows(row, count, parent);
}

bool DictionaryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	return QAbstractItemModel::removeRows(row, count, parent);
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
