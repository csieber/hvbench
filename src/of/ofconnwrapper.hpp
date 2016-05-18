#include <fluid/OFServer.hh>
#include <atomic>
#include "../globals.h"
#include "../statistics/hasStatistics.hpp"
#include <memory>
#include "../statistics/StatsVisitor.h"

#ifndef OFCONNWRAPPER_HPP_
#define OFCONNWRAPPER_HPP_

class OFConnWrapper
	: public hasStatistics,
	  public std::enable_shared_from_this<OFConnWrapper> {

	public:
		virtual ~OFConnWrapper() {};

		friend class Collector;

		typedef std::shared_ptr<OFConnWrapper> ptr;

		OFConnWrapper(OFConnection* conn, datapath_T dp)
			: _conn(conn), _dp(dp), byte_cnt_(0) {
		};

		OFConnection* conn() { return this->_conn; };
		const datapath_T& dp() { return this->_dp; };

		bool is_alive() { return this->_conn->is_alive(); };
		int get_id() { return this->_conn->get_id(); };

		const uint64_t byte_cnt() { return this->byte_cnt_; };

		void send(ScheduledMsg& msg) {

			if (this->_conn->is_alive()) {
				this->_conn->send(msg.of_msg->_pkt.get(), msg.of_msg->_pkt_len);
				byte_cnt_ += msg.of_msg->_pkt_len;
			}
		};

		void send_raw(uint8_t* buffer, size_t pkt_len) {

			if (this->_conn->is_alive()) {
				this->_conn->send(buffer, pkt_len);
			}
		};

		/*virtual*/void accept(std::shared_ptr<StatsVisitor> v) {

			v->visit(shared_from_this());
		}

	private:
		OFConnection* _conn;
		datapath_T _dp;
		std::atomic<uint64_t> byte_cnt_;
};

#endif /* OFCONNWRAPPER_HPP_ */
