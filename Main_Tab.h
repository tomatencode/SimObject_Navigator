#include <QTabWidget>
#include "Flow_sheet_graph/Flow_sheet_graph.hpp"

class Main_Tab : public QTabWidget {
public:
    Flow_Sheet_Graph * flow_sheet_graph = nullptr;
    
    Main_Tab(QWidget * parent) : QTabWidget(parent) {
        bool success = connect(this, &QTabWidget::currentChanged, this, &Main_Tab::onTabActivated);
        assert(success);
    }
private slots:
  void onTabActivated(int index) {
    if (index == 2) /// \todo use label rather than index
    {
        flow_sheet_graph->exec();
    }
  }
};
