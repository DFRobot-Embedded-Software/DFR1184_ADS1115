# -*- coding:utf-8 -*-
'''!
    @file DFRobot_GNSSAndRTC.py
    @brief
    @details
    @copyright	Copyright (c) 2024 DFRobot Co.Ltd (http://www.dfrobot.com)
    @license The MIT License (MIT)
    @author
    @version V0.0.1
    @date 2024-07-10
    @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
'''
from __future__ import absolute_import
import os
import sys
import time
import serial
import smbus
import struct
import six
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), './')))


class DFRobot_ADS1115_I2C():
    def __init__(self, i2c_bus=1, addr=DFRobot_GNSSAndRTC.MODULE_I2C_ADDRESS):
        super(DFRobot_ADS1115_I2C, self).__init__()
        self.i2c_uart_flag = DFRobot_GNSSAndRTC.GNSS_I2C_FLAG
        self.__i2c_bus = smbus.SMBus(i2c_bus)
        self.__device_addr = addr


class DFRobot_ADS1115_UART(DFRobot_GNSSAndRTC):
    UART_BAUDRATE = 57600
    UART0_READ_REGBUF = 0xBB
    UART0_WRITE_REGBUF = 0xCC

    UART_SERIAL_NAME = "/dev/serial0"

    __baud = UART_BAUDRATE
    __rxpin = 0x00
    __txpin = 0x00

    __serial = None
    __serial_name = UART_SERIAL_NAME

    def __init__(self, serial_name=UART_SERIAL_NAME, baud=UART_BAUDRATE):
        super(DFRobot_ADS1115_UART, self).__init__()
        self.i2c_uart_flag = self.GNSS_UART_FLAG
        self.__serial_name = serial_name
        self.__baud = baud


