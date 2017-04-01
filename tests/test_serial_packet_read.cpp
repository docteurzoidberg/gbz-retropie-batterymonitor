

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "../debug-Lib/src/Logger.h"
#include "../serial-Lib/src/Serial.h"

#include <iostream>


#define PACKET_BATTERY_STATUS 'b'


// DrZoid: battery infos from serial
struct BatteryInfoStruct {
    BatteryInfoStruct() : percent(0), charging(false), voltage(.0f) {}
    uint8_t percent;
    bool charging;
    float voltage;    
};

BatteryInfoStruct battInfos = {};
Serial* serialLib = new Serial();

// called when terminating
void signal_callback_handler(int signum)
{
    Logger::info("Pocket PiGRRL Battery Monitor Finished.");
    exit(signum);
}

void process_serial_data() {

    if(serialLib->processData()==false) {
        return;
    }
        
    int packetType = serialLib->getPacketType();
    
    if(packetType == PACKET_BATTERY_STATUS) {

        //On a un paquet,
        serialLib->readBytes(1, (char*) &battInfos.percent);     //premiere valeur = % batterie sur de (00 a FF)
        serialLib->readBytes(1, (char*) &battInfos.charging);    //deuxieme valeur = charge on/off (00 ou FF)
        serialLib->readBytes(4, (char*) &battInfos.voltage);     //troisieme valeur = float voltage batterie

        //ou directement:
        //serialLib->readBytes(sizeof(battInfos), (char*) &battInfos);

		Logger::info(std::to_string(battInfos.percent));
		Logger::info(std::to_string(battInfos.charging));
		Logger::info(std::to_string(battInfos.voltage));

        return;
    }

    Logger::error("Unknown packet type.");
}


// application entry point
int main(int argc, char* argv[])
{
    Logger::redirectTo(Logger::All, std::cout);
    Logger::info("GBZ Serial Battery Monitor Started");

    //Open serial protocol lib on arduino serial usb
    if(!serialLib->open("/dev/ttyACM0")) {
        Logger::error("Error opening serial port.");
        return(1);
    }

    // Register signal and signal handler for cleanup
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    // inifinite loop
    for(;;) {
        process_serial_data();



        usleep(100);
    }

    return 0;
}
