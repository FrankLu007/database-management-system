#include <string>
#include <vector>
#include <cstdio>

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
		printf("(");
		for(int i = 0 ; i < content.size() ; i++)
		{
			if(i) printf(", ");
			switch(content[i])
			{
				case 0:
					std::printf("%d", id);
					break;
				case 1:
					std::printf("\"%s\"", name.c_str());
					break;
				case 2:
					std::printf("\"%s\"", email.c_str());
					break;
				case 3:
					std::printf("%d", age);
					break;
				default: 
					std::fprintf(stderr, "Error item number to print.\n");
			}
		}
		printf(")\n");
	}
}