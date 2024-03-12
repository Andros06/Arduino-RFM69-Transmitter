//inkluderer bibliotek
#include <SPI.h>
#include <RH_RF95.h>

//definerer pinner
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Definerer frekvens til radio (915Mhz)
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT); //setter RFM95_RST reset pin til output
  digitalWrite(RFM95_RST, HIGH); //setter reset pin til høg, lar rf module boote


  //venter til seriel kommunikasjon mellom arduino og pc er satt opp, starter deretter seriel med 9600 bits per sekund
  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa TX Test!"); //kjøyre en liten test melding på seriel kommunikasjon

  // manuel reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // initialiserer rf95 kommunikasjon, printer status på initialisering
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");


  // Printer frekvensen som blei satt tidligare i koden, dersom frekvens ikkje blir satt riktig printast feilmelding
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  //setter rf95 styrke nivå, setter til 23dBm. false deaktiverer "boost" pin til rf95
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // variabel som teller kor mange meldinger som blir sendt

void loop()
{
  Serial.println("Sending to rf95_server"); //seriel printer at det skal bli sendt en melding
  
  //lager til meldinga som skal bli sendt over RF og seriel printer
  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[19] = 0;
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

//printer "waiting for reply" og venter tidspunkt for å sikre seg at tilbakemelding er klar
  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 

    // Printer dersom tilbakemelding er mottatt og RSSI verdi  
    if (rf95.recv(buf, &len)) //prøver å mottatt tilbakemelding med buf og len verdier
   {
    //blir utført ved tilbakemelding
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed"); //printer dersom rf95.recv failer
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?"); //Printer dersom tilbakemelding ikkje blir mottatt
  }
  delay(1000);
}