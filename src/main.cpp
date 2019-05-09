#include <cstdio>
#include <cstring>
#include "table.cpp"

const char help_msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";

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
	static char command[250], * type;
	int len;
	CONDITION * condition = NULL;
	FILE * fp = stdout;
	TABLE table;
	if(N == 2) table.load(args[1]);
	while(true)
	{
		if(fp == stdout) std::printf("db > ");
		std::fgets(command, 250, stdin);
		len = std::strlen(command) - 1;
		if(len <= 3) continue;
		command[len] = 0;
		if(condition) condition->clean();
		condition = parse_condition(command);
		type = std::strtok(strdup(command), " \n");

		if(!std::strcmp(type, "select"))
		{
			std::vector <int> content;
			char * item, * tmp;
			unsigned limit = (tmp = std::strstr(command, " limit ")) ? std::atoll(tmp + 7) : table.size(), 
					offset = (tmp = std::strstr(command, " offset ")) ? std::atoll(tmp + 7) : 0;
			
			content.clear();
			if(std::strstr(command, " sum(") || std::strstr(command, " avg(") || std::strstr(command, " count("))
			{
				if(offset || !limit) {std::fprintf(stderr, "Empty set\n"); continue;}
				bool first = true;
				int target;

				std::fprintf(fp, "(");
				while(type = std::strtok(NULL, "(), "))
				{
					target = -1;
					if(!std::strcmp(type, "sum"))
					{
						item = std::strtok(NULL, "() ,");
						if(!std::strcmp(item, "id")) target = 0;
						else if(!std::strcmp(item, "age")) target = 3;
						if(!first) std::fprintf(fp, ", ");
						else first = false;
						fprintf(fp, "%d", (int)table.aggre(condition, target, 0));
					}
					else if(!std::strcmp(type, "avg"))
					{
						item = std::strtok(NULL, "() ,");
						if(!std::strcmp(item, "id")) target = 0;
						else if(!std::strcmp(item, "age")) target = 3;
						if(!first) std::fprintf(fp, ", ");
						else first = false;
						fprintf(fp, "%.3lf", table.aggre(condition, target, 1));
					}
					else if(!std::strcmp(type, "count"))
					{
						item = std::strtok(NULL, "() ,");
						if(!first) std::fprintf(fp, ", ");
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
			if(!table.print(condition, content, limit, offset, fp)) std::fprintf(stderr, "Empty set\n");
		}
		else if(!std::strcmp(type, "insert")) 
		{
			if(!table.insert(strdup(command))) 
				std::fprintf(stderr, "Invalid insert with command : %s\n", command);
		}
		else if(!std::strcmp(type, "update"))
		{
			int target;
			std::strtok(NULL, " ");
			std::strtok(NULL, " ");
			type = std::strtok(NULL, "=");
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
			if(!target && (table.aggre(condition, 0, 2) > 1 || table.id_check(std::atoi(std::strstr(command, "=") + 1))))
			{
				std::fprintf(stderr, "Invalid update with command : %s\n", command);
				continue;
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
		}
		else if(!std::strcmp(type, ".load"))
		{
			table.del(NULL);
			type = std::strtok(NULL, " \n");
			table.load(type);
		}
		else if(!std::strcmp(type, ".help")) std::printf("%s", help_msg);
		else if(!std::strcmp(type, ".exit")) 
		{
			table.store();
			return 0;
		}
		else fprintf(stderr, "Unrecognized command %s\n", command);
	}
}