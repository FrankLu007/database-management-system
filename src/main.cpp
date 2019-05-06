#include <cstdio>
#include <cstring>
#include "table.cpp"

TABLE table;
extern bool print_data;
extern std::FILE * fp;

CONDITION * parse_condition(char * command)
{
	static char sub_command[250];
	char * item;
	if(item = std::strstr(command, " where "))
	{
		std::memset(sub_command, 0, 250);
		char * limit_p = std::strstr(command, " limit "), * offset_p = std::strstr(command, " offset ");
		if(limit_p && offset_p) std::strncpy(sub_command, item + 7, std::min(offset_p, limit_p) - item - 7);
		else if(!limit_p && !offset_p) std::strcpy(sub_command, item + 7);
		else if(limit_p) std::strncpy(sub_command, item + 7, limit_p - item - 7);
		else std::strncpy(sub_command, item + 7, offset_p - item - 7);
		return new CONDITION(sub_command);
	}
	else return NULL;
}

int main(int N, char ** args)
{
	char command[250], * type;
	int len;
	CONDITION * condition;
	print_data = true;
	fp = stdout;
	while(1)
	{
		std::printf("db > ");
		std::fgets(command, 250, stdin);
		len = std::strlen(command);
		command[len - 1] = 0;
		if(len <= 3) continue;
		condition = parse_condition(command);
		type = std::strtok(strdup(command), " \n");

		if(!std::strcmp(type, "select"))
		{
			std::vector <int> content;
			content.clear();
			char * item;
			
			int limit = table.size(), offset = 0;

			if(std::strstr(command, " sum(") || std::strstr(command, " avg(") || std::strstr(command, " count("))
			{
				bool first = true;
				int target;
				std::fprintf(fp, "(");
				type = std::strtok(command, " ");
				while(type = std::strtok(NULL, "(), "))
				{
					target = -1;
					if(!std::strcmp(type, "sum"))
					{
						item = std::strtok(NULL, "() ,");
						if(!std::strcmp(item, "id")) target = 0;
						else if(!std::strcmp(item, "age")) target = 3;
						if(!first) fprintf(fp, ", ");
						else first = false;
						fprintf(fp, "%d", (int)table.aggre(condition, target, 0));
					}
					else if(!std::strcmp(type, "avg"))
					{
						item = std::strtok(NULL, "() ,");
						if(!std::strcmp(item, "id")) target = 0;
						else if(!std::strcmp(item, "age")) target = 3;
						if(!first) fprintf(fp, ", ");
						else first = false;
						fprintf(fp, "%.3lf", table.aggre(condition, target, 1));
					}
					else if(!std::strcmp(type, "count"))
					{
						item = std::strtok(NULL, "() ,");
						if(!first) fprintf(fp, ", ");
						else first = false;
						fprintf(fp, "%d", (int)table.aggre(condition, target, 2));
					}
				}
				std::fprintf(fp, ")\n");
				continue;
			}
			while(item = std::strtok(NULL, ", "))
			{
				if(!std::strncmp(item, "id", 2)) content.push_back(0);
				else if(!std::strncmp(item, "name", 4)) content.push_back(1);
				else if(!std::strncmp(item, "email", 5)) content.push_back(2);
				else if(!std::strncmp(item, "age", 3)) content.push_back(3);
				else break;
			}
			if(!content.size()) {for(int i = 0 ; i < 4 ; i++) content.push_back(i);}
			if(item)
			do
			{
				if(!std::strcmp(item, "limit")) limit = std::atoi(std::strtok(NULL, ", "));
				else if(!std::strcmp(item, "offset")) offset = std::atoi(std::strtok(NULL, ", "));
			} while(item = std::strtok(NULL, ", "));
			table.print(condition, content, limit, offset);
		}
		else if(!std::strcmp(type, "insert")) 
		{
			if(!table.insert(command)) 
				std::fprintf(stderr, "Insert fail with command : %s", command);
		}
		else if(!std::strcmp(type, "update"))
		{
			int target;
			type = std::strtok(std::strstr(command, " set ") + 5, "=");
			switch(std::strlen(type))
			{
				case 2 :
					target = 0;
					break;
				case 3 :
					target = 3;
					break;
				case 4 :
					target = 1;
					break;
				case 5 :
					target = 2;
			}
			table.update(condition, target, std::strtok(NULL, " "));
		}
		else if(!std::strcmp(type, "delete")) table.del(condition);
		else if(!std::strcmp(type, ".output"))
		{
			std::strtok(strdup(command), " \n");
			char * filename = std::strtok(NULL, " \n");
			if(fp != stdout) std::fclose(fp);
			fp = std::strcmp(filename, "stdout") ? std::fopen(filename, "w") : stdout;
			if(!fp) fprintf(stderr, "Output error with command : %s", command);
		}
		else if(!std::strcmp(type, ".load"))
		{

		}
		else if(!std::strcmp(type, ".exit")) return 0;
		else fprintf(stderr, "Unrecognized command %s", command);
	}
}