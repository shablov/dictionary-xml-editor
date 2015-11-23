#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dictionarymodel.h"

class QLineEdit;
class QMenu;
class QActionGroup;
class QUndoStack;

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
	void onOpenRecentFile();
	void onNewFile();
	bool onSaveFile();
	bool onSaveAs();

	void onAdd(QAction *action);
	void onRemove();
	void onUp();
	void onDown();
	void onCut();
	void onCopy();
	void onPaste();

	void onCustomContextMenuRequested(const QPoint &point);
	void leavePermittedActions();

	void onError(DictionaryModel::ModelError, const QString &description);

	void onCleanChanged(bool clean);
	void onModifiedData(const QModelIndex& index);

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

	DictionaryModel *sourceModel;
	DictionarySortFilterProxyModel *proxyModel;

	QUndoStack *pUndoStack;

	// QWidget interface
protected:
	virtual void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
