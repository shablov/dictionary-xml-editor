#include "dictionarymodel.h"

#include <QPixmap>
#include <QFile>

#include <QDomElement>
#include <QMimeData>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

DictionaryModel::DictionaryModel(QObject *parent) :
	QAbstractItemModel(parent), pRootItem(0), pCutItem(0)
{
	createNew();
}

DictionaryModel::~DictionaryModel()
{
	delete pRootItem;
}

bool DictionaryModel::load(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		emit error(FileOpenedError, file.errorString());
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
		/// NOTE: возможно валидатор обрабатывает все ошибки
		return false;
	}

	QDomElement rootElement = doc.documentElement();
	pRootItem = new DictionaryItem(rootElement);
	reset();
	mIsModified = false;
	return true;
}

bool DictionaryModel::save(const QString &fileName)
{
	if (!pRootItem)
	{
		return false;
	}
	QDomDocument doc = pRootItem->toDomDocument();
	QByteArray data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" + doc.toByteArray();
	if (!validate(data))
	{
		return false;
	}
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		emit error(FileOpenedError, file.errorString());
		return false;
	}
	file.write(data);
	mIsModified = false;
	return true;
}

void DictionaryModel::clear()
{
	delete pRootItem;
	pRootItem = 0;
	reset();
}

void DictionaryModel::createNew()
{
	if (pRootItem)
	{
		clear();
	}
	pRootItem = new DictionaryItem;
	mIsModified = false;
}

bool DictionaryModel::validate(const QByteArray &data)
{
	QFile file(":/files/xsd_schema");
	if (!file.open(QIODevice::ReadOnly))
	{
		emit error(XsdSchemaOpenedError, file.errorString());
		return false;
	}
	QXmlSchema schema;
	if (!schema.load(file.readAll()))
	{
		emit error(InvalidXsdSchema, QString("Loading is failed"));
		return false;
	}

	QXmlSchemaValidator validator(schema);
	bool isValid = validator.validate(data);
	if (!isValid)
	{
		emit error(InvalidXmlFile, "Xml dictionary is not valid");
	}

	/// TODO: не сделана проверка на повторяющиеся теги
	return isValid;
}

bool DictionaryModel::isModified()
{
	return mIsModified;
}

DictionaryItem::ItemType DictionaryModel::typeForIndex(const QModelIndex &index) const
{
	DictionaryItem *item = itemForIndex(index);
	if (item)
	{
		return item->type();
	}
	return DictionaryItem::Invalid;
}

void DictionaryModel::insertDictionaryItem(DictionaryItem::ItemType type, const QModelIndex &index)
{
	DictionaryItem *item = new DictionaryItem(type);
	if (!insertItem(item, index))
	{
		delete item;
		item = 0;
	}
}

void DictionaryModel::cutItem(const int &row, const QModelIndex &parent)
{
	DictionaryItem *parentItem = itemForIndex(parent);
	beginRemoveRows(parent, row, row);
	if (pCutItem)
	{
		delete pCutItem;
		pCutItem = 0;
	}
	pCutItem = parentItem->takeChild(row);
	endRemoveRows();
}

void DictionaryModel::copyItem(const int &row, const QModelIndex &parent)
{

	DictionaryItem *parentItem = itemForIndex(parent);
	if (pCutItem)
	{
		delete pCutItem;
		pCutItem = 0;
	}
	pCutItem = new DictionaryItem(parentItem->childAt(row));
}

DictionaryItem::ItemType DictionaryModel::typeForCutItem()
{
	if (pCutItem)
	{
		return pCutItem->type();
	}
	return DictionaryItem::Invalid;
}

void DictionaryModel::pasteItem(const QModelIndex &index)
{
	DictionaryItem *item = new DictionaryItem(pCutItem);
	if (!insertItem(item, index))
	{
		delete item;
		item = 0;
	}
}

bool DictionaryModel::upItem(const int &itemRow, const QModelIndex &parent)
{
	DictionaryItem *parentItem = itemForIndex(parent);
	if (!parentItem || (itemRow <= 0))
	{
		return false;
	}
	parentItem->moveChild(itemRow, itemRow - 1);
	reset();
	return true;
}

bool DictionaryModel::downItem(const int &itemRow, const QModelIndex &parent)
{
	DictionaryItem *parentItem = itemForIndex(parent);
	if (!parentItem || (itemRow - 1 >= parentItem->childCount()))
	{
		return false;
	}
	parentItem->moveChild(itemRow, itemRow + 1);
	reset();
	return true;
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
	return pRootItem;
}

bool DictionaryModel::insertItem(DictionaryItem *itemForInsert, const QModelIndex &index)
{
	if (!pRootItem)
	{
		return false;
	}
	DictionaryItem *item = itemForIndex(index);
	DictionaryItem *parentItem;
	QModelIndex parentIndex;
	switch (itemForInsert->type())
	{
		case DictionaryItem::ContextType:
		{
			parentItem = pRootItem;
			parentIndex = parent(index);
			break;
		}
		case DictionaryItem::StringType:
		case DictionaryItem::EnumType:
		{
			parentItem = (item->parent() == pRootItem) ? (item) : (item->parent());
			parentIndex = (item->parent() == pRootItem) ? (index) : (parent(index));
			break;
		}
		case DictionaryItem::ArgType:
		{
			parentItem = (item->type() == DictionaryItem::EnumType) ? (item) : (item->parent());
			parentIndex = (item->type() == DictionaryItem::EnumType) ? (index) : (parent(index));
			break;
		}
		default:
			return false;
	}
	int row = parentItem->rowOfChild(item);
	row = (row == -1) ? parentItem->childCount() : (row + 1);
	parentItem->insertChild(row, itemForInsert);
	insertRow(row, parentIndex);
	mIsModified = true;
	emit dataChanged(QModelIndex(), QModelIndex());
	return true;
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
	if (!pRootItem || row < 0 || column < 0 || column >= ColumnCount
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
			if (parentItem == pRootItem)
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

int DictionaryModel::columnCount(const QModelIndex &) const
{
	return ColumnCount;
}

QVariant DictionaryModel::data(const QModelIndex &index, int role) const
{
	if (!pRootItem || !index.isValid() || index.column() < 0 ||
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
				case PixmapColumn: return item->type() == DictionaryItem::ArgType ? item->enumValue()
																				  : QVariant();
				case EnglishColumn: return item->englishName();
				case RussiaColumn: return item->russiaName();
			}
		}

		if (role == Qt::TextAlignmentRole)
		{
			return static_cast<int>(Qt::AlignLeft);
		}

		if ((role == Qt::DecorationRole) && (index.column() == PixmapColumn))
		{
			QPixmap pixmap = item->pixmap();
			return pixmap.scaledToHeight(16, Qt::SmoothTransformation);
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
			case DictionaryModel::PixmapColumn:
			{
				quint32 enumValue = value.toUInt();
				if (enumValue == item->enumValue())
				{
					return false;
				}
				item->setEnumValue(value.toUInt());
				break;
			}
			case DictionaryModel::EnglishColumn:
			{
				QString engName = value.toString();
				if (engName == item->englishName())
				{
					return false;
				}
				item->setEnglishName(value.toString());
				break;
			}
			case DictionaryModel::RussiaColumn:
			{
				QString rusName = value.toString();
				if (rusName == item->russiaName())
				{
					return false;
				}
				item->setRussiaName(value.toString());
				break;
			}
			default:
				return false;
		}
		emit dataChanged(index, index);
		return mIsModified = true;
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
		case PixmapColumn: return QString();
		case EnglishColumn: return QString(tr("English"));
		case RussiaColumn: return QString(tr("Russian"));
		default: return QVariant();
	}
}

QStringList DictionaryModel::mimeTypes() const
{
	return QAbstractItemModel::mimeTypes();
}

QMimeData *DictionaryModel::mimeData(const QModelIndexList &indexes) const
{
	return QAbstractItemModel::mimeData(indexes);
}

bool DictionaryModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
}

Qt::DropActions DictionaryModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool DictionaryModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (!pRootItem)
	{
		return false;
	}
	beginInsertRows(parent, row, row + count - 1);
	endInsertRows();
	return true;
}

bool DictionaryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (!pRootItem)
	{
		return false;
	}
	DictionaryItem *item = parent.isValid() ? itemForIndex(parent) : pRootItem;
	beginRemoveRows(parent, row, row + count - 1);
	for (int i = 0; i < count; ++i)
	{
		DictionaryItem *childItem = item->takeChild(row);
		if (childItem)
		{
			delete childItem;
		}
	}
	endRemoveRows();
	mIsModified = true;
	return true;
}

Qt::ItemFlags DictionaryModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
	theFlags |= Qt::ItemIsDropEnabled;
	if (index.isValid())
	{
		theFlags |= Qt::ItemIsDragEnabled;
		if ((index.column() != PixmapColumn) || (typeForIndex(index) == DictionaryItem::ArgType))
		{
			theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEditable;
		}
	}
	return theFlags;
}
