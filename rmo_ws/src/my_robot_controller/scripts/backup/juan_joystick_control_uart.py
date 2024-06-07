#!/usr/bin/env python3
import rospy
#from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
import serial
import curses
import struct
import time

START_BYTE = 0xAA
END_BYTE = 0x55

#  clase proporciona una forma organizada y eficiente de gestionar la funcionalidad de transmision serie, lo que mejora
#  la legibilidad, la modularidad y la mantenibilidad del codigo.

class SerialTransmitter:
    def __init__(self, port, baudrate):     # self=objeto creado a partir de la clase
        self.ser = serial.Serial(           # configuracion de la UART
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )

    def calculate_crc(self, data):
        # Simple XOR-based CRC calculation (you can replace with a more complex CRC if needed)
        crc = 0
        for byte in data:
            crc ^= byte
        return crc

    def send_velocity(self, vx, vy, wz):

        
        data = struct.pack('fff', vx, vy, wz)
        crc = 0 
        framed_data = data + struct.pack('B', crc)    # Agrega el byte de CRC al final de la trama
        self.ser.write(framed_data)                   # Enviar los datos por el puerto serie
        
    def close(self):
        self.ser.close()

def modificar_variable(stdscr, fin, paso, serial):
    variable = 0

    def mi_funcion(valor,serial):
        transmitter.send_velocity(valor, 0, 0)
        

    stdscr.clear()
    stdscr.addstr(0, 0, "Presiona flecha arriba para aumentar, flecha abajo para disminuir. Presiona 'q' para salir.")
    stdscr.addstr(1, 0, "Valor actual de la variable: {:.2f}".format(variable))
    stdscr.refresh()

    while True:
        key = stdscr.getch()
        if key == curses.KEY_UP:
            variable = min(variable + paso, fin)
        elif key == curses.KEY_DOWN:
            variable = max(variable - paso, -fin)
        elif key == ord('q'):
            break

        mi_funcion(variable,serial)  # Llamar a la función con el nuevo valor de la variable

        stdscr.clear()
        stdscr.addstr(0, 0, "Presiona flecha arriba para aumentar, flecha abajo para disminuir. Presiona 'q' para salir.")
        stdscr.addstr(1, 0, "Valor actual de la variable: {:.2f}".format(variable))
        stdscr.refresh()

if __name__ == '__main__':
    '''
    codigo para debuguear comportamiento del robot
    '''
    port = '/dev/ttyAMA0'
    baudrate = 2000000

    fin = 1.0
    paso = 0.02

    transmitter = SerialTransmitter(port, baudrate)
    curses.wrapper(modificar_variable, fin, paso, transmitter)
