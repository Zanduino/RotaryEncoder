/*******************************************************************************************************************
** This program defines the RotaryEncoder class header.                                                           **
**                                                                                                                **
** This program demonstrates the Rotary Encoder class which controls a commonly used rotary encoder with a clear  **
** knob using 3 colored LEDs (Red, Green and Blue) along with a pushbutton. These are available from sources such **
** as https://www.sparkfun.com/products/10982 or https://www.proto-pic.co.uk/rotary-encoder-illuminated-rgb.html  **
** along with numerous places on eBay or Amazon.  The product number is "EC12PSD-017" and it has two datasheets,  **
** one showing the physical layout and allowing hookup of LEDs and pins, the other showing the timings of the     **
** quadrature signature encoder itself.  The datasheets can be found referenced at the top-up industries page for **
** the encoder at http://www.top-up.com.tw/front/bin/ptdetail.phtml?Part=EC12PLRGBSDVBF-D&Category=325301         **
**                                                                                                                **
** Each Atmel processor has different pins available for interrupts, so the choice of pins to use for the         **
** ROTARY_PIN_1, ROTARY_PIN_2 and PUSHBUTTON_PIN is quite limited. A good description of the pins and their uses  **
** can be found at https://www.arduino.cc/en/Reference/AttachInterrupt.                                           **
**                                                                                                                **
** The library uses the Arduino functions "digitalRead()" to get pin state in the interrupt handlers. These are   **
** relatively slow compared to the direct register reading methods available. Since the library is designed for a **
** hand-turned encoder versus a high-speed machine with thousands of RPM the slower functions have been retained  **
** for more portability and legibility. Going directly to the PORTA , PORTB, etc. register reads would make the   **
** pin mapping that needs to be done too complex.                                                                 **
**                                                                                                                **
** The rotary encoder has a common cathode (+) for the 3 LED lights. The pins for the 3 colors need to be attached**
** to any 3 available PWM pins (not all of the Atmel pins are PWM, see https://www.arduino.cc/en/Tutorial/PWM) and**
** since these pins to be driven to ground, a value of 255 equates to "OFF" while a value of 0 equates to "ON".   **
** The encoder doesn't have any current limiting resistors for the 3 LEDs, so these need to be added. The forward **
** voltage for the red is 2.0V and the green/blue LEDs have 3.3V and with a 25mA current that equates to resistors**
** at 120 and 68 Ohms respectively.                                                                               **
**                                                                                                                **
** The pushbutton has a pull-down resistor of 4.7KOhm to reduce bounce                                            **
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
** 1.0.1  2016-12-14 Arnd@SV-Zanshin.Com Fixed error on condition to turn off LED lights.                         **
** 1.0.0  2016-12-14 Arnd@SV-Zanshin.Com Allowed defaults for LEDs on class constructer                           **
** 1.0.b3 2016-12-13 Arnd@SV-Zanshin.Com Made fading start only after the maximum setting was reached             **
** 1.0.b2 2016-12-12 Arnd@SV-Zanshin.Com Fixed interrupt calls                                                    **
** 1.0.b1 2016-12-04 Arnd@SV-Zanshin.Com Initial coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#ifndef RotaryEncoder_h                                                       // Guard code definition            //
  #define RotaryEncoder_h                                                     // Define the name inside guard code//
  class EncoderClass {                                                        //----------------------------------//
    public:                                                                   // Publicly visible class members   //
      EncoderClass(const uint8_t LeftPin, const uint8_t RightPin,             // Class constructor                //
                   const uint8_t PushbuttonPin, const uint8_t RedPin=255,     //                                  //
                   const uint8_t GreenPin=255,const uint8_t BluePin=255);     //                                  //
      uint8_t     GetButton();                                                // Returns number of button pushes  //
      int16_t     GetEncoderValue();                                          // Return the encoder value         //
      void        SetEncoderValue(const int16_t NewValue = 0);                // Set the encoder value            //
      void        SetLEDState(const bool Status);                             // Turns encoder LEDs on or off     //
      void        SetFade(const bool FadeState);                              // Sets the fader state             //
      void        SetColor(const uint8_t R,const uint8_t G,const uint8_t B);  // Sets the LED colors              //
      void        SetPushButtonColor(const uint8_t R,const uint8_t G,         // Sets the RGB values displayed    //
                                     const uint8_t B);                        // when the pushbutton is pressed   //
      void        SetCWTurnColor    (const uint8_t R,const uint8_t G,         // Sets the RGB values displayed    //
                                     const uint8_t B);                        // when rotated clockwise           //
      void        SetCCWTurnColor   (const uint8_t R,const uint8_t G,         // Sets the RGB values displayed    //
                                     const uint8_t B);                        // when rotated counterclockwise    //
      static void TimerISR();                                                 // Interim ISR calls real handler   //
    private:                                                                  // Declare private class members    //
      static void PushButtonISR();                                            // Interim ISR calls real handler   //
      static void RotateISR();                                                // Interim ISR calls real handler   //
      void        RotateHandler();                                            // Real handler for left/right turns//
      void        PushButtonHandler();                                        // Real handler for pushbutton event//
      void        TimerHandler();                                             // Called every millisecond for fade//
      static EncoderClass* ClassPtr;                                          // store pointer to class itself    //
      uint8_t _LeftPin;                                                       // Store local copies of the pins   //
      uint8_t _RightPin;                                                      // declared at class instantiation  //
      uint8_t _PushbuttonPin;                                                 //                                  //
      uint8_t _RedPin;                                                        //                                  //
      uint8_t _GreenPin;                                                      //                                  //
      uint8_t _BluePin;                                                       //                                  //
      bool    _Fade                  =  true;                                 // Default to fade to dark          //
      bool    _LEDOn                 =  true;                                 // Default to display LED lights    //
      volatile bool _LEDChanged      =  true;                                 // Set when rotate or click changes //
      volatile uint8_t _ButtonPresses =    0;                                 // The current number of pushes     //
      volatile long _EncoderValue    =     0;                                 // The current encoder value        //
      uint8_t _RedActual             =   255;                                 // Actual value for the Red LED     //
      uint8_t _RedTarget             =   255;                                 // Target value for the Red LED     //
      uint8_t _GreenActual           =   255;                                 // Actual value for the Green LED   //
      uint8_t _GreenTarget           =   255;                                 // Target value for the Green LED   //
      uint8_t _BlueActual            =   255;                                 // Actual value for the Blue LED    //
      uint8_t _BlueTarget            =   255;                                 // Target value for the Blue LED    //
      uint8_t _ColorPushButtonR      =     0;                                 // Default pushbutton to pure Red   //
      uint8_t _ColorPushButtonG      =   255;                                 //                                  //
      uint8_t _ColorPushButtonB      =   255;                                 //                                  //
      uint8_t _ColorCWR              =   255;                                 // Color for clockwise turns        //
      uint8_t _ColorCWG              =     0;                                 //                                  //
      uint8_t _ColorCWB              =   255;                                 //                                  //
      uint8_t _ColorCCWR             =   255;                                 // Color for counterclockwise turns //
      uint8_t _ColorCCWG             =   255;                                 //                                  //
      uint8_t _ColorCCWB             =     0;                                 //                                  //
  }; // of class header definition for EncoderClass                           //----------------------------------//
#endif