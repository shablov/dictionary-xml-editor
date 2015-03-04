#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dictionarymodel.h"

class QLineEdit;
class QMenu;
class QActionGroup;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

private:
	void createAction();
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
	bool saveToFile(const QString &fileName);

private slots:
	bool maybeSave();
	void onOpenFile();

	void onNewFile();

	bool onSaveFile();
	bool onSaveAs();

	void onAdd(QAction *action);
	void onRemove();

	void leavePermittedActions(const QModelIndex &index = QModelIndex());
	void onCustomContextMenuRequested(const QPoint &point);


	void onError(DictionaryModel::ModelError, const QString &description);


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
	QAction *actionLeft;
	QAction *actionRight;

	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionPaste;

	QAction *actionSearch;

	QLineEdit *searchLineEdit;

	QMenu *contextMenu;

	QString mFileName;

	DictionaryModel *pModel;

	// QWidget interface
protected:
	virtual void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
