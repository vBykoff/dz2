#ifndef XMLMODEL_H
#define XMLMODEL_H

#include <QAbstractItemModel>
#include <QFile>
#include <QXmlStreamReader>
#include <QStack>
#include <QMessageBox>

class XMLParser : public QAbstractItemModel {

    Q_OBJECT

public:
    explicit XMLParser(QObject *parent = nullptr);
    ~XMLParser();

    QVariant data(const QModelIndex& index, int) const override;
    QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex&, const QVariant&, int role = Qt::EditRole) override;
    bool removeRows(int, int, const QModelIndex &parent = QModelIndex()) override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex&) const override;
    QModelIndex index(int, int, const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex&) const override;

    void parsing(const QString&) const noexcept;
    void delete_row() noexcept;
    void delete_rows() noexcept;

private:

    struct Tree {

        Tree* parent;
        QVector<Tree*> child;
        QVector<QVariant> data;

        explicit Tree(const QVector<QVariant>& data, Tree* parent = nullptr) : parent(parent), data(data) {}
        ~Tree() {
            for (Tree* node : child) {
                parent = nullptr;
                delete node;
            }
        }

    };

    Tree* Root;
    QXmlStreamReader* XMLReader;
    QPersistentModelIndex PMI;
    QFont font;
};

#endif // XMLMODEL_H
