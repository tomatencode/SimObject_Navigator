#include <cstdio>
#include<fstream>
#include "mainwindow.h"

#include "Unit_Tab.h"

using namespace std::literals::string_literals; // allow suffix ""s for std::string literal

Unit_Tab::Unit_Tab(SimObject_Containers& simobject_container_) :
	simobject_containers(simobject_container_),
	not_yet_implemented("Not yet implemented")
{
	layout.addWidget(&unit_list_selector);
	layout.addWidget(&unit_details);
	unit_details.setLayout(&unit_details_layout);

	unit_name_label.setText("Name");

	int act_row = 0;

	unit_details_layout.addWidget(&unit_name_label, act_row,0);
	unit_details_layout.addWidget(&unit_name, act_row, 1);

	model_dll_name_label.setText("Model_DLL");

	unit_details_layout.addWidget(&model_dll_name_label, ++act_row, 0);
	unit_details_layout.addWidget(&model_dll_name, act_row, 1);

	unit_details_layout.addWidget(&unit_port_label, ++act_row, 0);
	unit_details_layout.addWidget(&unit_port_groups, act_row, 1);

	setLayout(&layout);

	auto success = connect(&unit_list_selector, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setListItem(QListWidgetItem*)));
	assert(success);

}

void reset_QListWidget(QListWidget& List)
{
	// https://stackoverflow.com/questions/60567277/remove-all-items-from-qlistwidget-in-a-cycle

	int nItems = List.count();
	for (int i = 0; i < nItems; ++i)
	{
		QListWidgetItem* item = List.item(0);
		List.takeItem(List.row(item));
	}
}

void Unit_Tab::update()
{
	if (simobject_containers.empty()) return;

	const auto& primary_sim_object = simobject_containers.at(0).sim_object;

	reset_QListWidget(unit_list_selector);

	for (const auto& unit : primary_sim_object.at("Flowsheet"s).items())
	{
		unit_list_selector.addItem(get_unique_unit_name(primary_sim_object,unit.key()).c_str());
	}
	current_unit_name = "";
	reset_QListWidget(unit_port_groups);
}

void Unit_Tab::setListItem(QListWidgetItem* item)
{
	current_unit_name = item->text().toUtf8().constData();
	unit_name.setText(current_unit_name.c_str());

	auto unit_uuid = get_uuid_from_unique_unit_name(simobject_containers.at(0).sim_object, current_unit_name);
	if (simobject_containers.at(0).sim_object.at("Flowsheet").at(unit_uuid).contains("ModelDLL"))
	{
		model_dll_name.setText(simobject_containers.at(0).sim_object.at("Flowsheet").at(unit_uuid).at("ModelDLL").get<std::string>().c_str());
	}
	const auto& unit = simobject_containers.at(0).sim_object.at("Flowsheet").at(unit_uuid);

	reset_QListWidget(unit_port_groups);
	//port_group.resize(0);

	int i = 0;
	for(const auto& ports : unit.at("Ports").items())
	{
		//port_group.emplace_back(std::to_string(i++));

		unit_port_groups.addItem(ports.key().c_str());
	}
}