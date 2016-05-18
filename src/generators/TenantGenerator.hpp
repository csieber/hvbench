#include <map>
#include <memory>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

#include "../globals.h"

#ifndef TENANTGENERATOR_HPP_
#define TENANTGENERATOR_HPP_

class TenantGenerator
	: public AbstractGenerator,
	  public etcd_helper {

	public:
		typedef std::shared_ptr<TenantGenerator> ptr;

		virtual ~TenantGenerator() {};

		TenantGenerator(std::string basep,
				        uint32_t rate,
				        std::vector<OFPT::OFPT_T> of_types,
				        std::vector<float> weights)
			: etcd_helper(basep),
			  _weights(weights),
			  _of_types(of_types),
			  _rate(rate),
			  _rng(std::time(NULL)),
			  _nd(rate),
			  _var_exp(new VarGenMtExp(_rng, _nd)),
			  _discrete_nd(_weights),
			  _var_disc(new VarGenMtDis(_rng, _discrete_nd))
			  {

			_next_msg += boost::chrono::microseconds(static_cast<uint64_t>((*_var_exp.get())() * 1000000.));
		};

		time_point_t peek() {

			auto tp = base_time_ + _next_msg;

			return tp;
		};

		ScheduledMsg next() {

			_idx++;

			auto next_type = _of_types[(*_var_disc.get())()];

			AbstractOFMsg::ptr ofmsg_ptr;

			switch(next_type) {

				case OFPT::FEATURE_REQUEST:
				{
					ofmsg_ptr = FeatureRequestOFMsg::gen(_idx);
				}
				break;
				case OFPT::ECHO_REQUEST:
				{
					ofmsg_ptr = EchoRequestOFMsg::gen(_idx);
				}
				break;
				case OFPT::STATS_REQUEST_PORT:
				{
					ofmsg_ptr = StatsRequestPortOFMsg::gen(_idx);
				}
				break;
				case OFPT::STATS_REQUEST_FLOW:
				{
					ofmsg_ptr = StatsRequestFlowOFMsg::gen(_idx);
				}
				break;
				case OFPT::PACKET_OUT_UDP:
				{
					ofmsg_ptr = PacketOutUDPOFMsg::gen(_idx);
				}
				break;
				case OFPT::FLOW_MOD:
				{
					ofmsg_ptr = FlowModOFMsg::gen(_idx);
				}
				break;
				default:
					BOOST_ASSERT(false);
			}

			auto scheduledm = ScheduledMsg(base_time_ + _next_msg, ofmsg_ptr);

			_next_msg += boost::chrono::microseconds(static_cast<uint64_t>((*_var_exp.get())() * 1000000.));

			return count(scheduledm);
		};

		void update(std::string key, std::string value) {

			std::cout << "UPDATE " << key << " to " << value << std::endl;
			
			if (key == "avg_rate") {

				//FIXME: This is not thread-safe!
				_nd = boost::exponential_distribution<>(boost::lexical_cast<uint32_t>(value));
				_var_exp = std::shared_ptr<VarGenMtExp>(new VarGenMtExp(_rng, _nd));
			}

			if (OFPT::RSTR.find(key) == OFPT::RSTR.end()) {

				std::cerr << "Unknown message type " << key << "!" <<std::endl;
				return;
			}

			auto it = std::find(this->_of_types.begin(), this->_of_types.end(), OFPT::RSTR.at(key));

			if (it == this->_of_types.end()) {

				std::cerr << "ERR: Updated message weight for message type " << key << " was not in the original message type set!" <<std::endl;
				return;

			} else {

				auto index = std::distance(this->_of_types.begin(), it);

				this->_weights[index] = boost::lexical_cast<float>(value);

				this->_discrete_nd = boost::random::discrete_distribution<>(_weights);

				this->_var_disc = std::make_shared<VarGenMtDis>(this->_rng, this->_discrete_nd);

				std::cout << "Weight of " << key << " updated successfully to " << value << "." << std::endl;
			}
		}


	private:
		std::vector<float>        _weights;
		std::vector<OFPT::OFPT_T> _of_types;
		uint32_t& _rate;

		boost::mt19937 _rng;

		// Inter-arrival distribution
		boost::exponential_distribution<> _nd;

		typedef boost::variate_generator<boost::mt19937&,
		         	 	 	 	 	 	 boost::exponential_distribution<> > VarGenMtExp;

		std::shared_ptr<VarGenMtExp> _var_exp;

		// Message type distribution
		boost::random::discrete_distribution<> _discrete_nd;

		typedef boost::variate_generator<boost::mt19937&,
					             boost::random::discrete_distribution<> > VarGenMtDis;

		std::shared_ptr<VarGenMtDis> _var_disc;

		std::map<OFPT::OFPT_T, AbstractOFMsgFactory::ptr> _msg_factories;

		boost::chrono::microseconds _next_msg;

		uint32_t _idx = 0;
};

enum TTYPES {

	TT_MISUSE_FEATURE_REQ = 0,
	TT_MISUSE_ECHO_REQ = 1,
	TT_EQUAL = 2,
	TT_STATS = 3,
	TT_PACKET_OUT_UDP = 4,
	TT_FLOW_MOD = 5
};


/*
typedef std::pair<std::vector<OFPT::OFPT_T>, std::vector<float> > OFPT_WEIGHTS_PAIR_T;

class TenantFactory {

	public:
		TenantFactory() {};

		static TenantGenerator::ptr create(const uint32_t avgr) {

			boost::mt19937 rng(std::time(NULL));

			std::vector<TTYPES> tt_types        = {TT_MISUSE_FEATURE_REQ, TT_MISUSE_ECHO_REQ, TT_EQUAL, TT_STATS, TT_PACKET_OUT_UDP, TT_FLOW_MOD};
			std::vector<float> tt_stats_weights = {                  0.1,                0.1,      0.2,      0.2,               0.2,         0.2};

			boost::random::discrete_distribution<> discrete_nd(tt_stats_weights);
			boost::variate_generator<boost::mt19937&,
						             boost::random::discrete_distribution<> > var_disc(rng, discrete_nd);

			const uint32_t avg_rate = avgr;
			const uint32_t std_rate = 50;
			const uint32_t min_rate = 10;
			const uint32_t max_rate = avg_rate * 2;

			boost::normal_distribution<> normal_nd(avg_rate, std_rate);
			boost::variate_generator<boost::mt19937&,
						 boost::normal_distribution<> > var_nor(rng, normal_nd);

			auto tenant_desc = getByTT(tt_types[var_disc()]);

			auto rate = std::max(min_rate, std::min(max_rate, static_cast<uint32_t>(var_nor())));

			auto tenant = TenantGenerator::ptr(new TenantGenerator(rate, tenant_desc.first, tenant_desc.second));

			return tenant;
		};

		static OFPT_WEIGHTS_PAIR_T getByTT(TTYPES type) {

			std::vector<OFPT::OFPT_T> msg_types;
			std::vector<float> msg_weights;

			switch(type){
				case TT_MISUSE_FEATURE_REQ:
				{
					msg_types   = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST, OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW, OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};
					msg_weights =                   {0.5,                0.1,                      0.2,                      0.1,                  0.1,            0.1};
				}
				break;
				case TT_MISUSE_ECHO_REQ:
				{
					msg_types   = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST, OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW, OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};
					msg_weights =                   {0.1,                0.3,                      0.4,                      0.1,                  0.1,            0.1};
				}
				break;
				case TT_EQUAL:
				{
					msg_types   = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST, OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW, OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};
					msg_weights =                   {0.3,                0.3,                      0.2,                      0.1,                  0.1,            0.1};
				}
				break;
				case TT_STATS:
				{
					msg_types   = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST, OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW, OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};
					msg_weights =                  {0.05,               0.04,                     0.45,                     0.45,                  0.1,            0.1};
				}
				break;
				case TT_PACKET_OUT_UDP:
				{
					msg_types   = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST, OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW, OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};
					msg_weights =                  {   0,                  0,                        0,                        0,                    1,            0.};
				}
				break;
				case TT_FLOW_MOD:
				{
					msg_types   = {OFPT::FEATURE_REQUEST, OFPT::ECHO_REQUEST, OFPT::STATS_REQUEST_PORT, OFPT::STATS_REQUEST_FLOW, OFPT::PACKET_OUT_UDP, OFPT::FLOW_MOD};
					msg_weights =                  { 0.1,                0.1,                      0.1,                      0.1,                  0.1,            0.5};
				}
				break;
				default:
					BOOST_ASSERT(false);
			}

			return OFPT_WEIGHTS_PAIR_T(msg_types, msg_weights);
		}
};
*/
#endif /* TENANTGENERATOR_HPP_ */
