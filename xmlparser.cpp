#include "xmlparser.h"

XMLParser::XMLParser(QObject* parent)
    : QAbstractItemModel(parent)
    , Root(new Tree({tr("")}))
    , XMLReader(new QXmlStreamReader())
    , PMI()
    , font()
{}

XMLParser::~XMLParser()
{
    delete Root;
    delete XMLReader;
}

QVariant XMLParser::data(const QModelIndex& index, int role) const {
    if (index.isValid())
    {
        if (role == Qt::FontRole && index == PMI) {
            return font;
        }

        if (role != Qt::DisplayRole) {
            return QVariant();
        }

        Tree* item = static_cast<Tree*>(index.internalPointer());

        return item->data.at(index.column());
    } else
        return QVariant();
}

QVariant XMLParser::headerData(int section, Qt::Orientation orientation, int role) const {
    return (orientation == Qt::Horizontal && role == Qt::DisplayRole) ? Root->data.at(section) : QVariant();
}

bool XMLParser::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid()) {
        Tree* item = static_cast<Tree*>(index.internalPointer());
        if (role == Qt::EditRole) {
            item->data[index.column()] = value;
            return true;
        }

        if (role == Qt::FontRole) {
            PMI = index;
            font = value.value<QFont>();
            return true;
        }
    }

    return false;
}

bool XMLParser::removeRows(int row, int count, const QModelIndex &parent) {
    Tree* node;
    QModelIndex Parent;
    if (parent.isValid()) {
        node = static_cast<Tree*>(parent.internalPointer());
        Parent = parent;
    } else {
        node = Root;
        Parent = QModelIndex();
    }
    beginRemoveRows(Parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        delete node->child[row];
        node->child.remove(row);
    }
    endRemoveRows();

    return true;
}

int XMLParser::rowCount(const QModelIndex& parent) const {
    Tree* Parent;
    if (parent.column() <= 0) {

        if (!parent.isValid())
            Parent = Root;
        else
            Parent = static_cast<Tree*>(parent.internalPointer());

        return Parent->child.size();   
    } else
        return 0;
}

int XMLParser::columnCount(const QModelIndex& parent) const {
    return Root->data.size();
}

QModelIndex XMLParser::parent(const QModelIndex& index) const {
    Tree* Child = static_cast<Tree*>(index.internalPointer());
    Tree* Parent = Child->parent;
    return (Parent != Root && Child != Root) ? createIndex((Parent->parent) ? Parent->parent->child.indexOf(Parent) : 0, 0, Parent) : QModelIndex();
}

QModelIndex XMLParser::index(int row, int column, const QModelIndex& parent) const {
    Tree* Parent;
    if (!parent.isValid())
        Parent = Root;
    else
        Parent = static_cast<Tree*>(parent.internalPointer());

    Tree* Child;
    if (row < 0 || row >= Parent->child.size())
        Child = nullptr;
    else
        Child = Parent->child.at(row);

    if (Child)
        return createIndex(row, column, Child);

    return QModelIndex();
}

Qt::ItemFlags XMLParser::flags(const QModelIndex& index) const {
    return (index.isValid()) ? QAbstractItemModel::flags(index) : Qt::NoItemFlags;
}

void XMLParser::parsing(const QString& str) const noexcept {
    QFile file(str);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QStack<int> Crab;
    QStack<int> Lime;

    XMLReader->setDevice(&file);
    QString name = str.section("/", -1, -1);
    Tree* node = new Tree{{name}, Root};
    Root->child.append(node);
    Tree* cur = node;

    int curLime = 0;
    while (!XMLReader->atEnd() && !XMLReader->hasError()) {
        QXmlStreamReader::TokenType token = XMLReader->readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString str = XMLReader->name().toString();

            ++curLime;
            if (Lime.isEmpty() || curLime != Lime.top()) {
                Crab.append(0);
                Lime.append(curLime);
            }

            Tree* node = new Tree({str}, cur);
            cur->child.append(node);
            cur = node;
        }

        if (token == QXmlStreamReader::Characters) {
            Tree* node = new Tree({XMLReader->text().toString()}, cur);
            cur->child.append(node);
        }

        if (token == QXmlStreamReader::EndElement) {
            QString str = XMLReader->name().toString();
            --curLime;
            cur = cur->parent;

            if (str == "array" && curLime + 1 != Lime.top()) {
                Crab.pop();
                Lime.pop();
            }

        }
    }

    if (XMLReader->hasError()) {
        QMessageBox message(QMessageBox::Critical,
                            tr("Error Message"),
                            tr("Parsing has failed!"),
                            QMessageBox::Ok);
        message.exec();
        Root->child.pop_back();
    }

    file.close();
}

void XMLParser::delete_row() noexcept {
    Tree* ptr = static_cast<Tree*>(PMI.internalPointer());
    removeRow((ptr->parent) ? ptr->parent->child.indexOf(ptr) : 0, PMI.parent());
}

void XMLParser::delete_rows() noexcept {
    removeRows(0, Root->child.size(), createIndex(-1, -1, nullptr));
}
