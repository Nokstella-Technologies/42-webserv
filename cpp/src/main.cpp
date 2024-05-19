#include "Configuration.hpp"



int main(int argc, char *argv[])
{
	Config::Configuration config;
	WebServer *webserver = NULL;

	std::string path = "./configuration/server.toml";
	std::string mimeType = "./configuration/mimeType.toml";
	if (argc != 2)
	{
		FILE *file = std::fopen(path.c_str(), "r");
		if (file == NULL)
		{
			std::cerr << "Usage: " << argv[0] << " <filename>.toml" << std::endl
					  << "Usage optional: save the file in the same directory as the executable in the folder configuration as 'server.config'" << std::endl;
			return 1;
		}
		std::fclose(file);
	}
	else
		path = argv[1];
	try
	{
		std::cout << "Loading file: " << path << std::endl;
		config.loadMimeTypes(mimeType);
		initialize_http_messages();
		config.loadFile(path);
		std::cout << config;
		webserver = config.createSockets();
		std::cout << "Start Server:" << path << std::endl;
		webserver->start();
		delete webserver;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}