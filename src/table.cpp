#include <cstring>
#include <set>
#include "condition.cpp"

class TABLE
{
	DATA * head, * tail;
	std::set <int> id_set;
	unsigned num_data;
	char * filename;
public:
	TABLE(): head(NULL), tail(NULL), num_data(0), filename(NULL)
	{
		id_set.clear();
	}
	bool id_check(int id) {return id_set.find(id) != id_set.end();}
	bool insert(char * command)
	{
		int id, age;
		char * name, * email;
		std::strtok(command, " ");
		id = std::atoi(std::strtok(NULL, " "));
		if(id_check(id)) return false;
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
	unsigned print(CONDITION * condition, std::vector <int> & content, int limit, int offset, FILE * fp)
	{
		DATA * cur = head;
		while(offset-- && cur) cur = cur->next;
		if(!cur || !limit) return 0;
		
		unsigned num = 0;
		while(limit-- && cur)
		{
			if(!condition || condition->test(cur)) {cur->print(content, fp); num++;}
			else limit++;
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
	void load(const char * _filename)
	{
		if(filename) store();
		const unsigned data_size = 520;
		static char name[256], email[256];
		static int id, age;
		FILE * fp = std::fopen(_filename, "rb");
		if(!fp) 
		{
			std::fprintf(stderr, "File doesn't exist : %s.\n", _filename);
			fp = std::fopen(_filename, "ab");
		}
		while(std::fread(&id, 1, 4, fp))
		{
			std::fread(name, 256, 1, fp);
			std::fread(email, 256, 1, fp);
			std::fread(&age, 1, 4, fp);
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
		}
		std::fclose(fp);
		printf("%s\n", _filename);
		filename = strdup(_filename);
	}
	void store()
	{
		if(!filename) {del(NULL); return ;}
		FILE * fp = std::fopen(filename, "wb");
		DATA * cur = head;
		while(cur)
		{
			std::fwrite(&(cur->id), 1, 4,  fp);
			std::fwrite(cur->name.c_str(), 256, 1,  fp);
			std::fwrite(cur->email.c_str(), 256, 1,  fp);
			std::fwrite(&(cur->age), 1, 4,  fp);
			cur = cur->next;
		}
		std::fclose(fp);
		del(NULL);
		delete filename;
	}
};