#include <string>
#include <cstring>
#include "data.cpp"

class CONDITION
{
	bool endpoint : 1, attribute : 1;         //  endpoint     0     1
	std::string * str_eq, * str_ne, * str_sub;// attribute
	double * be, * se, * ne;                  //    0         and   num
public:										  //    1         or   string
	CONDITION * left, * right;
	CONDITION()
	{
		be = se = ne = NULL;
		str_eq = str_ne = str_sub = NULL;
		left = right = NULL;
	}
	CONDITION(char * command, int l, int r) {}
	bool test(DATA * data) {}
	void set_endpoint(const char * type) 
	{
		endpoint = 1;
		attribute = std::strcmp(type, "num") ? 1 : 0;
	}
	void set_not_endpoint(const char * type)
	{
		endpoint = 0;
		attribute = std::strcmp(type, "and") ? 1 : 0;
	}
	void set_num(const char * type, double num)
	{
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
					ne = new double(num);
					break;
				case '<' : 
					se = new double(num);
					ne = new double(num);
			}
	}
	void set_str(const char * type, const  char * str)
	{
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
};