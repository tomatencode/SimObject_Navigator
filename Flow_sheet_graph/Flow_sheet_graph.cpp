#include <iostream>
#include <fstream>
#include<set>
#include "Flow_sheet_graph.hpp"
#include "message.hpp"
#include "utilities.hpp"

using namespace std::literals::string_literals; // allow suffix ""s for std::string literal

void SimObject_to_dot_input(const nlohmann::json& sim_object, std::ostream& dot_input, const std::set<std::string>& suppress_connectors);

void Flow_Sheet_Graph::exec()
{
	if (simobject_container.empty())
	{
		return;
	}
	if (config.contains("dot_graph_tool_pathname")) {
		dot_graph_tool_pathname = config.at("dot_graph_tool_pathname").get<std::string>();
	}

	const auto& sim_object = simobject_container.at(0).sim_object;

	update_connector_list();

    char dot_input_fname[2048], dot_output[2048], image_file[2048]; /// \todo replace by std::filesystem operations
	std::tmpnam(dot_input_fname);
	//std::strcpy(dot_input_fname, R"x(C:\Users\TTV032\AppData\Local\Temp\my_graph.txt)x"); /// \todo remove later
	std::tmpnam(dot_output);
	std::tmpnam(image_file);
    //strcat(image_file, ".png"); /// \todo this is obviously not a valid temp file, just for testing...

	//std::strcpy(image_file, R"x(C:\Users\TTV032\AppData\Local\Temp\my_graph.png)x"); /// \todo remove later

	{
		std::ofstream  dot_input(dot_input_fname);
		SimObject_to_dot_input(sim_object, dot_input, suppress_connectors);
	}

	auto command = std::string(dot_graph_tool_pathname) + " "s + std::string(dot_input_fname)
		+ " -Tpng -o "s + std::string(image_file);
	int ret = std::system(command.c_str());
	if (ret != 0) {
		message("dot tool returned w error");
		return;
	}

	QImage image;
    if (not image.load(image_file)) {
		message("Failed to load image %s"s + qPrintable(image_file));
		return;
	}
	graph_imageviewer.setImage(image);
}

void SimObject_to_dot_input(const nlohmann::json& sim_object, std::ostream& dot_input, const std::set<std::string>& suppress_connectors)
{
	auto is_out = [](std::string port_name)->bool
	{
		return port_name.find("OUT") != std::string::npos;
	};

	std::map<std::string, std::string> source;          // connector_uuid -> source_unit_uuid one source per connector (UUID)
	std::map<std::string, std::set<std::string>> sinks; // connector_uuid -> set of sink_unit_uuid 
	try {

		for (const auto& [unit_uuid, unit] : sim_object.at("Flowsheet").get<nlohmann::json::object_t>())
		{
			for (const auto& port_name : unit.at("Ports").items())
			{
				bool is_out_ = is_out(port_name.key());

				for (const auto& connector_uuid : unit.at("Ports").at(port_name.key()).at("UUIDs"))
				{
					if (is_out_)
					{
						assert(not source.contains(connector_uuid));
						source[connector_uuid.get<std::string>()] = unit_uuid;
					}
					else
					{
						sinks[connector_uuid.get<std::string>()].insert(unit_uuid);
					}
				}
			}
		}
	}
	catch (std::exception& ex)
	{
		message(ex.what());
	}

	dot_input << "digraph G {\n";
	for (const auto& [connector_uuid, source_unit_uuid] : source)
	{
		if (suppress_connectors.contains(connector_uuid)) continue;

		if (sinks.contains(connector_uuid))
		{
			for (const auto& sink_unit_uuid : sinks.at(connector_uuid))
			{
				dot_input << "\"" << get_unique_unit_name(sim_object, source_unit_uuid) << "\" -> \"" << get_unique_unit_name(sim_object, sink_unit_uuid) << "\""
					<< "[label=\"" << get_unique_connector_name(sim_object, connector_uuid) << "\"]\n";
			}
		}
		else
		{
			// no SINK for connector with connector_uuid (which has a source)
			dot_input << "\"" << get_unique_unit_name(sim_object, source_unit_uuid) << "\" -> \"SINK\""
				<< "[label=\"" << get_unique_connector_name(sim_object, connector_uuid) << "\"]\n";
		}
	}
	for (const auto& [connector_uuid, sink_unit_uuids] : sinks)
	{
		if (suppress_connectors.contains(connector_uuid)) continue;

		if (not source.contains(connector_uuid)) {
			// connector with uuid connector_uuid has no source
			for (const auto& sink_unit_uuid : sink_unit_uuids)
			{
				dot_input << "\"SOURCE\" -> \"" << get_unique_unit_name(sim_object, sink_unit_uuid) << "\""
					<< "[label=\"" << get_unique_connector_name(sim_object, connector_uuid) << "\"]\n";
			}
		}
	}
	dot_input << "}\n";
}

void Flow_Sheet_Graph::update_connector_list()
{
	if (simobject_container.empty()) return;

	const auto& sim_object = simobject_container.at(0).sim_object;
    if(first_call)
    {
        checkbox_holder.resize(0);
        remove_all_entries_QListWidget(m_listWidget);

        int i = 0;
        for (const auto& [connector_uuid, connector] : sim_object.at("Connectors").get<nlohmann::json::object_t>())
        {
            auto con_name = get_unique_connector_name(sim_object, connector_uuid);

            list_widget_holder.emplace_back(new QListWidgetItem());
            auto listWidgetItem = list_widget_holder.back();

            m_listWidget.addItem(listWidgetItem);

            checkbox_holder.emplace_back(new QCheckBox(this));
            auto checkBox = checkbox_holder.back();

            checkBox->setChecked(not suppress_connectors.contains(connector_uuid));
            checkBox->setText(con_name.c_str());
            connect(checkBox, &QCheckBox::clicked, [&]() { on_itemClicked(listWidgetItem); });

            m_listWidget.setItemWidget(listWidgetItem, checkBox);

            i++;

        }
        first_call = false;
    }
    else
    {
        int i = 0;

        for (const auto& [connector_uuid, connector] : sim_object.at("Connectors").get<nlohmann::json::object_t>())
        {
            auto con_name = get_unique_connector_name(sim_object, connector_uuid);

            auto checkBox = checkbox_holder.at(i);

            assert(checkBox->text() ==con_name.c_str());
            checkBox->setChecked(not suppress_connectors.contains(connector_uuid));

            i++;
        }

    }
}

void Flow_Sheet_Graph::on_itemClicked(QListWidgetItem* item) {
	QWidget* widget = m_listWidget.itemWidget(item);
	const QCheckBox& checkBox = *qobject_cast<QCheckBox*>(widget);

	auto con_name = checkBox.text().toStdString();
	const auto& sim_object = simobject_container.at(0).sim_object;
	auto con_uuid = get_uuid_from_unique_connector_name(sim_object, con_name);

	if (not checkBox.isChecked()) {
		suppress_connectors.insert(con_uuid);
	}
	else {
		if (suppress_connectors.contains(con_uuid)) {
			suppress_connectors.erase(con_uuid);
		}
	}
	exec();
}
