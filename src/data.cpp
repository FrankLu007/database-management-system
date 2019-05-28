#include <string>
#include <vector>
#include <cstdio>

class DATA
{
public:
	int id, age; // in 'like' table, 'id' means 'id1' and 'age' means 'id2'
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

unsigned distinguish_item(const char * item)
{
	switch(std::strlen(item))
	{
		case 2: return 0;
		case 4 : return item[0] == 'n' ? 1 : -1;
		case 5 : return item[0] == 'e' ? 2 : -1;
		case 3 : return item[2] == '1' ? 0 : 3;
		default : return -1;
	}
}