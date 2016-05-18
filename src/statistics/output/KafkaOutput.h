
#ifndef SRC_STATISTICS_OUTPUT_KAFKAOUTPUT_H_
#define SRC_STATISTICS_OUTPUT_KAFKAOUTPUT_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "OutputBase.h"

// Disable snappy compression
#define LIBKAFKAASIO_NO_COMPRESSION_SNAPPY
#include "../../libs/libkafka_asio/libkafka_asio.h"

class KafkaOutput: public OutputBase {
public:

	typedef std::shared_ptr<KafkaOutput> ptr;

	static KafkaOutput::ptr create(std::string ip, std::string instance_id);

	virtual void write(std::string tenant, Collector_ptr col);

	virtual ~KafkaOutput();

private:
	KafkaOutput();

	boost::asio::io_service ios_;
	std::shared_ptr<libkafka_asio::Connection> connection_;

	std::shared_ptr<boost::thread> asio_thr_;
	std::shared_ptr<boost::asio::io_service::work> asio_work_;
};

#endif /* SRC_STATISTICS_OUTPUT_KAFKAOUTPUT_H_ */
