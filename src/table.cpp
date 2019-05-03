#include <cstring>
#include <set>
#include "data.cpp"

class TABLE
{
	DATA * head, * tail;
	std::set <int> id_set;
public:
	TABLE(): head(NULL), tail(NULL) 
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
}