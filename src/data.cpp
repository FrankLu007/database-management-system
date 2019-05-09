#include <string>
#include <vector>
#include <cstdio>

class DATA
{
public:
	int id, age;
	std::string name, email;
	DATA * next;
	DATA(int _id, char * _name, char * _email, int _age) : 
		id(_id), name(std::string(_name)), email(std::string(_email)), age(_age), next(NULL) {}
	void print(std::vector <int> & content, FILE * fp)
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