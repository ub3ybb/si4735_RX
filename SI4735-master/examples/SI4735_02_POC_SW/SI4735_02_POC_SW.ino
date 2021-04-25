/*
   Test and validation of the SI4735 Arduino Library.
   This example shows how can setup the radio on SW by using the Si4735 Arduino Library.

   The main advantages of using this sketch are:
   1) It is a easy way to check if your circuit is working;
   2) You do not need to connect any display device to make your radio works;
   3) You do not need connect any push buttons or encoders to change volume and frequency;
   4) The Arduino IDE is all you need to control the radio.

   Schematic: https://github.com/pu2clr/SI4735/blob/master/extras/images/basic_schematic.png

   By Ricardo Lima Caratti, Nov 2019.
*/

#include <SI4735.h>

#define RESET_PIN 12

#define AM_FUNCTION 1
#define FM_FUNCTION 0


typedef struct {
  uint16_t   minimumFreq;
  uint16_t   maximumFreq;
  uint16_t   currentFreq;
  uint16_t   currentStep;
} Band;


Band band[] = {{4700, 5200, 4850, 5},
  {5700, 6200, 6000, 5},
  {7100, 7600, 7300, 5},
  {9300, 10000, 9600, 5},
  {11400, 12200, 11940, 5},
  {13500, 13900, 13600, 5},
  {15000, 15800, 15200, 5},
  {17400, 17900, 17600, 5},
  {21400, 21800, 21500, 5},
  {27000, 27500, 27220, 1}
};

const int lastBand = (sizeof band / sizeof(Band)) - 1;
int  currentFreqIdx = 1; // 41M

uint16_t currentFrequency;

SI4735 si4735;

void setup()
{
  Serial.begin(9600);
  Serial.println("Test and validation of the SI4735 Arduino Library.");
  Serial.println("AM and FM station tuning test.");

  showHelp();

  delay(500);

  si4735.setup(RESET_PIN, FM_FUNCTION);

  // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 100KHz)
  si4735.setFM(8400, 10800,  10390, 10);

  delay(500);

  currentFrequency = si4735.getFrequency();
  si4735.setVolume(45);
  showStatus();
}

// Instructions
void showHelp() {
  Serial.println("Type F to FM; A to MW; and 1 or 2 to SW");
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type S or s to seek station Up or Down");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type 0 to show current status");
  Serial.println("Type 1 to go to the next band");
  Serial.println("Type 2 to go to the previous band");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Show current frequency and status
void showStatus()
{

  delay(250);
  band[currentFreqIdx].currentFreq = currentFrequency = si4735.getFrequency();

  Serial.print("You are tuned on ");
  if (si4735.isCurrentTuneFM() ) {
    Serial.print(String(currentFrequency / 100.0, 2));
    Serial.print("MHz ");
    Serial.print((si4735.getCurrentPilot()) ? "STEREO" : "MONO");
  } else {
    Serial.print(currentFrequency);
    Serial.print("KHz");
  }

  si4735.getCurrentReceivedSignalQuality();
  Serial.print(" [SNR:" );
  Serial.print(si4735.getCurrentSNR());
  Serial.print("dB");

  Serial.print(" Signal:" );
  Serial.print(si4735.getCurrentRSSI());
  Serial.println("dBuV]");

}

// Main
void loop()
{
  if (Serial.available() > 0)
  {
    char key = Serial.read();
    switch (key)
    {
      case '+':
        si4735.volumeUp();
        break;
      case '-':
        si4735.volumeDown();
        break;
      case 'a':
      case 'A':
        si4735.setAM(570, 1710,  810, 10);
        showStatus();
        break;
      case 'f':
      case 'F':
        si4735.setFM(8600, 10800,  10390, 10);
        showStatus();
        break;
      case '2':
        if ( currentFreqIdx < lastBand ) {
          currentFreqIdx++;
        } else {
          currentFreqIdx = 0;
        }
        si4735.setAM(band[currentFreqIdx].minimumFreq, band[currentFreqIdx].maximumFreq, band[currentFreqIdx].currentFreq, band[currentFreqIdx].currentStep);
        delay(100);
        currentFrequency = band[currentFreqIdx].currentFreq;
        showStatus();
        break;
      case '1':
        if ( currentFreqIdx > 0 ) {
          currentFreqIdx--;
        } else {
          currentFreqIdx = lastBand;
        }
        si4735.setAM(band[currentFreqIdx].minimumFreq, band[currentFreqIdx].maximumFreq, band[currentFreqIdx].currentFreq, band[currentFreqIdx].currentStep);
        delay(100);
        currentFrequency = band[currentFreqIdx].currentFreq;
        showStatus();
        break;
      case 'U':
      case 'u':
        si4735.frequencyUp();
        showStatus();
        break;
      case 'D':
      case 'd':
        si4735.frequencyDown();
        showStatus();
        break;
      case 'S':
        si4735.seekStationUp();
        showStatus();
        break;
      case 's':
        si4735.seekStationDown();
        showStatus();
        break;
      case '0':
        showStatus();
        break;
      case '?':
        showHelp();
        break;
      default:
        break;
    }
  }

  delay(200);
}
