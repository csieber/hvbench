
#ifndef CONTROLLEREVENTS_HPP_
#define CONTROLLEREVENTS_HPP_

class ControllerEvent {
public:
	ControllerEvent(OFConnection* ofconn, int type) {
		this->ofconn = ofconn;
		this->type = type;
	}
	virtual ~ControllerEvent() {}

	virtual int get_type() {
		return this->type;
	}

	OFConnection* ofconn;

private:
	int type;
};

class PacketInEvent : public ControllerEvent {
public:
	PacketInEvent(OFConnection* ofconn, OFHandler* ofhandler, void* data, size_t len) :
		ControllerEvent(ofconn, EVENT_PACKET_IN) {
		this->data = (uint8_t*)data;
		this->len = len;
		this->ofhandler = ofhandler;
	}

	virtual ~PacketInEvent() {
		this->ofhandler->free_data(this->data);
	}

	OFHandler* ofhandler;
	uint8_t* data;
	size_t len;
};

class SwitchUpEvent : public ControllerEvent {
public:
	SwitchUpEvent(OFConnection* ofconn, OFHandler* ofhandler) :
		ControllerEvent(ofconn, EVENT_SWITCH_UP) {
		this->ofhandler = ofhandler;
	}

	OFHandler* ofhandler;
	uint8_t* data;
	size_t len;
};

class SwitchDownEvent : public ControllerEvent {
public:
	SwitchDownEvent(OFConnection* ofconn) :
		ControllerEvent(ofconn, EVENT_SWITCH_DOWN) {}
};


class EchoReplyEvent : public ControllerEvent {
public:
	EchoReplyEvent(OFConnection* ofconn, OFHandler* ofhandler, void* data, size_t len) :
		ControllerEvent(ofconn, EVENT_ECHO_REPLY) {
		this->data = (uint8_t*)data;
		this->len = len;
		this->ofhandler = ofhandler;
	}

	virtual ~EchoReplyEvent() {
		this->ofhandler->free_data(this->data);
	}

	OFHandler* ofhandler;
	uint8_t* data;
	size_t len;
};


class FeaturesReplyEvent : public ControllerEvent {
public:
	FeaturesReplyEvent(OFConnection* ofconn, OFHandler* ofhandler, void* data, size_t len) :
		ControllerEvent(ofconn, EVENT_FEATURES_REPLY) {
		this->data = (uint8_t*)data;
		this->len = len;
		this->ofhandler = ofhandler;
	}

	virtual ~FeaturesReplyEvent() {
		this->ofhandler->free_data(this->data);
	}

	OFHandler* ofhandler;
	uint8_t* data;
	size_t len;
};


class StatsReplyEvent : public ControllerEvent {
public:
	StatsReplyEvent(OFConnection* ofconn, OFHandler* ofhandler, void* data, size_t len) :
		ControllerEvent(ofconn, EVENT_STATS_REPLY) {
		this->data = (uint8_t*)data;
		this->len = len;
		this->ofhandler = ofhandler;
	}

	virtual ~StatsReplyEvent() {
		this->ofhandler->free_data(this->data);
	}

	OFHandler* ofhandler;
	uint8_t* data;
	size_t len;
};

#endif /* CONTROLLEREVENTS_HPP_ */
