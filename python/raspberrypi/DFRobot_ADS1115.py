# -*- coding:utf-8 -*-
"""
    @file DFRobot_GNSSAndRTC.py
    @brief
    @details
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


class DFRobot_ADS1115_I2C:
    # 写选择ad通道寄存器地址
    CHANNEL_SELECT_ADDRESS = 0x20
    # 读ad数据地址 30数据位 31-33 数据
    CHANNEL_DATA_ADDRESS = 0x30
    __device_addr = 0x48

    def __init__(self, i2c_bus, addr):
        self.__i2c_bus = smbus.SMBus(i2c_bus)
        self.__device_addr = addr

    def begin(self):
        try:
            self.__i2c_bus.read_byte(self.__device_addr)
            print("I2C init ok!")
            return True
        except:
            print("I2C init fail!")
            return False

    def get_value(self, channel):
        size = 4
        try:
            self.__i2c_bus.write_byte_data(self.__device_addr, self.CHANNEL_SELECT_ADDRESS, channel)
            buf = self.__i2c_bus.read_i2c_block_data(self.__device_addr, self. CHANNEL_DATA_ADDRESS, size)
            time.sleep(0.02)
            return buf[1]*65536+buf[2]*256+buf[3]
        except:
            print("Read: I2C init fail, please check connect!")
            return 0


class DFRobot_ADS1115_UART:

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

    def int_to_bytes(self, n, length, byteorder='big'):
        if byteorder == 'big':
            return struct.pack('>I', n)[-length:]
        elif byteorder == 'little':
            return struct.pack('<I', n)[:length]
        else:
            raise ValueError("byteorder 必须是 'big' 或 'little'")

    def get_value(self, channel):
        self.__serial.write(self.int_to_bytes(channel, 1))
        time.sleep(0.05)
        size = 3
        i = 0
        p_buf = [0x00] * size
        nowtime = time.time() * 1000
        print("ok")
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
        time.sleep(0.02)
        return p_buf[0]*65536+p_buf[1]*256+p_buf[2]
