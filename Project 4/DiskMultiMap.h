#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

const int MAX_STR_LEN = 120;
const BinaryFile::Offset NULL_OFFSET = 0;

class DiskMultiMap
{
public:

	class Iterator
	{
	public:
		Iterator();
		// You may add additional constructors
		Iterator(BinaryFile::Offset ofs, DiskMultiMap* map);
		bool isValid() const;
		Iterator& operator++();
		MultiMapTuple operator*();

	private:
		// Your private member declarations will go here
		BinaryFile::Offset m_offs;
		DiskMultiMap* m_map;
		MultiMapTuple m_readCache;
		bool m_cacheValid;
	};

	DiskMultiMap();
	~DiskMultiMap();
	bool createNew(const std::string& filename, unsigned int numBuckets);
	bool openExisting(const std::string& filename);
	void close();
	bool insert(const std::string& key, const std::string& value, const std::string& context);
	Iterator search(const std::string& key);
	int erase(const std::string& key, const std::string& value, const std::string& context);

	//TODO: REMOVE THIS
	int getSize() {
		return m_file.fileLength();
	}

private:
	struct MultiMapHeader {
		int total;
		BinaryFile::Offset firstRemoved;
	};

	struct Bucket {
		BinaryFile::Offset head;
	};

	struct TupleItem {
		char key[MAX_STR_LEN + 1];
		char value[MAX_STR_LEN + 1];
		char context[MAX_STR_LEN + 1];
		BinaryFile::Offset next;
	};

	BinaryFile m_file;

	unsigned int hashKey(const std::string &key, const unsigned int numBuckets);
};

#endif // DISKMULTIMAP_H_

