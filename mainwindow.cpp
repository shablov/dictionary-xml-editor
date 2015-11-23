#include "mainwindow.h"

#include <QButtonGroup>
#include <QErrorMessage>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QRadioButton>
#include <QToolBar>
#include <QToolButton>
#include <QLayout>
#include <QCloseEvent>
#include <QUndoStack>
#include <QSettings>

#include "cutpasteitemcommand.h"
#include "datachangedcommand.h"
#include "dictionarysortfilterproxymodel.h"
#include "extreeview.h"
#include "headerview.h"
#include "insertitemcommand.h"
#include "removeitemcommand.h"
#include "updownitemcommand.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	pUndoStack = new QUndoStack(this);

	setWindowTitle(tr("Dictionary editor[*]"));
	setWindowIcon(QIcon(":images/dictionary"));
	setMinimumWidth(600);
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

	for (int i = 0; i < MaxRecentFiles; ++i)
	{
		recentFilesActions[i] = new QAction(this);
		recentFilesActions[i]->setVisible(false);
		connect(recentFilesActions[i], &QAction::triggered, [=] () {
			if (maybeSave())
			{
				loadFile(recentFilesActions[i]->data().toString());
			}
		});
	}
	updateRecentFileActions();

	actionNewFile->setShortcut(QKeySequence::New);
	actionOpenFile->setShortcut(QKeySequence::Open);
	actionSaveFile->setShortcut(QKeySequence::Save);
	actionSaveAs->setShortcut(QKeySequence::SaveAs);

	actionNewFile->setIconVisibleInMenu(true);
	actionOpenFile->setIconVisibleInMenu(true);
	actionSaveFile->setIconVisibleInMenu(true);
	actionSaveAs->setIconVisibleInMenu(true);

	actionNewFile->setToolTip(tr("Create new file"));
	actionOpenFile->setToolTip(tr("Open file"));
	actionSaveFile->setToolTip(tr("Save file"));
	actionSaveAs->setToolTip(tr("Save file as ..."));

	connect(actionNewFile, &QAction::triggered, [=] () {
		if (maybeSave())
		{
			sourceModel->createNew();
			setFileName(QString());
			pUndoStack->clear();
		}
	});
	connect(actionOpenFile, &QAction::triggered, [=] () {
		if (maybeSave())
		{
			QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
															QDir::currentPath(), tr("Xml (*.xml)"));
			loadFile(fileName);
		}
	});
	connect(actionSaveFile, &QAction::triggered, this, &MainWindow::onSaveFile);
	connect(actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAs);
}

void MainWindow::createItemsActions()
{
	actionGroupAdd = new QActionGroup(this);
	for (int i = 0; i < DictionaryItem::Invalid; i++)
	{
		QString actionName = DictionaryItem::ruTagNameForType(static_cast<DictionaryItem::ItemType>(i));
		QAction *actionAdd =  new QAction(QIcon(":images/add_element"), tr("Add") + " " + actionName, this);
		actionAdd->setIconVisibleInMenu(true);
		actionAdd->setData(i);
		actionAdd->setVisible(false);
		actionGroupAdd->addAction(actionAdd);
	}

	actionRemove = new QAction(QIcon(":images/remove_element"), tr("Remove"), this);
	actionRemove->setIconVisibleInMenu(true);
	actionRemove->setShortcut(QKeySequence::Delete);

	connect(actionRemove, &QAction::triggered, this, [=] () {
		if (pTreeView->currentIndex().isValid())
		{
			pUndoStack->push(new RemoveItemCommand(pTreeView, pTreeView->currentIndex()));
		}
	});
	connect(actionGroupAdd, &QActionGroup::triggered, this, [=] (QAction *action) {
		DictionaryItem::ItemType type = static_cast<DictionaryItem::ItemType>(action->data().toUInt());
		pUndoStack->push(new InsertItemCommand(pTreeView, pTreeView->currentIndex(), type));
	});
}

void MainWindow::createMoveActions()
{
	actionUp = new QAction(QIcon(":images/arrow_up"), tr("Up"), this);
	actionDown = new QAction(QIcon(":images/arrow_down"), tr("Down"), this);

	actionUp->setIconVisibleInMenu(true);
	actionDown->setIconVisibleInMenu(true);

	actionUp->setToolTip(tr("Move item up"));
	actionDown->setToolTip(tr("Move item down"));

	connect(actionUp, &QAction::triggered, this, [=] () {
		if (pTreeView->currentIndex().isValid())
		{
			pUndoStack->push(new UpDownItemCommand(pTreeView, pTreeView->currentIndex(), UpDownItemCommand::UpMove));
		}
	});
	connect(actionDown, &QAction::triggered, this, [=] () {
		if (pTreeView->currentIndex().isValid())
		{
			pUndoStack->push(new UpDownItemCommand(pTreeView, pTreeView->currentIndex(), UpDownItemCommand::DownMove));
		}
	});
}

void MainWindow::createEditActions()
{
	actionUndo = pUndoStack->createUndoAction(this, tr("Undo"));
	actionUndo->setIcon(QIcon(":images/undo"));
	actionRedo = pUndoStack->createRedoAction(this, tr("Redo"));
	actionRedo->setIcon(QIcon(":images/redo"));
	actionCut = new QAction(QIcon(":images/cut"), tr("Cut"), this);
	actionCopy = new QAction(QIcon(":images/copy"), tr("Copy"), this);
	actionPaste = new QAction(QIcon(":images/paste"), tr("Paste"), this);

	actionCut->setShortcut(QKeySequence::Cut);
	actionCopy->setShortcut(QKeySequence::Copy);
	actionPaste->setShortcut(QKeySequence::Paste);
	actionUndo->setShortcut(QKeySequence::Undo);
	actionRedo->setShortcut(QKeySequence::Redo);

	actionCut->setIconVisibleInMenu(true);
	actionCopy->setIconVisibleInMenu(true);
	actionPaste->setIconVisibleInMenu(true);
	actionUndo->setIconVisibleInMenu(true);
	actionRedo->setIconVisibleInMenu(true);

	connect(actionCut, &QAction::triggered, this, [=] () {
		if (pTreeView->currentIndex().isValid())
		{
			pUndoStack->push(new CutItemCommand(pTreeView, pTreeView->currentIndex()));
		}
	});
	connect(actionCopy, &QAction::triggered, this, [=] () {
		QModelIndex currentIndex = pTreeView->currentIndex();
		if (currentIndex.isValid())
		{
			sourceModel->copyItem(currentIndex.row(), currentIndex.parent());
		}
	});
	connect(actionPaste, &QAction::triggered, this, [=] () {
		if (pTreeView->currentIndex().isValid())
		{
			pUndoStack->push(new PasteItemCommand(pTreeView, pTreeView->currentIndex()));
		}
	});

	actionCut->setDisabled(true);
	actionCopy->setDisabled(true);
	actionPaste->setDisabled(true);

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
	QMenu *recentFilesMenu = fileMenu->addMenu(tr("Recent files..."));
	for (int i = 0; i < MaxRecentFiles; i++)
	{
		recentFilesMenu->addAction(recentFilesActions[i]);
	}
	fileMenu->addSeparator();
	QAction *actionExit = fileMenu->addAction(QIcon(":images/exit"), tr("Exit"));
	actionExit->setIconVisibleInMenu(true);
	connect(actionExit, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::updateRecentFileActions()
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
	for (int i = 0; i < numRecentFiles; ++i)
	{
		QString text = QString("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		recentFilesActions[i]->setText(text);
		recentFilesActions[i]->setData(files[i]);
		recentFilesActions[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
	{
		recentFilesActions[j]->setVisible(false);
	}

}

void MainWindow::createEditMenu()
{
	createAddMenu();

	QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
	editMenu->addAction(actionUndo);
	editMenu->addAction(actionRedo);
	editMenu->addSeparator();

	editMenu->addMenu(menuAdd);
	editMenu->addAction(actionRemove);

	QMenu *moveMenu = editMenu->addMenu(QIcon(":images/drag_arrow"), tr("Move"));
	moveMenu->addAction(actionUp);
	moveMenu->addAction(actionDown);

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
	toolBar->addAction(actionUndo);
	toolBar->addAction(actionRedo);
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
	searchLineEdit = new QLineEdit(toolBar);
	searchLineEdit->setPlaceholderText(tr("Quick search(Ctrl+F)"));
	searchLineEdit->setObjectName("searchLineEdit");
	toolBar->addWidget(searchLineEdit);
	searchLineEdit->addAction(actionSearch, QLineEdit::TrailingPosition);
	connect(actionSearch, &QAction::triggered,
			searchLineEdit, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));
	connect(searchLineEdit, &QLineEdit::textChanged, this, [=] (const QString &text) {
		proxyModel->setFilterText(text);
	});
}

void MainWindow::createFilterTool(QToolBar *toolBar)
{
	QRadioButton *onlyStringsCheckBox = new QRadioButton(tr("Only strings"));
	QRadioButton *onlyEnumsCheckBox = new QRadioButton(tr("Only enums"));
	QRadioButton *allCheckBox = new QRadioButton(tr("All"));
	QButtonGroup *checkBoxGroup = new QButtonGroup;
	checkBoxGroup->setExclusive(true);
	checkBoxGroup->addButton(onlyStringsCheckBox, DictionaryItem::StringType);
	checkBoxGroup->addButton(onlyEnumsCheckBox, DictionaryItem::EnumType);
	checkBoxGroup->addButton(allCheckBox, DictionaryItem::Invalid);
	connect(checkBoxGroup, static_cast<void (QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), this, [=] (int button, bool checked) {
		if (checked)
		{
			proxyModel->setFilterType(static_cast<DictionaryItem::ItemType>(button));
		}
	});

	QWidget *widget = new QWidget;
	widget->setLayout(new QVBoxLayout);
	widget->layout()->addWidget(onlyStringsCheckBox);
	widget->layout()->addWidget(onlyEnumsCheckBox);
	widget->layout()->addWidget(allCheckBox);

	toolBar->addWidget(widget);
}

void MainWindow::createContextMenu()
{
	contextMenu = new QMenu;
	contextMenu->addAction(actionRemove);
	contextMenu->addMenu(menuAdd);
	contextMenu->addAction(actionCut);
	contextMenu->addAction(actionCopy);
	contextMenu->addAction(actionPaste);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &MainWindow::customContextMenuRequested, [=] (const QPoint &point) {
		QPoint globalPoint = mapToGlobal(point);
		contextMenu->exec(globalPoint);
	});
}

void MainWindow::createDictionaryView()
{
	sourceModel = new DictionaryModel;
	connect(sourceModel, &DictionaryModel::error, this, [=] (DictionaryModel::ModelError, const QString &description) {
		QErrorMessage::qtHandler()->showMessage(description);
	});
	connect(pUndoStack, &QUndoStack::cleanChanged, this, [=] (bool clean) {
		setWindowModified(!clean);
	});
	connect(sourceModel, static_cast<void (DictionaryModel::*)(const QModelIndex &)>(&DictionaryModel::modifiedData),
			this, [=] (const QModelIndex &index) {
		pUndoStack->push(new DataChangedCommand(pTreeView, proxyModel->mapFromSource(index)));
	});

	proxyModel = new DictionarySortFilterProxyModel(this);
	proxyModel->setSourceModel(sourceModel);
	proxyModel->setDynamicSortFilter(true);

	pTreeView = new ExTreeView(this);
	pTreeView->installEventFilter(this);
	pTreeView->setModel(proxyModel);
	pTreeView->setSortingEnabled(true);
	pTreeView->setColumnPercentWidth(DictionaryModel::PixmapColumn, 30);
	pTreeView->setColumnPercentWidth(DictionaryModel::EnglishColumn, 35);
	pTreeView->setColumnPercentWidth(DictionaryModel::RussiaColumn, 35);

	/// Drag'n'Drop
	pTreeView->setDragEnabled(true);
	pTreeView->setDropIndicatorShown(true);
	pTreeView->setAcceptDrops(true);

	connect(pTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(leavePermittedActions()));
	connect(sourceModel, SIGNAL(modified(bool)), this, SLOT(leavePermittedActions()));

	setCentralWidget(pTreeView);
}

bool MainWindow::maybeSave()
{
	if (!isWindowModified())
	{
		return true;
	}
	int buttonRole = QMessageBox::information(this, tr("Dictionary is modified"),
											  tr("Do you want to save the changes you made to Dictionary?"),
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
	if (mFileName.isEmpty())
	{
		return;
	}

	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	files.removeAll(mFileName);
	files.prepend(mFileName);
	while (files.size() > MaxRecentFiles)
	{
		files.removeLast();
	}
	settings.setValue("recentFileList", files);
	updateRecentFileActions();
}

void MainWindow::loadFile(const QString &fileName)
{
	if ((!fileName.isEmpty()) && (sourceModel) && (sourceModel->load(fileName)))
	{
		setFileName(fileName);
		pUndoStack->clear();
	}
}

void MainWindow::onOpenFile()
{
	if (!maybeSave())
	{
		return;
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
													QDir::currentPath(), tr("Xml (*.xml)"));
	loadFile(fileName);
}

bool MainWindow::onSaveFile()
{
	return mFileName.isEmpty() ? onSaveAs() : saveToFile(mFileName);
}

bool MainWindow::onSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
													QDir::currentPath(), tr("Xml (*.xml)"));
	return !fileName.isEmpty() ? saveToFile(fileName) : true;
}

bool MainWindow::saveToFile(const QString &fileName)
{
	if ((sourceModel) && (sourceModel->save(fileName)))
	{
		setFileName(fileName);
		pUndoStack->clear();
		return true;
	}
	return false;
}

void MainWindow::leavePermittedActions()
{
	QModelIndex index = proxyModel->mapToSource(pTreeView->currentIndex());
	DictionaryItem::ItemType indexType = sourceModel->typeForIndex(index);

	actionGroupAdd->actions()[DictionaryItem::ContextType]->setVisible(
				isPossiblePlugIn(DictionaryItem::ContextType, indexType));

	actionGroupAdd->actions()[DictionaryItem::StringType]->setVisible(
				isPossiblePlugIn(DictionaryItem::StringType, indexType));

	actionGroupAdd->actions()[DictionaryItem::EnumType]->setVisible(
				isPossiblePlugIn(DictionaryItem::EnumType, indexType));

	actionGroupAdd->actions()[DictionaryItem::ArgType]->setVisible(
				isPossiblePlugIn(DictionaryItem::ArgType, indexType));

	actionRemove->setDisabled(indexType == DictionaryItem::Invalid);
	actionCut->setDisabled(indexType == DictionaryItem::Invalid);
	actionCopy->setDisabled(indexType == DictionaryItem::Invalid);
	DictionaryItem::ItemType cutItemType = sourceModel->typeForCutItem();
	actionPaste->setEnabled(isPossiblePlugIn(cutItemType, indexType));

	actionUp->setEnabled(index.row() > 0);
	actionDown->setEnabled(index.sibling(index.row() + 1, 0).isValid());
}

bool MainWindow::isPossiblePlugIn(DictionaryItem::ItemType plugInType, DictionaryItem::ItemType indexType)
{
	if (plugInType == DictionaryItem::ContextType)
	{
		return (indexType == DictionaryItem::Invalid ||
				indexType == DictionaryItem::ContextType);
	}
	if (plugInType == DictionaryItem::StringType ||
		plugInType == DictionaryItem::EnumType)
	{
		return (indexType == DictionaryItem::ContextType ||
				indexType == DictionaryItem::StringType ||
				indexType == DictionaryItem::EnumType);
	}
	if (plugInType == DictionaryItem::ArgType)
	{
		return (indexType == DictionaryItem::EnumType ||
				indexType == DictionaryItem::ArgType);
	}
	return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	event->setAccepted(maybeSave());
}
