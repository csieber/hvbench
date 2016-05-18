
#ifndef ETCD_HELPER_HPP_
#define ETCD_HELPER_HPP_

#include <iostream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "../../globals.h"
#include "etcd.hpp"
#include "etcd_reply.hpp"

class etcd_helper {

public:
	static std::string get(etcd::Client<etcd_reply::RapidReply>& etcd, std::string path) {

		auto reply = etcd.Get(path);

		etcd_reply::RapidReply::KvPairs pairs;
		reply.GetAll(pairs);

		return pairs[path];
	}

protected:

	etcd_helper(std::string basep)
		: etcd_(gl_etcd_ip, gl_etcd_port),
		  basep_(basep) {

	}

	std::string etcd_get(std::string path, bool abs = false) {

		std::string abs_path;

		if (!abs)
			abs_path = boost::str(boost::format("%s/%s") % basep_ % path);
		else
			abs_path = path;

		std::cout << "GET: " << abs_path << std::endl;

		auto reply = etcd_.Get(abs_path);

		etcd_reply::RapidReply::KvPairs pairs;
		reply.GetAll(pairs);

		std::cout << "RESULT: " << pairs[abs_path] << std::endl;

		return pairs[abs_path];
	};

	void etcd_write(std::string path, std::string value, bool abs = false) {

		std::string abs_path;

		if (abs)
			abs_path = boost::str(boost::format("%s/%s") % basep_ % path);
		else
			abs_path = path;

		std::cout << "WRITE: " << abs_path << ": " << value << std::endl;

		etcd_.Set(abs_path, value);
	}

	etcd::Client<etcd_reply::RapidReply> etcd_;
	std::string basep_;
};

#endif /* ETCD_HELPER_HPP_ */
