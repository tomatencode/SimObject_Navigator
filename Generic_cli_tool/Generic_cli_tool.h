#pragma once

#include <qobject.h>
#include <qwidget.h>
#include <QHBoxLayout>
#include <qgridlayout.h>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>

#include "SimObject_Container.hpp"

/**
	\brief UI for generic cli tool which uses SimObjects, e.g. SimObject Validator
*/
class Generic_cli_tool : public QWidget {
	Q_OBJECT

private:
	QGridLayout     layout;

	QTextEdit			command_line,
		log_output,
		error_message;
	QScrollArea command_line_s,
		log_output_s,
		error_message_s;
	QLabel
		command_line_label,
		log_output_label,
		error_message_label;

	std::vector<std::unique_ptr<QLabel>>		cli_parameter_name;
	std::vector<std::unique_ptr<QLineEdit>>		cli_parameter_value;

	std::string command_line_string;
public:
	Generic_cli_tool(const::nlohmann::json& config);
	std::string  exec(const SimObject_Containers& sim_objs);
};
