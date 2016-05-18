#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <boost/chrono.hpp>
#include <map>

#include "conf.h"

extern std::string gl_etcd_ip;
extern uint32_t gl_etcd_port;

namespace OFPT {

	enum OFPT_T {
		PACKET_IN = 10,
		FEATURES_REPLY = 6,
		ECHO_REQUEST = 2,
		STATS_REPLY = 17,
		ECHO_REPLY = 3,
		HELLO = 0,
		ERROR = 1,
		PORT_STATUS = 12,
		FEATURE_REQUEST = 99, //FIXME: Wrong ID
		STATS_REQUEST_PORT = 101, //FIXME: Wrong ID
		STATS_REQUEST_FLOW = 102, //FIXME: Wrong ID
		STATS_REQUEST = 103, // Stats Requests in general (we do not distinguish between different types)
		PACKET_OUT_UDP = 104, //FIXME: Wrong ID
		FLOW_MOD = 105, //FIXME: Wrong ID
		OFPT_UNKNOWN = 199
	};

	const std::map<OFPT_T, std::string> STR = { {PACKET_OUT_UDP, "PACKET_OUT_UDP"},
			                                    {ECHO_REQUEST, "ECHO_REQUEST"},
			                                    {FEATURE_REQUEST, "FEATURE_REQUEST"},
	                                            {STATS_REQUEST, "STATS_REQUEST"},
	                                            {FLOW_MOD, "FLOW_MOD"},
	                                            {STATS_REQUEST_PORT, "STATS_REQUEST_PORT"},
	                                            {STATS_REQUEST_FLOW, "STATS_REQUEST_FLOW"}};

	const std::map<std::string, OFPT_T> RSTR = { {"PACKET_OUT_UDP", PACKET_OUT_UDP},
			                                     {"ECHO_REQUEST", ECHO_REQUEST},
			                                     {"FEATURE_REQUEST", FEATURE_REQUEST},
	                                             {"STATS_REQUEST", STATS_REQUEST},
	                                             {"FLOW_MOD", FLOW_MOD},
	                                             {"STATS_REQUEST_PORT", STATS_REQUEST_PORT},
	                                             {"STATS_REQUEST_FLOW", STATS_REQUEST_FLOW}};

	const std::vector<OFPT::OFPT_T> req_types = {OFPT::FEATURE_REQUEST,
												 OFPT::ECHO_REQUEST,
												 OFPT::STATS_REQUEST_PORT,
												 OFPT::STATS_REQUEST_FLOW,
												 OFPT::PACKET_OUT_UDP,
												 OFPT::FLOW_MOD};

	const std::vector<OFPT::OFPT_T> latency_types = {OFPT::FEATURE_REQUEST,
										 		 	 OFPT::ECHO_REQUEST,
									 			 	 OFPT::STATS_REQUEST_PORT,
	                                                 OFPT::STATS_REQUEST_FLOW};
}

typedef boost::chrono::system_clock clock_source;
typedef boost::chrono::time_point<clock_source> time_point_t;

typedef uint64_t datapath_T;

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

const boost::posix_time::ptime reference_t(boost::gregorian::date(2015, boost::gregorian::Jan,1));

#endif /* GLOBALS_H_ */
