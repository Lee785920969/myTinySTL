#include<iostream>
#include <vector>
using namespace std;

int main()
{
	std::vector<int> v;
	v.push_back(1);
	auto first = v.begin();
	std::cout /*<< first */ << *first << " " << &*first << std::endl;
}