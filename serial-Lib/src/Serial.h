#ifndef __SERIAL_H__
#define __SERIAL_H__

#define HEADER "!PKT" //new version
#define HEADER_LEN 4

#define BYTE_END '\n'
#define TAILLE_BUFFER 512

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "../../debug-Lib/src/Logger.h"

    enum Status {
        WAIT_BEGIN,     //  on attend byte debut
        WAIT_END,       //  taille atteinte, on attend byte fin
        READING_TYPE,   //  tant que l'on a pas lu le type
        READING_LEN,    //  tant que l'on a pas lu la taille
        READING_DATA    //  tant que l'on a pas eu Fin
    };



class Serial {
public:
    Serial();
    bool open(std::string path);
    void close();
    bool processData();
    bool readBytes(int len, char* obj);
    bool isPacketReady();
    int getPacketType();
    int getPacketLen();
    ~Serial();
private:
    void _compute(char &b);   
    std::ifstream serialStream;

    int st = WAIT_BEGIN;
    int readIndex = 0;
    int headerIndex = 0;
    int bufferIndex = 0;    
    int packetLen = 0;
    int packetType = 0;
    bool packetReady = false;
    char buffer[TAILLE_BUFFER];
};

#endif