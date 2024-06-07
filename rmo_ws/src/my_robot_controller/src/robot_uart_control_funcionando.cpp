#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Joy.h>
#include <cmath>
#include <vector>

// Utiliza el namespace de Boost.Asio
using namespace boost::asio;
using namespace std;

// Estructura de datos
struct DataPacket {
    float Vx;
    float Vy;
    float Wz;
    uint8_t crc;
};
DataPacket data_global;

union PacketUnion {
    DataPacket packet;
    uint8_t bytes[sizeof(DataPacket)];
};

io_service io;
serial_port serial(io);

uint8_t enviar = 0;

// Estructura de Vel_espacial
struct Vel_espacial {
    float vx;
    float vy;
    float wz;
};

// Función generarRampaVelocidad
void generarRampaVelocidad(Vel_espacial* v_inicial, Vel_espacial* v_final,
                           std::vector<float>& rampa_vx,
                           std::vector<float>& rampa_vy,
                           std::vector<float>& rampa_wz) {
    float a_max = 0.5; // [m/s^2]
    int data_freq = 20; // [ms] periodo con el que se envian los datos de la rampa
    float t_step = data_freq / 1000.0;
    float Delta_t = 0;
    int N = 0;
    float vel_inicial = 0, vel_final = 0;

    if (v_inicial->vx != v_final->vx) {
        vel_inicial = v_inicial->vx;
        vel_final = v_final->vx;
    } else if (v_inicial->vy != v_final->vy) {
        vel_inicial = v_inicial->vy;
        vel_final = v_final->vy;
    } else if (v_inicial->wz != v_final->wz) {
        vel_inicial = v_inicial->wz;
        vel_final = v_final->wz;
    }

    if (vel_final < vel_inicial) a_max = -a_max;
    Delta_t = (vel_final - vel_inicial) / a_max;
    Delta_t = std::round(Delta_t / t_step) * t_step; // me aseguro que el delta sea multiplo de t_step
    N = Delta_t / t_step + 1;

    rampa_vx.assign(N, v_inicial->vx);
    rampa_vy.assign(N, v_inicial->vy);
    rampa_wz.assign(N, v_inicial->wz);
    std::vector<float> rampa(N, 0);

    for (int n = 0; n < N; ++n) {
        rampa[n] = a_max * t_step * n - a_max * Delta_t + vel_final;
    }

    if (v_inicial->vx != v_final->vx) {
        std::copy(rampa.begin(), rampa.end(), rampa_vx.begin());
    } else if (v_inicial->vy != v_final->vy) {
        std::copy(rampa.begin(), rampa.end(), rampa_vy.begin());
    } else if (v_inicial->wz != v_final->wz) {
        std::copy(rampa.begin(), rampa.end(), rampa_wz.begin());
    }
}

// Callback para el joystick
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
    // Mapea los valores de los ejes del joystick
    float vx = joy->axes[1] / 2.0; // Ajusta la velocidad lineal en el eje x
    float vy = joy->axes[0] / 2.0; // Ajusta la velocidad lineal en el eje y
    float wz = joy->axes[3];       // Ajusta la velocidad angular

    static float vx_anterior = 0;
    static float vy_anterior = 0;
    static float wz_anterior = 0;

    if(vx <= 0.1 && vx >= -0.1)
        vx = 0.0;
    else if(vx <= -0.45)
        vx = -0.45;
    else if(vx >= 0.45)
        vx = 0.45;

    if(vy <= 0.1 && vy >= -0.1)
        vy = 0.0;
    else if(vy <= -0.45)
        vy = -0.45;
    else if(vy >= 0.45)
        vy = 0.45;

    if(wz <= 0.1 && wz >= -0.1)
        wz = 0.0;
    else if(wz <= -0.9)
        wz = -0.9;
    else if(wz >= 0.9)
        wz = 0.9;

    enviar = 1;

    vx = std::round((vx) * 10) / 10.0;
    vy = std::round((vy) * 10) / 10.0;
    wz = std::round((wz) * 10) / 10.0;

    Vel_espacial vel_inicial = {vx_anterior, vy_anterior, wz_anterior};
    Vel_espacial vel_final = {vx, vy, wz};

    std::vector<float> rampa_vx, rampa_vy, rampa_wz;
    generarRampaVelocidad(&vel_inicial, &vel_final, rampa_vx, rampa_vy, rampa_wz);

    // Aquí podrías usar la rampa de velocidad para enviar datos gradualmente
    // Por simplicidad, vamos a tomar solo el último valor de la rampa para enviar
    data_global.Vx = rampa_vx.back();
    data_global.Vy = rampa_vy.back();
    data_global.Wz = rampa_wz.back();
    data_global.crc = 0;

    vx_anterior = vx;
    vy_anterior = vy;
    wz_anterior = wz;
}

int main(int argc, char** argv) {
    // Inicializa el nodo de ROS
    PacketUnion packetUnion;

    ros::init(argc, argv, "serial_node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe<sensor_msgs::Joy>("joy", 10, joyCallback);

    try {
        serial.open("/dev/ttyAMA0"); // Cambia /dev/ttyUSB0 por el puerto correcto

        serial.set_option(serial_port_base::baud_rate(2000000));
        serial.set_option(serial_port_base::character_size(8));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

        // Definir la velocidad del bucle principal de ROS
        ros::Rate loop_rate(100);

        // Bucle principal de ROS
        while (ros::ok()) {
            ros::spinOnce(); // Procesa los callbacks pendientes
            loop_rate.sleep();

            if (enviar)
            {
                //cout<<"vx: "<< data_global.Vx <<" |vy: "<< data_global.Vy << " |wz"<< data_global.Wz<<endl;
                //cout << "Union bytes: ";
                for (size_t i = 0; i < sizeof(packetUnion.bytes); ++i) {
                    printf("%02x ", packetUnion.bytes[i]);
                }
                std::cout << std::endl;
                packetUnion.packet = data_global;
                write(serial, boost::asio::buffer(packetUnion.bytes, sizeof(packetUnion.bytes)));
                enviar = 0;
            }
        }
        serial.close();
    } catch (boost::system::system_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

