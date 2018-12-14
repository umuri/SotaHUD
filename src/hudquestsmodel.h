#ifndef HUDQUESTSMODEL_H
#define HUDQUESTSMODEL_H

#include <QAbstractItemModel>
#include "questspluscontroller.h"

class hudQuestsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit hudQuestsModel(QObject *parent = 0, questsPlusController * inController = NULL);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
     const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool hasChildren(const QModelIndex &parent);


    void updateData();

public slots:
    void indexDoubleClicked(const QModelIndex &index);
    void qcollapsed(const QModelIndex &index);
    void qexpanded(const QModelIndex &index);

private:
    questsPlusController * questController;

};

#endif // HUDQUESTSMODEL_H
