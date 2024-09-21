#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<memory>
#include<vector>
#include<map>
#include<list>

#include<QMainWindow>
#include<QGroupBox>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QListWidget>
#include<QTableView>
#include<QMessageBox>
#include<QPushButton>
#include<QTabWidget>
#include<QLabel>
#include<Connector_value_table_model.hpp>
#include<DropLabel.hpp>
#include<QClipboard>
#include<QAction>

#include "message.hpp"

#include "Flow_sheet_graph/Flow_sheet_graph.hpp"
#include "Generic_cli_tool/Generic_cli_tool.h"
#include "Unit_Tab/Unit_Tab.h"
#include "Main_Tab.h"
#include "Eval_functional_model_net.h"
#include "SimObject_Container.hpp"

/**
	Todos see mainwindows.cpp
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

		static constexpr int NSimObjects = 3;

    QGroupBox      * main_window_group,
		// Top of main window: Overview SimObject
        * sim_object_selector_group,
        * sim_object_selector_subgroup[NSimObjects],
		//
        * Connector_form_group,
		// Show Stream Contents
        * connector_form_values,
		// Not yet implemented
        * unit_form;

	QHBoxLayout
		// Button Select SimObject, Filename
        * sim_object_selector_layout_group[NSimObjects],
        * connector_form_layout_;

    QVBoxLayout    * main_window_horizontal_split,
        * sim_object_selector_layout,
        * connector_form_values_layout,
        * unit_form_layout;
    // Create a QAction for the copy action
    QPushButton    * copyAction;

    std::list<Generic_cli_tool* >  generic_cli_tools;
#ifdef BUILD_WITH_EXPERIMENTAL_WEB_API
    Eval_functional_model_net * eval_functional_model_net_tab;
#endif
    //Flow_Sheet_Graph      flow_sheet_graph_tab;
    Unit_Tab       * unit_tab;
    QListWidget    * StreamListSelector;
    QTableView     * connector_value_table;
    Main_Tab       * main_tab;

	SimObject_Containers   sim_objects;

	nlohmann::json    configuration; // found in %user%/.SimulationObject_Navigator.json

	QPushButton
        * select_new_simobject[NSimObjects],
        * eval_functional_model,
        * clear_all;
    QLabel         * simobj_fname[NSimObjects],
        * not_yet_implemented;
	DropLabel      drop_label;

	QTableView     connector_values;

	Connector_value_table_model  cvtm;

	static constexpr char pathname_evaluate_SimulationObject[] =
		//		R"(\\le.grp\dept\ITP_Public\SMILE\Redist\SMILE_Redist_latest_cloud\Visual_Studio_2019\Release\bin\evaluate_Simulation_Object.exe)";
		R"(\\le.grp\dept\ITP_Public\SMILE\Tools\EXPERIMENTAL_SimObject_Navigator\Temporary_evaluate_SimulationObject\evaluate_Simulation_Object.exe)";
	// use Redist version once PR 3679 available
	std::string current_connector_name;

	void generic_cli_tool_factory();

	void read_config_file();
public:
	MainWindow(QWidget* parent = nullptr);

	/**
	 \brief a new SimObject has been selected
	 */
	bool readSimObject(int i, std::string fname);

public slots:
	void setListItem(QListWidgetItem* item);
	bool selectSimObjectFile(int i);
	void selectSimObjectFile1();
	void selectSimObjectFile2();
	void selectSimObjectFile3();

	void exec_generic_cli_tool();

    void copyToClipboard();

private:
	void update_connector_form_values();
};

#endif // MAINWINDOW_H
