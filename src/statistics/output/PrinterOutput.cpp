#include "PrinterOutput.h"

PrinterOutput::PrinterOutput() {
}

PrinterOutput::~PrinterOutput() {

}

void PrinterOutput::write(std::string tenant, Collector_ptr col) {

	std::cout << "Output" << std::endl;
}

/*static*/
PrinterOutput::ptr PrinterOutput::create() {

	return PrinterOutput::ptr(new PrinterOutput());
}
