
#include "OFLatencyStats.h"
#include "StatsVisitor.h"

OFLatencyStats::OFLatencyStats() {

	this->reset();
}

OFLatencyStats::~OFLatencyStats() {
}

void OFLatencyStats::reset() {

	boost::mutex::scoped_lock lock(this->mutex_);

	for (auto e : OFPT::latency_types)
		stats_latency_[e] = boost_acc_mean_min_max();
}

void OFLatencyStats::sent(int32_t xid, OFPT::OFPT_T type, time_point_t tp) {

	reply_waiting_list.try_enqueue(std::make_tuple(xid, type, tp));
}

void OFLatencyStats::received(int32_t xid, OFPT::OFPT_T type, time_point_t tp) {

	while (true) {

		sent_msg_t last;

		if (!reply_waiting_list.try_dequeue(last)){
			break;
		}

		if (std::get<0>(last) != xid)
			continue;

		const auto& latency = tp - std::get<2>(last);

		{
			boost::mutex::scoped_lock lock(this->mutex_);
			stats_latency_[std::get<1>(last)](boost::chrono::duration_cast<boost::chrono::microseconds>(latency).count());
		}

		break;
	}
}

void OFLatencyStats::accept(std::shared_ptr<StatsVisitor> v) {

	v->visit(shared_from_this());
}
