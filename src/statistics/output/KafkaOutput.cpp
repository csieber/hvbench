
#include "KafkaOutput.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include "../Collector.h"

using libkafka_asio::ProduceRequest;
using libkafka_asio::Connection;
using libkafka_asio::ProduceResponse;

KafkaOutput::KafkaOutput() {

	study_id_ = "test";
	run_id_ = 1;
}

KafkaOutput::~KafkaOutput() {

	this->ios_.stop();
	this->asio_thr_->join();
}

void KafkaOutput::write(std::string tenant, Collector_ptr col) {

	boost::mutex::scoped_lock(this->class_lock_);

	ProduceRequest request;

	col->addMeta("tenant", tenant);
	col->addMeta("study_id", this->study_id_);
	col->addMeta("run_id", boost::lexical_cast<std::string>(this->run_id_));

	request.AddValue(col->toJson(), "hvbench", 0);

	this->connection_->AsyncRequest(
	    request,
	    [&](const Connection::ErrorCodeType& err,
	        const ProduceResponse::OptionalType& response)
	  {
	    if (err)
	    {
	      std::cerr
	        << "Error: " << boost::system::system_error(err).what()
	        << std::endl;
	      return;
	    }
	  });
}

/*static*/
KafkaOutput::ptr KafkaOutput::create(std::string ip, std::string instance_id) {

	libkafka_asio::Connection::Configuration configuration;

	configuration.auto_connect = true;
	configuration.client_id = "hvbench";
	configuration.socket_timeout = 10000;
	configuration.SetBrokerFromString(ip);

	auto ptr = KafkaOutput::ptr(new KafkaOutput());

	ptr->connection_ = std::make_shared<libkafka_asio::Connection>(ptr->ios_, configuration);

	ptr->asio_work_ = std::make_shared<boost::asio::io_service::work>(ptr->ios_);

	ptr->asio_thr_ = std::make_shared<boost::thread>(boost::bind(&boost::asio::io_service::run, &ptr->ios_));

	return ptr;
}

