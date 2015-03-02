#include "mainwindow.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QErrorMessage>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

#include "extreeview.h"
#include "headerview.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("Dictionary editor"));
	setWindowIcon(QIcon(":images/dictionary"));
	setMinimumWidth(520);
	setMinimumHeight(400);

	createAction();
	createMenuBar();
	createToolBar();
	createDictionaryView();
}

void MainWindow::createAction()
{
	actionNewFile = new QAction(QIcon(":images/new_file"), tr("New file"), this);
	actionOpenFile = new QAction(QIcon(":images/open_file"), tr("Open file"), this);
	actionSaveFile = new QAction(QIcon(":images/save_file"), tr("Save file"), this);
	actionSaveAs = new QAction(QIcon(":images/save_file"), tr("Save as ..."), this);

	actionAdd = new QAction(QIcon(":images/add_element"), tr("Add"), this);
	actionRemove = new QAction(QIcon(":images/remove_element"), tr("Remove"), this);
	actionRemove->setShortcut(QKeySequence::Delete);

	actionUp = new QAction(QIcon(":images/arrow_up"), tr("Up"), this);
	actionDown = new QAction(QIcon(":images/arrow_down"), tr("Down"), this);
	actionLeft = new QAction(QIcon(":images/arrow_left"), tr("Left"), this);
	actionRight = new QAction(QIcon(":images/arrow_right"), tr("Right"), this);

	actionCut = new QAction(QIcon(":images/cut"), tr("Cut"), this);
	actionCopy = new QAction(QIcon(":images/copy"), tr("Copy"), this);
	actionPaste = new QAction(QIcon(":images/paste"), tr("Paste"), this);
	actionEdit = new QAction(QIcon(":images/edit"), tr("Edit"), this);

	actionSearch = new QAction(QIcon(":images/search"), tr("Search"), this);

	actionNewFile->setIconVisibleInMenu(true);
	actionOpenFile->setIconVisibleInMenu(true);
	actionSaveFile->setIconVisibleInMenu(true);
	actionSaveAs->setIconVisibleInMenu(true);

	actionAdd->setIconVisibleInMenu(true);
	actionRemove->setIconVisibleInMenu(true);

	actionUp->setIconVisibleInMenu(true);
	actionDown->setIconVisibleInMenu(true);
	actionLeft->setIconVisibleInMenu(true);
	actionRight->setIconVisibleInMenu(true);

	actionCut->setIconVisibleInMenu(true);
	actionCopy->setIconVisibleInMenu(true);
	actionPaste->setIconVisibleInMenu(true);
	actionEdit->setIconVisibleInMenu(true);

	actionSearch->setIconVisibleInMenu(true);

	connect(actionNewFile, SIGNAL(triggered()), this, SLOT(onNewFile()));
	connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
	connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(onSaveFile()));
	connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));

	connect(actionAdd, SIGNAL(triggered()), this, SLOT(onAdd()));
	connect(actionRemove, SIGNAL(triggered()), this, SLOT(onRemove()));

	connect(actionUp, SIGNAL(triggered()), this, SLOT(onUp()));
	connect(actionDown, SIGNAL(triggered()), this, SLOT(onDown()));
	connect(actionLeft, SIGNAL(triggered()), this, SLOT(onLeft()));
	connect(actionRight, SIGNAL(triggered()), this, SLOT(onRight()));

	connect(actionCut, SIGNAL(triggered()), this, SLOT(onCut()));
	connect(actionCopy, SIGNAL(triggered()), this, SLOT(onCopy()));
	connect(actionPaste, SIGNAL(triggered()), this, SLOT(onPaste()));
	connect(actionEdit, SIGNAL(triggered()), this, SLOT(onEdit()));

	actionSearch->setShortcut(QKeySequence::Find);
}

void MainWindow::createMenuBar()
{
	createFileMenu();
	createEditMenu();
}

void MainWindow::createFileMenu()
{
	QMenu *fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addAction(actionNewFile);
	fileMenu->addAction(actionOpenFile);
	fileMenu->addAction(actionSaveFile);
	fileMenu->addAction(actionSaveAs);
	fileMenu->addSeparator();
	QAction *actionExit = fileMenu->addAction(QIcon(":images/exit"), tr("Exit"));
	actionExit->setIconVisibleInMenu(true);
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createEditMenu()
{
	QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
	editMenu->addAction(actionAdd);
	editMenu->addAction(actionRemove);

	QMenu *moveMenu = editMenu->addMenu(QIcon(":images/drag_arrow"), tr("Move"));
	moveMenu->addAction(actionUp);
	moveMenu->addAction(actionDown);
	moveMenu->addAction(actionLeft);
	moveMenu->addAction(actionRight);

	editMenu->addSeparator();
	editMenu->addAction(actionCut);
	editMenu->addAction(actionCopy);
	editMenu->addAction(actionPaste);
	editMenu->addAction(actionEdit);

}

void MainWindow::createToolBar()
{
	createFileEditToolBar();
	addToolBarBreak(Qt::TopToolBarArea);
	createIndexesToolBar();
}

void MainWindow::createFileEditToolBar()
{
	QToolBar *toolBar = new QToolBar(tr("File && Edit"));
	addToolBar(Qt::TopToolBarArea, toolBar);
	createFileTool(toolBar);
	toolBar->addSeparator();
	createEditTool(toolBar);
	toolBar->addSeparator();
	createElementTool(toolBar);
	toolBar->addSeparator();
	createMoveTool(toolBar);
}

void MainWindow::createFileTool(QToolBar *toolBar)
{
	toolBar->addAction(actionNewFile);
	toolBar->addAction(actionOpenFile);
	toolBar->addAction(actionSaveFile);
}

void MainWindow::createEditTool(QToolBar *toolBar)
{
	toolBar->addAction(actionCut);
	toolBar->addAction(actionCopy);
	toolBar->addAction(actionPaste);
	toolBar->addAction(actionEdit);
}

void MainWindow::createElementTool(QToolBar *toolBar)
{
	toolBar->addAction(actionAdd);
	toolBar->addAction(actionRemove);
}

void MainWindow::createMoveTool(QToolBar *toolBar)
{
	toolBar->addAction(actionUp);
	toolBar->addAction(actionDown);
	toolBar->addAction(actionLeft);
	toolBar->addAction(actionRight);
}

void MainWindow::createIndexesToolBar()
{
	QToolBar *toolBar = new QToolBar(tr("Search && Filter"));
	addToolBar(Qt::TopToolBarArea, toolBar);
	createSearchTool(toolBar);
	toolBar->addSeparator();
	createFilterTool(toolBar);
}

void MainWindow::createSearchTool(QToolBar *toolBar)
{
	searchLineEdit = new QLineEdit();
	searchLineEdit->setPlaceholderText(tr("Quick search(Ctrl+F)"));
	toolBar->addWidget(searchLineEdit);
    qDebug() << qVersion();
#if QT_VERSION >= 0x050300
	searchLineEdit->addAction(actionSearch, QLineEdit::TrailingPosition);
#else
	searchLineEdit->setStyleSheet(QString("QLineEdit"
										  "{"
										  "border: 1px solid #000000;"
										  "image: url(:/images/search);"
										  "image-position: right;"
										  "}"));

	searchLineEdit->addAction(actionSearch);
#endif
	connect(actionSearch, SIGNAL(triggered()), searchLineEdit, SLOT(setFocus()));
	connect(searchLineEdit, SIGNAL(textEdited(QString)), this, SLOT(onSearch(QString)));
}

void MainWindow::createFilterTool(QToolBar *toolBar)
{
	QCheckBox *onlyStringsCheckBox = new QCheckBox(tr("Only strings"));
	QCheckBox *onlyEnumsCheckBox = new QCheckBox(tr("Only enums"));

	QWidget *widget = new QWidget;
	widget->setLayout(new QVBoxLayout);
	widget->layout()->addWidget(onlyStringsCheckBox);
	widget->layout()->addWidget(onlyEnumsCheckBox);

	toolBar->addWidget(widget);

	QButtonGroup *checkBoxGroup = new QButtonGroup;
	checkBoxGroup->setExclusive(true);
	checkBoxGroup->addButton(onlyStringsCheckBox);
	checkBoxGroup->addButton(onlyEnumsCheckBox);
	connect(checkBoxGroup, SIGNAL(buttonClicked(int)), this, SLOT(onFilter()));
}

void MainWindow::createDictionaryView()
{
	ExTreeView *treeView = new ExTreeView(this);
	pModel = new DictionaryModel;
	connect(pModel, SIGNAL(error(DictionaryModel::ModelError,QString)),
			this, SLOT(onError(DictionaryModel::ModelError, QString)));
	treeView->setModel(pModel);
	treeView->normalizeColumnsWidth();
	setCentralWidget(treeView);
}

bool MainWindow::maybeSave()
{
	///TODO: диалог о сохранении текущего словаря
	if (!pModel->isModified())
	{
		return true;
	}
	int buttonRole = QMessageBox::information(this, "Dictionary is modified",
							 "Do you want to save the chamges you made to Dictionary?",
							 QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
	switch (buttonRole)
	{
		case QMessageBox::Yes:
		{
			return onSaveFile();
		}
		case QMessageBox::No:
		{
			return true;
		}
		default:
		{
			return false;
		}
	}
}

void MainWindow::setFileName(const QString &fileName)
{
	mFileName = fileName;
}

void MainWindow::onOpenFile()
{
	if (!maybeSave())
	{
		return;
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
													QDir::currentPath(), tr("Xml (*.xml)"));
	if ((pModel) && (pModel->load(fileName)))
	{
		setFileName(fileName);
	}
}

void MainWindow::onNewFile()
{
	if (!maybeSave())
	{
		return;
	}

	if (pModel)
	{
		pModel->createNew();
		setFileName(QString());
	}
}

bool MainWindow::saveToFile(const QString &fileName)
{
	if ((pModel) && (pModel->save(fileName)))
	{
		setFileName(fileName);
		return true;
	}
	return false;
}

bool MainWindow::onSaveFile()
{
	if (mFileName.isEmpty())
	{
		 return onSaveAs();
	}
	else
	{
		 return saveToFile(mFileName);
	}
}

bool MainWindow::onSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
													QDir::currentPath(), tr("Xml (*.xml)"));
	return saveToFile(fileName);
}

void MainWindow::onAdd()
{
}

void MainWindow::onRemove()
{
	ExTreeView *treeView = qobject_cast<ExTreeView*>(centralWidget());
	if (treeView)
	{
		QModelIndex currentIndex = treeView->currentIndex();
		if (currentIndex.isValid())
		{
			pModel->removeRow(currentIndex.row(), currentIndex.parent());
		}
	}
}

void MainWindow::onError(DictionaryModel::ModelError, const QString &description)
{
	QErrorMessage *message = QErrorMessage::qtHandler();
	message->showMessage(description);
}
