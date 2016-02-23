#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QLineEdit>
#include <QMainWindow>
#include <QStyledItemDelegate>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>

#include "dictionarymodel.h"

class QLineEdit;
class QMenu;
class QActionGroup;
class QUndoStack;
class ExTreeView;
class DictionarySortFilterProxyModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);

private:
	void createAction();
	void updateRecentFileActions();
	void createFileActions();
	void createItemsActions();
	void createMoveActions();
	void createEditActions();
	void createSearchAction();

	void createMenuBar();
	void createFileMenu();
	void createEditMenu();
	void createAddMenu();

	void createToolBar();
	void createFileEditToolBar();
	void createFileTool(QToolBar *toolBar);
	void createEditTool(QToolBar *toolBar);
	void createItemsTool(QToolBar *toolBar);
	void createMoveTool(QToolBar *toolBar);
	void createIndexesToolBar();
	void createSearchTool(QToolBar *toolBar);
	void createFilterTool(QToolBar *toolBar);

	void createContextMenu();

	void createDictionaryView();

	void setFileName(const QString &fileName);
	void loadFile(const QString &fileName);
	bool saveToFile(const QString &fileName);

	bool isPossiblePlugIn(DictionaryItem::ItemType plugInType, DictionaryItem::ItemType indexType);

private slots:
	bool maybeSave();

	void onOpenFile();
	bool onSaveFile();
	bool onSaveAs();

	void leavePermittedActions();

private:
	QAction *actionNewFile;
	QAction *actionOpenFile;
	QAction *actionSaveFile;
	QAction *actionSaveAs;

	QMenu *menuAdd;
	QActionGroup *actionGroupAdd;
	QAction *actionRemove;
	QAction *actionUp;
	QAction *actionDown;
	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionPaste;
	QAction *actionUndo;
	QAction *actionRedo ;

	enum { MaxRecentFiles = 5 };
	QAction *recentFilesActions[MaxRecentFiles];

	QAction *actionSearch;
	QLineEdit *searchLineEdit;

	QMenu *contextMenu;

	QString mFileName;

	ExTreeView *pTreeView;
	DictionaryModel *sourceModel;
	DictionarySortFilterProxyModel *proxyModel;

	QUndoStack *pUndoStack;

	// QWidget interface
protected:
	virtual void closeEvent(QCloseEvent *event);
};

class LineDelegate : public QStyledItemDelegate
{


	// QAbstractItemDelegate interface
public:
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return new QLineEdit(parent);
	}
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		editor->setProperty("text", index.data(Qt::EditRole));
	}
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		qDebug() << Q_FUNC_INFO;
		model->setData(index, editor->property("text"));
	}

	// QAbstractItemDelegate interface
public:
	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
	{
		qDebug() << event;
		if (QFocusEvent *focusEvent = dynamic_cast<QFocusEvent*>(event))
		{
			if (focusEvent->reason() == Qt::ActiveWindowFocusReason)
			{
				focusEvent->ignore();
				return false;
			}
		}
		return QStyledItemDelegate::editorEvent(event, model, option, index);
	}

	// QObject interface
public:
	virtual bool eventFilter(QObject *object, QEvent *event)
	{
		static bool isSwitch = false;
		qDebug() << event;
		if (QInputMethodQueryEvent *keyEvent = dynamic_cast<QInputMethodQueryEvent*>(event))
		{
			if (keyEvent->queries() == 0x101)
			{
				isSwitch = true;
				qDebug() << 1 << isSwitch << keyEvent->value(Qt::ImEnabled) << keyEvent->value(Qt::ImHints);
			}
		}
		if (QFocusEvent *focusEvent = dynamic_cast<QFocusEvent*>(event))
		{
			qDebug() << 2 << isSwitch;
			if (focusEvent->reason() == Qt::ActiveWindowFocusReason && isSwitch && focusEvent->type() == QEvent::FocusOut)
			{
				focusEvent->ignore();
				isSwitch = false;
				return false;
			}
		}
		return QStyledItemDelegate::eventFilter(object, event);
	}
};

#endif // MAINWINDOW_H
