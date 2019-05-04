#include <cstdio>
#include <cstring>
#include "table.cpp"

TABLE table;
extern bool print_data;
extern std::FILE * fp;

int main(int N, char ** args)
{
	char command[250], * type;
	int len;
	print_data = true;
	fp = stdin;
	while(1)
	{
		//std::printf("db > ");
		std::fgets(command, 250, stdin);
		len = std::strlen(command);
		type = std::strtok(strdup(command), " \n");

		if(!std::strcmp(type, "select"))
		{
			std::vector <int> content;
			content.clear();
			char * item;
			CONDITION * root = NULL;
			int start = 0, end = len - 1, limit = table.size(), offset = 0;

			while(item = std::strtok(NULL, ", "))
			{
				if(!std::strcmp(item, "*")) break;
				else if(!std::strncmp(item, "id", 2)) content.push_back(0);
				else if(!std::strncmp(item, "name", 4)) content.push_back(1);
				else if(!std::strncmp(item, "email", 5)) content.push_back(2);
				else if(!std::strncmp(item, "age", 3)) content.push_back(3);
				else break;
			}
			if(!content.size()) {for(int i = 0 ; i < 4 ; i++) content.push_back(i);}

			do
			{
				if(!std::strcmp(item, "limit")) limit = std::atoi(std::strtok(NULL, ", "));
				else if(!std::strcmp(item, "offset")) offset = std::atoi(std::strtok(NULL, ", "));
			} while(item = std::strtok(NULL, ", "));
			
			for(int i = 0 ; i < len ; i++)
			{
				if(command[i] == 'w' && i + 4 < len && !std::strncmp(command + i, "where", 5)) start = i + 5;
				else if(command[i] == 'l' && i + 4 < len && !std::strncmp(command + i, "limit", 5)) {end = i - 1; break;}
				else if(command[i] == 'o' && i + 5 < len && !std::strncmp(command + i, "offset", 6)) {end = i - 1; break;}
			}
			if(start) root = new CONDITION(command, start, end);
			table.print(root, content, limit, offset);
		}
		else if(!std::strcmp(type, "insert")) fprintf(stderr, "Insert %s.\n", table.insert(command) ? "success" : "fail");
		else if(!std::strcmp(type, "update"))
		{

		}
		else if(!std::strcmp(type, "delete"))
		{

		}
		else if(!std::strcmp(type, ".output"))
		{
			std::strtok(command, " \n");
			char * filename = std::strtok(NULL, " \n");
			fp = std::strcmp(filename, "stdout") ? std::fopen(filename, "w") : stdin;
		}
		else if(!std::strcmp(type, ".load"))
		{

		}
		else if(!std::strcmp(type, ".exit")) return 0;
		else fprintf(stderr, "Unrecognized command %s", command);
	}
}