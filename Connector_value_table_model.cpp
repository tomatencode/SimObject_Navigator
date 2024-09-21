#include "Connector_value_table_model.hpp"

using namespace std::literals::string_literals; // allow suffix ""s for std::string literal
/*
 \todo allow for Excel-Like copy-pasting of values https://stackoverflow.com/questions/21675330/copy-paste-in-qtableview
*/

QVariant Connector_value_table_model::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int col = index.column();
        assert(col>=0 and col < simobject_container.size());
        int row = index.row();
        if (row >= 0 and row < simobject_container.at(col).current_connector_values.size())
        {
            return QVariant(simobject_container.at(col).current_connector_values.at(row));
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return QVariant ();
    }
}

Qt::ItemFlags Connector_value_table_model::flags(const QModelIndex& index) const
{
    if (not index.isValid())
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant Connector_value_table_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal or role != Qt::DisplayRole) return {};

    //assert(section==0);

    return QVariant(("Values #"s + std::to_string(1+section)).c_str());
}
