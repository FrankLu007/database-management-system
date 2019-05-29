#include <algorithm>
#include "condition.cpp"

class TABLE
{
	DATA * tmp;
	DATA_SET ans, data;
	ID_MAP id_map;
	AGE_MAP age_map;
	STR_MAP name_map, email_map;
	unsigned print_order;
public:
	TABLE(): print_order(0)
	{
		data.clear();
		id_map.clear();
		age_map.clear();
		name_map.clear();
		email_map.clear();
	}
	bool id_check(unsigned id) {return id_map.find(id) != id_map.end();}
	void insert(AGE_MAP & age_map, unsigned num, DATA * data)
	{
		if(age_map.find(num) == age_map.end()) age_map[num].clear();
		age_map[num].insert(data);
	}
	void insert(STR_MAP & str_map, const std::string & str, DATA * data)
	{
		if(str_map.find(str) == str_map.end()) str_map[str].clear();
		str_map[str].insert(data);
	}
	void erase(AGE_MAP & age_map, unsigned num, DATA * data)
	{
		age_map[num].erase(data);
		if(!age_map[num].size()) age_map.erase(num);
	}
	void erase(STR_MAP & str_map, const std::string & str, DATA * data)
	{
		str_map[str].erase(data);
		if(!str_map[str].size()) str_map.erase(str);
	}
	bool insert(char * command)
	{
		unsigned id, age;
		char * name, * email;

		std::strtok(command, " "); // skip the word 'insert'
		std::strtok(NULL, " "); // skip the word 'into'
		if(std::strcmp(std::strtok(NULL, " "), "like")) // if the target table is 'user'
		{
			id = std::atoll(std::strtok(NULL, " "));
			if(id_check(id)) return false;
			name = std::strtok(NULL, " ");
			email = std::strtok(NULL, " ");
			age = std::atoll(std::strtok(NULL, " "));

			tmp = new DATA(id, name, email, age, print_order++);
			data.insert(tmp);
			id_map[id] = tmp;
			insert(name_map, name, tmp);
			insert(email_map, email, tmp);
			insert(age_map, age, tmp);
		}
		else // if the target table is 'like'
		{
			id = std::atoll(std::strtok(NULL, " ")); // id1
			age = std::atoll(std::strtok(NULL, " ")); // id2

			tmp = new DATA(id, age, print_order++);
			data.insert(tmp);
			id_map[id] = tmp;
			insert(age_map, age, tmp);
		}
		return true;
	}
	unsigned print(CONDITION * condition, std::vector <unsigned> & content, unsigned limit, unsigned offset, FILE * fp)
	{
		if(!limit || offset >= data.size()) return 0;

		static std::vector <DATA *> output;
		ans.clear();

		if(!condition) output = std::vector <DATA *>(data.begin(), data.end()); // put them into a vector
		else 
		{	
			condition->test(id_map, name_map, email_map, age_map, ans, 0); // get the answer set
			output = std::vector <DATA *>(ans.begin(), ans.end()); // put them into a vector
		}
		std::sort(output.begin(), output.end(), cmp); // sort by 'print_order'

		limit  = std::max((unsigned)output.size(), limit + offset);
		for(unsigned i = offset ; i < limit ; i++) output[i]->print(content, fp);
		return limit - offset;
	}
	void del(CONDITION * condition)
	{
		if(!condition) //XDD, not good but fast
		{
			print_order = 0;
			id_map.clear();
			data.clear();
			age_map.clear();
			name_map.clear();
			email_map.clear();
			return;
		}

		DATA * tmp;
		ans.clear();

		condition->test(id_map, name_map, email_map, age_map, ans, 0); // get the answer set
		for(std::unordered_set <DATA *>::iterator it = ans.begin() ; it != ans.end() ; it++)
		{
			tmp = * it;
			id_map.erase(tmp->id);
			data.erase(tmp);
			erase(name_map, tmp->name, tmp);
			erase(email_map, tmp->email, tmp);
			erase(age_map, tmp->age, tmp);
		}
	}
	double aggre(CONDITION * condition, unsigned content, unsigned mode, unsigned join, TABLE * like)
	{
		// int sum = 0, count = 0;
		// DATA * cur = head;

		// while(cur)
		// {
		// 	if(!condition || condition->test(cur))
		// 	{
		// 		if(!content) sum += cur->id;
		// 		else if(content == 3) sum += cur->age;
		// 		if(join)
		// 		{
		// 			if(join & 1) count += like->id_map.find(cur->id) != like->id_map.end();
		// 			else count += like->age_map.find(cur->id) == like->age_map.end() ? 0 : like->age_map[cur->id].size();
		// 		}
		// 		else count++;
		// 	}
		// 	cur = cur->next;
		// }
		// if(!mode) return sum;
		// if(mode & 1) return (double)sum/count;
		// return count;
		return 0;
	}
	void update(CONDITION * condition, unsigned content, char * input)
	{
		DATA * tmp;
		unsigned num;
		std::string str;
		switch(content)
		{
			case 0 :
				num = std::atoll(input);
				if(!condition)
				{
					tmp = * data.begin();
					id_map.erase(tmp->id);
					tmp->id = num;
					id_map[num] = tmp;
					break;
				}
				ans.clear();
				condition->test(id_map, name_map, email_map, age_map, ans, 0);
				tmp = * ans.begin();
				id_map.erase(tmp->id);
				tmp->id = num;
				id_map[num] = tmp;
				break;
			case 1 :
				str = std::string(input);
				if(!condition)
				{
					for(DATA_SET::iterator it = data.begin() ; it != data.end() ; it++)
					{
						tmp = * it;
						erase(name_map, tmp->name, tmp);
						tmp->name = str;
						insert(name_map, str, tmp);
					}
					break;
				}
				ans.clear();
				condition->test(id_map, name_map, email_map, age_map, ans, 0);
				for(DATA_SET::iterator it = ans.begin() ; it != ans.end() ; it++)
				{
					tmp = * it;
					erase(name_map, tmp->name, tmp);
					tmp->name = str;
					insert(name_map, str, tmp);
				}
				break;
			case 2 :
				str = std::string(input);
				if(!condition)
				{
					for(DATA_SET::iterator it = data.begin() ; it != data.end() ; it++)
					{
						tmp = * it;
						erase(email_map, tmp->email, tmp);
						tmp->email = str;
						insert(email_map, str, tmp);
					}
					break;
				}
				ans.clear();
				condition->test(id_map, name_map, email_map, age_map, ans, 0);
				for(DATA_SET::iterator it = ans.begin() ; it != ans.end() ; it++)
				{
					tmp = * it;
					erase(email_map, tmp->email, tmp);
					tmp->email = str;
					insert(email_map, str, tmp);
				}
				break;
			case 3 :
				num = std::atoll(input);
				if(!condition)
				{
					for(DATA_SET::iterator it = data.begin() ; it != data.end() ; it++)
					{
						tmp = * it;
						erase(age_map, tmp->age, tmp);
						tmp->age = num;
						insert(age_map, num, tmp);
					}
					break;
				}
				ans.clear();
				condition->test(id_map, name_map, email_map, age_map, ans, 0);
				for(DATA_SET::iterator it = ans.begin() ; it != ans.end() ; it++)
				{
					tmp = * it;
					erase(age_map, tmp->age, tmp);
					tmp->age = num;
					insert(age_map, num, tmp);
				}
		}
	}
	int size() {return data.size();}
};