#ifndef DICTIONARYTREEVIEW_H
#define DICTIONARYTREEVIEW_H

#include <QTreeView>

class DictionaryModel;

class DictionaryTreeView : public QTreeView
{
	Q_OBJECT

public:
	DictionaryTreeView(QWidget *parent = 0);
	~DictionaryTreeView();

	void loadFromFile(const QString &fileName);
	bool saveToFile(const QString &fileName);
	void newFile();

private:
	DictionaryModel *pModel;
};

#endif // DICTIONARYTREEVIEW_H
