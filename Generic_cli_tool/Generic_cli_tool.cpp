#include <cstdio>
#include<map>
#include<fstream>
#include "mainwindow.h"
#include <chrono>
#include <regex>  

#include "Generic_cli_tool.h"

using namespace std::literals::string_literals; // allow suffix ""s for std::string literal

namespace {

	using CliParameter = struct {
		std::string		type;
		std::string		default_value;
		std::string		actual_value;
	};

	using CliToolParameters = std::map<std::string, CliParameter>;

	using SimObj_tool_filenames = std::map<std::string, std::string>;

	// LSTACK:int:100000
	std::regex cli_tool_parameter_pattern("\\{([a-zA-Z_0-9 ]+(?::[^\\}]+)?)\\}");
	std::regex cli_tool_parameter_detail_pattern("([a-zA-Z_0-9 ]+):((?:int)|(?:string)|(?:file)|(?:folder)):([^\\}]*)");

	/*
	* syntax for cli tool argument: LSTACK:int:100000
	*/
	CliToolParameters preprocess_command_line(std::string command_line, SimObj_tool_filenames& sim_obj_tool_filenames)
	{
		CliToolParameters cli_tool_parameters;

		// replace {sim_obj_1} with cli_input_filenames["sim_obj_1"] etc
		// implementation looks clumsy, could be made nicer using boost::regex_replace with lambda
		std::string result = command_line;
		std::sregex_iterator matches(command_line.begin(), command_line.end(), cli_tool_parameter_pattern);
		std::sregex_iterator end;

		size_t pos_ofs = 0;
		// Iterate over all matches  
		while (matches != end) {
			std::smatch match = *matches;
			std::string subgroup = match[1];
			if (subgroup.find(':') != std::string::npos) {
				// match is parameter, e.g. LWSTACK:int:10000

				std::smatch match;

				if (std::regex_search(subgroup, match, cli_tool_parameter_detail_pattern)) {
					std::string
						parameter_name = match[1],
						parameter_type = match[2],
						parameter_default = match[3];

					cli_tool_parameters[parameter_name] = { parameter_type, parameter_default, parameter_default };
				}
				else {
					assert(false);
				}
			}
			else
			{
				sim_obj_tool_filenames[subgroup] = ""s; // mark as used
			}
			++matches;
		}

		return cli_tool_parameters;
	}

	std::string process_command_line(std::string command_line, const SimObject_Containers& sim_objs,
		const std::map<std::string, std::string>& cli_input_filenames, const CliToolParameters& cli_tool_parameters)
	{
		// replace {sim_obj_1} with cli_input_filenames["sim_obj_1"] etc
		// implementation looks clumsy, could be made nicer using boost::regex_replace with lambda
		std::string result = command_line;
		std::sregex_iterator matches(command_line.begin(), command_line.end(), cli_tool_parameter_pattern);
		std::sregex_iterator end;

		size_t pos_ofs = 0;
		auto replace_str = [&pos_ofs](std::string& result, const std::smatch& match, std::string replace_str)->void
		{
			auto pos = match.position() + pos_ofs;
			auto len = match.length();
			result.replace(pos, len, replace_str);
			pos_ofs += replace_str.length() - len;
		};

		// Iterate over all matches  
		while (matches != end) {
			std::smatch match = *matches;
			std::string subgroup = match[1];

			if (subgroup.find(':') == std::string::npos) { // {sim_obj},...
				if (cli_input_filenames.contains(subgroup)) {
					// Replace the match with "LONG_WORD"
					replace_str(result, match, cli_input_filenames.at(subgroup));
				}
				else {
					message("Dont know how to expand {" + subgroup + "} in cli tool"s, QMessageBox::Critical);
					return ""s;
				}
			}
			else // cli tool parameter, e.g. LSTACK
			{
				std::smatch match2;
				if (std::regex_search(subgroup, match2, cli_tool_parameter_detail_pattern)) {
					std::string parameter_name = match2[1];
					assert(cli_tool_parameters.contains(parameter_name));
					std::string actual_value = cli_tool_parameters.at(parameter_name).actual_value;

					replace_str(result, match, actual_value);
				}
				else {
					assert(false);
				}

			}
			++matches;
		}

		return result;
	}
}

Generic_cli_tool::Generic_cli_tool(const::nlohmann::json& config) :
	command_line_label("Command Line:"),
	log_output_label("Log Output:"),
	error_message_label("Error Message:")
{
	command_line_string = config.at("command line").get<std::string>();

	setLayout(&layout);

	command_line.setText(command_line_string.c_str());

	//command_line.setFixedHeight(100);
	command_line_s.setFixedHeight(50);
	command_line.setReadOnly(true);
	command_line_s.setWidget(&command_line);
	command_line_s.setWidgetResizable(true);
	//command_line_s.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	command_line.setWordWrapMode(QTextOption::NoWrap);
	command_line_s.setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

	int act_row = 0;

	layout.addWidget(&command_line_label, act_row, 0);
	layout.addWidget(&command_line_s, act_row, 1);

	act_row++;

	SimObj_tool_filenames cli_input_filenames; // should be stored in 'this'
	auto cli_tool_parameters = preprocess_command_line(command_line_string, cli_input_filenames); 
	for (const auto& [par_name, par] : cli_tool_parameters)
	{
		cli_parameter_name.push_back(std::make_unique<QLabel>());
		cli_parameter_value.push_back(std::make_unique<QLineEdit>());

		cli_parameter_name.back()->setText(par_name.c_str());;
		cli_parameter_value.back()->setText(par.actual_value.c_str());;

		layout.addWidget(cli_parameter_name.back().get(), act_row, 0);
		layout.addWidget(cli_parameter_value.back().get(), act_row, 1);

		act_row++;
	}

	log_output.setText("<log output>");
	log_output_s.setWidget(&log_output);
	log_output_s.setWidgetResizable(true);
	log_output.setWordWrapMode(QTextOption::NoWrap);
	log_output.setReadOnly(true);
	layout.addWidget(&log_output_label, act_row, 0);
	layout.addWidget(&log_output_s, act_row, 1);

	error_message.setText("<error_message>");
	error_message_s.setFixedHeight(150);
	error_message_s.setWidget(&error_message);
	error_message_s.setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

	error_message_s.setWidgetResizable(true);
	error_message.setWordWrapMode(QTextOption::NoWrap);
	error_message.setReadOnly(true);

	act_row++;

	layout.addWidget(&error_message_label, act_row, 0);
	layout.addWidget(&error_message_s, act_row, 1);
}

/**
	\todo allow using different versions of evaluate_SimulationObject
	\todo make stack size an additional argument
	\todo make "output_flags" an additional argument
	\todo add run time measurement
*/
std::string Generic_cli_tool::exec(const SimObject_Containers& sim_objs)
{
	SimObj_tool_filenames cli_input_filenames;

	auto cli_tool_parameters = preprocess_command_line(command_line_string, cli_input_filenames); // could be stored in 'this'

	for (size_t i = 0; i < sim_objs.size(); ++i) {
		cli_input_filenames["sim_obj_"s + std::to_string(i + 1)] = sim_objs[i].sim_object_filename;
	}

	cli_input_filenames["log_output"] = "";
	cli_input_filenames["err_output"] = "";
	for (auto cli_file_parameter : { "sim_obj_new"s, "err_output"s, "log_output"s})
	{
		if (cli_input_filenames.contains(cli_file_parameter)) {
			char tmpnam_SimObject[2048];
			std::tmpnam(tmpnam_SimObject);
			cli_input_filenames[cli_file_parameter] = tmpnam_SimObject;
		}
	}

	// get actual values for cli tool parameters

	int i = 0;
	for (const auto& [par_name, par] : cli_tool_parameters)
	{
		std::string value = cli_parameter_value[i]->text().toStdString();
		cli_tool_parameters[par_name].actual_value = value;

		i++;
	}

	auto command_line_string_processed = process_command_line(command_line_string, sim_objs, cli_input_filenames, cli_tool_parameters);

	if (command_line_string_processed.empty()) return ""s;

	command_line.setText(command_line_string_processed.c_str());

	auto start = std::chrono::high_resolution_clock::now();
	int ret = std::system(command_line_string_processed.c_str());
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	//std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;

	{
		std::ifstream ifs(cli_input_filenames["log_output"]);
		std::string log_file_contents((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		log_file_contents += "\n{Execution time: "s + std::to_string(static_cast<double>(duration.count()) * 1.0e-6) + " s}\n"s;

		log_output.setText(log_file_contents.c_str());
	}
	{
		std::ifstream ifs(cli_input_filenames["err_output"]);
		std::string err_message((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		error_message.setText(err_message.c_str());
	}
	if (ret != 0)
	{
		message("cli tool issued an error -- see tab for details", QMessageBox::Critical);
		return ""s;
	}


	if(cli_input_filenames.contains("sim_obj_new")) {
		if (std::filesystem::exists(cli_input_filenames.at("sim_obj_new"))) {
			// https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
			std::ifstream sim_obj_file(cli_input_filenames.at("sim_obj_new"));

			if (sim_obj_file.peek() == std::ifstream::traits_type::eof()) {
				if (ret == 0) {
					message("cli tool did not generate sim obj though it should have!", QMessageBox::Critical);
				}
			}
			else {
				return std::string(cli_input_filenames.at("sim_obj_new"));
			}
		}
	}
	return ""s;
}
