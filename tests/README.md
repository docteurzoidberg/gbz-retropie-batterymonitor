# Installer les outils arduino pour compiler la partie test "hardware"

 Telecharger arduino sur le disque, et

    cd /chemin/vers/arduino

# Compiler et uploader le sketch sur l'arduino

    cd tests/

Modifier le chemin vers arduino dans compile_arduino.sh,

Puis

    ./compile_arduino.sh

# Lancer le test sur le port serie

    cd tests/
    ./test_serial_packet_read

(sudo si probleme de droits sur le port, ou ajout du user dans le group "dialout" sous debian)