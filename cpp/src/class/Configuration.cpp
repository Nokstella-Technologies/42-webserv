#include <Configuration.hpp>



Config::Configuration::Configuration() : isError(false), isLocation(false), location("")
{
}

Config::Configuration::~Configuration()
{
    for (std::vector<Server *>::iterator it = _config.begin(); it != _config.end(); it++)
        delete *it;
    _config.clear();
}

const std::vector<Config::Server *> & Config::Configuration::getConfig() const {
    return _config;
}

void Config::Configuration::_populateServer(Config::Server *server, std::string line) {
    if (utils::starts_with(line, "[")) {
        line = utils::trim(utils::trim(line, "[]"));
        if (line ==  std::string(SLABEL) + "." + std::string(ERRORLABEL)) {
            isError = true;
            isLocation = false;
        } else if (utils::starts_with(line, SLABEL + std::string(".") + LOCALABEL + std::string(".\""))) {
            isLocation = true;
            isError = false;
            std::vector<std::string> split = utils::split(line, ".");
            location = utils::trim(split[2]," \"");
        } else {
            throw Excp::BadLabel(line);
        }
    } else if (line.find("=")) {
        std::string key = utils::trim(line.substr(0, line.find("=")));
        std::string value = utils::trim(line.substr(line.find("=") + 1));
        if (isError)
            server->setErrorPages(atoi(key.c_str()), value);
        else if (isLocation) 
            server->setLocationsConfig(location, key, value);
        else
            server->setConfig(key, value);
    } else {
        throw Excp::BadLabel(line);
    }
}

void Config::Configuration::loadMimeTypes(std::string filename) throw(Excp::FileNotOpen, Excp::WrongFile) {
    std::ifstream config_file(filename.c_str());
    std::string line;

    if (!utils::ends_with(filename, ".toml"))
        throw Excp::WrongFile(filename);
    if (!config_file.is_open())
        throw Excp::FileNotOpen(filename);
    while (std::getline(config_file, line))
    {
        if (line.find("#") != std::string::npos)
            line = line.substr(0, line.find("#"));
        if (line.empty())
            continue;
        line = utils::trim(line);
        if (utils::starts_with(line, "[[") || utils::starts_with(line, "[")) {
            line = utils::trim(utils::trim(line, "[]"));
            if (line == MIMETYPES) {
                continue ;
            }
            throw Excp::BadLabel(line);
        }
        if (line.find("=")) {
            std::string key = utils::trim(line.substr(0, line.find("=")), " \t\r\n \"");
            std::vector<std::string> value = _parseArray<std::string>(utils::trim(line.substr(line.find("=") + 1)));
            for (std::vector<std::string>::iterator it = value.begin(); it != value.end(); it++) {
                mimeTypes[*it] = key;
            }
        }
    }
}


void Config::Configuration::loadFile(std::string filename) throw(Excp::FileNotOpen, Excp::WrongFile, Excp::BadLabel)
{
    std::ifstream config_file(filename.c_str());
    std::string line;
    Config::Server *server = NULL;

    if (!utils::ends_with(filename, ".toml"))
        throw Excp::WrongFile(filename);
    if (!config_file.is_open())
        throw Excp::FileNotOpen(filename);
    while (std::getline(config_file, line))
    {
        if (line.find("#") != std::string::npos)
            line = line.substr(0, line.find("#"));
        if (line.empty())
            continue;
        line = utils::trim(line);
        if (utils::starts_with(line, "[[")) {
            line = utils::trim(utils::trim(line, "[]"));
            if (line == SLABEL) {
                if (server != NULL) {
                    server->parseConfig();
                    server->setMimeType(&mimeTypes);
                    _config.push_back(server);
                    isError = false;
                    isLocation = false;
                    location = "";
                }
                server = new Config::Server();
            }
            else
                throw Excp::BadLabel(line);
        }
        else {
            _populateServer(server, line);
        }
    }
    if (server != NULL) {
        server->parseConfig();
        _config.push_back(server); 
    }
}

WebServer::WebServer *Config::Configuration::createSockets(){
    WebServer::WebServer *webServer = new WebServer::WebServer();
     for (std::vector<Config::Server *>::const_iterator server = _config.begin(); server != _config.end(); server++) {
        std::vector<std::string> port = (*server)->getPort();
        for (std::vector<std::string>::iterator ipPort = port.begin(); ipPort != port.end(); ipPort++) {
            webServer->addServerToSocket(SocketServer::getFullIp(*ipPort), *server);
        }
    }
    return webServer;
}

std::ostream &operator<<(std::ostream &os, const Config::Configuration &config) {
    for (std::vector<Config::Server *>::const_iterator it = config.getConfig().begin(); it != config.getConfig().end(); it++) {
        os << **it;
    }
    return os;
}
