//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_DATA_DISPATCHER_HPP
#define ESP32_DATA_DISPATCHER_HPP

class IDataOutput
{
public:
	virtual ~IDataOutput(){}

	virtual void handle(const DataContext& context) = 0;
};

#endif // ESP32_DATA_DISPATCHER_HPP
