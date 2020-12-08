/***************************************************************************************************
** Defines the RotaryEncoder class. See the "Encoder.h" file for program documentation            **
***************************************************************************************************/
#include "RotaryEncoder.h"             // Include the header file
EncoderClass* EncoderClass::ClassPtr;  // Declare Class Reference pointer
/*
The class constructor stores the pin values as part of the initializer and then uses an indirect
method to attach the interrupts to the appropriate routines. The Arduino design method doesn't allow
interrupts to be attached to class members. The interrupt ISR is attached to the local static
function, which in turn uses a pointer to the class with an offset to the appropriate function to
call the correct function.
*/
EncoderClass::EncoderClass(const uint8_t LeftPin, const uint8_t RightPin,  // Class constructor
                           const uint8_t PushbuttonPin, const uint8_t RedPin,
                           const uint8_t GreenPin, const uint8_t BluePin,
                           const bool HWDebounce)
    : _LeftPin(LeftPin),
      _RightPin(RightPin),
      _PushbuttonPin(PushbuttonPin),
      _RedPin(RedPin),
      _GreenPin(GreenPin),
      _BluePin(BluePin) {
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);                         // Set LED color pins to output
  if (RedPin != 255) analogWrite(RedPin, 255);      // If the LED pins are defined,
  if (GreenPin != 255) analogWrite(GreenPin, 255);  // then turn them off at the start
  if (BluePin != 255) analogWrite(BluePin, 255);
  pinMode(LeftPin, INPUT);         // Define encoder pins as input
  pinMode(RightPin, INPUT);        // Define encoder pins as input
  pinMode(PushbuttonPin, INPUT);   // Define pushbutton pin as input
  if (!HWDebounce) {               // If SW debounce then enable pullup
    digitalWrite(LeftPin, HIGH);   // Turn the pull-up resistor on
    digitalWrite(RightPin, HIGH);  // Turn the pull-up resistor on
  }                                // of if-then hardware or software debounce
  _EncoderValue = 0;               // Reset in case it was changed
  ClassPtr      = this;            // pointer to current instance
  attachInterrupt(digitalPinToInterrupt(LeftPin), RotateISR,
                  CHANGE);  // Attach static internal function
  attachInterrupt(digitalPinToInterrupt(RightPin), RotateISR,
                  CHANGE);  // Attach static internal function
  attachInterrupt(digitalPinToInterrupt(PushbuttonPin), PushButtonISR,
                  RISING);  // Attach static internal function
  if (RedPin == 255 && GreenPin == 255 && BluePin == 255)
    SetFadeRate(0);  // If no LEDs, turn off fader
  else
    SetFadeRate(1);  // turn on fader to max speed
}  // of class constructor
/*
Define the 5 ISR (Interrupt Service Routines). These definitions are done as static functions which
can be set directly as part of the Arduino IDE inside a class definition. They, in turn, redirect
the interrupt to a class member function where the actual interrupt is handled
*/
ISR(TIMER0_COMPA_vect) { EncoderClass::TimerISR(); }  // Call the ISR every millisecond
ISR(TIMER0_COMPB_vect) { EncoderClass::TimerISR(); }  // Call the ISR every millisecond
static void EncoderClass::PushButtonISR() {
  ClassPtr->PushButtonHandler();
}  // Redirect to real handler function
static void EncoderClass::RotateISR() {
  ClassPtr->RotateHandler();
}  // Redirect to real handler function
static void EncoderClass::TimerISR() {
  ClassPtr->TimerHandler();
}  // Redirect to real handler function
/*
function TimerHandler() is linked to the millis() timer 0 interrupt. The TimerISR() limits how often
this is called and each time it is triggered it performs any adjustments between the actual and
target values of the RGB LEDs of the device. It is also the only place where the actual PWM values
for RGB are set.
*/
void EncoderClass::TimerHandler() {
  if (_LEDChanged ||  // Only check if LEDs aren't off
      !(_RedActual == 255 && _GreenActual == 255 && _BlueActual == 255)) {
    if (millis() % _FadeMillis == 0) {  // If we are fading colors, then
      _LEDChanged = false;              // Reset the value
      if (_RedActual != _RedTarget) {   // adjust accordingly
        if (_RedActual < _RedTarget)
          _RedActual++;
        else
          _RedActual--;
      }  // of if-then actual and target don't match
      if (_GreenActual != _GreenTarget) {
        if (_GreenActual < _GreenTarget)
          _GreenActual++;
        else
          _GreenActual--;
      }  // of if-then actual and target don't match
      if (_BlueActual != _BlueTarget) {
        if (_BlueTarget < _BlueTarget)
          _BlueActual++;
        else
          _BlueActual--;
      }  // of if-then actual and target don't match
      if (_RedTarget != 255 && _RedActual == _RedTarget)
        _RedTarget++;  // Fade Red if max has been reached
      if (_GreenTarget != 255 && _GreenActual == _GreenTarget)
        _GreenTarget++;  // Fade Green          "   "
      if (_BlueTarget != 255 && _BlueActual == _BlueTarget)
        _BlueTarget++;                       // Fade Blue           "   "
      analogWrite(_RedPin, _RedActual);      // show the Red,
      analogWrite(_GreenPin, _GreenActual);  // Green, and
      analogWrite(_BluePin, _BlueActual);    // Blue values
    }                                        // of if-then we want to fade LED brightness
  }                                          // of if-then we need to do something
}  // of method FaderButtonHandler()
/*
function PushButtonHandler() is called when the interrupt occurs on a rising signal. Debouncing is
done in the hardware but we'll also add some checking here. The _ButtonPushed flag is cleared when
the ButtonPushed() function is called.
*/
void EncoderClass::PushButtonHandler() {
  static long lastPushed = 0;                // Store the last button time
  if (millis() - lastPushed > 150) {         // if time > 250ms then allow set
    _ButtonPresses++;                        // Increment the counter
    _LEDChanged  = true;                     // We are changing target values
    changed      = true;                     // Something has changed
    lastPushed   = millis();                 // Store new push button time
    _RedTarget   = _ColorPushButtonR;        // Set target color
    _GreenTarget = _ColorPushButtonG;        // Set target color
    _BlueTarget  = _ColorPushButtonB;        // Set target color
    if (_FadeMillis == 0) {                  // Manually set if no fade
      analogWrite(_RedPin, _RedTarget);      // show the Red,
      analogWrite(_GreenPin, _GreenTarget);  // Green, and
      analogWrite(_BluePin, _BlueTarget);    // Blue values
    }                                        // of if fading is turned off
  }                                          // of if-then we have a valid pushbutton event
}  // of method PushButtonHandler()
   /*
   function RotateHandler() is the actual ISR called when a pin change on the left or right pin is
   detected. The    quadrature values for the 2 pins are read and the two bits are stored in the
   "encoded"    variable. This is ORd    with the previous value to get 4 bits and there are only 8
   possible values    that denote a valid reading and    inform us as to which direction the dial was
   turned.
   */
void EncoderClass::RotateHandler() {
  static uint8_t lastEncoded = 0;                            // Save last encoded raw value
  uint8_t encoded = ((uint8_t)digitalRead(_LeftPin) << 1) |  // converting the 2 pin values into
                    (uint8_t)digitalRead(_RightPin);         // a  single number
  uint8_t sum = (lastEncoded << 2) | encoded;                // add to previously encoded value
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 ||
      sum == 0b1011) {         // if clockwise turn direction
    _EncoderValue++;           // add value and brighten the
    changed      = true;       // Something has changed
    _LEDChanged  = true;       // We are changing target values
    _RedTarget   = _ColorCWR;  // Set target color
    _GreenTarget = _ColorCWG;  // Set target color
    _BlueTarget  = _ColorCWB;  // Set target color
  } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 ||
             sum == 0b1000) {              // if a counterclockwise turn
    _EncoderValue--;                       // subtract the value and brighten
    changed      = true;                   // Something has changed
    _LEDChanged  = true;                   // We are changing target values
    _RedTarget   = _ColorCCWR;             // Set target color
    _GreenTarget = _ColorCCWG;             // Set target color
    _BlueTarget  = _ColorCCWB;             // Set target color
  }                                        // of if-then a CCW turn
  lastEncoded = encoded;                   // store the value for next time
  if (_FadeMillis == 0) {                  // Manually set if no fade
    analogWrite(_RedPin, _RedTarget);      // show the Red,
    analogWrite(_GreenPin, _GreenTarget);  // Green, and
    analogWrite(_BluePin, _BlueTarget);    // Blue values
  }                                        // of if fading is turned off
}  // of method RotateHandler()
/*
function ButtonPushes() returns number of button pushes since the last call and resets the value
*/
uint8_t EncoderClass::GetButton() {
  uint8_t returnValue = _ButtonPresses;  // Set the return value
  _ButtonPresses      = 0;               // reset value
  return (returnValue);
}  // of method GetButton()
/*
function SetColor() is called to set the RGB values to set when the button is pushed
*/
void EncoderClass::SetColor(const uint8_t R, const uint8_t G, const uint8_t B) {
  _RedActual   = R;                        // set internal values
  _GreenActual = G;                        // set internal values
  _BlueActual  = B;                        // set internal values
  _RedTarget   = R;                        // set internal values
  _GreenTarget = G;                        // set internal values
  _BlueTarget  = B;                        // set internal values
  _LEDChanged  = true;                     // Mark that we have a change
  if (_FadeMillis == 0) {                  // Manually set if no fade
    analogWrite(_RedPin, _RedTarget);      // show the Red,
    analogWrite(_GreenPin, _GreenTarget);  // Green, and
    analogWrite(_BluePin, _BlueTarget);    // Blue values
  }                                        // of if fading is turned off
}  // of method SetColor
/*
function SetPushButtonColor() is called to set the RGB values to set when the button is pushed
*/
void EncoderClass::SetPushButtonColor(const uint8_t R, const uint8_t G, const uint8_t B) {
  _ColorPushButtonR = R;  // set internal values
  _ColorPushButtonG = G;  // set internal values
  _ColorPushButtonB = B;  // set internal values
}  // of method SetPushButtonColor
/*
function SetCWTurnColor() is called to set the RGB values to set when the dial is turned clockwise
*/
void EncoderClass::SetCWTurnColor(const uint8_t R, const uint8_t G, const uint8_t B) {
  _ColorCWR = R;  // set internal values
  _ColorCWG = G;  // set internal values
  _ColorCWB = B;  // set internal values
}  // of method SetCWTurnColor()
/*
function SetCCWTurnColor() is called to set the RGB values to set when the dial is turned
counterclockwise
*/
void EncoderClass::SetCCWTurnColor(const uint8_t R, const uint8_t G, const uint8_t B) {
  _ColorCCWR = R;  // set internal values
  _ColorCCWG = G;  // set internal values
  _ColorCCWB = B;  // set internal values
}  // of method SetCCWTurnColor()
/*
function SetLEDState() is called to turn the rotary LED lights on or off. If the lights are set to
OFF (false) then none of the other color-related functions will have any effect until the LEDs are
turn ON (true)
*/
void EncoderClass::SetLEDState(const bool Status) {
  _LEDOn = Status;              // Set the internal switch variable
  if (!Status) {                // if we are turning off the LEDs
    analogWrite(_RedPin, 255);  // ensure everything is turned off
    analogWrite(_GreenPin, 255);
    analogWrite(_BluePin, 255);
  }  // of if-then we are turning LEDs off
}  // of method SetLEDState()
/*
function GetEncoderValue() is called to return the current encoder rotator value as a signed integer
number
*/
int16_t EncoderClass::GetEncoderValue() {
  return (_EncoderValue);  // Return the current value
}  // of method GetEncoderValue()
/*
function SetEncoderValue() is called to set the current encoder rotator value as a signed integer
number
*/
void EncoderClass::SetEncoderValue(const int16_t NewValue) {
  _EncoderValue = NewValue;  // Set the new value
}  // of method SetEncoderValue()
/*
function SetFadeRate() is called to turn the fade functionality on or off and adjust the rate at
which the fade occurs. The fade is done by accessing the Timer0 interrupt, which is used by the
millis() function and is an 8-bit register with a clock divisor of 64 which triggers it to overflow
at a rate of 976.5625Hz, or roughly every millisecond. We set the TIMER0_COMPA_vect to 0x01 which
triggers when the value is equal to 64. This then gives us an identical trigger speed but different
trigger point to the millis() function which triggers when the Timer0 overflows. The same setup is
done for the TIMER0_COMPB_vect but that is set to trigger halfway along the full range of 255 at
192, thus giving an interrupt rate of 2 times per milli second. The FadeSpeed equates to how many
milliseconds there are between incremental fades, the fastest is 1 which is every 1/2 millisecond, 2
is every millisecond, etc. Each time the trigger is reached all of the LED values which are not
"off" are dimmed by 1/255 of the total value. A setting of 10 would fade the LEDs from full on to
OFF 1/255 of their brightness in 1.28 seconds
*/
void EncoderClass::SetFadeRate(const uint8_t FadeSpeed) {
  _FadeMillis = FadeSpeed;   // Set the private variable to value
  if (FadeSpeed) {           // If turning on, set the ISR
    cli();                   // Disable interrupts
    OCR0A = 0x40;            // Comparison register A to 64
    OCR0B = 0xC0;            // Comparison register B to 192
    TIMSK0 |= _BV(OCIE0A);   // TIMER0_COMPA trigger on 0x01
    TIMSK0 |= _BV(OCIE0B);   // TIMER0_COMPB trigger on 0x80
    sei();                   // Enable interrupts
  } else {                   // If turning off, unset the ISR
    cli();                   // Disable interrupts
    TIMSK0 &= ~_BV(OCIE0A);  // TIMER0_COMPA trigger off
    TIMSK0 &= ~_BV(OCIE0B);  // TIMER0_COMPB trigger off
    sei();                   // Enable interrupts
  }                          // of if-then-else we need to turn fading on or off
}  // of method SetFadeRate
