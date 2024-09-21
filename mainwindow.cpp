#include "iso646.h"
#include<fstream>
#include<QFileDialog>
#include "mainwindow.h"
#include "utilities.hpp"
/*
	General:
	\todo integrate validate_SimulationObject
	\todo integrate keyword file checker
	\todo show dot graph
	\todo show unit list
	\todo allow dropping of SimObjects on drop field
    \todo generate log output file https://stackoverflow.com/questions/45787260/how-to-print-logs-into-file-in-console-application-in-qt-creator

	Connector Form:
	\todo show UUID
 */
using namespace std::literals::string_literals; // allow suffix ""s for std::string literal

MainWindow::MainWindow(QWidget* parent)	: 
	QMainWindow(parent),
    //flow_sheet_graph_tab(sim_objects, configuration),
    copyAction("Copy to clipboard"),
    not_yet_implemented("<not yet implemented>"),
    unit_tab(sim_objects),
    main_tab(),
    cvtm(sim_objects)
{
	read_config_file();
	/*
		Main Windows

		SimObject Selectors

		-----------------------

		Streams | Units | ...

		Streams tab
	 */
	main_window_horizontal_split.addWidget(&sim_object_selector_group);
	main_window_horizontal_split.addWidget(&eval_functional_model);
	main_window_horizontal_split.addWidget(&main_tab);

	main_tab.setFocusPolicy(Qt::NoFocus);

	main_window_group.setLayout(&main_window_horizontal_split);

	for (int i = 0; i < NSimObjects; ++i)
	{
		select_new_simobject[i].setText(("Select SimObject #"s + std::to_string(1 + i)).c_str());
		sim_object_selector_subgroup[i].setLayout(&sim_object_selector_layout_group[i]);

		//sim_object_selector_layout.addWidget(&drop_label);

		select_new_simobject[i].setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

		sim_object_selector_layout_group[i].addWidget(&select_new_simobject[i]);

		sim_object_selector_layout_group[i].addWidget(&simobj_fname[i]);

		sim_object_selector_layout.addWidget(&sim_object_selector_subgroup[i]);
	}
	sim_object_selector_group.setLayout(&sim_object_selector_layout);

	eval_functional_model.setText("Execute cli tool");

	// "Streams" Tab

	// Stream Selector is simple list
	connector_form_layout_.addWidget(&StreamListSelector, 5);
	Connector_form_group.setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

	// Connector tab is more complex
    connector_form_layout_.addWidget(&connector_form_values);
    connector_form_values_layout.addWidget(&copyAction);

	Connector_form_group.setLayout(&connector_form_layout_);

	StreamListSelector.setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));

	auto success = connect(&StreamListSelector, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setListItem(QListWidgetItem*)));
	assert(success);
	success = connect(&eval_functional_model, SIGNAL(clicked()), this, SLOT(exec_generic_cli_tool()));
	assert(success);

	success = connect(&select_new_simobject[0], SIGNAL(clicked()), this, SLOT(selectSimObjectFile1()));
	assert(success);
	success = connect(&select_new_simobject[1], SIGNAL(clicked()), this, SLOT(selectSimObjectFile2()));
	assert(success);
	success = connect(&select_new_simobject[2], SIGNAL(clicked()), this, SLOT(selectSimObjectFile3()));
	assert(success);


    // Connect the triggered signal of the copy action to a slot
    success = connect(&copyAction, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
    assert(success);

	connector_value_table.setModel(&cvtm);
	connector_value_table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    connector_value_table.setSelectionMode(QAbstractItemView::ExtendedSelection);
	cvtm.setHeaderData(0, Qt::Horizontal, "Values");

	//connector_form_layout.addWidget(&select_new_simobject);
	connector_form_values_layout.addWidget(&connector_value_table);
	connector_form_values.setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

	connector_form_values.setLayout(&connector_form_values_layout);

	main_tab.addTab(&Connector_form_group, "Streams");
    main_tab.addTab(&unit_form, "Units");
    //main_tab.addTab(&flow_sheet_graph_tab, "Graph");

	generic_cli_tool_factory();

#ifdef BUILD_WITH_EXPERIMENTAL_WEB_API
	// "Units" Tab
    main_tab.addTab(&eval_functional_model_net_tab, "SMILE Web API");
#endif

	unit_form_layout.addWidget(&unit_tab);
	unit_form.setLayout(&unit_form_layout);

	setCentralWidget(&main_window_group);
}

/*
 *
*/

/*
	\todo distinguish primary SimObject / other?
*/
bool MainWindow::readSimObject(int i, std::string fname)
{
	try {
		std::ifstream sim_obj_file(fname.c_str());

		if (not sim_obj_file.is_open())
		{
			message("Error opening file '"s + fname + "'", QMessageBox::Critical);
			return false;
		}
		auto  new_sim_obj = nlohmann::json::parse(sim_obj_file);

		remove_all_entries_QListWidget(StreamListSelector);

		for (const auto& [stream_uuid,stream] : new_sim_obj.at("Connectors"s).get<nlohmann::json::object_t>())
		{
			StreamListSelector.addItem(get_unique_connector_name(new_sim_obj,stream_uuid).c_str());
		}

		if (sim_objects.size() < i)
		{
			sim_objects.resize(i);
		}
		sim_objects.back().sim_object_filename = fname;
		sim_objects.back().sim_object = new_sim_obj;
		simobj_fname[i - 1].setText(sim_objects.back().sim_object_filename.c_str());

		unit_tab.update();


        if(i==1)
        {
            //flow_sheet_graph_tab.exec();
        }
	}
	catch (const std::exception& e) {
		message("Error reading simobject:"s + e.what(),QMessageBox::Critical);
		return false;
	}
	update_connector_form_values();
	return true;
}

bool MainWindow::selectSimObjectFile(int i)
{
	if (sim_objects.size() < i - 1)
	{
		message("Cannot select SimObject #"s + std::to_string(i) + " as there is no previous one"s,QMessageBox::Warning);
		return false;
	}

	QFileDialog selector(this, ("Select SimulationObject #" + std::to_string(i)).c_str());
	selector.setFileMode(QFileDialog::FileMode::ExistingFiles);

	QString filePath = QFileDialog::getOpenFileName(this, "Select a file...", QDir::homePath(), "*.json");

	return readSimObject(i, filePath.toUtf8().constData());
}

void MainWindow::selectSimObjectFile1() {
	selectSimObjectFile(1);
}
void MainWindow::selectSimObjectFile2() {
	selectSimObjectFile(2);
}
void MainWindow::selectSimObjectFile3() {
	selectSimObjectFile(3);
}


// populate values:
void MainWindow::update_connector_form_values()
{
	for (auto& sim_object_container : sim_objects)
	{
		const auto& sim_object = sim_object_container.sim_object;

		sim_object_container.current_connector_values.resize(0);
		for (const auto& [stream_uuid,stream] : sim_object.at("Connectors"s).get<nlohmann::json::object_t>())
		{
			if (get_unique_connector_name(sim_object, stream_uuid) == current_connector_name)
			{
				//uuid = stream_uuid;

				assert(stream.contains("Values"s));
				for (const auto& value : stream.at("Values"s))
				{
					sim_object_container.current_connector_values.push_back(value.get<double>());
				}
			}
		}
	}
	cvtm.layoutChanged();
	connector_value_table.resizeRowsToContents();
	connector_value_table.setItemDelegateForColumn(1, &cvtm.delegate);
}

void message(std::string message, QMessageBox::Icon icon)
{
	QMessageBox    msgBox;
	std::string err_msg = "Not yet implemented";
	msgBox.setIcon(icon);
	msgBox.setText(message.c_str());
	msgBox.exec();
}

void MainWindow::setListItem(QListWidgetItem* item)
{
	assert(item);

	current_connector_name = item->text().toUtf8().constData();

	update_connector_form_values();
}

void MainWindow::read_config_file()
{
	auto my_home_dir = home_dir();

	// first local file, then 'global' file
	std::vector<std::string> config_file_names =
	{
		home_dir() + "/.SimulationObject_Navigator_config.json"s,
		R"x(\\le.grp\dept\ITP_Public\SMILE\Tools\EXPERIMENTAL_SimObject_Navigator\.SimulationObject_Navigator_config.json)x"
	};

	for (const auto& config_file_name : config_file_names)
	{
		std::ifstream config_file(config_file_name);
		if (config_file.is_open())
		{
			try {
				configuration = nlohmann::json::parse(config_file);
			}
			catch (std::exception& exc)
			{
				message(exc.what(), QMessageBox::Critical);
			}
			break;
		}
	}
}

/**
	\todo allow using different versions of evaluate_SimulationObject
	\todo make stack size an additional argument
	\todo make "output_flags" an additional argument
	\todo add run time measurement
*/
void MainWindow::exec_generic_cli_tool()
{
	if (sim_objects.empty())
	{
		message("No SimulationObject given", QMessageBox::Critical);
		return;
	}

	// Get the index of the active tab  
	int activeTabIndex = main_tab.currentIndex();
	// Get the label of the active tab  
	QString activeTabLabel = main_tab.tabText(activeTabIndex);

	// Use the label to do something with the active tab  
	QWidget* activeTabWidget = main_tab.widget(activeTabIndex);

	auto gen_cli_tool = dynamic_cast<Generic_cli_tool*>(activeTabWidget);

	std::string result_sim_obj;

	if (gen_cli_tool)
	{
		result_sim_obj = gen_cli_tool->exec(sim_objects);
		if (not result_sim_obj.empty())
		{
			readSimObject(static_cast<int>(sim_objects.size()) + 1, result_sim_obj);
		}
	}
	else
	{
		message("No cli tool selected", QMessageBox::Critical);
	}
}


void MainWindow::copyToClipboard()
{
    QItemSelectionModel* selection = connector_value_table.selectionModel();
	QModelIndexList selectedRows = selection->selectedRows();
	QModelIndexList selectedColumns = selection->selectedColumns();

    if (selectedRows.isEmpty())
		return;

	QString copiedData;

	for (int i = 0; i < selectedRows.count(); i++)
	{
        for (int j = 0; j < sim_objects.size(); j++)
		{
            //QModelIndex index = connector_value_table.model()->index(selectedRows.at(i).row(), selectedColumns.at(j).column());
            QModelIndex index = connector_value_table.model()->index(selectedRows.at(i).row(), j);
            QString data = connector_value_table.model()->data(index).toString();

			copiedData += data;

			if (j != selectedColumns.count() - 1)
				copiedData += "\t";
		}

		copiedData += "\n";
	}

    if(copiedData.length()>0)
    {
        QApplication::clipboard()->setText(copiedData);
    }
}
