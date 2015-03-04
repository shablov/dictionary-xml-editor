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
#include <QRadioButton>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <qevent.h>

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
	createContextMenu();
	createDictionaryView();

	leavePermittedActions();
}

void MainWindow::createAction()
{
	createFileActions();
	createItemsActions();
	createEditActions();
	createMoveActions();
	createSearchAction();
}

void MainWindow::createFileActions()
{
	actionNewFile = new QAction(QIcon(":images/new_file"), tr("New file"), this);
	actionOpenFile = new QAction(QIcon(":images/open_file"), tr("Open file"), this);
	actionSaveFile = new QAction(QIcon(":images/save_file"), tr("Save file"), this);
	actionSaveAs = new QAction(QIcon(":images/save_file"), tr("Save as ..."), this);

	actionNewFile->setIconVisibleInMenu(true);
	actionOpenFile->setIconVisibleInMenu(true);
	actionSaveFile->setIconVisibleInMenu(true);
	actionSaveAs->setIconVisibleInMenu(true);

	connect(actionNewFile, SIGNAL(triggered()), this, SLOT(onNewFile()));
	connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
	connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(onSaveFile()));
	connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));
}

void MainWindow::createItemsActions()
{
	actionGroupAdd = new QActionGroup(this);
	for (int i = 0; i < DictionaryItem::Invalid; i++)
	{
		QByteArray actionName = DictionaryItem::tagNameForType(static_cast<DictionaryItem::ItemType>(i));
		QAction *actionAdd =  new QAction(QIcon(":images/add_element"), tr("Add " + actionName), this);
		actionAdd->setIconVisibleInMenu(true);
		actionAdd->setData(i);
		actionAdd->setVisible(false);
		actionGroupAdd->addAction(actionAdd);
	}


	actionRemove = new QAction(QIcon(":images/remove_element"), tr("Remove"), this);
	actionRemove->setIconVisibleInMenu(true);
	actionRemove->setShortcut(QKeySequence::Delete);

	connect(actionRemove, SIGNAL(triggered()), this, SLOT(onRemove()));
	connect(actionGroupAdd, SIGNAL(triggered(QAction*)), this, SLOT(onAdd(QAction*)));
}

void MainWindow::createMoveActions()
{
	actionUp = new QAction(QIcon(":images/arrow_up"), tr("Up"), this);
	actionDown = new QAction(QIcon(":images/arrow_down"), tr("Down"), this);
	actionLeft = new QAction(QIcon(":images/arrow_left"), tr("Left"), this);
	actionRight = new QAction(QIcon(":images/arrow_right"), tr("Right"), this);

	actionUp->setIconVisibleInMenu(true);
	actionDown->setIconVisibleInMenu(true);
	actionLeft->setIconVisibleInMenu(true);
	actionRight->setIconVisibleInMenu(true);

	connect(actionUp, SIGNAL(triggered()), this, SLOT(onUp()));
	connect(actionDown, SIGNAL(triggered()), this, SLOT(onDown()));
	connect(actionLeft, SIGNAL(triggered()), this, SLOT(onLeft()));
	connect(actionRight, SIGNAL(triggered()), this, SLOT(onRight()));
}

void MainWindow::createEditActions()
{
	actionCut = new QAction(QIcon(":images/cut"), tr("Cut"), this);
	actionCopy = new QAction(QIcon(":images/copy"), tr("Copy"), this);
	actionPaste = new QAction(QIcon(":images/paste"), tr("Paste"), this);

	actionCut->setIconVisibleInMenu(true);
	actionCopy->setIconVisibleInMenu(true);
	actionPaste->setIconVisibleInMenu(true);

	connect(actionCut, SIGNAL(triggered()), this, SLOT(onCut()));
	connect(actionCopy, SIGNAL(triggered()), this, SLOT(onCopy()));
	connect(actionPaste, SIGNAL(triggered()), this, SLOT(onPaste()));

}

void MainWindow::createSearchAction()
{
	actionSearch = new QAction(QIcon(":images/search"), tr("Search"), this);
	actionSearch->setIconVisibleInMenu(true);
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
	createAddMenu();

	QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
	editMenu->addMenu(menuAdd);
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
}

void MainWindow::createAddMenu()
{
	menuAdd = new QMenu(tr("Add"), this);
	menuAdd->setIcon(QIcon(":images/add_element"));
	menuAdd->addActions(actionGroupAdd->actions());
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
	toolBar->addSeparator();
	createItemsTool(toolBar);
}

void MainWindow::createItemsTool(QToolBar *toolBar)
{
	/// TODO: styleSheet перенести в отдельный файл
	QToolButton *menuAddButton = new QToolButton(this);
	menuAddButton->setObjectName("menuAddButton");
	menuAddButton->setIcon(menuAdd->icon());
	menuAddButton->setPopupMode( QToolButton::InstantPopup);
	menuAddButton->setMenu(menuAdd);
	menuAddButton->setStyleSheet("QToolButton#menuAddButton{border-style: none}");

	toolBar->addWidget(menuAddButton);
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
	/// TODO: вынести stylesheet в отдельный файл
	searchLineEdit = new QLineEdit(toolBar);
	searchLineEdit->setPlaceholderText(tr("Quick search(Ctrl+F)"));
	searchLineEdit->setObjectName("searchLineEdit");
	toolBar->addWidget(searchLineEdit);
#if QT_VERSION >= 0x050300
	searchLineEdit->addAction(actionSearch, QLineEdit::TrailingPosition);
#else
	searchLineEdit->setStyleSheet(QString("QLineEdit#searchLineEdit"
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
	QRadioButton *onlyStringsCheckBox = new QRadioButton(tr("Only strings"));
	QRadioButton *onlyEnumsCheckBox = new QRadioButton(tr("Only enums"));
	QButtonGroup *checkBoxGroup = new QButtonGroup;
	checkBoxGroup->setExclusive(true);
	checkBoxGroup->addButton(onlyStringsCheckBox);
	checkBoxGroup->addButton(onlyEnumsCheckBox);
	connect(checkBoxGroup, SIGNAL(buttonClicked(int)), this, SLOT(onFilter()));

	QWidget *widget = new QWidget;
	widget->setLayout(new QVBoxLayout);
	widget->layout()->addWidget(onlyStringsCheckBox);
	widget->layout()->addWidget(onlyEnumsCheckBox);

	toolBar->addWidget(widget);
}

void MainWindow::createContextMenu()
{
	contextMenu = new QMenu;
	contextMenu->addAction(actionRemove);
	contextMenu->addMenu(menuAdd);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)),
			SLOT(onCustomContextMenuRequested(QPoint)));
}

void MainWindow::createDictionaryView()
{
	pModel = new DictionaryModel;
	connect(pModel, SIGNAL(error(DictionaryModel::ModelError,QString)),
			this, SLOT(onError(DictionaryModel::ModelError, QString)));

	ExTreeView *treeView = new ExTreeView(this);
	treeView->installEventFilter(this);
	treeView->setModel(pModel);
	treeView->normalizeColumnsWidth();
	connect(treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(leavePermittedActions(QModelIndex)));

	setCentralWidget(treeView);
}

bool MainWindow::maybeSave()
{
	if (!pModel->isModified())
	{
		return true;
	}
	int buttonRole = QMessageBox::information(this, "Dictionary is modified",
											  "Do you want to save the changes you made to Dictionary?",
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
	if ((!fileName.isEmpty()) && (pModel) && (pModel->load(fileName)))
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
	if (fileName.isEmpty())
	{
		return true;
	}
	return saveToFile(fileName);
}

void MainWindow::onAdd(QAction *action)
{
	DictionaryItem::ItemType type = static_cast<DictionaryItem::ItemType>(action->data().toUInt());
	ExTreeView *treeView = qobject_cast<ExTreeView*>(centralWidget());
	if (treeView)
	{
		pModel->insertDictionaryItem(type, treeView->currentIndex());
		leavePermittedActions(treeView->currentIndex());
	}
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
		leavePermittedActions(treeView->currentIndex());
	}
}

void MainWindow::leavePermittedActions(const QModelIndex &index)
{
	DictionaryItem::ItemType type = pModel->typeForIndex(index);

	actionGroupAdd->actions()[DictionaryItem::ContextType]->setVisible(
				type == DictionaryItem::ContextType ||
				type == DictionaryItem::Invalid);

	actionGroupAdd->actions()[DictionaryItem::StringType]->setVisible(
				type == DictionaryItem::ContextType ||
				type == DictionaryItem::StringType ||
				type == DictionaryItem::EnumType);

	actionGroupAdd->actions()[DictionaryItem::EnumType]->setVisible(
				type == DictionaryItem::ContextType ||
				type == DictionaryItem::StringType ||
				type == DictionaryItem::EnumType);

	actionGroupAdd->actions()[DictionaryItem::ArgType]->setVisible(
				type == DictionaryItem::EnumType ||
				type == DictionaryItem::ArgType);

	actionRemove->setDisabled(type == DictionaryItem::Invalid);
}

void MainWindow::onCustomContextMenuRequested(const QPoint &point)
{
	if (!contextMenu)
	{
		return;
	}

	ExTreeView *treeView = qobject_cast<ExTreeView*>(centralWidget());
	if (treeView)
	{
		QPoint globalPoint = mapToGlobal(point);
		leavePermittedActions(treeView->currentIndex());
		contextMenu->exec(globalPoint);
	}
}

void MainWindow::onError(DictionaryModel::ModelError, const QString &description)
{
	QErrorMessage *message = QErrorMessage::qtHandler();
	message->showMessage(description);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	event->setAccepted(maybeSave());
}
