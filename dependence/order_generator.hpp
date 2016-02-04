#ifndef ORDER_GENERATOR
#define ORDER_GENERATOR

#include <map>
#include <set>
#include <vector>
#include <cstdlib>

template <typename T>
static void switch_num(int a, int b, std::vector<T>& v)
{
	T tmp = v[a];
	v[a] = v[b];
	v[b] = tmp;
}

template <typename T>
void generate_order(std::vector<T>& queue)
{
	int total = queue.size();
	for (int i = 0; i < total / 2; ++i) {
		int rand1 = rand() % total;
		int rand2 = rand() % total;
		switch_num(rand1, rand2, v);
	}
}

#endif