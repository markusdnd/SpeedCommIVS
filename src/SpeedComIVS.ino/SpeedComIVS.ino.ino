/*Tested on arduino every*/
#include <loopTimer.h> //manually downloaded via zip
#include <SerialTransfer.h> //can be found from arduino lib


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


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  
  pinMode(led, OUTPUT);  // initialize digital pin led as an output.
  ledDelay.start(1000); // start the ledDelay, toggle every 1000mS
  printDelay.start(5000); // start the printDelay, print every 5000mS
  serialPacketDelay.start(10000);

  myTransfer.begin(Serial1);
  testStruct.z = '$';
  testStruct.y = 4.5;
}





/* the task method */
void packetizedTransfer(){
  if(serialPacketDelay.justFinished()){ // check if delay has timed out 
    serialPacketDelay.repeat();       
    uint16_t sendSize = 0;
    sendSize = myTransfer.txObj(testStruct, sendSize);
    sendSize = myTransfer.txObj(arr, sendSize);
    myTransfer.sendData(sendSize);
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
  loopTimer.check(Serial);  //debug output
  blinkLed13();             // blink the led
  packetizedTransfer();     //Packetized transfer for data exchange
}
