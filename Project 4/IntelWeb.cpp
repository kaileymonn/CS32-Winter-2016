#include "DiskMultiMap.h"
#include "InteractionTuple.h"
#include "IntelWeb.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <set>
#include <list>
#include <unordered_map>

using namespace std;

IntelWeb::IntelWeb()
	: m_filesReady(false)
{}

IntelWeb::~IntelWeb()
{}

bool IntelWeb::createNew(const std::string &filePrefix, unsigned int maxDataItems) {
	bool b = false, c = false;
	unsigned int hashMapSize = maxDataItems / HASHMAPLOADFACTOR;

	b = m_originToD.createNew(filePrefix + "-originToD.dat", hashMapSize);
	c = m_destinationToO.createNew(filePrefix + "-destinationToO.dat", hashMapSize);

	if (b == false || c == false) {
		close();
		return false;
	}

	m_filesReady = true;
	return true;
}

bool IntelWeb::openExisting(const std::string &filePrefix) {
	bool b = false, c = false;

	b = m_originToD.openExisting(filePrefix + "-originToD.dat");
	c = m_destinationToO.openExisting(filePrefix + "-destinationToO.dat");

	if (b == false || c == false) {
		close();
		return false;
	}

	m_filesReady = true;
	return true;
}

void IntelWeb::close() {
	m_originToD.close();
	m_destinationToO.close();
	m_filesReady = false;
}

bool IntelWeb::ingest(const std::string &telemetryFile) {
	if (!m_filesReady) {
		cerr << "Files Not Ready!!" << endl;
		return false;
	}
	ifstream f(telemetryFile);
	if (!f) {
		cerr << "Failed to ready telemetry files" << endl;
		return false;
	}

	string line;
	while (getline(f, line)) {
		istringstream iss(line);
		string context, origin, destination;

		if (!(iss >> context >> origin >> destination)) {
			cerr << "Read a poorly formatted line!" << endl;
			continue;
		}
		bool b = false, c = false;
		b = m_originToD.insert(origin, destination, context);
		c = m_destinationToO.insert(destination, origin, context);

		if (b == false || c == false) {
			close();
			cerr << "Error writing to file" << endl;
			return false;
		}

		cerr << "#";
	}
	cerr << endl << "DONE INGESTING ITEMS" << endl << endl;


	return true;
}

unsigned int IntelWeb::crawl(const std::vector<std::string> &indicators,
	unsigned int minPrevalenceToBeGood,
	std::vector<std::string> &badEntitiesFound,
	std::vector<InteractionTuple> &interactions) {
	if (!m_filesReady) {
		cerr << "Files Not Ready!!" << endl;
		return false;
	}

	queue<string> examineQueue;
	set<string> badItems;

	//interactions set - this helps create a list of unique interactions
	set<InteractionTuple, compare_interactionTuple> interactionSet;

	//hashmap to act as a marker to indicate the vertex is visited
	unordered_map<string, bool> m_marker;

	//for every known bad element -
	for (auto it = indicators.begin(); it != indicators.end(); it++) {
		m_marker[*it] = true; //mark them as visited

		int count = prevalence(*it, minPrevalenceToBeGood);
		if (count >= minPrevalenceToBeGood) {
			//the indicator is good!!
			cerr << "Indicator '" << *it << "' exceeds minPrevalenceToBeGood!" << endl;
		}
		else if (count > 0) {
			badItems.insert(*it);
			examineQueue.push(*it);
		}
		else {
			cerr << "Indicator '" << *it << "' is not found in telemetry logs!" << endl;
		}
	}

	//as long as there are still unexamined bad items in our graph
	while (!examineQueue.empty()) {
		string obj = examineQueue.front();
		examineQueue.pop();

		auto otdcached = m_otd_cache.find(obj);
		if (otdcached != m_otd_cache.end()) {
			auto& cache_list = otdcached->second;
			for (auto it = cache_list.begin(); it != cache_list.end(); it++) {
				interactionSet.insert(*it);
				if (prevalence(it->to, minPrevalenceToBeGood) >= minPrevalenceToBeGood) //item is good
					continue;

				badItems.insert(it->to);

				if (m_marker[it->to] == true)
					continue;

				m_marker[it->to] = true;
				examineQueue.push(it->to);
			}
		}
		else {
			auto originIt = m_originToD.search(obj);
			while (originIt.isValid()) {
				auto t = *originIt;
				//inserts this new interaction
				InteractionTuple newIT(t.key, t.value, t.context);
				interactionSet.insert(newIT);

				if (prevalence(newIT.to, minPrevalenceToBeGood) >= minPrevalenceToBeGood) {
					//this item is good
					++originIt;
					continue;
				}

				badItems.insert(newIT.to);

				if (m_marker[newIT.to] == true) {
					++originIt;
					continue;
				}

				m_marker[newIT.to] = true;
				examineQueue.push(newIT.to);
			}
		}

		//** INBOUND INTERACTIONS **//
		auto dtocached = m_dto_cache.find(obj);
		if (dtocached != m_dto_cache.end()) {
			auto& cache_list = dtocached->second;
			for (auto it = cache_list.begin(); it != cache_list.end(); it++) {
				interactionSet.insert(*it);
				if (prevalence(it->from, minPrevalenceToBeGood) >= minPrevalenceToBeGood) //item is good
					continue;

				badItems.insert(it->from);

				if (m_marker[it->from] == true)
					continue;

				m_marker[it->from] = true;
				examineQueue.push(it->from);
			}
		}
		else {
			auto destIt = m_destinationToO.search(obj);
			while (destIt.isValid()) {
				auto t = *destIt;
				//inserts this new interaction
				InteractionTuple newIT(t.value, t.key, t.context);
				interactionSet.insert(newIT);

				if (prevalence(newIT.from, minPrevalenceToBeGood) >= minPrevalenceToBeGood) {
					//this item is good
					++destIt;
					continue;
				}

				badItems.insert(newIT.from);

				if (m_marker[newIT.from] == true) {
					++destIt;
					continue;
				}

				//mark this as visited and push onto queue
				m_marker[newIT.from] = true;
				examineQueue.push(newIT.from);
			}
		}
	}

	badEntitiesFound.assign(badItems.begin(), badItems.end());
	interactions.assign(interactionSet.begin(), interactionSet.end());

	for (auto it = indicators.begin(); it != indicators.end(); it++) {
		badItems.insert(*it);
	}

	return badItems.size();
}

unsigned int IntelWeb::prevalence(const std::string& key, int min) {
	auto it = m_objectsCount.find(key);

	if (it == m_objectsCount.end()) {
		//key not found in map - need to calculate
		cerr << "Key Not Found, Calculating: " << key << endl;

		int instances = 0;
		//all the interactions where it is pointing
		auto originIt = m_originToD.search(key);
		while (originIt.isValid()) {
			//cache this
			auto interaction = *originIt;
			InteractionTuple i(interaction.key, interaction.value, interaction.context);
			m_otd_cache[key].push_back(i);

			if (++instances >= min) {
				m_objectsCount[key] = instances;
				return instances;
			}

			//proceed
			++originIt;
		}
		//all the interactions where it is pointed to
		auto destIt = m_destinationToO.search(key);
		while (destIt.isValid()) {
			//cache this
			auto interaction = *destIt;
			InteractionTuple i(interaction.value, interaction.key, interaction.context);
			m_dto_cache[key].push_back(i);

			if (++instances >= min) {
				m_objectsCount[key] = instances;
				return instances;
			}

			//proceed
			++destIt;
		}
		m_objectsCount[key] = instances;
		return instances;
	}
	else {
		return it->second;
	}
}

bool IntelWeb::purge(const std::string &entity) {
	if (!m_filesReady) {
		cerr << "Files Not Ready!!" << endl;
		return false;
	}
	bool isPurged = false;

	// remove from cache first (just in case)
	m_otd_cache.erase(entity);
	m_dto_cache.erase(entity);

	// get all references
	list<InteractionTuple> otd;
	list<InteractionTuple> dto;
	auto originIt = m_originToD.search(entity);
	while (originIt.isValid()) {
		auto interaction = *originIt;
		InteractionTuple i(interaction.key, interaction.value, interaction.context);
		otd.push_back(i);
		++originIt;
	} //T

	auto destIt = m_destinationToO.search(entity);
	while (destIt.isValid()) {
		auto interaction = *destIt;
		InteractionTuple i(interaction.value, interaction.key, interaction.context);
		dto.push_back(i);
		++destIt;
	} //2T

	  // remove from diskfiles - not using the default iterator since it breaks on remove
	for (auto it = otd.begin(); it != otd.end(); it++) {
		cerr << "Deleting origin " << it->from << endl;
		int i = m_originToD.erase(it->from, it->to, it->context);
		int j = m_destinationToO.erase(it->to, it->from, it->context);
		if (i > 0 || j > 0)
			isPurged = true;
	}

	for (auto it = dto.begin(); it != dto.end(); it++) {
		cerr << "Deleting dest " << it->to << endl;
		int i = m_destinationToO.erase(it->to, it->from, it->context);
		int j = m_originToD.erase(it->from, it->to, it->context);
		if (i > 0 || j > 0)
			isPurged = true;
	}

	return isPurged;
}

