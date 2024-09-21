#include "../mainwindow.h"

#include<memory>

void MainWindow::generic_cli_tool_factory()
{
	if (not configuration.contains("cli_tools")) return;

	for (const auto& [cli_tool_name,cli_tool_config] : configuration.at("cli_tools").get<nlohmann::json::object_t>())
	{
		generic_cli_tools.push_back(std::make_unique<Generic_cli_tool>(cli_tool_config));
		main_tab->addTab(generic_cli_tools.back(), cli_tool_name.c_str());
	}
}
