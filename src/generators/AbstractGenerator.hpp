
#include <vector>
#include "../libs/etcd/etcd_helper.hpp"

#ifndef GENERATORS_HPP_
#define GENERATORS_HPP_

class AbstractGenerator {

	public:
		typedef std::shared_ptr<AbstractGenerator> ptr;

		virtual ~AbstractGenerator() {};

		virtual time_point_t peek() = 0;
		virtual ScheduledMsg next() = 0;

		virtual void update(std::string key, std::string value) = 0;

		virtual void set_time_reference(time_point_t base_time) {
			base_time_ = base_time;
		}

		ScheduledMsg& count(ScheduledMsg& msg) {

			if (_counters.find(msg.of_msg->_type) == _counters.end()) {
				_counters[msg.of_msg->_type] = 1;
			} else {
				_counters[msg.of_msg->_type] += 1;
			}

			return msg;
		};

		virtual std::map<OFPT::OFPT_T, uint32_t> counters() { return _counters; };

		virtual const uint32_t size() { return 0; };

		virtual void reset_counters() {

			 for (auto& iter : _counters) {
				iter.second = 0;
			 }
		};

	protected:
		AbstractGenerator() {};

		time_point_t base_time_;
		std::map<OFPT::OFPT_T, uint32_t> _counters;
};

class GeneratorContainer : public AbstractGenerator {

	public:
		typedef std::shared_ptr<GeneratorContainer> ptr;

		virtual ~GeneratorContainer() {};

		void add(AbstractGenerator::ptr gen) {
			_generators.push_back(gen);
		};

		void set_time_reference(time_point_t base_time) {
			base_time_ = base_time;
			for (auto& iter : this->_generators) {
				iter->set_time_reference(base_time);
			}
		}

		time_point_t peek() {

			BOOST_ASSERT(!this->_generators.empty());

			auto min = this->_generators.front()->peek();

			for (auto& iter : this->_generators) {

				if (iter->peek() < min) {
					min = iter->peek();
				}
			}

			return min;
		};

		const uint32_t size() { return this->_generators.size(); };

		ScheduledMsg next() {

			BOOST_ASSERT(!this->_generators.empty());

			auto min = this->_generators.front();

			for (auto& iter : this->_generators) {

				if (iter->peek() < min->peek()) {
					min = iter;
				}
			}

			auto scheduledmsg = min->next();

			return count(scheduledmsg);
		};

		bool empty() {
			return _generators.empty();
		}

	private:
		std::vector<AbstractGenerator::ptr> _generators;
};

#endif /* GENERATORS_HPP_ */
