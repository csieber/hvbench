
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <stdio.h>
#include <arpa/inet.h>

#include <unordered_map>
#include <list>
#include <memory>

#include <fluid/OFServer.hh>
#include "fluid/of10msg.hh"

using namespace fluid_base;

#define EVENT_PACKET_IN 0
#define EVENT_SWITCH_DOWN 1
#define EVENT_SWITCH_UP 2
#define EVENT_ECHO_REQUEST 3
#define EVENT_ECHO_REPLY 4
#define EVENT_FEATURES_REQUEST 5
#define EVENT_FEATURES_REPLY 6
#define EVENT_STATS_REPLY 8

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/max.hpp>

#include "libs/etcd/etcd_helper.hpp"

#include "concurrentqueue/concurrentqueue.h"
#include "globals.h"
#include "of/messages.hpp"
#include "of/ofconnwrapper.hpp"
#include "generators/AbstractGenerator.hpp"
#include "of/ofconnwrapper.hpp"
#include "statistics/OFLatencyStats.h"
#include "statistics/Collector.h"
#include "statistics/hasStatistics.hpp"
#include "Tenant.h"


class HVBenchController
  : public OFServer,
    public etcd_helper,
    public hasStatistics,
    public std::enable_shared_from_this<HVBenchController> {
public:

	typedef std::shared_ptr<HVBenchController> ptr;

	friend class Collector;

	boost::condition_variable cond_switch_up;
	boost::mutex mut_switch_up;
	bool switch_up = false;

	typedef boost::accumulators::accumulator_set<uint32_t,
			                                     boost::accumulators::features<boost::accumulators::tag::max,
			                                                                   boost::accumulators::tag::mean>> boost_acc_set_late_sends;

	boost_acc_set_late_sends stats_late_sends;

	bool running;
	OFConnWrapper::ptr conn;

	uint32_t last_bytes_cnt;

	static
	HVBenchController::ptr create(std::string etcd_basep,
								  Tenant::ptr parent,
					  	  	  	  std::string address = "0.0.0.0",
					  	  	  	  const int port = 6653);
	void stop();

	void setupSwitch();

	/*virtual*/void accept(std::shared_ptr<StatsVisitor> v);

	void run(AbstractGenerator::ptr generator);

	virtual void message_callback(OFConnection* ofconn, uint8_t type, void* data, size_t len);


protected:

	HVBenchController(std::string etcd_basep,
					  std::string address = "0.0.0.0",
					  const int port = 6653,
					  const int n_workers = 2,
					  bool secure = false);

	virtual void connection_callback(OFConnection* ofconn, OFConnection::Event type);

private:
	typedef std::pair<uint32_t, time_point_t> xid_t_pair;
	moodycamel::ConcurrentQueue<xid_t_pair> reply_waiting_list;

	std::map<OFPT::OFPT_T, uint64_t> counters_;

	std::shared_ptr<OFLatencyStats> latency_stats_;

	time_point_t start_;

	Tenant::ptr parent_;
};

#endif
#pragma once

