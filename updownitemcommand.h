#ifndef UPDOWNITEMCOMMAND_H
#define UPDOWNITEMCOMMAND_H

#include "itemcommand.h"

class UpDownItemCommand : public ItemCommand
{
public:
	enum MoveAction
	{
		UpMove,
		DownMove
	};

	UpDownItemCommand(QAbstractItemView *view, const QModelIndex &index, MoveAction action);

	// QUndoCommand interface
public:
	virtual void undo();
	virtual void redo();

private:
	void moveItem(bool isRedo);
	bool successMove;
	MoveAction action;

};
#endif // UPDOWNITEMCOMMAND_H
