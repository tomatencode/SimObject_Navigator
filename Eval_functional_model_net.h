#pragma once
#include<qobject.h>
#include <qwidget.h>
#include<QHBoxLayout>
#include<qgridlayout.h>
#include<QVBoxLayout>
#include<QGroupBox>
#include<QLabel>
#include <QTextEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>


/**
	\todo Edit field for stack size

*/
class Eval_functional_model_net : public QWidget {
	Q_OBJECT

private:
	QGridLayout     layout;

	QPushButton     get_SimObj;

	QTextEdit		sim_obj,
		log_output,
		error_message;
	QScrollArea sim_obj_s,
		log_output_s,
		error_message_s;
	QLabel
		command_line_label,
		log_output_label,
		error_message_label;

	std::string		url = "127.0.0.1";
	int port		= 5000;
	std::string		sim_obj_, log_output_;

public:
	Eval_functional_model_net();

	std::string  exec(std::string sim_obj_name);
public slots:
#ifdef BUILD_WITH_EXPERIMENTAL_WEB_API
    void get_demo_SimulationObject();
#endif
};
