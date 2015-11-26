#ifndef DICTIONARYSORTFILTERPROXYMODEL_H
#define DICTIONARYSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "dictionaryitem.h"

class DictionarySortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
	Q_PROPERTY(QString filterText READ filterText WRITE setFilterText)
	Q_PROPERTY(DictionaryItem::ItemType filterType READ filterType WRITE setFilterType)
public:
	DictionarySortFilterProxyModel(QObject *parent = 0);
	QString filterText() const;
	DictionaryItem::ItemType filterType() const;

public slots:
	void setFilterText(QString filterText);
	void setFilterType(DictionaryItem::ItemType filterType);

private:
	QString mFilterText;
	DictionaryItem::ItemType mFilterType;

	// QSortFilterProxyModel interface
protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
	bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

#endif // DICTIONARYSORTFILTERPROXYMODEL_H
