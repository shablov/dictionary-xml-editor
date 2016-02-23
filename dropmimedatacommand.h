#ifndef DROPMIMEDATACOMMAND_H
#define DROPMIMEDATACOMMAND_H

#include "itemcommand.h"

class DropMimeDataCommand : public ItemCommand
{
public:
	DropMimeDataCommand(QAbstractItemView *view, const QByteArray &data, const int &row, const QModelIndex &proxyParentIndex);

	// QUndoCommand interface
public:
	virtual void undo();
	virtual void redo();

private:
	QByteArray mEncodingData;
	int mRow;
};

#endif // DROPMIMEDATACOMMAND_H
