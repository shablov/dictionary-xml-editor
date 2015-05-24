#ifndef REMOVEITEMCOMMAND_H
#define REMOVEITEMCOMMAND_H

#include <QUndoCommand>
#include <QModelIndex>

class DictionaryItem;
class DictionaryModel;
class QAbstractItemView;


class RemoveItemCommand : public QUndoCommand
{
public:
	RemoveItemCommand(QAbstractItemView *view, QModelIndex &index);
	~RemoveItemCommand();

	// QUndoCommand interface
public:
	virtual void undo();
	virtual void redo();

private:
	void reinitializeIndexes();

	QAbstractItemView *pView;
	DictionaryModel *pModel;
	DictionaryItem *pItem;
	QModelIndex mIndex;
	QModelIndex mParentIndex;
	QModelIndex mGrandParentIndex;
	QModelIndex mRootParentIndex;
};

#endif // REMOVEITEMCOMMAND_H
