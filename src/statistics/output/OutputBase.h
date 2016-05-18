#ifndef OUTPUTBASE_H_
#define OUTPUTBASE_H_

#include "../Collector_fwd.h"
#include <boost/thread/mutex.hpp>

class OutputBase {
public:
	virtual ~OutputBase();

	virtual void write(std::string tenant, Collector_ptr col) = 0;

	virtual void update_study(std::string study_id, int32_t run_id);

protected:
	OutputBase();

	boost::mutex class_lock_;
	std::string study_id_;
	int32_t run_id_;
};

#endif /* OUTPUTBASE_H_ */
