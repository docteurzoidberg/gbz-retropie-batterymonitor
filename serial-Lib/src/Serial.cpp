#include <iostream>
#include <fstream>
#include <string>

#include "Logger.h"

#define BYTE_START '!' 	
#define PACKET_STOP '\n'
#define TAILLE_BUFFER 512

class Serial {

public:
  
  std::ifstream serialStream;

  Serial::Serial(std::string path) {
   
  }
  
  void Serial::open (std::string path) {
 	if (!serialStream) {
      	throw std::run_time("Serial opening error !");
    }
    serialStream.open(path, std::ifstream::in | std::ifstream::binary);
  }
    
  //lit la stream et rempli le buffer  
  bool Serial::processData() {
      char byte;   
      while((byte = serialStream.get()) != EOF) {
          //read stream et regarde si data apres
          compute(byte);
      }
      return packetReady;
  }  

  bool Serial::isPacketReady() { return packetReady; }
    
  void Serial::close() { serialStream.close(); }

  Serial::~Serial() {
  	if (serialStream)
      	close();
  }

private:

    enum Status {
        WAIT_BEGIN,     //  on attend byte debut
        WAIT_END,       //  taille atteinte, on attend byte fin
        READING_LEN     //  tant que l'on a pas lu la taille
        READING_DATA    //  tant que l'on a pas eu Fin
    };

    Status st = WAIT_BEGIN;

    bool packetReady = false;
    int len = 0;
    int bufferIndex = 0;
    byte buffer[TAILLE_BUFFER];

   
    int readOffset = 0;

    byte[]* readBytes(int size) {

        if (!packetReady)
            Logger::warning("Packet not ready");
            return nullptr;
        }

        byte[size] bs;
        memcpy(bs, &buffer[readOffset], size);
        readOffset+=size; 
        return bs;
    }

    void compute(char& b) {

        //Byte de end '\n' et WAIT_END => validation du paquet, et passage etat WAIT_BEGIN
        if (st == WAIT_END) {

            if(b != BYTE_END) {
                Logger::error("Got other byte than end");
                st = WAIT_BEGIN; //  on recommence
                return;
            }

            Logger::info("Got a packet ! =)");

            st = WAIT_BEGIN;
            packetReady = true;
            readOffset = 0;
            return;
        }

        //Byte de degin => passage etat lecture
        if (st == WAIT_BEGIN) {

            if(b != BYTE_START) {
                Logger::error("Got other byte than begin");
                return;  //  on reste en begin
            }

            //byte de begin, on passe a READING_LEN
            st = READING_LEN;
            packetReady = false;
            len = 0;
            bufferIndex=0;
            return;
        }

        //Etat "READING_LEN", on lit la taille dans len
        if (st == READING_LEN) {
            //on lit et passe a READING_DATA
            st = READING_DATA;
            len = (int)b;
            return;
        }

        //Si etat lecture
        if (st == READING_DATA) {
            
            if (bufferIndex == (TAILLE_BUFFER-1)) {
                Logger::error("Buffer overflow !");
                st = WAIT_BEGIN;
                return;
            }

            buffer[bufferIndex++] = b;
            len--;  

            //Len atteinte ? on passe a WAIT_END
            if(len==0) {
                st = WAIT_END;                
            }
          
            return; //sinon on reste en READING_DATA
        }

        //
        Logger::error("On ne devrait jamais arriver ici");
    }

};


     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     