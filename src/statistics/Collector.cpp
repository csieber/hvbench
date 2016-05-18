/*
 * Collector.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: sieber
 */

#include "Collector.h"
#include "OFLatencyStats.h"
#include "../globals.h"
#include "../HVBenchController.h"
#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Collector::Collector() {
}

Collector::~Collector() {
}

void Collector::addMeta(std::string key, std::string value) {
	this->meta_[key] = value;
}

std::string Collector::toJson() {

	using boost::property_tree::ptree;
	ptree pt;

	for (const auto& e : this->statistics_) {
		pt.add(e.first, e.second);
	}
	for (const auto& e : this->meta_) {
		pt.add(e.first, e.second);
	}

	std::ostringstream oss;

	write_json(oss, pt);

	return oss.str();
}

void Collector::visit(std::shared_ptr<OFLatencyStats> s) {

	boost::mutex::scoped_lock lock(s->mutex_);

	for (auto& e : s->stats_latency_) {

		OFPT::OFPT_T key = e.first;

		const auto key_mean = boost::str(boost::format("latency_%s_mean") % OFPT::STR.at(key));
		const auto key_min = boost::str(boost::format("latency_%s_min") % OFPT::STR.at(key));
		const auto key_max = boost::str(boost::format("latency_%s_max") % OFPT::STR.at(key));

		if (boost::accumulators::count(e.second) != 0) {

			this->statistics_[key_mean] = static_cast<uint64_t>(boost::accumulators::mean(e.second));
			this->statistics_[key_min] = static_cast<uint64_t>(boost::accumulators::min(e.second));
			this->statistics_[key_max] = static_cast<uint64_t>(boost::accumulators::max(e.second));
		} else {
			this->statistics_[key_mean] = static_cast<uint64_t>(0);
			this->statistics_[key_min] = static_cast<uint64_t>(0);
			this->statistics_[key_max] = static_cast<uint64_t>(0);
		}
	}
}


void Collector::visit(std::shared_ptr<HVBenchController> s) {

	auto duration = boost::chrono::duration_cast<boost::chrono::milliseconds>(clock_source::now() - s->start_).count() + 1;

	this->statistics_["duration_ms"] = static_cast<uint64_t>(duration);

	this->statistics_["timestamp"] = (boost::posix_time::microsec_clock::universal_time() - reference_t).total_milliseconds();

	for (auto& e : OFPT::req_types) {

		const auto key = boost::str(boost::format("requests_%s_cnt") % OFPT::STR.at(e));

		this->statistics_[key] = static_cast<uint64_t>(s->counters_[e]);
	}

}

const std::map<std::string, uint64_t> Collector::statistics() {
	return statistics_;
}

void Collector::visit(std::shared_ptr<OFConnWrapper> s) {

	this->statistics_["tx_bytes"] = s->byte_cnt_;
}

void Collector::print() {

	for (auto& e : this->statistics_) {

		std::cout << e.first << ": " << e.second << std::endl;
	}
}

