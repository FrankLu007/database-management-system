#include <set>
#include <unordered_map>
#include "condition.cpp"

class TABLE
{
	DATA * head, * tail;
	std::unordered_map <int, DATA *> id_map;
	std::unordered_map <int, std::set<DATA *>>age_map;
	std::unordered_map <std::string, std::set<DATA *>> name_map, email_map;
	unsigned num_data;
public:
	TABLE(): head(NULL), tail(NULL), num_data(0)
	{
		id_map.clear();
		age_map.clear();
		name_map.clear();
		email_map.clear();
	}
	bool id_check(int id) {return id_map.find(id) != id_map.end();}
	bool insert(char * command)
	{
		int id, age;
		char * name, * email;

		std::strtok(command, " "); // skip the word 'insert'
		std::strtok(NULL, " "); // skip the word 'into'
		if(std::strcmp(std::strtok(NULL, " "), "like")) // if the target table is 'user'
		{
			id = std::atoi(std::strtok(NULL, " "));
			if(id_check(id)) return false;
			name = std::strtok(NULL, " ");
			email = std::strtok(NULL, " ");
			age = std::atoi(std::strtok(NULL, " "));
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
			id_map[id] = tail;
			name_map[std::string(name)].insert(tail);
			email_map[std::string(name)].insert(tail);
			age_map[age].insert(tail);
		}
		else // if the target table is 'like'
		{
			id = std::atoi(std::strtok(NULL, " ")); // id1
			age = std::atoi(std::strtok(NULL, " ")); // id2
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
			id_map[id] = tail;
			age_map[age].insert(tail);
		}
		num_data++;
		return true;
	}
	unsigned print(CONDITION * condition, std::vector <int> & content, int limit, int offset, FILE * fp)
	{
		DATA * cur = head;
		if(!cur || !limit) return 0;
		
		unsigned num = 0;
		while(limit && cur)
		{
			if(!condition || condition->test(cur)) 
			{
				if(offset) offset--;
				else 
				{
					cur->print(content, fp); 
					limit--;
					num++;
				}
			}
			cur = cur->next;
		}
		return num;
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
					//id_set.erase(cur->id);
					delete cur;
					num_data--;
					cur = head;
				}
				else
				{
					last->next = cur->next;
					//id_set.erase(cur->id);
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
							// if(id_set.find(id) != id_set.end()) break;
							// id_set.erase(cur->id);
							// id_set.insert(id);
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