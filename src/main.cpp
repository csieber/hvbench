#include <string>
#include <tclap/CmdLine.h>
#include <boost/chrono.hpp>
#include "statistics/output/PrinterOutput.h"
#include "statistics/output/KafkaOutput.h"
#include "globals.h"

#include "HVBench.h"

int main(int argc, char **argv) {

	uint32_t instance_id;
	std::string kafkaIp;

	try {

	TCLAP::CmdLine cmd("hvbench: A scalable SDN hypervisor benchmarking tool.", ' ', "0.1");

	// ValueArg Arguments are:
	//	short param e.g. "-p",
	//	long param "--port",
	//	short description,
	//	required,
	//	default value,
	//	data type

	// IP
	TCLAP::ValueArg<std::string>    ipArg("e", "etcd",     "etcd IP", false, "127.0.0.1", "string");
	TCLAP::ValueArg<std::string> kafkaArg("k", "kafka",   "kafka IP", false, "127.0.0.1", "string");
	TCLAP::ValueArg<uint32_t>     portArg("p", "port",   "etcd Port", false,        2379,   "int");
	TCLAP::ValueArg<uint32_t>     instArg("i",   "id", "Instance ID", false,           1,   "int");

	cmd.add( ipArg );
	cmd.add( kafkaArg );
	cmd.add( portArg );
	cmd.add( instArg );

	cmd.parse(argc, argv);

	gl_etcd_ip = ipArg.getValue();
	kafkaIp = kafkaArg.getValue();
	gl_etcd_port = portArg.getValue();
	instance_id = instArg.getValue();

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	auto output = KafkaOutput::create(boost::str(boost::format("%s:9092") % kafkaIp),
									  boost::str(boost::format("hvbench_%s") % instance_id));
	// PrinterOutput::create()

	HVBench::ptr hv = HVBench::create(output, gl_etcd_ip, gl_etcd_port, instance_id);

	if (!hv) {
		std::cerr << "! Failed to start hvbench !" << std::endl;
		return 1;
	}

	hv->run();

	return 0;
}
