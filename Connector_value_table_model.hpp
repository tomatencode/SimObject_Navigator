#pragma once
#include<vector>
#include<QAbstractTableModel>
#include<QStyledItemDelegate>
#include "SimObject_Container.hpp"
/*
    Model forr SimulationObject_Navigator Connector Form
*/
class Connector_value_table_model : public QAbstractTableModel
{
    Q_OBJECT

    const SimObject_Containers&     simobject_container;
public:
    Connector_value_table_model(SimObject_Containers& simobject_container_) : 
        simobject_container(simobject_container_) 
    {
    }

    QStyledItemDelegate             delegate;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE       {
        int row_count = 0;

        for (const auto& sim_object : simobject_container)
        {
            row_count = std::max((int)simobject_container.at(0).current_connector_values.size(), row_count);
        }

        return row_count;
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE    {
        return (int)simobject_container.size();
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE
    {
        return true;
    }


    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};
