
#ifndef PRINTEROUTPUT_H_
#define PRINTEROUTPUT_H_

#include "OutputBase.h"
#include "boost/shared_ptr.hpp"

class PrinterOutput: public OutputBase {
public:

	typedef std::shared_ptr<PrinterOutput> ptr;

	static PrinterOutput::ptr create();

	virtual void write(std::string tenant, Collector_ptr col);

	virtual ~PrinterOutput();

private:
	PrinterOutput();
};

#endif /* PRINTEROUTPUT_H_ */
