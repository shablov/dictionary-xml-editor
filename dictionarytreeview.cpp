#include "dictionarymodel.h"
#include "dictionarytreeview.h"

DictionaryTreeView::DictionaryTreeView(QWidget *parent) : QTreeView(parent)
{
	pModel = new DictionaryModel(this);
	setModel(pModel);
}

DictionaryTreeView::~DictionaryTreeView()
{

}

void DictionaryTreeView::loadFromFile(const QString &fileName)
{
	pModel->load(fileName);
}

