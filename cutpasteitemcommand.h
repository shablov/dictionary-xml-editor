#ifndef CUTITEMCOMMAND_H
#define CUTITEMCOMMAND_H

#include "itemcommand.h"

class CutItemCommand : public ItemCommand
{
public:
	CutItemCommand(QAbstractItemView *view, const QModelIndex &index);
	virtual ~CutItemCommand();
	// QUndoCommand interface
public:
	virtual void undo();
	virtual void redo();

private:
	DictionaryItem *pCutItem;
};

class PasteItemCommand : public ItemCommand
{
public:
	PasteItemCommand(QAbstractItemView *view, const QModelIndex &index);
	virtual ~PasteItemCommand();
	// QUndoCommand interface
public:
	virtual void undo();
	virtual void redo();

private:
	DictionaryItem *pPasteItem;

	// ItemCommand interface
protected:
	virtual void reinitializeIndexes();
};

#endif // CUTITEMCOMMAND_H
