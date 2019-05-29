#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include "data.cpp"

typedef std::unordered_set <DATA *> DATA_SET;
typedef std::unordered_map <unsigned, DATA_SET> AGE_MAP;
typedef std::unordered_map <std::string, DATA_SET> STR_MAP;
typedef std::unordered_map <unsigned, DATA *> ID_MAP;


class CONDITION
{
	unsigned endpoint : 1, attribute : 2;     //               0     1     endpoint
	std::string * str_eq, * str_ne;           //    0         and   id
	double * be, * se, * ne;                  //    1         or   name
	CONDITION * left, * right;                //    2          -   email
public:                                       //    3          -    age         
	void init()                               // attribute
	{
		be = se = ne = NULL;
		str_eq = str_ne = NULL;
		left = right = NULL;
	}
	CONDITION(char * command)
	{
		init();
		char * tmp, sub_command[250];

		if(tmp = std::strstr(command, " and "))
		{
			set_not_endpoint("and");
			std::memset(sub_command, 0, 250);
			std::strncpy(sub_command, command, tmp - command);
			left = new CONDITION(sub_command);
			std::memset(sub_command, 0, 250);
			std::strcpy(sub_command, tmp + 5);
			right = new CONDITION(sub_command);
			return ;
		}
		if(tmp = std::strstr(command, " or "))
		{
			set_not_endpoint("or");
			std::memset(sub_command, 0, 250);
			std::strncpy(sub_command, command, tmp - command);
			left = new CONDITION(sub_command);
			std::memset(sub_command, 0, 250);
			std::strcpy(sub_command, tmp + 4);
			right = new CONDITION(sub_command);
			return ;
		}
		tmp = std::strtok(strdup(command), " ");
		set_endpoint(tmp);
		//std::fprintf(stderr, "First : %s\n", tmp);
		if(!attribute || attribute == 3)
		{
			tmp = std::strtok(NULL, " ");
			//std::fprintf(stderr, "Second : |%s|\n", tmp);
			set_num(tmp, std::atof(std::strtok(NULL, " ")));
		}
		else
		{
			tmp = std::strtok(NULL, " ");
			//std::fprintf(stderr, "Second : |%s|\n", tmp);
			set_str(tmp, std::strtok(NULL, " "));
		}
	}
	void test(ID_MAP & id_map, STR_MAP & name_map, STR_MAP & email_map, AGE_MAP & age_map, DATA_SET & ans, const bool mode)
	{
		if(endpoint)
		{
			double num;
			std::string str;
			if(mode) // take out 
			{
				for(DATA_SET::iterator it = ans.begin() ; it != ans.end() ; it++)
				{
					if(attribute == 3 || !attribute)
					{
						num = attribute ? (* it)->age : (* it)->id;
						if(be && num < * be) ans.erase(it);
						if(se && num > * se) ans.erase(it);
						if(ne && num == * ne) ans.erase(it);
						continue;
					}
					str = attribute & 1 ? (* it)->name : (* it)->email;
					if(str_eq && str != * str_eq) ans.erase(it);
					if(str_ne && str == * str_ne) ans.erase(it);
				}
				return ;
			}
			// put in
			if(!attribute)
			{
				for(ID_MAP::iterator it = id_map.begin() ; it != id_map.end() ; it++)
				{
					num = it->first;
					if(be && num < * be) continue;
					if(se && num > * se) continue;
					if(ne && num == * ne) continue;
					ans.insert(it->second);
				}
				return;
			}
			if(attribute == 1)
			{
				for(STR_MAP::iterator it = name_map.begin() ; it != name_map.end() ; it++)
				{
					str = it->first;
					if(str_eq && str != * str_eq) continue;
					if(str_ne && str == * str_ne) continue;
					ans.insert(it->second.begin(), it->second.end());
				}
				return;
			}
			if(attribute == 2)
			{
				for(STR_MAP::iterator it = email_map.begin() ; it != email_map.end() ; it++)
				{
					str = it->first;
					if(str_eq && str != * str_eq) continue;
					if(str_ne && str == * str_ne) continue;
					ans.insert(it->second.begin(), it->second.end());
				}
				return;
			}
			for(AGE_MAP::iterator it = age_map.begin() ; it != age_map.end() ; it++)
			{
				num = it->first;
				if(be && num < * be) continue;
				if(se && num > * se) continue;
				if(ne && num == * ne) continue;
				ans.insert(it->second.begin(), it->second.end());
			}
			return ;
		}

		left->test(id_map, name_map, email_map, age_map, ans, 0);
		if(attribute) right->test(id_map, name_map, email_map, age_map, ans, 0); // 0 means put into 'ans' : or
		else right->test(id_map, name_map, email_map, age_map, ans, 1); // 1 means take out from 'ans' : and
	}
	void set_endpoint(const char * type) 
	{
		//std::fprintf(stderr, "Third : |%s|\n", type);
		endpoint = 1;
		attribute = distinguish_item(type);
	}
	void set_not_endpoint(const char * type)
	{
		endpoint = 0;
		attribute = std::strcmp(type, "and") ? 1 : 0;
	}
	void set_num(const char * type, double num)
	{
		//std::fprintf(stderr, "Third : |%f|\n", num);
		unsigned len = std::strlen(type);
		if(len == 1)
			switch(type[0])
			{
				case '=':
					be = new double(num);
					se = new double(num);
					break;
				case '>' :
					be = new double(num);
					ne = new double(num);
					break;
				case '<' : 
					se = new double(num);
					ne = new double(num);
			}
		else
			switch(type[0])
			{
				case '!':
					ne = new double(num);
					break;
				case '>' :
					be = new double(num);
					break;
				case '<' : 
					se = new double(num);
			}
	}
	void set_str(const char * type, const  char * str)
	{
		//std::fprintf(stderr, "Third : |%s|\n", str);
		unsigned len = std::strlen(type);
		switch(len)
		{
			case 1 :
				str_eq = new std::string(str);
				break;
			case 2 :
				str_ne = new std::string(str);
		}
	}
	void clean()
	{
		if(right) right->clean();
		if(left) left->clean();
		if(be) delete be;
		if(se) delete se;
		if(ne) delete ne;
		if(str_eq) delete str_eq;
		if(str_ne) delete str_ne;
		delete this;
	}
};