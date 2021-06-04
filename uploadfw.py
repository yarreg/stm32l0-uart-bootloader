#!/usr/bin/env python3

import sys
import binascii
import logging
from os import path
from time import sleep

from serial import Serial


class XMODEM:
    SOH = bytes([0x01])
    STX = bytes([0x02])
    EOT = bytes([0x04])
    ACK = bytes([0x06])
    DLE = bytes([0x10])
    NAK = bytes([0x15])
    CAN = bytes([0x18])
    CRC = bytes([0x43]) # C

    def __init__(self, serial, mode='xmodem', pad=b'\x1a'):
        self.serial = serial
        self.mode = mode
        self.pad = pad

    def abort(self, count=2):
        for _ in range(0, count):
            self.serial.write(XMODEM.CAN)

    def calc_checksum(self, data, checksum=0):
        return (sum(data) + checksum) % 256            

    def calc_crc(self, data, crc=0):
        return binascii.crc_hqx(data, crc)

    def send(self, stream, retry=32, quiet=0, callback=None):
        # initialize protocol
        try:
            packet_size = dict(xmodem=128, xmodem1k=1024)[self.mode]
        except AttributeError:
            raise ValueError("An invalid mode was supplied")

        error_count = 0
        crc_mode = 0
        cancel = 0
        while True:
            char = self.serial.read(1)
            if char:
                if char == XMODEM.NAK:
                    crc_mode = 0
                    break
                elif char == XMODEM.CRC:
                    crc_mode = 1
                    break
                elif char == XMODEM.CAN:
                    if not quiet:
                        logging.info('received CAN')
                    if cancel:
                        return False
                    else:
                        cancel = 1
                else:
                    logging.error('send ERROR expected NAK/CRC, got %s' % char)

            error_count += 1
            if error_count >= retry:
                self.abort()
                return False

        # send data
        error_count = 0
        success_count = 0
        total_packets = 0
        sequence = 1
        while True:
            data = stream.read(packet_size)
            if not data:
                #logging.info('Sending EOT')
                # end of stream
                break
            total_packets += 1
            data = data.ljust(packet_size, self.pad)
            if crc_mode:
                crc = self.calc_crc(data)
            else:
                crc = self.calc_checksum(data)

            # emit packet
            while True:
                if packet_size == 128:
                    self.serial.write(XMODEM.SOH)
                else:  # packet_size == 1024
                    self.serial.write(XMODEM.STX)
                self.serial.write(bytes([sequence]))
                self.serial.write(bytes([0xff - sequence]))
                self.serial.write(data)
                if callable(callback):
                    callback(sequence, data)
                if crc_mode:
                    self.serial.write(bytes([crc >> 8]))
                    self.serial.write(bytes([crc & 0xff]))
                else:
                    self.serial.write(bytes([crc]))

                char = self.serial.read(1)
                if char == XMODEM.ACK:
                    success_count += 1
                    break
                if char == XMODEM.NAK:
                    error_count += 1
                    if error_count >= retry:
                        # excessive amounts of retransmissions requested,
                        # abort transfer
                        self.abort()
                        logging.warning('excessive NAKs, transfer aborted')
                        return False

                    # return to loop and resend
                    continue
                else:
                    logging.error('Not ACK, Not NAK')
                    error_count += 1
                    if error_count >= retry:
                        # excessive amounts of retransmissions requested,
                        # abort transfer
                        self.abort()
                        logging.warning('excessive protocol errors, transfer aborted')
                        return False

                    # return to loop and resend
                    continue

                # protocol error
                #self.abort()
                #logging.error('protocol error')
                #return False

            # keep track of sequence
            sequence = (sequence + 1) % 0x100

        while True:
            # end of transmission
            self.serial.write(XMODEM.EOT)

            #An ACK should be returned
            char = self.serial.read(1)
            if char == XMODEM.ACK:
                break
            else:
                error_count += 1
                if error_count >= retry:
                    self.abort()
                    logging.warning('EOT was not ACKd, transfer aborted')
                    return False

        return True


class Device:
    def __init__(self, port_name):
        self.serial = Serial(port_name, baudrate=115200, timeout=10, dsrdtr=True)

    def start_bootloader(self):
        self.serial.flush()
        self.serial.dtr = True
        sleep(1)
        self.serial.dtr = False
        sleep(1)
        self.serial.write(b"bl1\n")
        boot_msg = self.serial.readline().decode()    
        return boot_msg.strip().endswith("bootloader")

    def _upload_indication(self, sequence, data, n):
        progress = ((sequence-1) * len(data) / n) * 100
        print("%d%%" % progress)

    def upload_firmware(self, fname):
        xmodem = XMODEM(self.serial, "xmodem1k")
        with open(fname, "rb") as fp:
            file_size = path.getsize(fname)
            return xmodem.send(fp, callback=lambda s, d: self._upload_indication(s, d, file_size))


def main():
    logging.basicConfig(level=logging.INFO, format="[%(levelname)s] %(message)s")
    if len(sys.argv) < 3:
        logging.error("Usage: uploadfw.py <serial> <file>")
        return False

    port_name = sys.argv[1]
    device = Device(port_name)
    
    firmware_file = sys.argv[2]
    if not path.isfile(firmware_file):
        logging.error("Firmware file not found")
        return False

    if not device.start_bootloader():
        logging.error("Device not detected")
        return False

    if device.upload_firmware(firmware_file):
        logging.info("Done")
    
    return False
    

if __name__ == "__main__":
    status = main()
    if not status:
        exit(1)