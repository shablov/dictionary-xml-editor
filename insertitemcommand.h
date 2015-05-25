#ifndef INSERTITEMCOMMAND_H
#define INSERTITEMCOMMAND_H

#include "itemcommand.h"

class QAbstractItemView;
class DictionaryModel;
class DictionaryItem;

class InsertItemCommand : public ItemCommand
{
public:
	InsertItemCommand(QAbstractItemView *view, const QModelIndex &mIndex, int type);
	~InsertItemCommand();
	// QUndoCommand interface
public:
	void undo();
	void redo();

private:
	DictionaryItem *pItem;
	QModelIndex insertedIndex;

	// ItemCommand interface
protected:
	virtual void reinitializeIndexes();
};

#endif // INSERTITEMCOMMAND_H
