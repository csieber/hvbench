
#ifndef OFLATENCYSTATS_H_
#define OFLATENCYSTATS_H_

#include <tuple>
#include <map>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/thread/mutex.hpp>

#include "../concurrentqueue/concurrentqueue.h"

#include "../globals.h"
#include "hasStatistics.hpp"
#include "StatsVisitor_fwd.h"
#include "Collector_fwd.h"

class OFLatencyStats
	: public hasStatistics,
	  public std::enable_shared_from_this<OFLatencyStats> {
public:

	friend class Collector;

	typedef std::tuple<uint32_t, OFPT::OFPT_T, time_point_t> sent_msg_t;

	typedef boost::accumulators::accumulator_set<uint32_t,
				                                     boost::accumulators::features<boost::accumulators::tag::max,
				                                                                   boost::accumulators::tag::mean,
				                                                                   boost::accumulators::tag::min,
				                                                                   boost::accumulators::tag::count>> boost_acc_mean_min_max;

		boost_acc_mean_min_max stats_late_sends;

	void sent(int32_t xid, OFPT::OFPT_T type, time_point_t tp);
	void received(int32_t xid, OFPT::OFPT_T type, time_point_t tp);

	OFLatencyStats();
	virtual ~OFLatencyStats();

	void reset();

	/*virtual*/void accept(std::shared_ptr<StatsVisitor> v);

private:
	moodycamel::ConcurrentQueue<sent_msg_t> reply_waiting_list;

	std::map<OFPT::OFPT_T, boost_acc_mean_min_max> stats_latency_;

	boost::mutex mutex_;
};

#endif /* OFLATENCYSTATS_H_ */
