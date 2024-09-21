#pragma once

#include <nlohmann/json.hpp> // \todo not use SMILE's version, get by FetchContent
#include "utilities.hpp"
#include <string>
#include <vector>

struct SimObject_Container {
    nlohmann::json                 sim_object;
    std::string                    sim_object_filename;
    std::vector<double>            current_connector_values;
    bool                           sim_object_and_sim_object_file_in_sync = true;
};

using SimObject_Containers = std::vector<SimObject_Container>;

std::string get_unique_connector_name(const nlohmann::json sim_obj, std::string stream_uuid);
inline std::string get_uuid_from_unique_stream_name(const nlohmann::json sim_obj, std::string unique_stream_name)
{
    for (const auto& stream : sim_obj.at("Connector").items())
    {
        if (get_unique_connector_name(sim_obj, stream.key()) == unique_stream_name) return stream.key();
    }
    assert(false);
    return "";
}

inline std::string get_uuid_from_unique_unit_name(const nlohmann::json sim_obj, std::string unique_unit_name)
{
    for (const auto& unit : sim_obj.at("Flowsheet").items())
    {
        if (get_unique_unit_name(sim_obj, unit.key()) == unique_unit_name) return unit.key();
    }
    assert(false);
    return "";
}