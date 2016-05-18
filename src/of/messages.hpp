
#include <stdint.h>
#include <boost/chrono.hpp>
#include <map>

#include "../globals.h"

#ifndef GENERATORBASE_H_
#define GENERATORBASE_H_

static uint8_t dropbox_pkt[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0A, 0x00, 0x27,
    			              0x00, 0x00, 0x08, 0x08, 0x00, 0x45, 0x00, 0x00,
	     		              0x8D, 0x79, 0xA4, 0x40, 0x00, 0x40, 0x11, 0xFA, 0x6A,
				              0xC0, 0xA8, 0x22, 0x01, 0xC0, 0xA8, 0x22, 0xFF, 0x44,
				              0x5C, 0x44, 0x5C, 0x00, 0x79, 0xEF, 0x58, 0x7B, 0x22,
				              0x68, 0x6F, 0x73, 0x74, 0x5F, 0x69, 0x6E, 0x74, 0x22,
				              0x3A, 0x20, 0x39, 0x32, 0x31, 0x38, 0x37, 0x30, 0x39,
				              0x36, 0x32, 0x2C, 0x20, 0x22, 0x76, 0x65, 0x72, 0x73,
				              0x69, 0x6F, 0x6E, 0x22, 0x3A, 0x20, 0x5B, 0x31, 0x2C,
				              0x20, 0x38, 0x5D, 0x2C, 0x20, 0x22, 0x64, 0x69, 0x73,
				              0x70, 0x6C, 0x61, 0x79, 0x6E, 0x61, 0x6D, 0x65, 0x22,
				              0x3A, 0x20, 0x22, 0x22, 0x2C, 0x20, 0x22, 0x70, 0x6F,
				              0x72, 0x74, 0x22, 0x3A, 0x20, 0x31, 0x37, 0x35, 0x30,
				              0x31, 0x2C, 0x20, 0x22, 0x6E, 0x61, 0x6D, 0x65, 0x73,
				              0x70, 0x61, 0x63, 0x65, 0x73, 0x22, 0x3A, 0x20, 0x5B,
				              0x37, 0x33, 0x35, 0x38, 0x31, 0x30, 0x37, 0x34, 0x2C,
				              0x20, 0x35, 0x35, 0x31, 0x39, 0x33, 0x37, 0x34, 0x38,
				              0x37, 0x5D, 0x7D};


class AbstractOFMsg {

	public:
		typedef std::shared_ptr<AbstractOFMsg> ptr;
		virtual ~AbstractOFMsg() {};

		uint32_t _xid = 0;
		OFPT::OFPT_T  _type = OFPT::OFPT_UNKNOWN;

		std::shared_ptr<uint8_t> _pkt = 0;
		uint32_t _pkt_len = 0;
		bool _has_reply = true;

	protected:
		AbstractOFMsg(uint32_t xid,
					  OFPT::OFPT_T type,
				      std::shared_ptr<uint8_t> pkt,
				      uint32_t pkt_len,
				      bool has_reply = false):
			_xid(xid), _type(type), _pkt(pkt), _pkt_len(pkt_len), _has_reply(has_reply) {};
};

class AbstractOFMsgFactory {

	public:
		typedef std::shared_ptr<AbstractOFMsgFactory> ptr;
		virtual ~AbstractOFMsgFactory() {};

		virtual AbstractOFMsg::ptr make() = 0;

	protected:
		AbstractOFMsgFactory() {};

		uint32_t _cnt = 0;
};

class FeatureRequestOFMsg : public AbstractOFMsg {

	public:
		typedef std::shared_ptr<FeatureRequestOFMsg> ptr;
		virtual ~FeatureRequestOFMsg() {};

		FeatureRequestOFMsg(uint32_t xid,
			      	  	  	std::shared_ptr<uint8_t> pkt,
			      	  	  	uint32_t pkt_len):
			      	  	  	AbstractOFMsg(xid, OFPT::FEATURE_REQUEST, pkt, pkt_len, true) {};

		static FeatureRequestOFMsg::ptr gen(uint32_t xid) {

			fluid_msg::of10::FeaturesRequest fr(xid);

			std::shared_ptr<uint8_t> buffer(fr.pack(), fluid_msg::OFMsg::free_buffer);

			return std::make_shared<FeatureRequestOFMsg>(xid, buffer, fr.length());
		}
};

class EchoRequestOFMsg : public AbstractOFMsg {

	public:
		typedef std::shared_ptr<EchoRequestOFMsg> ptr;
		virtual ~EchoRequestOFMsg() {};

		EchoRequestOFMsg(uint32_t xid,
			      	  	 std::shared_ptr<uint8_t> pkt,
			      	  	 uint32_t pkt_len):
			      	  	 AbstractOFMsg(xid, OFPT::ECHO_REQUEST, pkt, pkt_len, true) {};

		static EchoRequestOFMsg::ptr gen(uint32_t xid) {

			fluid_msg::of10::EchoRequest fr(xid);

			std::shared_ptr<uint8_t> buffer(fr.pack(), fluid_msg::OFMsg::free_buffer);

			return std::make_shared<EchoRequestOFMsg>(xid, buffer, fr.length());
		}
};

class FlowModOFMsg : public AbstractOFMsg {

	public:
		typedef std::shared_ptr<FlowModOFMsg> ptr;
		virtual ~FlowModOFMsg() {};

		FlowModOFMsg(uint32_t xid,
							  std::shared_ptr<uint8_t> pkt,
							  uint32_t pkt_len):
							  AbstractOFMsg(xid,
									        OFPT::FLOW_MOD,
									        pkt,
									        pkt_len,
									        false) {};

		static FlowModOFMsg::ptr gen(uint32_t xid) {

			fluid_msg::of10::FlowMod fm(xid,  //xid
										123, // cookie
										fluid_msg::of10::OFPFC_ADD, // command
										5000, // idle timeout
										5000, // hard timeout
										100, // priority
										-1, //buffer id
										fluid_msg::of10::OFPP_NONE, // outport
										0); // flags
			fluid_msg::of10::Match m;
			m.in_port((uint16_t)1); // SWITCH physical in_port 1
			fm.match(m);
			fluid_msg::of10::OutputAction act((uint16_t)2, 1024); // Forward to physical port 2
			fm.add_action(act);

			std::shared_ptr<uint8_t> buffer(fm.pack(), fluid_msg::OFMsg::free_buffer);

			return std::make_shared<FlowModOFMsg>(xid, buffer, fm.length());
		}
};

class FlowModOFMsgFactory : public AbstractOFMsgFactory {

public:
	typedef std::shared_ptr<FlowModOFMsgFactory> ptr;
	virtual ~FlowModOFMsgFactory() {};

	AbstractOFMsg::ptr make() {

		this->_cnt++;

		return FlowModOFMsg::gen(this->_cnt);
	}

	static FlowModOFMsgFactory::ptr alloc() {
		return std::make_shared<FlowModOFMsgFactory>();
	}
};

class StatsRequestPortOFMsg : public AbstractOFMsg {

	public:
		typedef std::shared_ptr<StatsRequestPortOFMsg> ptr;
		virtual ~StatsRequestPortOFMsg() {};

		StatsRequestPortOFMsg(uint32_t xid,
			      	  	 	  std::shared_ptr<uint8_t> pkt,
			      	  	 	  uint32_t pkt_len):
			      	  	 	  AbstractOFMsg(xid, OFPT::STATS_REQUEST_PORT, pkt, pkt_len, true) {};

		static StatsRequestPortOFMsg::ptr gen(uint32_t xid) {

			fluid_msg::of10::StatsRequestPort sr(xid,
					                             (uint16_t)0,
					                             (uint16_t)1);

			std::shared_ptr<uint8_t> buffer(sr.pack(), fluid_msg::OFMsg::free_buffer);

			return std::make_shared<StatsRequestPortOFMsg>(xid, buffer, sr.length());
		}
};

class StatsRequestFlowOFMsg : public AbstractOFMsg {

	public:
		typedef std::shared_ptr<StatsRequestFlowOFMsg> ptr;
		virtual ~StatsRequestFlowOFMsg() {};

		StatsRequestFlowOFMsg(uint32_t xid,
			      	  	      std::shared_ptr<uint8_t> pkt,
			      	  	      uint32_t pkt_len):
			      	  	      AbstractOFMsg(xid, OFPT::STATS_REQUEST_FLOW, pkt, pkt_len, true) {};

		static StatsRequestFlowOFMsg::ptr gen(uint32_t xid) {

			fluid_msg::of10::StatsRequestFlow sr(xid,
					                             (uint16_t)0,
					                             (uint16_t)0,
					                             (uint16_t)2);

			std::shared_ptr<uint8_t> buffer(sr.pack(), fluid_msg::OFMsg::free_buffer);

			return std::make_shared<StatsRequestFlowOFMsg>(xid, buffer, sr.length());
		}
};

class PacketOutUDPOFMsg : public AbstractOFMsg {

	public:
		typedef std::shared_ptr<PacketOutUDPOFMsg> ptr;
		virtual ~PacketOutUDPOFMsg() {};

		PacketOutUDPOFMsg(uint32_t xid,
			      	      std::shared_ptr<uint8_t> pkt,
			      	      uint32_t pkt_len):
			      	      AbstractOFMsg(xid, OFPT::PACKET_OUT_UDP, pkt, pkt_len, false) {};

		static PacketOutUDPOFMsg::ptr gen(uint32_t xid) {

			fluid_msg::of10::PacketOut po(xid, (uint32_t)0xffffffff, 1);

			// Set the payload of the PacketOut
			po.data(dropbox_pkt, sizeof(dropbox_pkt));

			// Set the Action (Output, port N)
			fluid_msg::of10::OutputAction act((uint16_t)2, 1024); // Forward to physical port 2
			po.add_action(act);

			std::shared_ptr<uint8_t> buffer(po.pack(), fluid_msg::OFMsg::free_buffer);

			return std::make_shared<PacketOutUDPOFMsg>(xid, buffer, po.length());
		}
};

class PacketOutUDPFactory : public AbstractOFMsgFactory {

public:
	typedef std::shared_ptr<PacketOutUDPFactory> ptr;
	virtual ~PacketOutUDPFactory() {};

	AbstractOFMsg::ptr make() {

		this->_cnt++;

		return PacketOutUDPOFMsg::gen(this->_cnt);
	}

	static PacketOutUDPFactory::ptr alloc() {
		return PacketOutUDPFactory::ptr(new PacketOutUDPFactory());
	}
};

class StatsRequestFlowFactory : public AbstractOFMsgFactory {

public:
	typedef std::shared_ptr<StatsRequestFlowFactory> ptr;
	virtual ~StatsRequestFlowFactory() {};

	AbstractOFMsg::ptr make() {

		this->_cnt++;

		return StatsRequestFlowOFMsg::gen(this->_cnt);
	}

	static StatsRequestFlowFactory::ptr alloc() {
		return StatsRequestFlowFactory::ptr(new StatsRequestFlowFactory());
	}
};

class StatsRequestPortFactory : public AbstractOFMsgFactory {

public:
	typedef std::shared_ptr<StatsRequestPortFactory> ptr;
	virtual ~StatsRequestPortFactory() {};

	AbstractOFMsg::ptr make() {

		this->_cnt++;

		return StatsRequestPortOFMsg::gen(this->_cnt);
	}

	static StatsRequestPortFactory::ptr alloc() {
		return StatsRequestPortFactory::ptr(new StatsRequestPortFactory());
	}
};

class FeatureRequestFactory : public AbstractOFMsgFactory {

public:
	typedef std::shared_ptr<FeatureRequestFactory> ptr;
	virtual ~FeatureRequestFactory() {};

	AbstractOFMsg::ptr make() {

		this->_cnt++;

		return FeatureRequestOFMsg::gen(this->_cnt);
	}

	static FeatureRequestFactory::ptr alloc() {
		return FeatureRequestFactory::ptr(new FeatureRequestFactory());
	}
};

class EchoRequestFactory : public AbstractOFMsgFactory {

public:
	typedef std::shared_ptr<EchoRequestFactory> ptr;
	virtual ~EchoRequestFactory() {};

	AbstractOFMsg::ptr make() {

		this->_cnt++;

		return EchoRequestOFMsg::gen(this->_cnt);
	}

	static EchoRequestFactory::ptr alloc() {
		return EchoRequestFactory::ptr(new EchoRequestFactory());
	}
};

static
AbstractOFMsgFactory::ptr get_message_factory(std::string name) {

	if (name == "FEATURE_REQUEST") {
		return FeatureRequestFactory::alloc();
	} else if (name == "ECHO_REQUEST") {
		return EchoRequestFactory::alloc();
	} else if (name == "STATS_REQUEST_PORT") {
		return StatsRequestPortFactory::alloc();
	} else if (name == "STATS_REQUEST_FLOW") {
		return StatsRequestFlowFactory::alloc();
	} else if (name == "PACKET_OUT_UDP") {
		return PacketOutUDPFactory::alloc();
	} else if (name == "FLOW_MOD") {
		return FlowModOFMsgFactory::alloc();
	} else {

		std::cout << "Unknown message type " << name << ". Using ECHO_REQUEST" << std::endl;
		return EchoRequestFactory::alloc();
	}
}

class ScheduledMsg {

	public:
		ScheduledMsg(time_point_t time_ns,
				     AbstractOFMsg::ptr of_msg):
			time_ns(time_ns),
			of_msg(of_msg),
			_awaits_reply(of_msg->_has_reply) {
		}

		time_point_t time_ns;
		AbstractOFMsg::ptr of_msg;
		bool _awaits_reply;
};


#endif // GENERATORBASE_H_
