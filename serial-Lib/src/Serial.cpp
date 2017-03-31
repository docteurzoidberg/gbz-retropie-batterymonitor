#include <iostream>
#include <fstream>
#include <string>

#include "Logger.h"

#define PACKET_START "PCK" 	// 3 octets
#define PACKET_STOP "\n"		// 1 octet
#define TAILLE_BUFFER 2048


class Serial {

public:
  
  std::ifstream serialStream;

  Serial::Serial(std::string path) {
   
  }
  
  void Serial::open (std::string path) {
 
 		if (!serialStream)
      	throw std::run_time("Already Open !");
        
    serialStream.open(path, std::ifstream::in | std::ifstream::binary); // on ouvre en binaire ? => toutafé
  
  }
    
  //lit la stream et rempli le buffer  
  bool Serial::processData() {
      char byte;      
      while((byte = serialStream.get()) != EOF) {
          //read stream et regarde si data apres
          compute(byte);
      }
      
      return package_done;
  }  
    
    bool Serial::isPackageDone() { return package_done; }
    
  void Serial::close() { serialStream.close(); }

  Serial::~Serial() {
  	if (serialStream)
      	close();
  }

private:
  enum Status {
    WAIT_BEGIN, // on attend
    READING_LEN //  tant que l'on a pas lu la taille
    READING_DATA //  tant que l'on a pas eu Fin

  };
  Status st = WAIT_BEGIN;
    
  bool package_done = false;
  
  int len = 0;
  int bufferIndex = 0;
  byte buffer[TAILLE_BUFFER];          //buffer lecture port serie (entete + len + data + fin  tout ce qui est lu sur la stream en fait)

    // PROTO : <BEGIN> <DATALEN> <DATA> <END> (end facultatif en fait, mais pourra servir de checksum plus tard)
    int readOffset = 0;
    byte[]* getData(int size) {
        // [0, 1, 2, 3, 4] comme ça on dit si on veut que [2, 3] par exmeple
        if (!package_done)
            Logger::warning("Can't recover data");
            return nullptr;
        }
        byte[size] bs;
        memcpy(bs, &buffer[readOffset], size * sizeof(byte)); // vraiment pas sur de mon coup mais je crois que c'est bon
        readOffset+=size; // ah oui ^^ (derniers efforts du cerveau ^^) x)
        return bs;
    }
    
    void compute(char& b) {
        //Byte de degin = etat lecture
        if (st == WAIT_BEGIN && b == '!') {
            st = READING_LEN;
            package_done = false;
            len = 0;
            bufferIndex=0;
            readOffset = 0;
            return;
        }
        else if (st == READING_LEN) {
            st = READING_DATA;
            len = (int)b;
        }
        // Tant que read data et len pas depassé
        else if (st == READING_DATA && len == 0 && b == '\n' /* end ? */) {
            st = WAIT_BEGIN;
            package_done = true;
            readOffset=0;
            // consrtuire paquet à l'aide du buffer ?
            
            
        }
        else if (st == READING_DATA && len == 0) { // ERROR (quel statut ?)
            Logger::error("End of data, need a END token");
            st = WAIT_BEGIN; //  on recommence
            package_done = false; // au cas ou
        }
        else if (st == READING_DATA) {
            len--;
            
            if (bufferIndex == (TAILLE_BUFFER-1)) {
                Logger::error("Buffer overflow !");
                st = WAIT_BEGIN;
                return;
            }
            buffer[bufferIndex++] = b;
        }
    }

};


     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     