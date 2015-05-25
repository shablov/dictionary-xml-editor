#ifndef DATACHANGEDCOMMAND_H
#define DATACHANGEDCOMMAND_H

#include "itemcommand.h"

#include <QModelIndex>

class MainWindow;
class DictionaryModel;

class DataChangedCommand : public ItemCommand
{
public:
	DataChangedCommand(QAbstractItemView *view, const QModelIndex &index, MainWindow *w);

	// QUndoCommand interface
public:
	void undo();
	void redo();

private:
	void changeData(const QVariant &value);

	MainWindow *w;
	QVariant mNewValue;
	QVariant mOldValue;
	int mRole;
};

#endif // DATACHANGEDCOMMAND_H
