#include "Cgi.hpp"

Cgi::Cgi(std::string const fullPath, Request *request) {
	this->fullPath = fullPath;
	this->request = request;
}

Cgi::~Cgi() {
	std::remove("tempFile");
}

std::string Cgi::exec() {
	if (access(this->fullPath.c_str(), R_OK) != 0)
		return "Error";

	std::signal(SIGCHLD, SIG_IGN);
	int pid = fork();
	if (pid == 0)
	{
		prepareCGI();
		executeCgi();
		return "";
	}

	int attempt = 0;
	int status;
	while (attempt < 3)
	{
		waitpid(pid, &status, WNOHANG);

		if (status & WNOHANG)
		{
			attempt++;
			sleep(1);
		}
		else
			attempt = 3;
	}

	if (status & WNOHANG)
		kill(pid, SIGKILL);

	waitpid(pid, NULL, 0);

	return utils::getFile("tempFile");
}

void Cgi::prepareCGI() {
	std::vector<std::string> temp;

	this->tempFd = open("tempFile", O_CREAT | O_RDWR | O_TRUNC, 0644);

	temp.push_back("python3");
	temp.push_back(fullPath);
	temp.push_back(request->getBody());
	this->args = createArrayOfStrings(temp);

	temp.clear();
	temp.push_back("REQUEST_BODY=" + request->getBody());
	this->envp = createArrayOfStrings(temp);
}

char **Cgi::createArrayOfStrings(std::vector<std::string> const &vector) const {
	char **array = new char *[vector.size() + 1];

	for (std::size_t i = 0; i < vector.size(); ++i) {
		array[i] = new char[vector[i].size() + 1];
		std::strcpy(array[i], vector[i].c_str());
	}
	array[vector.size()] = NULL;

	return array;
}

void Cgi::destroyArrayOfStrings(char **array) const {
	for (char **p = array; *p; ++p)
		delete[] *p;
	delete[] array;
}

void Cgi::executeCgi() {
	if (dup2(this->tempFd, STDOUT_FILENO) == -1)
		throw std::runtime_error("dup2");
	if (execve("/usr/bin/python3", this->args, this->envp) == -1)
		throw std::runtime_error("execve");
}
