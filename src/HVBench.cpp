
#include "HVBench.h"
#include "globals.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "statistics/output/OutputBase.h"
#include "Tenant.h"
#include "Node.h"

HVBench::HVBench()
  : etcd_helper(c_etcd_pglobals),
    instance_id_(0),
    etcd_watchdog_(gl_etcd_ip, gl_etcd_port),
    etcd_watchdog_tenant_conf(gl_etcd_ip, gl_etcd_port),
	etcd_watchdog_global_(gl_etcd_ip, gl_etcd_port),
    shutdown_(false) {

}

HVBench::~HVBench() {
}

/*static*/
HVBench::ptr HVBench::create(OutputBase_ptr output,
							 std::string etcd_ip,
				           	 uint32_t etcd_port,
					         uint32_t instance_id) {

	std::cout << "HVBench startup" << std::endl;

	etcd::Client<etcd_reply::RapidReply> etcd(etcd_ip, etcd_port);

	// First check if there is a configuration directory
	try {
		etcd.Get(c_etcd_phvbench);
	} catch (const etcd::ReplyException& e) {

		std::cerr << "! etcd seems not to be populated with configuration data. Please use the API to create the configuration skeleton !" << std::endl;
		return HVBench::ptr(0);
	}

	// Check the API version
	std::string api_version = etcd_helper::get(etcd, boost::str(boost::format("%s/api_version") % c_etcd_pglobals));

	if (api_version != conf_api_version) {

		std::cerr << "! API versions do not match! (" << api_version << " vs. " << conf_api_version << ")" << std::endl;
		return HVBench::ptr(0);
	}

	std::string study_id = etcd_helper::get(etcd, boost::str(boost::format("%s/study_id") % c_etcd_pglobals));
	auto run_id = boost::lexical_cast<uint32_t>(etcd_helper::get(etcd, boost::str(boost::format("%s/run_id") % c_etcd_pglobals)));

	output->update_study(study_id, run_id);

	// Second register the instance with etcd
	const auto path = boost::str(boost::format("%s/%d") % c_etcd_phvbench % instance_id);

	try {
		etcd.AddDirectory(path);
	} catch (const etcd::ReplyException& e) {
	}

	std::cout << "HVBench instance registered" << std::endl;

	auto tmp = HVBench::ptr(new HVBench());

	tmp->output_ = output;
	tmp->instance_id_ = instance_id;

	return tmp;
}

void HVBench::run() {

	std::cout << "Running HVBench instance." << std::endl;

	// Watch tenant configuration
	thread_watch_conf_ = boost::thread(&HVBench::_watchTenantConf, this);

	thread_watch_hvbench_ = boost::thread(&HVBench::_watch, this);

	thread_watch_global_ = boost::thread(&HVBench::_watchGlobal, this);

	boost::unique_lock<boost::mutex> lock(wait_shutdown_mt_);

	while(!this->shutdown_) {
		wait_shutdown_.wait(lock);
	}

	for (auto& e : tenants_) {
		e.second->stop();
	}

	for (auto& e : nodes_) {
		e.second->stop();
	}

	std::cout << "Quitting HVBench." << std::endl;
}

void HVBench::_onChange(const etcd_reply::RapidReply& reply) {

	std::cout << "Detected change on instance configuration" << std::endl;

	// Ignore everything besides SET for now
	if (reply.GetAction() != etcd::Action::SET) {
		return;
	}

	etcd_reply::RapidReply::KvPairs pairs;
	reply.GetAll(pairs);

	auto tenant = *(pairs.begin());

	std::vector<std::string> path;
	boost::split(path, tenant.first, boost::is_any_of("/"));

	if (tenant.first == boost::str(boost::format("%s/%d/shutdown") % c_etcd_phvbench % this->instance_id_)) {

		std::cout << "SHUTDOWN" << std::endl;

		{
			boost::unique_lock<boost::mutex> lock(wait_shutdown_mt_);
			this->shutdown_ = true;
		}

		wait_shutdown_.notify_all();

	} else {

		if (tenant.second == "tenant") {

			this->_onNewTenant(path.back());

		} else if (tenant.second == "node") {

			this->_onNewNode(path.back());

		} else if (tenant.second == "ignore") {
			// just ignore it
		} else {

			std::cerr << "ERR: Unknown type " << tenant.second << "!!" << std::endl;
			BOOST_ASSERT(false);
		}
	}
}

void HVBench::_onNewNode(const std::string node) {

	std::cout << "New node: " << node << std::endl;

	BOOST_ASSERT(nodes_.count(node) == 0);

	auto pnode = Node::create(shared_from_this(), node);

	std::cout << "Starting node " << node << std::endl;
	pnode->start();

	this->nodes_[node] = pnode;
}

void HVBench::_onNewTenant(const std::string tenant) {

	std::cout << "New tenant: " << tenant << std::endl;

	BOOST_ASSERT(tenants_.count(tenant) == 0);

	auto ptenant = Tenant::create(shared_from_this(), tenant, this->output_);

	std::cout << "Starting tenant.." << std::endl;
	ptenant->start();

	tenants_[tenant] = ptenant;
}

void HVBench::_onChangeTenantConf(const etcd_reply::RapidReply& reply) {

	std::cout << "Update to tenant configuration !" << std::endl;
	reply.Print();

	etcd_reply::RapidReply::KvPairs pairs;
	reply.GetAll(pairs);

	if (pairs.size() == 0) {

		std::cout << "No relevant change found." << std::endl;
		return;
	}

	// Get the changed key and value
	auto pair = *(pairs.begin());

	// Remove the //c_etcd_ptenants_conf// path from the key
	std::string tenant_conf = pair.first.substr(c_etcd_ptenants_conf.length() + 1);

	// Remove the instance ID from the key
	//tenant_conf = tenant_conf.substr(boost::lexical_cast<std::string>(instance_id_).length() + 1);

	std::cout << "Path: " << tenant_conf << std::endl;

	std::vector<std::string> path;
	boost::split(path, tenant_conf, boost::is_any_of("/"));

	auto tenant = path.front();
	auto conf_key = tenant_conf.substr(tenant.length() + 1);

	if (tenants_.count(tenant) == 0) {
		std::cout << "WARN: Configuration change to unknown tenant " << tenant << std::endl;
		return;
	}

	tenants_[tenant]->onChange(conf_key, pair.second);
}

void HVBench::_watch() {

	auto path = boost::str(boost::format("%s/%d/") % c_etcd_phvbench % instance_id_);

	std::cout << "HVBench: Watching " << path << std::endl;
	etcd_watchdog_.Run(path, boost::bind(&HVBench::_onChange, this, _1), true);
}

void HVBench::_watchTenantConf() {

	std::cout << "HVBench: Watching tenant configuration " << c_etcd_ptenants_conf << std::endl;
	etcd_watchdog_tenant_conf.Run(c_etcd_ptenants_conf, boost::bind(&HVBench::_onChangeTenantConf, this, _1), true);
}

void HVBench::_watchGlobal() {

	std::cout << "HVBench: Watching global configuration " << c_etcd_pglobals << std::endl;
	etcd_watchdog_global_.Run(c_etcd_pglobals, boost::bind(&HVBench::_onChangeGlobal, this, _1), true);
}

void HVBench::_onChangeGlobal(const etcd_reply::RapidReply& reply) {

	std::cout << "Detected change on global configuration" << std::endl;

	// Ignore everything besides UPDATE for now
	if (reply.GetAction() != etcd::Action::UPDATE) {
		return;
	}

	std::string study_id = this->etcd_get(boost::str(boost::format("%s/study_id") % c_etcd_pglobals), true);
	auto run_id = boost::lexical_cast<uint32_t>(this->etcd_get(boost::str(boost::format("%s/run_id") % c_etcd_pglobals), true));

	this->output_->update_study(study_id, run_id);
}
