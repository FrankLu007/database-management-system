#include <string>
#include <vector>
#include <cstdio>

bool print_data;
std::FILE * fp;

class DATA
{
	int id, age;
	std::string name, email;
public:
	DATA * next;
	DATA(int _id, char * _name, char * _email, int _age) : 
		id(_id), name(std::string(_name)), email(std::string(_email)), age(_age), next(NULL) {}
	void print(std::vector <int> & content)
	{
		if(!print_data) return ;
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
					break;
				default: 
					std::fprintf(stderr, "Error item number to print.\n");
			}
		}
		std::fprintf(fp, ")\n");
	}
};