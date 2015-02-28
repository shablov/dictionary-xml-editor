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

bool DictionaryTreeView::saveToFile(const QString &fileName)
{
	return pModel->save(fileName);
}

void DictionaryTreeView::newFile()
{
	pModel->clear();
}

