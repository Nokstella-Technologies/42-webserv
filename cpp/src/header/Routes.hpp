#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <map>
#include <iostream>

namespace Config {
    class Routes
    {
        private:
            std::map<std::string, std::string> config;

        public:
            Routes(/* args */);
            ~Routes();
            const std::map<std::string, std::string> & getConfig() const;
            void setConfig(std::string key, std::string value);
           
    };
};

std::ostream &operator<<(std::ostream &os, const Config::Routes &routes);



#endif