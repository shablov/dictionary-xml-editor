#ifndef INSERTITEMCOMMAND_H
#define INSERTITEMCOMMAND_H

#include "cutpasteitemcommand.h"

class QAbstractItemView;
class DictionaryModel;
class DictionaryItem;

class InsertItemCommand : public PasteItemCommand
{
public:
	InsertItemCommand(QAbstractItemView *view, const QModelIndex &mIndex, int type);
	~InsertItemCommand();
};

#endif // INSERTITEMCOMMAND_H
