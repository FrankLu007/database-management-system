#include <string>
#include <vector>
#include <cstdio>

class DATA
{
public:
	unsigned id, age, print_order; // in 'like' table, 'id' means 'id1' and 'age' means 'id2'
	std::string name, email;
	DATA(unsigned _id1, unsigned _id2, unsigned _print_order) : id(_id1), age(_id2), print_order(_print_order) {}
	DATA(unsigned _id, char * _name, char * _email, unsigned _age, unsigned _print_order) : 
		id(_id), name(std::string(_name)), email(std::string(_email)), age(_age), print_order(_print_order) {}
	void print(std::vector <unsigned> & content, FILE * fp)
	{
		std::fprintf(fp, "(");
		for(unsigned i = 0 ; i < content.size() ; i++)
		{
			if(i) std::fprintf(fp, ", ");
			switch(content[i])
			{
				case 0:
					std::fprintf(fp, "%d", id);
					break;
				case 1:
					std::fprintf(fp, "%s", name.c_str());
					break;
				case 2:
					std::fprintf(fp, "%s", email.c_str());
					break;
				case 3:
					std::fprintf(fp, "%d", age);
			}
		}
		std::fprintf(fp, ")\n");
	}
};

bool cmp(const DATA * a, const DATA * b) {return a->print_order < b->print_order;}

unsigned distinguish_item(const char * item)
{
	switch(std::strlen(item))
	{
		case 2: return 0; //id
		case 4 : return item[0] == 'n' ? 1 : -1; //name
		case 5 : return item[0] == 'e' ? 2 : -1; //email
		case 3 : return item[2] == '1' ? 0 : 3; // id1 or age (id2)
		default : return -1;
	}
}