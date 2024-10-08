#ifndef UNIT_TAB_H
#define UNIT_TAB_H
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>
#include <qgridlayout.h>
#include <qobject.h>
#include <qwidget.h>

#include "SimObject_Container.hpp"
/**
        \todo Edit field for stack size

*/
class Unit_Tab : public QWidget {
  Q_OBJECT

private:
  class PortGroup : public QWidget {
    QLabel port_group_name;

  public:
    PortGroup(std::string name = "") { port_group_name.setText(name.c_str()); }
  };

  QLabel not_yet_implemented;
  QHBoxLayout *layout = nullptr;
  QGridLayout *unit_details_layout = nullptr;
  QGroupBox *unit_details = nullptr;
  QListWidget *unit_list_selector = nullptr;
  QListWidget *unit_port_groups = nullptr;
  std::vector<PortGroup> port_group;
  SimObject_Containers &simobject_containers;

  QLabel unit_name_label, unit_name, unit_port_label, model_dll_name_label,
      model_dll_name;

  std::string current_unit_name;

public:
  Unit_Tab(SimObject_Containers &simobject_container_);

  void update();
public slots:
  void setListItem(QListWidgetItem *item);
};
#endif // UNIT_TAB_H
