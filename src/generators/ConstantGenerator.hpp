#ifndef CONSTANTGENERATOR_HPP_
#define CONSTANTGENERATOR_HPP_

#include <atomic>
#include <boost/lexical_cast.hpp>

class ConstantGenerator
	: public AbstractGenerator,
	  public etcd_helper {

	public:
		typedef std::shared_ptr<ConstantGenerator> ptr;

		virtual ~ConstantGenerator() {};

		ConstantGenerator(std::string basep)
			: etcd_helper(basep) {

			auto avg_rate = boost::lexical_cast<uint32_t>(etcd_get("avg_rate"));

			auto msg_T = etcd_get("msg_t");

			_msg_factory = get_message_factory(msg_T);

			inter_arrival_t_ = boost::chrono::nanoseconds((uint32_t)(1000000000. / (double)avg_rate));
		};

		time_point_t peek() {

			auto tp = base_time_ + local_ns_ + inter_arrival_t_;

			return tp;
		};

		ScheduledMsg next() {

			local_ns_ += inter_arrival_t_;

			auto scheduledm = ScheduledMsg(base_time_ + local_ns_, this->_msg_factory->make());

			return count(scheduledm);
		};

		void update(std::string key, std::string value) {

			std::cout << "UPDATE " << key << " to " << value << std::endl;

			if (key == "avg_rate") {

				std::cout << "Updating tenenat message rate." << std::endl;

				const auto new_avg_rate = boost::lexical_cast<uint32_t>(value);

				//FIXME: this is not thread-safe !
				this->inter_arrival_t_ = boost::chrono::nanoseconds((uint32_t)(1000000000. / (double)new_avg_rate));
			}
		}

	private:
		boost::chrono::nanoseconds inter_arrival_t_ = boost::chrono::nanoseconds(0);
		boost::chrono::nanoseconds local_ns_ = boost::chrono::nanoseconds(0);
		AbstractOFMsgFactory::ptr _msg_factory;
};

#endif /* CONSTANTGENERATOR_HPP_ */
