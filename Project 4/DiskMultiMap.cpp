#include "DiskMultiMap.h"
#include <functional>
#include <cstring>

using namespace std;

//** ITERATOR **//

DiskMultiMap::Iterator::Iterator()
	: m_offs(NULL_OFFSET)
	, m_map(nullptr)
	, m_cacheValid(false)
{}

DiskMultiMap::Iterator::Iterator(BinaryFile::Offset ofs, DiskMultiMap* map)
	: m_offs(ofs)
	, m_map(map)
	, m_cacheValid(false)
{}

bool DiskMultiMap::Iterator::isValid() const {
	if (m_offs == NULL_OFFSET || m_map == nullptr) return false;
	return true;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++() {
	if (!isValid()) return *this;

	//invalidates cache
	m_cacheValid = false;

	DiskMultiMap::TupleItem currentTuple;

	if (!m_map->m_file.read(currentTuple, m_offs)) {
		cerr << "Failed to read tuple from file" << endl;
		return *this;
	}

	BinaryFile::Offset i = currentTuple.next;
	for (;;) {
		if (i == NULL_OFFSET) {
			m_offs = NULL_OFFSET;
			return *this;
		}

		DiskMultiMap::TupleItem t;
		if (!m_map->m_file.read(t, i)) {
			cerr << "Failed to read tuple from file" << endl;
			return *this;
		}

		if (strcmp(t.key, currentTuple.key) == 0) {
			m_offs = i;
			return *this;
		}
		i = t.next;
	}
}

MultiMapTuple DiskMultiMap::Iterator::operator*() {

	if (m_cacheValid) {
		return m_readCache;
	}

	MultiMapTuple t;
	DiskMultiMap::TupleItem ti;

	if (!isValid()) {
		cerr << "Attempted to access an iterator in an invalid state!" << endl;
		return t;
	}

	if (!m_map->m_file.read(ti, m_offs)) {
		cerr << "Failed to read tuple from file" << endl;
		return t;
	}

	t.key = ti.key;
	t.value = ti.value;
	t.context = ti.context;
	m_cacheValid = true;
	m_readCache = t;
	return m_readCache;
}

DiskMultiMap::DiskMultiMap() {}

DiskMultiMap::~DiskMultiMap() {
	if (m_file.isOpen()) m_file.close();
}

bool DiskMultiMap::createNew(const std::string &filename, unsigned int numBuckets) {
	//Close file if it's open
	//Create and overwrite a new file of filename!
	close();

	if (!m_file.createNew(filename)) {
		cerr << "Creating new file failed!" << endl;
		return false;
	}

	//create a header
	MultiMapHeader header;
	header.firstRemoved = NULL_OFFSET;
	header.total = numBuckets;

	if (!m_file.write(header, NULL_OFFSET)) {
		cerr << "Writing header failed!" << endl;
		return false;
	}

	//initialize the "array" of buckets
	for (int i = 0; i < numBuckets; ++i) {
		Bucket b;
		b.head = NULL_OFFSET;
		if (!m_file.write(b, m_file.fileLength())) {
			cerr << "Writing bucket failed!" << endl;
			return false;
		}
	}

	return true;
}

bool DiskMultiMap::openExisting(const std::string &filename) {
	close();

	if (!m_file.openExisting(filename)) {
		cerr << filename << " not found!" << endl;
		return false;
	}

	return true;
}

void DiskMultiMap::close() {
	if (m_file.isOpen()) {
		m_file.close();
	}
}

bool DiskMultiMap::insert(const std::string &key, const std::string &value, const std::string &context) {
	if (key.length() > MAX_STR_LEN || value.length() > MAX_STR_LEN || context.length() > MAX_STR_LEN) {
		cerr << "Unable to write string exceeding max string len" << endl;
		return false;
	}

	if (!m_file.isOpen()) {
		cerr << "BinaryFile is not open!" << endl;
		return false;
	}

	MultiMapHeader headerInfo;
	if (!m_file.read(headerInfo, NULL_OFFSET)) {
		cerr << "Failed to read header info from BinaryFile!" << endl;
		return false;
	}

	//determine where to write my item to
	BinaryFile::Offset writeToOffset = headerInfo.firstRemoved == NULL_OFFSET ? m_file.fileLength() : headerInfo.firstRemoved;
	BinaryFile::Offset hashBucketOffset = sizeof(MultiMapHeader) + sizeof(Bucket)*hashKey(key, headerInfo.total);
	BinaryFile::Offset newRemovedHeadOffset = NULL_OFFSET;

	if (headerInfo.firstRemoved != NULL_OFFSET) {
		TupleItem t;
		if (!m_file.read(t, writeToOffset)) {
			cerr << "Failed to read a tuple item!" << endl;
			return false;
		}
		newRemovedHeadOffset = t.next;
	}

	Bucket b;
	if (!m_file.read(b, hashBucketOffset)) {
		cerr << "Failed to read a bucket!" << endl;
		return false;
	}

	TupleItem t;
	strcpy_s(t.key, key.c_str());
	strcpy_s(t.value, value.c_str());
	strcpy_s(t.context, context.c_str());
	t.next = b.head;
	b.head = writeToOffset;

	if (!m_file.write(t, writeToOffset)) {
		cerr << "Failed to write tuple!" << endl;
		return false;
	}

	if (!m_file.write(b, hashBucketOffset)) {
		cerr << "Failed to update a bucket!" << endl;
		return false;
	}

	if (headerInfo.firstRemoved != NULL_OFFSET) {
		headerInfo.firstRemoved = newRemovedHeadOffset;
		if (!m_file.write(headerInfo, NULL_OFFSET)) {
			cerr << "Failed to update binaryfile header!" << endl;
			return false;
		}
	}

	return true;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key) {
	Iterator it; //invalid iterator

	if (key.size() > MAX_STR_LEN) {
		cerr << "Invalid key!" << endl;
		return it;
	}

	if (!m_file.isOpen()) {
		cerr << "BinaryFile is not open!" << endl;
		return it;
	}

	MultiMapHeader headerInfo;
	if (!m_file.read(headerInfo, NULL_OFFSET)) {
		cerr << "Failed to read header info from BinaryFile!" << endl;
		return it;
	}

	BinaryFile::Offset hashBucketOffset = sizeof(MultiMapHeader) + sizeof(Bucket)*hashKey(key, headerInfo.total);
	Bucket b;
	if (!m_file.read(b, hashBucketOffset)) {
		cerr << "Failed to read bucket information!" << endl;
	}

	BinaryFile::Offset i = b.head;

	for (;;) {
		if (i == NULL_OFFSET) {
			return it;
		}
		TupleItem t;
		if (!m_file.read(t, i)) {
			cerr << "Failed to read tuple from file" << endl;
			return it;
		}
		if (t.key == key) {
			return Iterator(i, this);
		}
		i = t.next;
	}
}

int DiskMultiMap::erase(const std::string &key, const std::string &value, const std::string &context) {
	if (key.length() > MAX_STR_LEN || value.length() > MAX_STR_LEN || context.length() > MAX_STR_LEN) {
		cerr << "One or more of the specified key(s) is/are invalid." << endl;
		return 0;
	}

	if (!m_file.isOpen()) {
		cerr << "BinaryFile is not open!" << endl;
		return 0;
	}

	MultiMapHeader headerInfo;
	if (!m_file.read(headerInfo, NULL_OFFSET)) {
		cerr << "Failed to read header info from BinaryFile!" << endl;
		return 0;
	}

	BinaryFile::Offset hashBucketOffset = sizeof(MultiMapHeader) + sizeof(Bucket)*hashKey(key, headerInfo.total);
	Bucket b;
	if (!m_file.read(b, hashBucketOffset)) {
		cerr << "Failed to read bucket information!" << endl;
	}

	int noRemoved = 0;

	BinaryFile::Offset i, prevOffset;
	TupleItem prev;
	i = b.head;

	for (;;) {
		if (i == NULL_OFFSET) {
			break;
		}
		TupleItem t;
		if (!m_file.read(t, i)) {
			cerr << "Failed to read tuple from file" << endl;
			return noRemoved;
		}
		if (key.compare(t.key) == 0 && value.compare(t.value) == 0 && context.compare(t.context) == 0) {
			if (b.head == i) {
				b.head = t.next;
				if (!m_file.write(b, hashBucketOffset)) {
					cerr << "Failed to update hashbucket head" << endl;
					return noRemoved;
				}
			}
			else {
				prev.next = t.next;
				if (!m_file.write(prev, prevOffset)) {
					cerr << "Failed to update prev node" << endl;
					return noRemoved;
				}
			}
			t.next = headerInfo.firstRemoved;
			headerInfo.firstRemoved = i;

			if (!m_file.write(t, i)) {
				cerr << "Failed to update erasing node" << endl;
				return noRemoved;
			}

			if (!m_file.write(headerInfo, NULL_OFFSET)) {
				cerr << "Failed to update header with removed node" << endl;
				return noRemoved;
			}
			noRemoved++;
			i = prev.next;
		}
		else {
			//NOT MATCH
			prev = t;
			prevOffset = i;
			i = t.next;
		}
	}

	return noRemoved;
}


//** PRIVATE HELPER FUNCTIONS **//

unsigned int DiskMultiMap::hashKey(const std::string &key, const unsigned int numBuckets) {
	hash<string> str_hash;
	unsigned int hashVal = str_hash(key) % numBuckets;
	return hashVal;
}







