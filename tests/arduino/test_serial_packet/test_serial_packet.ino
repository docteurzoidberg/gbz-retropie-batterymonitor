void setup() {
  Serial.begin(9600);
}

void loop() {
    
  Serial.write("!PKT");      //header
  Serial.write('b');         //type=battery status
  Serial.write(0x06);        //len
  Serial.write(75);          //batt percent
  Serial.write(0x1);         //batt charging
  Serial.write(0x0);         //batt voltage 1/4
  Serial.write(0x0);         //batt voltage 2/4
  Serial.write(0x0);         //batt voltage 3/4
  Serial.write(0x0);         //batt voltage 4/4
  Serial.write('\n');        //END
  
  delay(1000);
}