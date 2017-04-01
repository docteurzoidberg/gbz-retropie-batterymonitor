#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>

#include "../../debug-Lib/src/Logger.h"


#define HEADER "!PKT" //new version
#define HEADER_LEN 4

#define BYTE_END '\n'
#define TAILLE_BUFFER 512

class Serial {

public:
  
  std::ifstream serialStream;
  
  Serial() {
    
  }
  
  bool open (std::string path) {
 	if (!serialStream) {
      	throw std::run_time("Serial opening error !");
    }
    return serialStream.open(path, std::ifstream::in | std::ifstream::binary);
  }
    
  //lit la stream et rempli le buffer  
  bool processData() {
      char byte;   
      while((byte = serialStream.get()) != EOF) {
          //read stream et regarde si data apres
          compute(byte);
      }
      return packetReady;
  }  

  //lit X octet dans le paquet
  bool readBytes(int len, char* obj) {

        if (!packetReady) {
            Logger::warning("Packet not ready");
            return false;
        }

        memcpy(obj, &buffer[readIndex], len);
        readIndex+=len; 
        return true;
  }

  /* getters */
  bool isPacketReady() { return packetReady; }
  int getPacketLen() { return packetReady ? packetLen : -1; }
  int getPacketType() { return packetReady ? packetType : -1; }
    
  void close() { serialStream.close(); }

  ~Serial() {
  	if (serialStream)
      	close();
  }

private:

    enum Status {
        WAIT_BEGIN,     //  on attend byte debut
        WAIT_END,       //  taille atteinte, on attend byte fin
        READING_TYPE,   //  tant que l'on a pas lu le type
        READING_LEN,    //  tant que l'on a pas lu la taille
        READING_DATA    //  tant que l'on a pas eu Fin
    };

    int st = WAIT_BEGIN;

    int readIndex = 0;
    int headerIndex = 0;
    int bufferIndex = 0;    

    int packetLen = 0;
    int packetType = 0;
    bool packetReady = false;

    char buffer[TAILLE_BUFFER];
   
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
            readIndex = 0;
            headerIndex = 0;
            return;
        }

        //Byte de degin => passage etat lecture
        if (st == WAIT_BEGIN) {

            if(b != HEADER[headerIndex]) {
                Logger::error("Got other header byte than expected");
                headerIndex=0;
                return;  //  on reste en begin
            }

            //b correspond au header, on avance dedans
            headerIndex++;

            //Si header pas fini, on sort et on attend le prochain.
            if(headerIndex < HEADER_LEN) {
                return; //on reste en begin
            }
           
            //Fin du header, c'est bon, on passe a READING_TYPE !
            st = READING_TYPE;
            packetReady = false;
            packetLen = 0;
            packetType = 0;
            bufferIndex = 0;
            return;
        }

        //Etat "READING_TYPE", on lit le type dans packetType
        if (st == READING_TYPE) {
            //on lit le type, et passe a READING_LEN
            st = READING_LEN;
            packetType = (int)b;
            return;
        }

        //Etat "READING_LEN", on lit la taille dans len
        if (st == READING_LEN) {
            //on lit et passe a READING_DATA
            st = READING_DATA;
            packetLen = (int)b;
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

            int len = packetLen - bufferIndex;

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


     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     