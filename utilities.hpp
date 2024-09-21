#pragma once
#include "nlohmann/json.hpp"
#include "qlistwidget.h"

std::string home_dir();
std::string get_unique_unit_name(const nlohmann::json sim_obj, std::string unit_uuid);
std::string get_unique_connector_name(const nlohmann::json sim_obj, std::string stream_uuid);
std::string get_uuid_from_unique_connector_name(const nlohmann::json sim_obj, std::string unique_connector_name);

// QT:
void remove_all_entries_QListWidget(QListWidget& qlist);