#include <stdio_ext.h>
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
		char * limit_p = std::strstr(command, " limit "), * offset_p = std::strstr(command, " offset ");// used to check the end of condition
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
	int len, target_item, target_table;
	CONDITION * condition = NULL;
	FILE * fp = stdout;
	TABLE table[2]; // first is 'user', the other is 'like'

	while(true)
	{
		// if we output to file then we don't need this
		if(fp == stdout) std::printf("db > ");

		// get the command and filter it if it's lenght is shorter than or equal to 3
		std::fgets(command, 250, stdin);
		len = std::strlen(command) - 1;
		if(len <= 3) continue;
		command[len] = 0;

		// reset the condition
		if(condition) condition->clean();
		condition = parse_condition(command);

		// get the first word and store in 'type'
		type = std::strtok(strdup(command), " ");

		if(!std::strcmp(type, "select"))
		{
			std::vector <int> content;
			char * item, * tmp;
			unsigned limit = (tmp = std::strstr(command, " limit ")) ? std::atoll(tmp + 7) : -1, // get the value of limit
					offset = (tmp = std::strstr(command, " offset ")) ? std::atoll(tmp + 7) : 0; // get the value of offset
			
			content.clear();
			if(std::strstr(command, " sum(") || std::strstr(command, " avg(") || std::strstr(command, " count(")) // if the query contains aggregation
			{
				if(offset || !limit) {std::fprintf(stderr, "Empty set\n"); continue;}
				bool first = true;

				std::fprintf(fp, "(");
				while(type = std::strtok(NULL, "(, "))
				{
					if(!std::strcmp(type, "sum"))
					{
						item = std::strtok(NULL, ")");
						target_item = distinguish_item(item);
						if(!first) std::fprintf(fp, ", ");
						else first = false;
						std::fprintf(fp, "%d", (int)table[target_table].aggre(condition, target_item, 0));
					}
					else if(!std::strcmp(type, "avg"))
					{
						item = std::strtok(NULL, ")");
						target_item = distinguish_item(item);
						if(!first) std::fprintf(fp, ", ");
						else first = false;
						std::fprintf(fp, "%.3lf", table[target_table].aggre(condition, target_item, 1));
					}
					else if(!std::strcmp(type, "count"))
					{
						item = std::strtok(NULL, ")");
						if(!first) std::fprintf(fp, ", ");
						else first = false;
						fprintf(fp, "%d", (int)table[target_table].aggre(condition, target_item, 2));
					}
				}
				std::fprintf(fp, ")\n");
				continue;
			}

			while(item = std::strtok(NULL, ", "))
			{
				target_item = distinguish_item(item);
				if(target_item != -1) content.push_back(target_item);
				else break;
			}

			target_table = std::strcmp(std::strtok(NULL, " "), "like") ? 0 : 1; // get the name of table
			limit = limit == -1 ? table[target_table].size() : limit;

			if(!content.size()) // if the query field is '*'
			{
				if(target_table) {content.push_back(0);content.push_back(3);}
				else for(int i = 0 ; i < 4 ; i++) content.push_back(i);
			}
			if(!table[target_table].print(condition, content, limit, offset, fp)) std::fprintf(stderr, "Empty set\n");
		}
		else if(!std::strcmp(type, "insert")) 
		{
			std::strtok(NULL, " "); //skip the word 'into'
			target_table = std::strcmp(std::strtok(NULL, " "), "like") ? 0 : 1; // get the name of table
			if(!table[target_table].insert(strdup(command))) 
				std::fprintf(stderr, "Invalid insert with command : %s\n", command);
		}
		else if(!std::strcmp(type, "update"))
		{
			std::strtok(NULL, " "); //skip the table name
			std::strtok(NULL, " "); //skip the word 'set'
			target_item = distinguish_item(std::strtok(NULL, " "));
			if(!target_item && (table[target_table].aggre(condition, 0, 2) > 1 || table[target_table].id_check(std::atoi(std::strstr(command, "=") + 2))))
			{
				std::fprintf(stderr, "Invalid update with command : %s\n", command);
				continue;
			}
			table[0].update(condition, target_item, std::strtok(NULL, " ="));
		}
		else if(!std::strcmp(type, "delete")) table[0].del(condition);
		else if(!std::strcmp(type, ".output"))
		{
			__fpurge(stdout);
			std::strtok(strdup(command), " \n");
			char * filename = std::strtok(NULL, " \n");
			if(fp != stdout) std::fclose(fp);
			fp = std::strcmp(filename, "stdout") ? std::fopen(filename, "w") : stdout;
		}
		else if(!std::strcmp(type, ".help")) std::printf("%s", help_msg);
		else if(!std::strcmp(type, ".exit")) 
		{
			//table.store();
			return 0;
		}
		else fprintf(stderr, "Unrecognized command %s\n", command);
	}
}