
#include "DiskMultiMap.h"
#include "IntelWeb.h"

int main() {
	/*
	DiskMultiMap x;
	x.createNew("myhashtable.dat",100); // empty, with 100 buckets
	x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m10001");
	x.insert("blah.exe", "bletch.exe", "m0003");
	// line 1
	if (x.erase("hmm.exe", "pfft.exe", "m52902") == 2)
	cout << "Just erased 2 items from the table!\n";
	// line 2
	if (x.erase("hmm.exe", "pfft.exe", "m10001") > 0)
	cout << "Just erased at least 1 item from the table!\n";
	// line 3
	if (x.erase("blah.exe", "bletch.exe", "m66666") == 0)
	cout << "I didn't erase this item cause it wasn't there\n";

	if (x.erase("blah.exe", "bletch.exe", "m0003") == 1)
	cout << "I removed one!\n";

	cout << x.getSize() << endl;

	x.insert("hmm.exe", "pfft.exe", "m52902");
	cout << "After insert, size: " << x.getSize() << endl;

	x.insert("blah.exe", "bletch.exe", "m0003");
	cout << "After insert, size: " << x.getSize() << endl;

	x.insert("hmm.exe", "pfft.exe", "m52902");
	cout << "After insert, size: " << x.getSize() << endl;

	x.insert("hmm.exe", "pfft.exe", "m52902");
	cout << "After insert, size: " << x.getSize() << endl;

	DiskMultiMap::Iterator it = x.search("hmm.exe");
	if (it.isValid())
	{
	cout << "I found at least 1 item with a key of hmm.exe\n";
	do {
	MultiMapTuple m = *it; // get the association
	cout << "The key is: " << m.key << endl;
	cout << "The value is: " << m.value << endl;
	cout << "The context is: " << m.context << endl; cout << endl;
	++it; // advance iterator to the next matching item
	} while (it.isValid());
	}
	*/

	IntelWeb a;
	a.createNew("pikachu", 200);
}

