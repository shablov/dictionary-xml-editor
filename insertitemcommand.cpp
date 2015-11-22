#include "insertitemcommand.h"
#include "dictionarymodel.h"

#include <QAbstractItemView>

InsertItemCommand::InsertItemCommand(QAbstractItemView *view, const QModelIndex &index, int type) :
	PasteItemCommand(view, index)
{
	if (pItem)
	{
		delete pItem;
	}
	pItem = new DictionaryItem(static_cast<DictionaryItem::ItemType>(type));
}

InsertItemCommand::~InsertItemCommand()
{
	delete pItem;
}
