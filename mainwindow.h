#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <QTreeView>
#include "xmlparser.h"

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:

    void ActiveAction(const QPoint&) noexcept;
    void openFile();
    void closeAllFiles();
    void makeActive();
    void closeFile();

private:

    XMLParser* mXMLParser;
    QTreeView* mTree;
    QModelIndex mQModelIndex;

};
#endif // MAINWINDOW_H
