
#ifndef HASSTATISTICS_HPP_
#define HASSTATISTICS_HPP_

#include "StatsVisitor_fwd.h"

class hasStatistics {

public:
	virtual void accept(std::shared_ptr<StatsVisitor> v) = 0;
};

#endif /* HASSTATISTICS_HPP_ */
