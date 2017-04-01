#Format des paquets                                 
    "!PKT" (4) + $LEN (1) + $DATA ($LEN) + '\n' (1)

#Utilisation

    Serial* serialLib = new Serial();
    
    //Open serial protocol lib on arduino serial usb
    if(!serialLib->open("/dev/ttyACM0")) {
        Logger::error("Error opening serial port");
        return(1);
    }

    //Serial packet ready ?
    if(serialLib->processData()) {
        
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