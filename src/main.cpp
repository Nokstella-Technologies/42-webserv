#include <Server.hpp>

int main(int argc, char *argv[]) {
  
  std::string path = "./configuration/server.toml";

  if(argc != 2) {
    FILE *file = std::fopen(path.c_str(), "r");
      if (file == NULL) {
        std::cerr << "ERROR" << std::endl 
          << "Usage: "<< argv[0] << "<filename>.toml" << std::endl
            << "Optional: save the .toml file in the same directory as the executable in the folder configuration as 'server.config'" << std::endl;
        return 1;
      }
    std::fclose(file);
  }
  if(argc == 2)
    path = argv[1];

  return 0;
}