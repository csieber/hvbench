#ifndef SRC_CONF_H_
#define SRC_CONF_H_

const std::string c_etcd_pbase = "/hvbench";
const std::string c_etcd_phvbench = c_etcd_pbase + "/instances";
const std::string c_etcd_pglobals = c_etcd_pbase + "/globals";
const std::string c_etcd_ptenants_conf = c_etcd_pbase + "/tenants";
const std::string c_etcd_pnodes_conf = c_etcd_pbase + "/nodes/conf";

// hvbench-api version
const std::string conf_api_version = "0.1";

#endif /* SRC_CONF_H_ */
