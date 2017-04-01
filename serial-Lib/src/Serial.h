#ifndef __SERIAL_H__
#define __SERIAL_H__

class Serial {
public:
     Serial();
     bool open(std::string path);
     bool processData();
     bool readBytes(int len, char* obj);
     int getPacketType();
    ~Serial();
private:
     void compute(char &b);    
};

#endif