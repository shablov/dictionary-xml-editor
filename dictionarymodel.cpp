#include "dictionaryitem.h"
#include "dictionarymodel.h"

#include <QDomElement>

#include <QFile>

#include <QDebug>

DictionaryModel::DictionaryModel(QObject *parent) : QAbstractItemModel(parent)
{
}

DictionaryModel::~DictionaryModel()
{

}

void DictionaryModel::load(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QDomDocument doc("dictionary");
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement rootElement = doc.documentElement();
	rootItem = new DictionaryItem(rootElement, 0);
}


QModelIndex DictionaryModel::index(int row, int column, const QModelIndex &parent) const
{
	return QModelIndex();
}

QModelIndex DictionaryModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}

int DictionaryModel::rowCount(const QModelIndex &parent) const
{
	return 0;
}

int DictionaryModel::columnCount(const QModelIndex &parent) const
{
	return 0;
}

QVariant DictionaryModel::data(const QModelIndex &index, int role) const
{
	return QVariant();
}

bool DictionaryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return false;
}

QVariant DictionaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

bool DictionaryModel::insertRows(int row, int count, const QModelIndex &parent)
{
	return false;
}

bool DictionaryModel::insertColumns(int column, int count, const QModelIndex &parent)
{
	return false;
}

bool DictionaryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	return false;
}

bool DictionaryModel::removeColumns(int column, int count, const QModelIndex &parent)
{
	return false;
}
