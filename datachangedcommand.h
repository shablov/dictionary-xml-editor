#ifndef DATACHANGEDCOMMAND_H
#define DATACHANGEDCOMMAND_H

#include <QUndoCommand>
#include <QModelIndex>

class MainWindow;
class DictionaryModel;

class DataChangedCommand : public QUndoCommand
{
public:
	DataChangedCommand(QAbstractItemModel *model, const QModelIndex &index, MainWindow *w);

	// QUndoCommand interface
public:
	void undo();
	void redo();

private:
	void command(const QVariant &value);
	void reinitializeIndexes();

	DictionaryModel *pModel;
	MainWindow *w;
	QVariant mNewValue;
	QVariant mOldValue;
	int mRole;

	QModelIndex mIndex;
	QModelIndex mParentIndex;
	QModelIndex mGrandParentIndex;
	QModelIndex mRootParentIndex;
};

#endif // DATACHANGEDCOMMAND_H
