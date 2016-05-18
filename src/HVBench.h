/*
 * HVBench.h
 *
 *  Created on: Jan 15, 2016
 *      Author: sieber
 */

#ifndef HVBENCH_H_
#define HVBENCH_H_

#include <boost/thread.hpp>
#include "libs/etcd/etcd.hpp"
#include "libs/etcd/etcd_reply.hpp"
#include "libs/etcd/etcd_helper.hpp"
#include "Tenant_fwd.h"
#include "Node_fwd.h"
#include "statistics/output/OutputBase_fwd.h"

class HVBench
	: public std::enable_shared_from_this<HVBench>,
	  public etcd_helper {
public:

	typedef std::shared_ptr<HVBench> ptr;

	virtual ~HVBench();

	static HVBench::ptr create(OutputBase_ptr output,
							   std::string etcd_ip,
							   uint32_t etcd_port,
							   uint32_t instance_id);

	void run();

	const uint32_t instance_id() { return instance_id_; };

private:

	HVBench();

	void _watch();
	void _watchTenantConf();
	void _watchGlobal();

	void _onChange(const etcd_reply::RapidReply& reply);
	void _onChangeTenantConf(const etcd_reply::RapidReply& reply);
	void _onChangeGlobal(const etcd_reply::RapidReply& reply);

	void _onNewTenant(const std::string tenant);
	void _onNewNode(const std::string node);

	std::map<std::string, Tenant_ptr> tenants_;
	std::map<std::string, Node_ptr> nodes_;

	uint32_t instance_id_;
	etcd::Watch<etcd_reply::RapidReply> etcd_watchdog_;
	etcd::Watch<etcd_reply::RapidReply> etcd_watchdog_tenant_conf;
	etcd::Watch<etcd_reply::RapidReply> etcd_watchdog_global_;

	boost::thread thread_watch_conf_;
	boost::thread thread_watch_hvbench_;
	boost::thread thread_watch_global_;

	bool shutdown_;

	boost::mutex wait_shutdown_mt_;
	boost::condition_variable wait_shutdown_;

	OutputBase_ptr output_;
};

#endif /* HVBENCH_H_ */
