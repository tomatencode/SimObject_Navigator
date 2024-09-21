#ifndef FLOW_SHEET_GRAPH_H
#define FLOW_SHEET_GRAPH_H
#include<qobject.h>
#include <qwidget.h>
#include<QHBoxLayout>
#include<qlistwidget.h>
#include<qgridlayout.h>
#include<QVBoxLayout>
#include<QGroupBox>
#include<QLabel>
#include<QLabel>
#include <QTextEdit>
#include <QScrollArea>
#include <QLabel>
#include <qcheckbox.h>
#include<set>

#include "Graph_Imageviewer.hpp"
#include "SimObject_Container.hpp"
/**
	\todo Edit field for stack size

*/
class Flow_Sheet_Graph : public QWidget {
	Q_OBJECT
	QHBoxLayout						layout;
	const SimObject_Containers&		simobject_container;
	const::nlohmann::json&			config;
	Graph_ImageViewer				graph_imageviewer;

	std::string						dot_graph_tool_pathname = R"x(\\le.grp\dept\ITP_Public\SMILE\Tools\Graphviz2.38\bin\dot.exe)x";

	std::set<std::string>			suppress_connectors; ///< uuids of connectors to be suppressed in graph

	std::vector<std::unique_ptr<QCheckBox>>  checkbox_holder;
	std::vector<std::unique_ptr<QListWidgetItem>> list_widget_holder;
private slots:
	void on_itemClicked(QListWidgetItem* item);
private:
	QListWidget m_listWidget;  

	void update_connector_list();

public:
	Flow_Sheet_Graph(SimObject_Containers& simobject_container_, const::nlohmann::json& config_) 
		: simobject_container(simobject_container_), config(config_)
	{
		layout.addWidget(&graph_imageviewer);
		layout.addWidget(&m_listWidget);
		m_listWidget.setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));

		setLayout(&layout);

		update_connector_list();
	}

	void exec();
};
#endif // FLOW_SHEET_GRAPH
