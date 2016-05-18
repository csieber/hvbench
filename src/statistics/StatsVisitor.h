
#ifndef STATSVISITOR_H_
#define STATSVISITOR_H_

#include <string>
#include <map>
#include "OFLatencyStats.h"

class OFLatencyStats;
class HVBenchController;
class OFConnWrapper;

class StatsVisitor {
public:
	StatsVisitor();
	virtual ~StatsVisitor();

	virtual void visit(std::shared_ptr<OFLatencyStats> s) = 0;
	virtual void visit(std::shared_ptr<HVBenchController> s) = 0;
	virtual void visit(std::shared_ptr<OFConnWrapper> s) = 0;
};

/*
class Collector : public StatsVisitor {

	void visit(OFLatencyStats* s) {

		for (auto& e : s->stats_latency_) {

			std::cout << e.first << ": " << e.second << std::endl;
		}
	}
};
*/
#endif /* STATSVISITOR_H_ */
