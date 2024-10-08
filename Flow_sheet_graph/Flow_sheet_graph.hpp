#ifndef FLOW_SHEET_GRAPH_H
#define FLOW_SHEET_GRAPH_H
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>
#include <qcheckbox.h>
#include <qgridlayout.h>
#include <qlistwidget.h>
#include <qobject.h>
#include <qwidget.h>
#include <set>


#include "Graph_Imageviewer.hpp"
#include "SimObject_Container.hpp"
/**
        \todo Edit field for stack size

*/
class Flow_Sheet_Graph : public QWidget {
  Q_OBJECT
  QHBoxLayout *layout = nullptr;
  const SimObject_Containers &simobject_container;
  const ::nlohmann::json &config;
  Graph_ImageViewer *graph_imageviewer = nullptr;

  std::string dot_graph_tool_pathname =
#ifdef _WIN32
      R"x(\\le.grp\dept\ITP_Public\SMILE\Tools\Graphviz2.38\bin\dot.exe)x"
#else
      "dot"
#endif
      ;

  std::set<std::string>
      suppress_connectors; ///< uuids of connectors to be suppressed in graph

  std::vector<QCheckBox *> checkbox_holder;
  std::vector<QListWidgetItem *> list_widget_holder;
private slots:
  void on_itemClicked(QListWidgetItem *item);

private:
  QListWidget *m_listWidget = nullptr;

  bool first_call = true;
  void update_connector_list();

public:
  Flow_Sheet_Graph(SimObject_Containers &simobject_container_,
                   const ::nlohmann::json &config_, QWidget *parent)
      : simobject_container(simobject_container_), config(config_),
        QWidget(parent), m_listWidget(new QListWidget),
        graph_imageviewer(new Graph_ImageViewer), layout(new QHBoxLayout) {
    layout->addWidget(graph_imageviewer);

    layout->addWidget(m_listWidget);
    m_listWidget->setSizePolicy(
        QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));

    setLayout(layout);

    update_connector_list();
  }

  void exec();
};
#endif // FLOW_SHEET_GRAPH
