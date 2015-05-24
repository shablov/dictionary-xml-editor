#ifndef INSERTITEMCOMMAND_H
#define INSERTITEMCOMMAND_H

#include <QUndoCommand>
#include <QModelIndex>

class QAbstractItemView;
class DictionaryModel;
class DictionaryItem;

class InsertItemCommand : public QUndoCommand
{
public:
	InsertItemCommand(QAbstractItemView *view, int type, const QModelIndex &mIndex);
	~InsertItemCommand();
	// QUndoCommand interface
public:
	void undo();
	void redo();

private:
	void reinitializeIndexes();

	QAbstractItemView *view;
	DictionaryModel *pModel;
	DictionaryItem *pItem;
	QModelIndex mIndex;
	QModelIndex mParentIndex;
	QModelIndex mGrandParentIndex;
	QModelIndex mRootParentIndex;
	QModelIndex insertedIndex;
};

#endif // INSERTITEMCOMMAND_H
