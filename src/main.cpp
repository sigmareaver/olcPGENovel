#include "Application.h"
#include "ConfigData.h"
#include "Serializer.h"

int main(int argc, char* argv[])
{
	Application& app = Application::GetInstance();

	Serializer::CheckDirs();
	Config::ConfigData app_config;
	if (!Serializer::LoadConfigFile(app_config))
	{
		Config::GenerateDefaultConfig(app_config);
		Serializer::SaveConfigFile(app_config);
	}

	app.Construct(app_config);
	app.Start();

	Application::Destroy();

	return 0;
}
