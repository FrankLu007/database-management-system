#include <string>
#include <cstring>
#include "data.cpp"

class CONDITION
{
	unsigned endpoint : 1, attribute : 2;     //               0     1     endpoint
	std::string * str_eq, * str_ne, * str_sub;//    0         and   id
	double * be, * se, * ne;                  //    1         or   name
public:                                       //    2          -   email
	CONDITION * left, * right;                //    3          -    age
	void init()                               // attribute
	{
		be = se = ne = NULL;
		str_eq = str_ne = str_sub = NULL;
		left = right = NULL;
	}
	CONDITION(char * command)
	{
		init();
		int len = std::strlen(command);
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
		tmp = std::strtok(strdup(command), "><=! ");
		set_endpoint(tmp);
		//std::fprintf(stderr, "First : %s\n", tmp);
		if(!attribute || attribute == 3)
		{
			tmp = std::strtok(strdup(command), "idage0123456789 ");
			//std::fprintf(stderr, "Second : |%s|\n", tmp);
			set_num(tmp, std::atof(std::strtok(strdup(command), "idage><=! ")));
		}
		else
		{
			tmp = std::strtok(strdup(command), "\"namemail ");
			//std::fprintf(stderr, "Second : |%s|\n", tmp);
			set_str(tmp, std::strstr(command, "\""));
		}
	}
	bool test(DATA * data) 
	{
		if(endpoint)
		{
			if(attribute == 3 || !attribute)
			{
				double num = attribute ? data->age : data->id;
				if(be && num < * be) return false;
				if(se && num > * se) return false;
				if(ne && num == * ne) return false;
				return true;
			}
			std::string & str = attribute & 1 ? data->name : data->email;
			if(str_eq && str != * str_eq) return false;
			if(str_ne && str == * str_ne) return false;
			return true;
		}
		if(attribute)
		{
			if(left->test(data)) return true;
			return right->test(data);
		}
		if(!left->test(data)) return false;
		return right->test(data);
	}
	void set_endpoint(const char * type) 
	{
		//std::fprintf(stderr, "Third : |%s|\n", type);
		int len = std::strlen(type);
		endpoint = 1;
		switch(len)
		{
			case 2 :
				attribute = 0;
				break;
			case 3 :
				attribute = 3;
				break;
			case 4 :
				attribute = 1;
				break;
			case 5 :
				attribute = 2;
		}
	}
	void set_not_endpoint(const char * type)
	{
		endpoint = 0;
		attribute = std::strcmp(type, "and") ? 1 : 0;
	}
	void set_num(const char * type, double num)
	{
		//std::fprintf(stderr, "Third : |%f|\n", num);
		int len = std::strlen(type);
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
		int len = std::strlen(type);
		switch(len)
		{
			case 1 :
				str_eq = new std::string(str);
				break;
			case 2 :
				str_ne = new std::string(str);
				break;
			case 4 :
				str_sub = new std::string(str);
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
		if(str_sub) delete str_sub;
		delete this;
	}
};