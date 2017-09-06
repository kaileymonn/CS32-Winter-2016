#include "newMap.h"

#include <iostream>
#include <cassert>

using namespace std;

int main()
{
	Map a(1000);   // a can hold at most 1000 key/value pairs
	Map b(5);      // b can hold at most 5 key/value pairs
	Map c;         // c can hold at most DEFAULT_MAX_ITEMS key/value pairs
	KeyType k[6] = { "a", "ab", "abc", "x", "xy", "xyz" };
	ValueType v = 5.5;

	for (int n = 0; n < 5; n++)
		assert(b.insert(k[n], v));

	assert(!b.insert(k[5], v));

	a.swap(b);

	assert(!a.insert(k[5], v) && b.insert(k[5], v));

	cout << "Passed tests." << endl;

	
}