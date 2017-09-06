#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <list>

const double HASHMAPLOADFACTOR = 0.75;

class IntelWeb
{
public:
	IntelWeb();
	~IntelWeb();
	bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
	bool openExisting(const std::string& filePrefix);
	void close();
	bool ingest(const std::string& telemetryFile);
	unsigned int crawl(const std::vector<std::string>& indicators,
		unsigned int minPrevalenceToBeGood,
		std::vector<std::string>& badEntitiesFound,
		std::vector<InteractionTuple>& interactions
		);
	bool purge(const std::string& entity);
private:
	// Your private member declarations will go here
	DiskMultiMap m_originToD;
	DiskMultiMap m_destinationToO;

	unordered_map<string, int> m_objectsCount;

	struct compare_interactionTuple {
		bool operator() (const InteractionTuple& lhs, const InteractionTuple& rhs) const {
			if (lhs.context == rhs.context) {
				if (lhs.from == rhs.from) {
					return rhs.to > lhs.to;
				}
				else {
					return rhs.from > lhs.from;
				}
			}
			else {
				return rhs.context > lhs.context;
			}
		}
	};

	unsigned int prevalence(const string& key, int min);

	bool m_filesReady;
	//Disk based cache
	unordered_map<string, list<InteractionTuple> > m_otd_cache;
	unordered_map<string, list<InteractionTuple> > m_dto_cache;
};

#endif // INTELWEB_H_