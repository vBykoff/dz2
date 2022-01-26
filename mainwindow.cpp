#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mQModelIndex(QModelIndex())
{
    setWindowTitle(tr("Обозреватель XML-файлов"));
    setMinimumSize(600, 400);
    resize(600,400);

    mXMLParser = new XMLParser;
    mTree = new QTreeView;
    mTree->setModel(mXMLParser);
    setCentralWidget(mTree);

    QMenu* FileMenu = new QMenu(tr("Файл"));

    QAction* OpenFileAction = new QAction(tr("Открыть"), this);
    QAction* CloseAllFilesAction = new QAction(tr("Закрыть все"), this);
    QAction* ExitAction = new QAction(tr("Выход"), this);

    FileMenu->addAction(OpenFileAction);
    FileMenu->addAction(CloseAllFilesAction);
    FileMenu->addAction(ExitAction);

    menuBar()->addMenu(FileMenu);
    mTree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(mTree, &QTreeView::customContextMenuRequested, this, &MainWindow::ActiveAction);

    connect(ExitAction         , &QAction::triggered, this, &QMainWindow::close);
    connect(OpenFileAction     , &QAction::triggered, this, &MainWindow::openFile);
    connect(CloseAllFilesAction, &QAction::triggered, this, &MainWindow::closeAllFiles);
}

MainWindow::~MainWindow()
{
    delete mXMLParser;
    delete mTree;
}

void MainWindow::closeAllFiles()
{
    mXMLParser->delete_rows();
    mTree->reset();
}

void MainWindow::openFile()
{
    QString name = QFileDialog::getOpenFileName(this, tr("Выбор XML файла"),
                                                QDir::homePath(), tr("XML file (*.xml)"));
    mXMLParser->parsing(name);
    mTree->reset();

    QFont font;
    font.setBold(true);
    mXMLParser->setData(mXMLParser->index(0, 0), font, Qt::FontRole);
}

void MainWindow::makeActive()
{
    QModelIndex currentIndex = mTree->currentIndex();
    QFont font;

    font.setBold(true);
    mXMLParser->setData(currentIndex, font, Qt::FontRole);
    mQModelIndex = currentIndex;
}

void MainWindow::closeFile()
{
    if (!mXMLParser->index(1, 0).isValid())
    {
        close();
    }

    QModelIndex currentIndex = mTree->currentIndex();
    QFont font;
    font.setBold(true);
    mXMLParser->setData(currentIndex, font, Qt::FontRole);

    int i;
    for (i = 0; mXMLParser->index(i, 0) != mQModelIndex; ++i) {}

    int curI;
    for (curI = 0; mXMLParser->index(curI, 0) != currentIndex; ++curI) {}

    mXMLParser->delete_row();
    mXMLParser->setData(mXMLParser->index((i > curI) ? i - 1 : ((i < curI) ? i : 0), 0), font, Qt::FontRole);
}

void MainWindow::ActiveAction(const QPoint& pPosition) noexcept
{
    QModelIndex currentIndex = mTree->currentIndex();

    if (currentIndex.parent() == QModelIndex() && mTree->indexAt(pPosition).isValid())
    {
        QMenu* active_menu = new QMenu(this);

        QAction* makeActiveAction = new QAction(tr("Сделать активным"), this);
        QAction* closeFileAction  = new QAction(tr("Закрыть"), this);

        active_menu->addAction(makeActiveAction);
        active_menu->addAction(closeFileAction);

        connect(makeActiveAction, &QAction::triggered, this, &MainWindow::makeActive);
        connect(closeFileAction , &QAction::triggered, this, &MainWindow::closeFile);

        active_menu->popup(mTree->viewport()->mapToGlobal(pPosition));
    }
}
