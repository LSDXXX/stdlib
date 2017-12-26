#include <iostream>
#include <Windows.h>
#include <mutex>
#include <list>
#include <vector>
#include "nocopy.h"
#include "locks.h"
#define MutiThread
#include "_vector.h"

#include "allocator.h"
#include "_type.h"
#include "_list.h"
#include "AVL.h"
#include "RBTree.h"
#include <deque>
#include <map>
#include <memory>
#include <set>
#include "_map.h"
//#include <ntddk.h>
#define FUNCTION(name) name##f("123")

class test
{
public:
	class tt
		:public lsd::NoCopy
	{
	public:
		int c;
		

	};
	test(){}
	test(const int& t,const int& s):c(t),a(s) {}
	~test(){}
	void dosome()
	{
		c += 10;
	}
	int c;
	int a;
	char* ll;
	char* cc;
	std::vector<int> bb;
};
template <typename T>
void change_int(T a)
{
	a = 10;
}
template <typename T>
T& get_ref(T& t)
{
	return static_cast<T&>(t);
}
int main()
{
	std::map<int, int> map;
	lsd::map<int, int> m;
	for (int i = 0;i < 10000000;++i)
		map.insert({ i,i });

	for (int i = 0;i < 10000000;++i)
		m.insert({ i,i });
	//std::cout << tree.get_depth(tree.root()->left()) << ",";
	//std::cout << tree.get_depth(tree.root()->right());
	//tt1 = tree;
	//auto node = tt1.begin();
	//vec.push_back(b);
	//aa.push_back(b);
	//aa[0] = 9;
	getchar();
}