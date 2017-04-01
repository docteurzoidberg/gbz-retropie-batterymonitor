/*
 * grrl_bat_mon.c
 *
 * A bettery monitor for the Adafruit Pocket PiGRRL
 *
 * FRAMEBUFFER DISLAY WORK BASED ON :::
 * http://raspberrycompote.blogspot.ie/2013/03/low-level-graphics-on-raspberry-pi-part.html
 *
 * Original work by J-P Rosti (a.k.a -rst- and 'Raspberry Compote')
 *
 * Licensed under the Creative Commons Attribution 3.0 Unported License
 * (http://creativecommons.org/licenses/by/3.0/deed.en_US)
 *
 * Distributed in the hope that this will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *

 * Modifié un peu par DrZoid :p

 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "debug-Lib/src/Logger.h"
#include "serial-Lib/src/Serial.h"

#include <iostream>


#define PACKET_BATTERY_STATUS 'b'


// 'global' variables to store screen info
char *fbp = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int fbfd = 0;
struct fb_var_screeninfo orig_vinfo;
long int screensize = 0;

// global triggered when program should exit !
int stop_program = 0;

// constants for battery icon
int batt_icon [20][35] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

//DrZoid: TODO: Dynamic icon =)


int batt_start_x = 10;
int batt_start_y = 10;
int batt_fore_colour = 65535;
int batt_back_colour = 0;

// refresh rate of the framebuffer
int fb_refresh = 10000;



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

    // cleanup
    // unmap fb file from memory
    munmap(fbp, screensize);
    // reset the display mode
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo)) {
        printf("Error re-setting variable information.\n");
    }
    // close fb file
    close(fbfd);
    printf("Pocket PiGRRL Battery Monitor Finished (%d).\n",signum);

    exit(signum);
}

// helper function to 'plot' a pixel in given color
void put_pixel(int x, int y, int c)
{
    // calculate the pixel's byte offset inside the buffer
    unsigned int pix_offset = x + y * finfo.line_length;

    // now this is about the same as 'fbp[pix_offset] = value'
    *((char*)(fbp + pix_offset)) = c;

}

// function to draw a battery to the frame buffer
void draw_battery(int start_x, int start_y, int fore_colour, int back_colour) {
    int x;
    int y;
    for (x = 0; x < 20; x++) {
        for (y = 0; y < 35; y++) {
            if (batt_icon[x][y] == 1) {
                put_pixel(start_y + y, start_x + x, fore_colour);
            } else {
                put_pixel(start_y + y, start_x + x, back_colour);
            }
        }
    }
}

// application entry point
int main(int argc, char* argv[])
{
    Logger::redirectTo(Logger::All, std::cout);
    Logger::info("GBZ Serial Battery Monitor Started");

    // Open the framebuffer file for reading and writing
    fbfd = open("/dev/fb1", O_RDWR);
    if (fbfd == -1) {
        Logger::error("Cannot open framebuffer device.");
        return(1);
    }

    //Open serial protocol lib on arduino serial usb
    if(!serialLib->open("/dev/ttyACM0")) {
        Logger::error("Error opening serial port");
        return(1);
    }



    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        Logger::error("Error reading variable information.");
    }

    //DEBUG info from framebuffer
    /*
    printf("Original %dx%d, %dbpp\n", 
            vinfo.xres, vinfo.yres, 
            vinfo.bits_per_pixel );
    */

    // Store for reset (copy vinfo to vinfo_orig)
    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

    // Change variable info
    vinfo.bits_per_pixel = 8;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
        Logger::error("Error setting variable information.");
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        Logger::error("Error reading fixed information.\n");
    }

    // map fb to user mem 
    screensize = finfo.smem_len;
    fbp = (char*)mmap(0, 
              screensize, 
              PROT_READ | PROT_WRITE, 
              MAP_SHARED, 
              fbfd, 
              0);

    if ( (int)(*fbp) == -1) {
        Logger::error("Failed to mmap.\n");
        return -1;
    }

    // Register signal and signal handler for cleanup
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    // inifinite loop
    for(;;) {

        //Serial packet ready ?
        if(serialLib->processData()) {
            
            int packetType = serialLib.getPacketType();
            if(packetType == PACKET_BATTERY_STATUS) {

                //On a un paquet,
                int     battPercent =   (int)       serialLib->readBytes(1);     //premiere valeur = % batterie sur de (00 a FF)
                bool    battCharging =  (bool)      serialLib->readBytes(1);         //deuxieme valeur = charge on/off (00 ou FF)
                float   battVoltage =   (float)     serialLib->readBytes(4);         //troisieme valeur = float voltage batterie

                /* ou:

                struct BatteryInfoStruct {
                    uint8_t percent;
                    bool charging;
                    float voltage;
                }
                BatteryInfoStruct battInfos;
                battInfos = (battInfos) serialLib.readBytes(sizeof(battInfos));
                */
            }
        }


        //TODO: icone batterie en fonction des battInfos...
        draw_battery(batt_start_x, batt_start_y, batt_fore_colour, batt_back_colour);
        usleep(fb_refresh);
        sleep(1);
    }

    return 0;
}
