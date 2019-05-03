#include <cstdio>
#include <cstring>
#include "table.cpp"

TABLE table;

int main(int N, char ** args)
{
	char command[250], * type;
	while(1)
	{
		std::printf("db > ");
		std::gets(command);
		type = std::strtok(command, " ");
		if(std::strcmp(type, "select"))
		{

		}
		else if(std::strcmp(type, "insert")) fprintf(srderr, "Insert %s.\n", table.insert(command) ? "success", "fail");
		else if(std::strcmp(type, "update"))
		{

		}
		else if(std::strcmp(type, "delete"))
		{

		}
		else fprintf(stderr, "Unrecognized command.\n");
	}
}