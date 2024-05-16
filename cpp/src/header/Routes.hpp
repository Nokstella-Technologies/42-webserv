#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <map>
#include <iostream>

namespace Config {
    class Routes
    {
        private:
            std::map<std::string, std::string> config;
            std::map<int, std::string> errorPages;

        public:
            Routes(/* args */);
            ~Routes();
            const std::map<std::string, std::string> & getConfig() const;
            void setConfig(std::string key, std::string value);
            std::string getErrorPage(int error_code);
            std::string getRoot();
            void parseConfig();
            bool isCGI();
            bool isStatic();
            bool isRedirection();
            bool isUpload();

           
    };
};

std::ostream &operator<<(std::ostream &os, const Config::Routes &routes);



#endif