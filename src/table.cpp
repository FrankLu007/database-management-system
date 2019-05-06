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
		std::strtok(command, " ");
		id = std::atoi(std::strtok(NULL, " "));
		if(id_set.find(id) != id_set.end()) return false;
		name = std::strtok(NULL, " ");
		email = std::strtok(NULL, " ");
		age = std::atoi(std::strtok(NULL, " "));
		id_set.insert(id);
		if(num_data)
		{
			tail->next = new DATA(id, name, email, age);
			tail = tail->next;
		}
		else
		{
			head = new DATA(id, name, email, age);
			tail = head;
		}
		num_data++;
		return true;
	}
	void print(CONDITION * condition, std::vector <int> & content, int limit, int offset)
	{
		DATA * cur = head;
		while(offset-- && cur) cur = cur->next;
		if(!cur || !limit) return ;
		
		while(limit-- && cur)
		{
			if(!condition || condition->test(cur)) cur->print(content);
			else limit++;
			cur = cur->next;
		}
	}
	void del(CONDITION * condition)
	{
		DATA * cur = head, * last = NULL;
		while(cur)
		{
			if(!condition || condition->test(cur))
			{
				if(!last)
				{
					head = cur->next;
					id_set.erase(cur->id);
					delete cur;
					num_data--;
					cur = head;
				}
				else
				{
					last->next = cur->next;
					id_set.erase(cur->id);
					delete cur;
					num_data--;
					cur = last->next;
					if(!cur) tail = last;
				}
			}
			else cur = (last = cur)->next;
		}
	}
	double aggre(CONDITION * condition, int content, int mode)
	{
		int sum = 0, count = 0;
		DATA * cur = head;

		while(cur)
		{
			if(!condition || condition->test(cur))
			{
				if(!content) sum += cur->id;
				else if(content == 3) sum += cur->age;
				count++;
			}
			cur = cur->next;
		}
		if(!mode) return sum;
		if(mode & 1) return (double)sum/count;
		return count;
	}
	void update(CONDITION * condition, int content, char * data)
	{
		DATA * cur = head;
		int id, age;
		std::string name, email;
		switch(content)
		{
			case 0 :
				id = std::atoi(data);
				break;
			case 1 :
				name = std::string(data);
				break;
			case 2 :
				email = std::string(data);
				break;
			case 3 :
				age = std::atoi(data);
		}
		while(cur)
		{
			if(!condition || condition->test(cur))
			{
				switch(content)
				{
					case 0 :
						if(id != cur->id)
						{
							if(id_set.find(id) != id_set.end()) break;
							id_set.erase(cur->id);
							id_set.insert(id);
						}
						cur->id = id;
						break;
					case 1 :
						cur->name = name;
						break;
					case 2 :
						cur->email = email;
						break;
					case 3 :
						cur->age = age;
				}
			}
			cur = cur->next;
		}
	}
	int size() {return num_data;}
};