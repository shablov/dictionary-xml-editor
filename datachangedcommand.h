#ifndef DATACHANGEDCOMMAND_H
#define DATACHANGEDCOMMAND_H

#include "itemcommand.h"

#include <QModelIndex>

class MainWindow;
class DictionaryModel;

class DataChangedCommand : public ItemCommand
{
public:
	DataChangedCommand(QAbstractItemView *view, const QModelIndex &index);

	// QUndoCommand interface
public:
	void undo();
	void redo();

private:
	void changeData(const QVariant &value);

	QVariant mNewValue;
	int mRole;
	QVariant mOldValue;
};

#endif // DATACHANGEDCOMMAND_H
