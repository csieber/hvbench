
#include "HVBenchController.h"

#include "globals.h"
//#include "of/messages.hpp"
#include "of/ofconnwrapper.hpp"
#include "statistics/output/OutputBase.h"

HVBenchController::HVBenchController(std::string etcd_basep,
				  	  	  	  	     std::string address,
				  	  	  	  	     const int port,
				  	  	  	  	     const int n_workers,
				  	  	  	  	     bool secure):

		// Base class OFServer
		OFServer(address.c_str(),
				port,
				n_workers,
				secure,
				OFServerSettings()
					.supported_version(1)
					.keep_data_ownership(false)
					.echo_interval(120)
					.liveness_check(false)
					.handshake(true)),

		// Base class etcd_helper
		etcd_helper(etcd_basep),

		reply_waiting_list(100) {

	for (auto& e : OFPT::req_types)
		this->counters_[e] = 0;

	start_ = clock_source::now();
	latency_stats_ = std::shared_ptr<OFLatencyStats>(new OFLatencyStats());

	this->set_config(ofsc);
	this->running = true;

	last_bytes_cnt = 0;
}

HVBenchController::ptr HVBenchController::create(std::string etcd_basep,
							  	  	  	  	  	 Tenant::ptr parent,
							  	  	  	  	  	 std::string address,
							  	  	  	  	  	 const int port) {

	HVBenchController::ptr newp = HVBenchController::ptr(new HVBenchController(etcd_basep, address, port));

	newp->parent_ = parent;

	return newp;
}

void HVBenchController::stop() {
	this->running = false;
	OFServer::stop();
}

void HVBenchController::connection_callback(OFConnection* ofconn, OFConnection::Event type) {

	if (type == OFConnection::EVENT_STARTED) {
		printf("Connection id=%d started\n", ofconn->get_id());

	} else if (type == OFConnection::EVENT_ESTABLISHED) {

		if (!this->switch_up) {

			printf("Connection id=%d established\n", ofconn->get_id());

			this->switch_up = true;
			this->conn = OFConnWrapper::ptr(new OFConnWrapper(ofconn, 0));

			{
				boost::lock_guard<boost::mutex> lock(mut_switch_up);
				switch_up=true;
			}
			cond_switch_up.notify_one();
		}

	} else if (type == OFConnection::EVENT_FAILED_NEGOTIATION) {
		//printf("Connection id=%d failed version negotiation\n", ofconn->get_id());

	} else if (type == OFConnection::EVENT_CLOSED) {
		//printf("Connection id=%d closed by the user\n", ofconn->get_id());
		//dispatch_event(new SwitchDownEvent(ofconn));

	} else if (type == OFConnection::EVENT_DEAD) {
		//printf("Connection id=%d closed due to inactivity\n", ofconn->get_id());
		//dispatch_event(new SwitchDownEvent(ofconn));
	}
}

void HVBenchController::message_callback(OFConnection* ofconn, uint8_t type, void* data, size_t len) {

	switch(type) {

	case OFPT::PACKET_IN:

	break;
	case OFPT::FEATURES_REPLY:
	{
		auto *reply = new fluid_msg::of10::FeaturesReply();
		reply->unpack(static_cast<uint8_t*>(data));

		latency_stats_->received(reply->xid(),
				                static_cast<OFPT::OFPT_T>(type),
				                clock_source::now());

		delete reply;
	}
	break;
	case OFPT::ECHO_REQUEST:
	{
		fluid_msg::of10::EchoReply fr(0);

		std::shared_ptr<uint8_t> buffer(fr.pack(), fluid_msg::OFMsg::free_buffer);

		if (ofconn->is_alive()) {
			ofconn->send(buffer.get(), fr.length());
		}

	}
	break;
	case OFPT::ECHO_REPLY:
	{
		auto *reply = new fluid_msg::of10::EchoReply();
		reply->unpack(static_cast<uint8_t*>(data));

		latency_stats_->received(reply->xid(),
								static_cast<OFPT::OFPT_T>(type),
				                clock_source::now());

		delete reply;
	}
	break;
	case OFPT::STATS_REPLY:
	{
		auto reply = new fluid_msg::of10::StatsReply();
		reply->unpack(static_cast<uint8_t*>(data));

		//BOOST_ASSERT(reply->stats_type() == OFPT::STATS_REQUEST_FLOW ||
		//		     reply->stats_type() == OFPT::STATS_REQUEST_PORT);

		latency_stats_->received(reply->xid(),
				                static_cast<OFPT::OFPT_T>(reply->stats_type()),
				                clock_source::now());

		delete reply;
	}
	break;
	case OFPT::HELLO:
	break;
	case OFPT::ERROR:
	{
		fluid_msg::of10::Error *error = new fluid_msg::of10::Error();
		error->unpack((uint8_t*)data);
		std::cout << "OF ERROR: "
			<< "Type: " << error->err_type()
			<< " Code: " << error->code() <<  "\n";
	}
	break;
	case OFPT::PORT_STATUS:
		std::cout << "EVENT: Port Status" << std::endl;
	break;
	default:
		printf("Message tye: %d", type);
		std::cout << "Unknown message type " << std::hex << type << " received!" << std::endl;
	}

	delete static_cast<uint8_t*>(data);
}

void HVBenchController::run(AbstractGenerator::ptr generator) {

	std::cout << "Waiting for switch to connect.." << std::endl;

	boost::unique_lock<boost::mutex> lock(mut_switch_up);
	while(!switch_up) {

		cond_switch_up.wait(lock);
	}

	usleep(1 * 1000000);

	setupSwitch();

	usleep(1 * 1000000);

	std::cout << "Starting message flow" << std::endl;

	this->start_ = clock_source::now();

	auto now = this->start_;
	auto last_stat_time = boost::chrono::seconds(std::time(NULL));

	generator->set_time_reference(now);

	while (running) {

		auto next = generator->next();

		while (true) {

			now = clock_source::now();

			const auto ux_time = boost::chrono::seconds(std::time(NULL));

			if (last_stat_time != ux_time) {

				last_stat_time = ux_time;

				auto col = std::shared_ptr<Collector>(new Collector());
				this->accept(col);

				col->print();

				this->parent_->output()->write(this->parent_->id(), col);
				//this->csv_->write(this->parent_->id(), col);

				stats_late_sends = boost_acc_set_late_sends();
			}

			if ((next.time_ns - now) > boost::chrono::nanoseconds(0)) {
				usleep(1000);
				continue;
			}

			now = clock_source::now();

			// Use _late_ as a metric for the sending process
			auto late = now - next.time_ns;

			this->conn->send(next);

			auto late_ns = boost::chrono::duration_cast<boost::chrono::nanoseconds>(late).count();
			stats_late_sends(late_ns);

			this->counters_[next.of_msg->_type] += 1;

			if (next._awaits_reply) {
				latency_stats_->sent(next.of_msg->_xid, next.of_msg->_type, now);
			}

			break;
		}
	}

	std::cout << "Finished sending process." << std::endl;
}

void HVBenchController::setupSwitch() {

	std::cout << "Setting up switch.." << std::endl;

	const uint16_t timeout = 3600;

	{
			uint8_t* buffer;
			fluid_msg::of10::FlowMod fm(1,  //xid
				123, // cookie
				fluid_msg::of10::OFPFC_ADD, // command
				timeout, // idle timeout
				timeout, // hard timeout
				100, // priority
				-1, //buffer id
				fluid_msg::of10::OFPP_NONE, // outport
				0); // flags
			fluid_msg::of10::Match m;
			m.in_port((uint16_t)1); // SWITCH physical in_port 1
			fm.match(m);
			fluid_msg::of10::OutputAction act((uint16_t)2, 1024); // Forward to physical port 2
			fm.add_action(act);
			buffer = fm.pack();
			this->conn->send_raw(buffer, fm.length());
			fluid_msg::OFMsg::free_buffer(buffer);
		}
		{
			uint8_t* buffer;
			fluid_msg::of10::FlowMod fm(2,  //xid
				123, // cookie
				fluid_msg::of10::OFPFC_ADD, // command
				timeout, // idle timeout
				timeout, // hard timeout
				100, // priority
				-1, //buffer id
				fluid_msg::of10::OFPP_NONE, // outport
				0); // flags
			fluid_msg::of10::Match m;
			m.in_port((uint16_t)2);
			fm.match(m);
			fluid_msg::of10::OutputAction act((uint16_t)1, 1024);
			fm.add_action(act);
			buffer = fm.pack();
			this->conn->send_raw(buffer, fm.length());
			fluid_msg::OFMsg::free_buffer(buffer);
		}
}

/*virtual*/void HVBenchController::accept(std::shared_ptr<StatsVisitor> v) {

	v->visit(shared_from_this());
	v->visit(this->latency_stats_);
	this->latency_stats_->reset();
	v->visit(this->conn);
}
