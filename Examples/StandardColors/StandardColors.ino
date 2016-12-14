/*******************************************************************************************************************
** This program defines the RotaryEncoder class header. It is published on GitHub and full description of the     **
** class as well as the public function descriptions can be found at the GitHub wiki pages located at:            **
**                                                                                                                **
**                              https://github.com/SV-Zanshin/RotaryEncoder/wiki                                  **
**                                                                                                                **
** This program demonstrates the Rotary Encoder class which controls a commonly used rotary encoder with a clear  **
** knob using 3 colored LEDs (Red, Green and Blue) along with a pushbutton. These are available from sources such **
** as https://www.sparkfun.com/products/10982 or https://www.proto-pic.co.uk/rotary-encoder-illuminated-rgb.html  **
** along with numerous places on eBay or Amazon.  The product number is "EC12PSD-017" and it has two datasheets,  **
** one showing the physical layout and allowing hookup of LEDs and pins, the other showing the timings of the     **
** quadrature signature encoder itself.  The datasheets can be found referenced at the top-up industries page for **
** the encoder at http://www.top-up.com.tw/front/bin/ptdetail.phtml?Part=EC12PLRGBSDVBF-D&Category=325301         **
**                                                                                                                **
** The pins for ROTARY_PIN_1, ROTARY_PIN2 and PUSHBUTTON_PIN need to be set up on actual hardware interrupt pins, **
** which are different on the various Atmel processors. The ones in this sample program will work on an Arduino   **
** micro. See https://www.arduino.cc/en/Reference/AttachInterrupt for details about which pins will work on which **
** processors/systems. The pins chosen for the RED_PIN, GREEN_PIN and BLUE_PIN should be PWM capable pins so that **
** the fading will work.                                                                                          **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer           Comments                                                                 **
** ====== ========== =================== ======================================================================== **
** 1.0.0  2016-12-14 Arnd@SV-Zanshin.Com Changed include from "Encoder" to "RotaryEncoder", added comments        **
** 1.0.0  2016-12-13 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include <RotaryEncoder.h>                                                    // Include Encoder library          //
                                                                              //----------------------------------//
const uint8_t  ROTARY_PIN_1   =  2;                                           // Pin for left rotary encoder pin  //
const uint8_t  ROTARY_PIN_2   =  3;                                           // Pin for right rotary encoder pin //
const uint8_t  PUSHBUTTON_PIN =  7;                                           // Pin for pushbutton connector pin //
const uint8_t  RED_PIN        = 11;                                           // Red LED PWM pin. Ground = FULL   //
const uint8_t  GREEN_PIN      = 10;                                           // Green LED PWM pin. Ground = FULL //
const uint8_t  BLUE_PIN       =  9;                                           // Blue LED PWM pin. Ground = FULL  //
                                                                              //----------------------------------//
EncoderClass Encoder(ROTARY_PIN_1, ROTARY_PIN_2, PUSHBUTTON_PIN,              // Instantiate class defining all   //
                     RED_PIN, GREEN_PIN, BLUE_PIN);                           // of the pins that are used        //
                                                                              //----------------------------------//
void setup() {                                                                // Start One-Time run section       //
  Serial.begin(115200);                                                       // Initialize Serial I/O at speed   //
  delay(1000);                                                                // Wait 1 second for initialization //
  Serial.println("Starting Encoder Program...");                              //                                  //
} // of method setup()                                                        //                                  //
                                                                              //----------------------------------//
void loop(){                                                                  // Main program infinite loop       //
static int last = Encoder.GetEncoderValue();                                  // Store previous Encoder value     //
   uint8_t presses = Encoder.GetButton();                                     // See how often button was pressed //
   if (presses>0) {                                                           // If the button was pushed,        //
     Serial.print(presses);                                                   // Display the value                //
     if (presses==1) Serial.println(" press.");                               //                                  //
                else Serial.println(" presses.");                             //                                  //
   } // of if-then we have one or more button presses                         //                                  //
   if (Encoder.GetEncoderValue()!=last) {                                     // If the reading has changed then  //
     last = Encoder.GetEncoderValue();                                        // display the new value            //
     Serial.print("Encoder position is ");                                    //                                  //
     Serial.println(last);                                                    //                                  //
   } // of if-then we have a changed value                                    //                                  //
} // of method loop()                                                         //----------------------------------//
