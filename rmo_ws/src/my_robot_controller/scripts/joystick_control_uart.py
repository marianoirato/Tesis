#!/usr/bin/env python3

import rospy
#from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
import serial
import struct
import time

#START_BYTE = 0xAA
#END_BYTE = 0x55

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

        #rospy.loginfo(f'DESPUES DE LA FUNCIOn:Enviado: vx={vx}, vy={vy}, wz={wz}') # mostrar velocidad enviada
        data = struct.pack('fff', vx, vy, wz)   # empaqueta las velocidades lineales y angulares en formato binario 
        crc = 1 # self.calculate_crc(data)
        framed_data = data + struct.pack('B', crc)    # Agrega el byte de CRC al final de la trama
        self.ser.write(framed_data)                   # Enviar los datos por el puerto serie

    def close(self):
        self.ser.close()

def joy_callback(data):
            # axes[k]
    vx = ((data.axes[1])/2)   # Ajusta la velocidad lineal en el eje x ..... k=1 : analogico izq-movimineto vertical
    vy = ((data.axes[0])/2)   # Ajusta la velocidad lineal en el eje y ..... k=0 : analogico izq-movimineto horizontal
    wz = (data.axes[3]    )   # Ajusta la velocidad angular ................ k=3 : analogico der-movimineto horizont
    
    # Elimina el ruido del joystick cuando no se mueve
    if(vx <= 0.05 and vx >= -0.05):
        vx = 0.0
    elif(vx <= -0.1):
        vx = -0.1
    elif(vx >= 0.1):
        vx = 0.1
    if(vy <= 0.1 and vy >= -0.1):
        vy = 0.0
    elif(vy <= -0.45):
        vy = -0.45
    elif(vy >= 0.45):
        vy = 0.45
    if(wz <= 0.1 and wz >= -0.1):
        wz = 0.0
    elif(wz <= -0.9):
        wz = -0.9
    elif(wz >= 0.9):
        wz = 0.9
   	
    print("Vx: {:.2f}".format(vx))
    print("Vy: {:.2f}".format(vy))
    print("Wz: {:.2f}".format(wz))
    #print(type(wz))
    # Publicar en el topico de ROS
    # pub.publish/(twist)

    #rospy.loginfo(f'ANTES DE LA FUNCION: Enviado: vx={vx}, vy={vy}, wz={wz}') # mostrar velocidad enviada
    transmitter.send_velocity(vx, vy, wz)
 #  time.sleep(0.1)
    transmitter.ser.flush()  # Limpia el buffer de salida del puerto serie        

if __name__ == '__main__':
    rospy.init_node("joystick_control")
    rospy.loginfo("Test node has been started")

    # Configurar el puerto serie
    port = '/dev/ttyAMA0'
    baudrate = 2000000

    transmitter = SerialTransmitter(port, baudrate)
    rate=rospy.Rate(2)
    sub = rospy.Subscriber("joy", Joy, joy_callback)

    try:
        while not rospy.is_shutdown():
            rospy.spin()
            rate.sleep()
    except KeyboardInterrupt:   # se produce cuando el usuario presiona Ctrl + C
        transmitter.close()     # cerrar la conexion serie para liberar los recursos antes de que el nodo finalice su ejecucion
        rospy.loginfo("Nodo terminado")
