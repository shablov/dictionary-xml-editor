#include "dictionarymodel.h"

#include <QPixmap>
#include <QFile>

#include <QDomElement>
#include <QMimeData>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

DictionaryModel::DictionaryModel(QObject *parent) :
	QAbstractItemModel(parent), pRootItem(0), pCutItem(0),
	isUndoStop(true)
{
	createNew();
}

DictionaryModel::~DictionaryModel()
{
	delete pRootItem;
}

bool DictionaryModel::load(const QString &fileName)
{
	beginResetModel();
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
	emit modified(false);
	endResetModel();
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
	file.close();
	emit modified(false);
	return true;
}

void DictionaryModel::clear()
{
	beginResetModel();
	delete pRootItem;
	pRootItem = 0;
	endResetModel();
}

void DictionaryModel::createNew()
{
	if (pRootItem)
	{
		clear();
	}
	pRootItem = new DictionaryItem;
	emit modified(false);
}

bool DictionaryModel::validate(const QByteArray &data)
{
	QFile file(":/files/xsd_schema");
	if (!file.open(QIODevice::ReadOnly))
	{
		emit error(XsdSchemaOpenedError, file.errorString());
		return false;
	}
	QByteArray fileContent = file.readAll();
	file.close();
	QXmlSchema schema;
	if (!schema.load(fileContent))
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

DictionaryItem::ItemType DictionaryModel::typeForIndex(const QModelIndex &index) const
{
	DictionaryItem *item = itemForIndex(index);
	if (item && item != pRootItem)
	{
		return item->type();
	}
	return DictionaryItem::Invalid;
}

QModelIndex DictionaryModel::insertDictionaryItem(DictionaryItem::ItemType type, const QModelIndex &index)
{
	DictionaryItem *item = new DictionaryItem(type);
	QModelIndex insertedIndex = insertItem(item, index);
	if (!insertedIndex.isValid())
	{
		delete item;
		item = 0;
	}
	return insertedIndex;
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
	emit modified(true);
	endRemoveRows();
}

DictionaryItem *DictionaryModel::cuttedItem()
{
	return pCutItem;
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

QModelIndex DictionaryModel::pasteItem(const QModelIndex &index)
{
	DictionaryItem *item = new DictionaryItem(pCutItem);
	QModelIndex insertedIndex = insertItem(item, index);
	if (!insertedIndex.isValid())
	{
		delete item;
		item = 0;
	}
	return insertedIndex;
}

QModelIndex DictionaryModel::upItem(int itemRow, const QModelIndex &parent)
{
	DictionaryItem *parentItem = itemForIndex(parent);
	if (parentItem && (itemRow > 0))
	{
		beginMoveRows(parent, itemRow, itemRow,
					  parent, itemRow - 1);
		parentItem->moveChild(itemRow, itemRow - 1);
		endMoveRows();
		--itemRow;
	}
	emit modified(true);
	return index(itemRow, 0, parent);
}

QModelIndex DictionaryModel::downItem(int itemRow, const QModelIndex &parent)
{
	DictionaryItem *parentItem = itemForIndex(parent);
	if (parentItem && (itemRow < parentItem->childCount()))
	{
		beginMoveRows(parent, itemRow, itemRow,
					  parent, itemRow + 2);
		parentItem->moveChild(itemRow, itemRow + 1);
		endMoveRows();
		++itemRow;
	}
	emit modified(true);
	return index(itemRow, 0, parent);
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

QModelIndex DictionaryModel::insertItem(DictionaryItem *itemForInsert, const QModelIndex &index)
{
	if (!pRootItem)
	{
		return QModelIndex();
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
			return QModelIndex();
	}
	int row = parentItem->rowOfChild(item);
	row = (row == -1) ? parentItem->childCount() : (row + 1);
	parentItem->insertChild(row, itemForInsert);
	insertRow(row, parentIndex);
	return this->index(row, 0, parentIndex);
}

void DictionaryModel::startUndo()
{
	isUndoStop = false;
}

void DictionaryModel::stopUndo()
{
	isUndoStop = true;
}

QVariant DictionaryModel::modifiedData() const
{
	return mModifiedData;
}

int DictionaryModel::modifiedRole() const
{
	return mModifiedRole;
}

QModelIndex DictionaryModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!pRootItem || row < 0 || column < 0 || column >= ColumnCount
			|| (parent.isValid() && parent.column() != 0))
	{
		return QModelIndex();
	}
	DictionaryItem *parentItem = itemForIndex(parent);
	Q_ASSERT_X(parentItem != 0, Q_FUNC_INFO, "not parent");
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

int DictionaryModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
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
	mModifiedData = index.data(role);
	mModifiedRole = role;
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
		if (isUndoStop)
		{
			emit modifiedData(index);
		}
		emit dataChanged(index, index);
		emit modified(true);
	}
	return false;
}

QVariant DictionaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole))
	{
		return QAbstractItemModel::headerData(section, orientation, role);
	}
	switch (section)
	{
		case PixmapColumn: return QString();
		case EnglishColumn: return QString(tr("English"));
		case RussiaColumn: return QString(tr("Russian"));
		default: return QAbstractItemModel::headerData(section, orientation, role);
	}
}

QStringList DictionaryModel::mimeTypes() const
{
	QStringList types;
	types << "dictionary/dictionaryitem.dat.bin";
	return types;
}

QMimeData *DictionaryModel::mimeData(const QModelIndexList &indexes) const
{
	QByteArray encodedData;
	QDataStream in(&encodedData, QIODevice::WriteOnly);
	foreach (QModelIndex index, indexes)
	{
		if (index.column() != 0)
		{
			continue;
		}
		DictionaryItem *item = itemForIndex(index);
		if (item == pRootItem)
		{
			continue;
		}
		in << item;
	}

	QMimeData *mimeData = new QMimeData();
	mimeData->setData("dictionary/dictionaryitem.dat.bin", encodedData);
	return mimeData;
}

bool DictionaryModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(row);
	Q_UNUSED(column);
	Q_UNUSED(parent);
	if (!data->hasFormat("dictionary/dictionaryitem.dat.bin"))
	{
		return false;
	}
	if (action != Qt::IgnoreAction &&
		action != Qt::MoveAction)
	{
		return false;
	}
	return true;
}

bool DictionaryModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (!canDropMimeData(data, action, row, column, parent))
	{
		return false;
	}
	if (action == Qt::IgnoreAction)
	{
		return true;
	}

	DictionaryItem *parentItem = itemForIndex(parent);
	QByteArray encodedData = data->data("dictionary/dictionaryitem.dat.bin");
	QDataStream out(encodedData);
	while (!out.atEnd())
	{
		DictionaryItem *item = new DictionaryItem;
		out >> item;
		if (item->type() == parentItem->type())
		{
			return false;
		}
		if (parentItem->type() == DictionaryItem::Invalid &&
			item->type() != DictionaryItem::ContextType)
		{
			return false;
		}

		parentItem->insertChild(row, item);
		insertRow(row, parent);
	}
	return true;
}

Qt::DropActions DictionaryModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::DropActions DictionaryModel::supportedDragActions() const
{
	return Qt::MoveAction;
}

bool DictionaryModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (!pRootItem)
	{
		return false;
	}
	beginInsertRows(parent, row, row + count - 1);
	endInsertRows();
	emit modified(true);
	return true;
}

bool DictionaryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (!pRootItem)
	{
		return false;
	}
	DictionaryItem *item = itemForIndex(parent);
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
	emit modified(true);
	return true;
}

Qt::ItemFlags DictionaryModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
	DictionaryItem *item = itemForIndex(index);
	if (item == pRootItem)
	{
		return theFlags | Qt::ItemIsDropEnabled;
	}
	if (item->type() != DictionaryItem::StringType &&
		item->type() != DictionaryItem::ArgType)
	{
		theFlags |= Qt::ItemIsDropEnabled;
	}
	theFlags |= Qt::ItemIsDragEnabled;
	theFlags |= Qt::ItemIsSelectable;
	if (index.column() != PixmapColumn ||
		item->type() == DictionaryItem::ArgType)
	{
		theFlags |= Qt::ItemIsEditable;
	}
	return theFlags;
}
