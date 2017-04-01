#ifndef __SERIAL_H__
#define __SERIAL_H__

class Serial {
public:
     Serial();
     bool open(std::string path);
     bool processData();
     char * readBytes(int len);
     int getPacketType();
    ~Serial();
private:
     void compute(char &b);    
};

#endif