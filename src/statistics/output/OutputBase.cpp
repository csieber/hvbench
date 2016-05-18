/*
 * OutputBase.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: sieber
 */

#include "OutputBase.h"

OutputBase::OutputBase() {
}

OutputBase::~OutputBase() {
}


void OutputBase::update_study(std::string study_id, int32_t run_id) {

	boost::mutex::scoped_lock(this->class_lock_);

	this->study_id_ = study_id;
	this->run_id_ = run_id;
}
