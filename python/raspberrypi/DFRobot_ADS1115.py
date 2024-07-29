# -*- coding:utf-8 -*-
"""
    @file DFRobot_GNSSAndRTC.py
    @brief Define the basic structure of class DFRobot_ADS1115
    @copyright	Copyright (c) 2024 DFRobot Co.Ltd (http://www.dfrobot.com)
    @license The MIT License (MIT)
    @author
    @version V0.0.1
    @date 2024-07-10
    @url https://github.com/DFRobot/DFRobot_GNSSAndRTC
"""
from __future__ import absolute_import
import time
import serial
import smbus
import struct
import six
import logging

logger = logging.getLogger()
logger.setLevel(logging.INFO)  # Show all print messages
#logger.setLevel(logging.FATAL)# Use this option if you don't want to show too much print and only print errors
ph = logging.StreamHandler()
formatter = logging.Formatter("%(asctime)s - [%(filename)s %(funcName)s]:%(lineno)d - %(levelname)s: %(message)s")
ph.setFormatter(formatter) 
logger.addHandler(ph)


class DFRobot_ADS1115:
    def __init__(self):
        {}
    # Channel address
    CHANNEL_SELECT_ADDRESS = 0x20
    # Read data address: 0x30 data bits, 0x31-0x33 data
    CHANNEL_DATA_ADDRESS = 0x31
    UART_READ_REGBUF   = 0xBB
    UART_WRITE_REGBUF  = 0xCC    
    def begin(self):
        {}
    def get_value(self, channel):    
        try:
            self._write_reg(self.CHANNEL_SELECT_ADDRESS,[channel], size=1)
            buf =self._read_reg(self. CHANNEL_DATA_ADDRESS,3)
            return buf[0]*65536+buf[1]*256+buf[2]  
        except :
            logger.error("Please check connect!")
            return 0


class DFRobot_ADS1115_I2C(DFRobot_ADS1115):

    __device_addr = 0x48
    def __init__(self, i2c_bus, addr):
        self.__i2c_bus = smbus.SMBus(i2c_bus)
        self.__device_addr = addr

    def begin(self):
        try:
            self.__i2c_bus.read_byte(self.__device_addr)
            logger.info("I2C init ok!")
            return True
        except:
            logger.error("I2C init fail!")
            return False
            
    def _write_reg(self, reg, p_buf,  size):
        self.__i2c_bus.write_byte_data(self.__device_addr, reg, p_buf[0])
    def _read_reg(self, reg,  size):
        return self.__i2c_bus.read_i2c_block_data(self.__device_addr,reg, size)

class DFRobot_ADS1115_UART(DFRobot_ADS1115):

    UART_SERIAL_NAME = "/dev/serial0"
    UART_BAUDRATE = 9600
    __serial = None
    TIME_OUT = 1000

    def __init__(self, serial_name=UART_SERIAL_NAME, baud=UART_BAUDRATE):
        self.__serial_name = serial_name
        self.__baud = baud
    def begin(self):
        self.__serial = serial.Serial(self.__serial_name, self.__baud)
        self.__serial.flush()
        if not self.__serial.isOpen():
            return False
        return True
    # @staticmethod
    def int_to_bytes(self, n, length, byteorder='big'):
        if byteorder == 'big':
            return struct.pack('>I', n)[-length:]
        elif byteorder == 'little':
            return struct.pack('<I', n)[:length]
        else:
            raise ValueError("byteorder 必须是 'big' 或 'little'")
        
    def _write_reg(self, reg, p_buf, size):
        self.__serial.write(self.int_to_bytes(self.UART_WRITE_REGBUF,1))
        self.__serial.write(self.int_to_bytes(reg,1))
        self.__serial.write(self.int_to_bytes(size,1))
        for i in range(0, size):
            self.__serial.write(self.int_to_bytes(p_buf[i], 1))
        time.sleep(0.02)

    def _read_reg(self, reg, size):
        i = 0
        self.__serial.write(self.int_to_bytes(self.UART_READ_REGBUF, 1))
        self.__serial.write(self.int_to_bytes(reg, 1))
        self.__serial.write(self.int_to_bytes(size, 1))
        time.sleep(0.02)
        p_buf = [0x00] * size
        nowtime = time.time() * 1000
        while time.time() * 1000 - nowtime < self.TIME_OUT:
            while self.__serial.in_waiting > 0:
                if six.PY3:
                    p_buf[i] = self.__serial.read(1)[0]
                else:
                    p_buf[i] = ord(self.__serial.read(1)[0])
                i += 1
                if i == size:
                    break
            if i == size:
                break
        return p_buf
