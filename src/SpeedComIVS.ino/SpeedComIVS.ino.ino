/*Tested on arduino every*/
#include <loopTimer.h>        //manually downloaded via zip
#include <SerialTransfer.h>   //can be found from arduino lib
#include <Adafruit_Sensor.h>  //adafruit bme lib + dep
#include "Adafruit_BME680.h"  //adafruit bme lib + dep


int led = 13;                       // Pin 13 has an led connected on most Arduino boards.
bool ledOn = false;                 // keep track of the led state
millisDelay ledDelay;
millisDelay printDelay;
millisDelay serialPacketDelay;


SerialTransfer myTransfer;
struct STRUCT {
  char z;
  float y;
} testStruct;

char arr[] = "hello";

struct BME680MEAS{      //this is struct for bme data
  uint32_t temperature;
  uint32_t humidity;
  uint32_t pressure;
} bme680dat;

uint8_t heartbeat = 0xF0; //this is standard heartbeat package internals



// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);           //serial to computer
  Serial1.begin(115200);          //serial device to device
  
  pinMode(led, OUTPUT);           // initialize digital pin led as an output.
  ledDelay.start(1000);           // start the ledDelay, toggle every 1000mS
  printDelay.start(5000);         // start the printDelay, print every 5000mS
  printDelayFill.start(500);     // how often do we try to fill the serial packet
  serialPacketDelay.start(10000);
  heartbeatDelay.start(5000);

  myTransfer.begin(Serial1);
  testStruct.z = '$';
  testStruct.y = 4.5;

  if (!Adafruit_BME680.begin()) {
  Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
  while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}





/* the task method
void packetizedTransfer(){
  if(serialPacketDelay.justFinished()){ // check if delay has timed out 
    serialPacketDelay.repeat();       
    uint16_t sendSize = 0;
    sendSize = myTransfer.txObj(testStruct, sendSize);
    sendSize = myTransfer.txObj(arr, sendSize);
    myTransfer.sendData(sendSize);
    }
}
*/

//heartbeat packet transmit
void heartBeatTransfer(){
  if(heartbeatDelay.justFinished()){ // check if delay has timed out 
    heartbeatDelay.repeat();       
    uint16_t sendSize = 0;
    sendSize = myTransfer.txObj(heartbeat, sendSize);
    myTransfer.sendData(sendSize);
    }
}

//bme680 read data
void getBmeData(){
  bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }


}

//bme680 fill transfer data
void fillBmeData(){
  if(heartbeatDelay.justFinished()){ // check if delay has timed out 
    heartbeatDelay.repeat();
    uint8_t bmeReadcount;
    if(bme.endReading()){
      bme680dat.temperature = bme.temperature;
      bme680dat.humidity = bme.pressure;
      bme680dat.pressure = bme.humidity;
      bmeReadcount = 0;
    }
    else if (!bme.endReading() & (bmeReadcount < 15)){
      bmeReadCount ++;
    }
    else {
      Serial.println(F("Failed to complete reading :("));
      bmeReadcount = 0;
    }
  }  
}



/* the task method */
void blinkLed13() {
  if (ledDelay.justFinished()) {    // check if delay has timed out
    ledDelay.repeat();              // start delay again without drift
    ledOn = !ledOn;                 // toggle the led
    digitalWrite(led, ledOn ? HIGH : LOW); // turn led on/off
 //   Serial1.print("noku") ;          //print noku when switch led
  }                                 // else nothing to do this call just return, quickly
}

/* the task method */
void print_mS() {
  if (printDelay.justFinished()) {
    printDelay.repeat();              // start delay again without drift
    Serial.println(millis());         // print the current mS
  }                                   // else nothing to do this call just return, quickly
}











// the loop function runs over and over again forever
void loop() {
  heartBeatTransfer();        //send heartbeat every 5s
  loopTimer.check(Serial);    //debug output
  blinkLed13();               // blink the led
  //todo                            //make bme measurements
  fillBmeData();              // fill out BME data packet with new information
  //Todo                            //add packetized data reciever (command)
  packetizedTransfer();       //Packetized transfer for data exchange
}
