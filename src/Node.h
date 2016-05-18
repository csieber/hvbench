#ifndef NODE_H_
#define NODE_H_

#include <string>
#include "HVBench_fwd.h"
#include "HVBenchNode_fwd.h"
#include "libs/etcd/etcd_helper.hpp"

class Node
  : public etcd_helper {
public:
	Node(const std::string id);
	virtual ~Node();

	typedef std::shared_ptr<Node> ptr;

	static Node::ptr create(HVBench_ptr parent, const std::string id);

	void start();
	void stop();

	void onChange(std::string key, std::string value);

private:
	std::string id_;
	HVBench_ptr parent_;

	HVBenchNode_ptr node_;
};

#endif /* NODE_H_ */
