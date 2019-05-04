#include <cstring>
#include <set>
#include "condition.cpp"

class TABLE
{
	DATA * head, * tail;
	std::set <int> id_set;
	unsigned num_data;
public:
	TABLE(): head(NULL), tail(NULL), num_data(0)
	{
		id_set.clear();
	}
	bool insert(char * command)
	{
		int id, age;
		char * name, * email;
		DATA * cur = head;
		std::strtok(command, " ");
		id = std::atoi(std::strtok(NULL, " "));
		if(id_set.find(id) != id_set.end()) return false;
		name = std::strtok(NULL, " ");
		email = std::strtok(NULL, " ");
		age = std::atoi(std::strtok(NULL, " "));
		id_set.insert(id);
		num_data++;
		if(head)
		{
			tail->next = new DATA(id, name, email, age);
			tail = tail->next;
		}
		else
		{
			head = new DATA(id, name, email, age);
			tail = head;
		}
		return true;
	}
	int print(CONDITION * root, std::vector <int> & content, int limit, int offset)
	{
		DATA * cur = head;
		while(offset-- && cur) cur = cur->next;
		if(!cur || !limit) return 0;
		
		int num = 0;
		while(limit-- && cur)
		{
			if(!root || root->test(cur)) {cur->print(content); num++;}
			else limit++;
			cur = cur->next;
		}
		return num = 0;
	}
	int size() {return num_data;}
};