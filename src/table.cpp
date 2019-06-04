#include <algorithm>
#include <cmath>
#include "condition.cpp"

class TABLE
{
	DATA * tmp;
	DATA_SET ans, data;
	ID_MAP id_map;
	AGE_MAP age_map;
	STR_MAP name_map, email_map;
	std::vector<DATA *> array;
public:
	TABLE()	
	{
		array.clear();
		data.clear();
		id_map.clear();
		age_map.clear();
		name_map.clear();
		email_map.clear();
	}
	inline bool id_check(unsigned id) {return id_map.find(id) != id_map.end();}
	inline void insert(AGE_MAP & age_map, unsigned num, DATA * data)
	{
		if(age_map.find(num) == age_map.end()) age_map[num].clear();
		age_map[num].insert(data);
	}
	inline void insert(STR_MAP & str_map, const std::string & str, DATA * data)
	{
		if(str_map.find(str) == str_map.end()) str_map[str].clear();
		str_map[str].insert(data);
	}
	inline void erase(AGE_MAP & age_map, unsigned num, DATA * data)
	{
		age_map[num].erase(data);
		if(!age_map[num].size()) age_map.erase(num);
	}
	inline void erase(STR_MAP & str_map, const std::string & str, DATA * data)
	{
		str_map[str].erase(data);
		if(!str_map[str].size()) str_map.erase(str);
	}
	inline bool insert(char * command)
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

			tmp = new DATA(id, name, email, age, array.size());
			array.push_back(tmp);
			data.insert(tmp);
			id_map[id] = tmp;
			insert(name_map, std::string(name), tmp);
			insert(email_map, std::string(email), tmp);
			insert(age_map, age, tmp);
		}
		else // if the target table is 'like'
		{
			id = std::atoll(std::strtok(NULL, " ")); // id1
			age = std::atoll(std::strtok(NULL, " ")); // id2

			tmp = new DATA(id, age, array.size());
			array.push_back(tmp);
			data.insert(tmp);
			id_map[id] = tmp;
			insert(age_map, age, tmp);
		}
		return true;
	}
	unsigned print(CONDITION * condition, std::vector <unsigned> & content, unsigned limit, unsigned offset, FILE * fp)
	{
		if(!limit || offset >= data.size()) return 0;

		unsigned count = 0;
		static std::vector <DATA *> output;
		ans.clear();

		if(!condition) 
		{
			if(data.size() * std::log2(data.size()) >= array.size()) 
			{
				for(unsigned i = 0 ; i < array.size() ; i++)
				{
					if(!array[i]) continue;
					if(offset) {offset--; continue;}
					if(limit)
					{
						count++;
						array[i]->print(content, fp);
						limit--;
					}
					else break;
				}
				return count;
			}
			else
			{
				output = std::vector <DATA *>(data.begin(), data.end()); // put them into a vector
				std::sort(output.begin(), output.end(), cmp); // sort by 'print_order'
			}
		}
		else 
		{	
			condition->test(id_map, name_map, email_map, age_map, ans, 0); // get the answer set
			output = std::vector <DATA *>(ans.begin(), ans.end()); // put them into a vector
			std::sort(output.begin(), output.end(), cmp); // sort by 'print_order'
		}
		
		for(unsigned i = offset ; limit && i < output.size(); i++) if(output[i]) {output[i]->print(content, fp); limit--; count++;}
		return count;
	}
	void del(CONDITION * condition)
	{
		if(!condition) //XDD, not good but fast
		{
			array.clear();
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
			array[tmp->print_order] = NULL;
			delete tmp;
		}
	}
	unsigned aggre(CONDITION * condition, unsigned content, unsigned mode, unsigned join, TABLE * like, unsigned & _sum)
	{
		unsigned sum = 0, count = 0;
		DATA_SET * target = condition ? & ans : & data;

		if(condition) 
		{
			ans.clear();
			condition->test(id_map, name_map, email_map, age_map, ans, 0);
		}

		if(!join)
		{
			count = target->size();
			if(mode) return count;
			if(!content) // id
				for(DATA_SET::iterator it = target->begin() ; it != target->end() ; it++) sum += (* it)->id;
			else if(content == 3) // age
				for(DATA_SET::iterator it = target->begin() ; it != target->end() ; it++) sum += (* it)->age;
			_sum = sum;
			return count;
		}
		if(join == 1)
		{
			for(DATA_SET::iterator it = target->begin() ; it != target->end() ; it++) 
				if(like->id_map.find((* it)->id) != like->id_map.end()) count++;
		}
		else
		{
			for(DATA_SET::iterator it = target->begin() ; it != target->end() ; it++) 
				if(like->age_map.find((* it)->id) != like->age_map.end())
					count += like->age_map[(* it)->id].size();
		}
		return count;
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
	inline unsigned size() {return data.size();}
};

