/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 * This library implements the Modbus protocol.
 * http://libmodbus.org/
 */
#ifndef IMODBUS_H
#define IMODBUS_H

#include "modbus.h"

class IModbus
{
public:
    virtual modbus_t*  modbus() = 0;
    virtual int        setupModbusPort() = 0;
};

#endif // IMODBUS_H
