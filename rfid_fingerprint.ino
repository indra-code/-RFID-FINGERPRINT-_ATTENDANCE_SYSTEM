#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Adafruit_Fingerprint.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define MAX_ITERATIONS 10 
#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

int code[] = {243,163,107,168}; //This is the stored UID
int codeRead = 0;
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
String uidString;
void setup() {
  
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK);
  display.setTextSize(2);
  display.setCursor(10,0); 
  display.print("SCAN");
  display.display();
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  Serial.println("Program Started!!!\n\n");
  
}

void loop() {
  if(  rfid.PICC_IsNewCardPresent())
  {
      readRFID();
  }
  delay(100);

}
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return p;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void readRFID()
{
  //String uid = "";
    rfid.PICC_ReadCardSerial();
    // Serial.print(F("\nPICC type: "));
    // MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    // Serial.println(rfid.PICC_GetTypeName(piccType));
    //Serial.println(String(rfid.uid.uidByte[0]));
    // String uid = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
    // clearUID(uid);
    //uint32_t UID0;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // A card is detected, get UID
    
  //   for (byte i = 0; i < rfid.uid.size; i++) {
  //     uidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
  //     uidString += String(rfid.uid.uidByte[i], HEX);
  //   }
    //UID0 = (rfid.uid.uidByte[0] << 24) + (rfid.uid.uidByte[1] << 16) + (rfid.uid.uidByte[2] << 8) + rfid.uid.uidByte[3];


  }
    //Serial.println(rfid.uid.size);
    // Serial.println(uidString);
    Serial.println("\nScanned PICC's UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);
    const int MAX_STRING_LENGTH = 50; // Adjust as needed
    char result[MAX_STRING_LENGTH];
    bufferToString(rfid.uid.uidByte, rfid.uid.size, result);
    printUID(result);
    Serial.println("\n\nUID:");
    Serial.println(result);
    delay(1500);
    // String res = bufferToString(rfid.uid.uidByte, rfid.uid.size);
    //Serial.print(res);
    // Serial.print("\n\nUID: ");
    // Serial.println(uid);
    // printUID(uid);
    int i = 0;
    boolean match = true;
    while(i<rfid.uid.size)
    {
      if(!(rfid.uid.uidByte[i] == code[i]))
      {
           match = false;
      }
      i++;
    }

    if(match)
    {
      //Serial.println(uid);
      Serial.println("\nI know this card!");
      Serial.println("\nPlace Finger");
      display.clearDisplay();
      display.display();
      display.setTextColor(WHITE); // or BLACK);
      display.setTextSize(2);
      display.setCursor(10,0); 
      display.print("FINGER");
      display.display();
      delay(4000);
      uint8_t fingerprintID;
      for (int j = 0; j < 10; j++) {
        fingerprintID = getFingerprintID();
        if (fingerprintID == FINGERPRINT_OK ) {
          Serial.println("\n\nPresent!!!");
          printUnlockMessage();
          break; // Break the loop if fingerprint is found
        }
        delay(1000); // Delay between each fingerprint check
    }
    if(fingerprintID != FINGERPRINT_OK){
      display.clearDisplay();
      display.display();
      display.setTextColor(WHITE); // or BLACK);
      display.setTextSize(2);
      display.setCursor(10,0); 
      display.print("INVALID");
      display.setCursor(10,17); 
      display.print("FINGER");
      display.display();
      delay(4000);
      scanmsg();
    }
      
    }else
    {
      display.clearDisplay();
      display.display();
      display.setTextColor(WHITE); // or BLACK);
      display.setTextSize(2);
      display.setCursor(10,0); 
      display.print("UNKNOWN");
      display.setCursor(10,17); 
      display.print("CARD");
      display.display();
      Serial.println("\nUnknown Card");
      delay(1000);
      scanmsg();
    }


  //   // Halt PICC
   rfid.PICC_HaltA();

  // // Stop encryption on PCD
   rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) {
  int k = 0;
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
void bufferToString(byte *buffer, byte bufferSize, char *result) {
  byte index = 0;
  for (byte i = 0; i < bufferSize; i++) {
    // Convert each byte to its decimal representation
    index += sprintf(result + index, "%d", buffer[i]);

    // Append a space if it's not the last byte
    if (i != bufferSize - 1) {
      result[index++] = ' ';
    }
  }
  result[index] = '\0'; // Null-terminate the string
}


  void clearUID(String UID)
  {
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(1);
    display.setCursor(30,20); 
    display.print(UID);
    display.display();
  }

  void printUID(char UID[])
  {
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(0,20); 
    display.print("UID: ");
    display.setCursor(30,20); 
    display.print(UID);
    display.display();
  }

  void printUnlockMessage()
  {
    display.display();
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("FINGER");
    display.display();
    
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("VALID");
    display.display();
    
    delay(2000);
    
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("VALID");

    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("SCAN");
    display.display();
  }
  void scanmsg(){
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(2);
    display.setCursor(10,0); 
    display.print("SCAN");
    display.display();
  }





