
#include "Tenant.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "globals.h"
#include <fluid/OFServerSettings.hh>
#include "HVBenchController.h"
#include "of/messages.hpp"
#include "generators/AbstractGenerator.hpp"
#include "generators/ConstantGenerator.hpp"
#include "generators/TenantGenerator.hpp"
#include "of/controllerevents.hpp"
#include "HVBench.h"

Tenant::Tenant(const std::string id)
  : etcd_helper(boost::str(boost::format("%s/%s") % c_etcd_ptenants_conf % id)),
    id_(id),
    etcd_(gl_etcd_ip, gl_etcd_port) {
}

Tenant::~Tenant() {
}

Tenant::ptr Tenant::create(HVBench::ptr parent, const std::string id, OutputBase_ptr output) {

	std::cout << "Creating tenant with ID " << id << std::endl;

	auto ptr = Tenant::ptr(new Tenant(id));

	ptr->parent_ = parent;
	ptr->output_ = output;

	return ptr;
}

void Tenant::onChange(std::string key, std::string value) {

	std::cout << "Tenant " << this->id_ << ": Key " << key << " updated to " << value << std::endl;

	std::vector<std::string> path;

	boost::split(path, key, boost::is_any_of("/"));

	auto component = path.front();
	auto conf_key = key.substr(component.length() + 1);

	if (component == "generator_conf") {

		// forward to generator
		msg_generator_->update(conf_key, value);

	}else if (component == "weights") {

		msg_generator_->update(conf_key, value);

	} else {
		std::cerr << "Unimplemented configuration update! " << key <<":  "<< component << std::endl;
	}
}

void Tenant::start() {

	thread_ = boost::thread(&Tenant::_run, this);
}

void Tenant::stop() {

	std::cout << "Stopping tenant " << this->id_ << "..." << std::endl;
	hvcontroller_->stop();
	thread_.join();
	std::cout << "Stopping tenant " << this->id_ << "...DONE" << std::endl;
}

void Tenant::_run() {

	std::cout << "Running tenant" << std::endl;

	std::string listen = this->etcd_get("setup/listen");
	auto port = boost::lexical_cast<uint32_t>(this->etcd_get("setup/port"));

	hvcontroller_ = HVBenchController::create(boost::str(boost::format("%s/%s/controller") % c_etcd_ptenants_conf % this->id_),
											  shared_from_this(),
			    	                          listen, port);

	hvcontroller_->start();

	auto generatorT = this->etcd_get("generator");

	std::cout << "Tenant uses generator " << generatorT << std::endl;

	auto basep = boost::str(boost::format("%s/%s/generator_conf") % c_etcd_ptenants_conf % this->id_);

	if (generatorT == "constant") {

		msg_generator_ = ConstantGenerator::ptr(new ConstantGenerator(basep));

	} else {

		std::vector<OFPT::OFPT_T> msg_types = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST,
				                               OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW,
				                               OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};

		std::vector<float> weights;

		for (auto& e : msg_types) {

			auto weight = this->etcd_get(boost::str(boost::format("weights/%s") % OFPT::STR.at(e)));

			weights.push_back(boost::lexical_cast<float>(weight));
		}

		auto avg_rate = boost::lexical_cast<uint32_t>(this->etcd_get("generator_conf/avg_rate"));

		msg_generator_ = TenantGenerator::ptr(new TenantGenerator(basep, avg_rate, msg_types, weights));
	}

	hvcontroller_->run(msg_generator_);
}

