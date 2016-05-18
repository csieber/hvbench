
#ifndef COLLECTOR_H_
#define COLLECTOR_H_

#include "StatsVisitor.h"
#include <map>

class OFConnWrapper;

class Collector : public StatsVisitor {
public:
	Collector();
	virtual ~Collector();

	void visit(std::shared_ptr<OFLatencyStats> s);
	void visit(std::shared_ptr<HVBenchController> s);
	void visit(std::shared_ptr<OFConnWrapper> s);

	void print();

	void addMeta(std::string key, std::string value);

	const std::map<std::string, uint64_t> statistics();

	std::string toJson();

private:
	std::map<std::string, uint64_t> statistics_;
	std::map<std::string, std::string> meta_;
};

#endif /* COLLECTOR_H_ */
