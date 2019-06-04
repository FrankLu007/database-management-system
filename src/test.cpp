#include <cstdio>
#include <string>
#include <random>
#include <unordered_set>
using namespace std;
unordered_set <unsigned> id_set;
int main(int N, char ** args)
{
	unsigned num = N > 2 ? atoll(args[2]) : 100000, tmp, count = 0;
	mt19937 generator(time(NULL));
	uniform_int_distribution <unsigned> random_age(0, 50), random_id(0, num);
	FILE * fp = N > 1 ? fopen(args[1], "w") : stdout;
	id_set.clear();

	fprintf(fp, ".output result.txt\n");
	for(unsigned i = 0 ; i < num ; i++)
	{
		fprintf(fp, "insert into user %u %s %s %u\n", i, (string("user") + to_string(i)).c_str(), (string("email") + to_string(i)).c_str(), tmp = random_age(generator));
		if(tmp < 25) id_set.insert(i);
	}
	for(unsigned i = 0 ; i < num ; i++)
	{
		fprintf(fp, "insert into like %u %u\n", i, tmp = random_id(generator));
		if(id_set.find(tmp) != id_set.end()) count++;
	}
	fprintf(fp, "select count(*) from user join like on id = id2 where age < 25\n");
	fprintf(fp, ".exit\n");
	printf("ans : %u\nnumber of id : %u\n", count, id_set.size());
	return 0;
}