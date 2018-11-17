#include <RCSwitch.h>
#include <ArduinoJson.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  mySwitch.enableReceive(1);  // Receiver on inerrupt 1 => that is pin #3
  mySwitch.enableTransmit(10);
  mySwitch.setProtocol(2, 450);
  mySwitch.setRepeatTransmit(15);
}

void loop() {
  if (mySwitch.available()) {
    unsigned int* raw = mySwitch.getReceivedRawdata();
    unsigned int length = mySwitch.getReceivedBitlength();
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    
    root["Value"] = mySwitch.getReceivedValue();
    root["Length"] = length;
    root["Delay"] = mySwitch.getReceivedDelay();
    root["Protocol"] = mySwitch.getReceivedProtocol();
    
    mySwitch.resetAvailable();
    
    /*String rawStrHex = "";
    for (int i=0; i<= length; i++) {
      rawStrHex += String(raw[i], HEX);
    }
    
    root["Data"] = rawStrHex;*/
    root.printTo(Serial);
    Serial.println();
  } /*else {
    unsigned int* raw = mySwitch.getReceivedRawdata();
    int siz = sizeof(&raw);
    if(siz > 2)
    {
      Serial.println(siz);
    }
  }*/

  if(Serial.available())
  {
    String inData = Serial.readStringUntil('\n');
    StaticJsonBuffer<200> jsonBuffer;
    //char json[] =
    //    "{\"Protocol\":2,\"Value\":39933942,\"Length\":28,\"Delay\":450}";
    
    JsonObject& root = jsonBuffer.parseObject(inData);
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    
    int protocol = root["Protocol"];
    long value = root["Value"];
    int length = root["Length"];
    int pulse = root["Delay"];
    
    // Print values.
    //Serial.println(protocol);
    //Serial.println(value);
    //Serial.println(length);
    //Serial.println(pulse);
    
    mySwitch.setProtocol(protocol, pulse);
    char target[length];
    int a = length;
    for(int i = 0; i < length; i++) {
      int bit = (value >> i) & 1U;
      if(bit) {
        target[--a] = '1';
      } else {
        target[--a] = '0';
      }
    }
    mySwitch.send(target);
  }
}
