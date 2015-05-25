#ifndef REMOVEITEMCOMMAND_H
#define REMOVEITEMCOMMAND_H

#include "itemcommand.h"

class DictionaryItem;
class DictionaryModel;
class QAbstractItemView;


class RemoveItemCommand : public ItemCommand
{
public:
	RemoveItemCommand(QAbstractItemView *view, const QModelIndex &index);
	~RemoveItemCommand();

	// QUndoCommand interface
public:
	virtual void undo();
	virtual void redo();

private:
	DictionaryItem *pItem;
	bool isExpanded;
};

#endif // REMOVEITEMCOMMAND_H
