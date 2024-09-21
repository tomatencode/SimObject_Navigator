#include<string>
#include "utilities.hpp"

#ifdef _WIN32  
#include <Windows.h>  
#include <ShlObj.h>  

std::string home_dir() {  
    char homedir[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, homedir))) {
        return std::string(homedir);
    }
    else {
        return "";
    }
}  
#elif __APPLE__  
#include <unistd.h>  
#include <sys/types.h>  
#include <pwd.h>  
std::string home_dir() {  
  uid_t uid = getuid();  
  struct passwd *pw = getpwuid(uid);  
  if (pw) {  
    return std::string(pw->pw_dir);  
  } else {  
    return "";  
  }  
}  
#else  
#include <unistd.h>  
#include <sys/types.h>  
#include <pwd.h>  
std::string home_dir() {  
  uid_t uid = getuid();  
  struct passwd *pw = getpwuid(uid);  
  if (pw) {  
    return std::string(pw->pw_dir);  
  } else {  
    return "";  
  }  
}  
#endif 

using namespace std::literals::string_literals; // allow suffix ""s for std::string literal

std::string get_unique_unit_name(const nlohmann::json sim_obj, std::string unit_uuid)
{
	// todo uniquification not actually done
	const auto& unit = sim_obj.at("Flowsheet").at(unit_uuid);

	if (unit.contains("Name"))
	{
		auto unit_name = unit.at("Name").get<std::string>();

		bool unique = true;
		for (const auto& [unit2_uuid, unit2] : sim_obj.at("Flowsheet").get<nlohmann::json::object_t>())
		{
			if (unit2_uuid != unit_uuid)
			{
				if (unit2.contains("Name"))
				{
					if (unit2.at("Name").get<std::string>() == unit_name)
					{
						unique = false;
					}
				}
			}
		}
		if (not unique) return unit_name + "{" + unit_uuid + "}";
		return unit_name;
	}
	return unit_uuid;
}

std::string get_unique_connector_name(const nlohmann::json sim_obj, std::string stream_uuid)
{
	assert(sim_obj.at("Connectors"s).contains(stream_uuid));

	const auto& stream = sim_obj.at("Connectors"s).at(stream_uuid);

	if (stream.contains("Name"))
	{
		auto stream_name = stream.at("Name").get<std::string>();

		bool unique_name = true;
		for (const auto& [stream2_uuid, stream2] : sim_obj.at("Connectors"s).get<nlohmann::json::object_t>())
		{
			if (stream2_uuid != stream_uuid)
			{
				if (stream2.contains("Name"))
				{
					if (stream2.at("Name").get<std::string>() == stream_name)
					{
						unique_name = false;
					}
				}
			}
		}
		if (not unique_name) {
			return stream_name + "{"s + stream_uuid + "}"s;
		}
		return stream_name;
	}
	return stream_uuid;
}


std::string get_uuid_from_unique_connector_name(const nlohmann::json sim_obj, std::string unique_connector_name)
{
	for (const auto& [con_uuid, con] : sim_obj.at("Connectors"s).get<nlohmann::json::object_t>())
	{
		if (get_unique_connector_name(sim_obj, con_uuid) == unique_connector_name) {
			return con_uuid;
		}
	}
	throw std::runtime_error("Cannot find connector '"s + unique_connector_name + "'!"s);
	return ""s; // avoid compiler warning
}

void remove_all_entries_QListWidget(QListWidget& qlist)
{
	int nItems = qlist.count();

	// https://stackoverflow.com/questions/60567277/remove-all-items-from-qlistwidget-in-a-cycle
	for (int i = 0; i < nItems; ++i)
	{
		QListWidgetItem* item = qlist.item(0);
		qlist.takeItem(qlist.row(item));
	}
}