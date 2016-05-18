
#ifndef TENANT_H_
#define TENANT_H_

#include <stdio.h>
#include <memory>
#include <boost/thread.hpp>

#include "HVBenchController_fwd.h"

#include "libs/etcd/etcd.hpp"
#include "libs/etcd/etcd_reply.hpp"
#include "libs/etcd/etcd_helper.hpp"

#include "statistics/output/OutputBase_fwd.h"

class HVBench;
typedef std::shared_ptr<HVBench> HVBench_ptr;

class AbstractGenerator;
typedef std::shared_ptr<AbstractGenerator> AbstractGenerator_ptr;

class Tenant
	: public etcd_helper,
	  public std::enable_shared_from_this<Tenant> {
public:

	typedef std::shared_ptr<Tenant> ptr;

	virtual ~Tenant();

	static Tenant::ptr create(HVBench_ptr parent,
			                  const std::string id,
							  OutputBase_ptr output);

	void start();
	void stop();

	OutputBase_ptr output() { return this->output_; };

	void onChange(std::string key, std::string value);

	const std::string id() { return this->id_; };

private:
	Tenant(const std::string id);

	void _run();

	HVBenchController_ptr hvcontroller_;
	AbstractGenerator_ptr msg_generator_;

	std::string id_;

	boost::thread thread_;

	etcd::Client<etcd_reply::RapidReply> etcd_;

	HVBench_ptr parent_;
	OutputBase_ptr output_;
};

#endif /* TENANT_H_ */
