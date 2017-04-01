
#include "Serial.h"


Serial::Serial() {}
  
bool Serial::open (std::string path) {
    if (!serialStream) return false;
    try{
        serialStream.open(path, std::ifstream::in | std::ifstream::binary);
        return true;
    }
    catch(std::exception const& e) {
        
    }
    return false;
}

//lit la stream et rempli le buffer  
bool Serial::processData() {
    char byte;   
    while((byte = serialStream.get()) != EOF) {
        //read stream et regarde si data apres
        _compute(byte);
    }
    return packetReady;
}  

//lit X octet dans le paquet
bool Serial::readBytes(int len, char* obj) {

    if (!packetReady) {
        Logger::warning("Packet not ready");
        return false;
    }

    std::memcpy(obj, &buffer[readIndex], len);
    readIndex+=len; 
    return true;
}

/* getters */
bool Serial::isPacketReady() { return packetReady; }
int Serial::getPacketLen() { return packetReady ? packetLen : -1; }
int Serial::getPacketType() { return packetReady ? packetType : -1; }

void Serial::close() { serialStream.close(); }

Serial::~Serial() {
    if (serialStream) {
        close();
    }
}
   
void Serial::_compute(char& b) {

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




     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     