#include "Node.h"
#include "HVBench.h"
#include "HVBenchNode.h"
#include "globals.h"


Node::Node(std::string id)
 : etcd_helper(boost::str(boost::format("%s/%s") % c_etcd_pnodes_conf % id)),
   id_(id) {

}

Node::~Node() {
}

/*static*/
Node::ptr Node::create(HVBench::ptr parent, const std::string id) {

	std::cout << "Creating data plane node with id " << id << std::endl;

	Node::ptr tmp = Node::ptr(new Node(id));

	tmp->parent_ = parent;

	return tmp;
}

void Node::start() {

	auto hv_ip = this->etcd_get("setup/hv_ip");
	auto hv_port = boost::lexical_cast<uint32_t>(this->etcd_get("setup/hv_port"));
	auto hv_dp = boost::lexical_cast<uint64_t>(this->etcd_get("setup/datapath_id"));

	node_ = HVBenchNode::create(0, hv_ip, hv_port, hv_dp);

	node_->start();
}

void Node::stop() {

	node_->stop();
}
