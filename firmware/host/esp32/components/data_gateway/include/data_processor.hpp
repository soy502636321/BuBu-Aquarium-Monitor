//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_DATA_PROCESSOR_HPP
#define ESP32_DATA_PROCESSOR_HPP

#pragma once

#include "data_context.hpp"

class IDataProcessor {

public:
	virtual ~IDataProcessor(){}

	virtual bool execute(DataContext& context)=0;

	void setNext(IDataProcessor* processor) {
		next = processor;
	}

protected:

	void nextProcess(
		DataContext& context)
	{
		if(next) {
			next->execute(context);
		}
	}


private:

	IDataProcessor* next=nullptr;

};

#endif // ESP32_DATA_PROCESSOR_HPP
