#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class DictionaryModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

private:
	void createAction();
	void createMenuBar();
	void createFileMenu();
	void createEditMenu();

	void createToolBar();
	void createFileEditToolBar();
	void createFileTool(QToolBar *toolBar);
	void createEditTool(QToolBar *toolBar);
	void createElementTool(QToolBar *toolBar);
	void createMoveTool(QToolBar *toolBar);
	void createIndexesToolBar();
	void createSearchTool(QToolBar *toolBar);
	void createSortTool(QToolBar *toolBar);
	void createDictionaryView();

	void setFileName(const QString &fileName);
	void saveToFile(const QString &fileName);

private slots:
	bool maybeSave();
	void onOpenFile();

	void onNewFile();

	void onSaveFile();
	void onSaveAs();


private:
	QAction *actionNewFile;
	QAction *actionOpenFile;
	QAction *actionSaveFile;
	QAction *actionSaveAs;

	QAction *actionAdd;
	QAction *actionRemove;

	QAction *actionUp;
	QAction *actionDown;
	QAction *actionLeft;
	QAction *actionRight;

	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionPaste;
	QAction *actionEdit;

	QAction *actionSearch;

	QLineEdit *searchLineEdit;

	QString mFileName;

	DictionaryModel *pModel;
};

#endif // MAINWINDOW_H
