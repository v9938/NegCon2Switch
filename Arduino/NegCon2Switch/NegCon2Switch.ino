////////////////////////////////////////////////////////////////////////
//
// PSX NEJI Controller to Switch Joystick Sample
// Copyright 2025 @V9938
//	
//	25/06/07 V0.1		1st version
//
// This program requires 
// "PsxNewLib”
// https://github.com/SukkoPera/PsxNewLib
//
// "SwitchControlLibrary"
// https://github.com/celclow/SwitchControlLibrary
//
// Use Arduino AVR board version v1.82 or below
//
// If modifying the PID/VID values in 'board.txt' is not possible at v2.xx IDE, 
// please add the settings to the file listed below." 
// 
// "C:\Users\????\AppData\Local\Arduino15\packages\hardware\avr\1.8.2\cores\arduino\USBCore.cpp"
// // added 
// #define USB_PRODUCT     "Pokken Tournament DX Pro Pad"
// #define USB_MANUFACTURER "HORI"
// #define USB_VID 0x0f0d
// #define USB_PID 0x0092
// 
////////////////////////////////////////////////////////////////////////

#include <PsxControllerHwSpi.h>
#include <DigitalIO.h>
#include <CustomHID.h>
#include <SwitchControlLibrary.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

/** \brief Pin used for Controller Attention (ATTN)
 *
 * This pin makes the controller pay attention to what we're saying. The shield
 * has pin 10 wired for this purpose.
 */
const byte PIN_PS2_ATT = 2;


/** \brief Pin for Button Press Led
 *
 * This led will light up whenever a button is pressed on the controller.
 */
const byte PIN_BUTTONPRESS = 7;

bool haveController = false;
/** \brief Dead zone for analog sticks
 *  
 * If the analog stick moves less than this value from the center position, it
 * is considered still.
 * 
 * \sa ANALOG_IDLE_VALUE
 */
const byte ANALOG_DEAD_ZONE = 50U;

PsxControllerHwSpi<PIN_PS2_ATT> psx;

// Controller Type
const char ctrlTypeUnknown[] PROGMEM = "Unknown";
const char ctrlTypeDualShock[] PROGMEM = "Dual Shock";
const char ctrlTypeDsWireless[] PROGMEM = "Dual Shock Wireless";
const char ctrlTypeGuitHero[] PROGMEM = "Guitar Hero";
const char ctrlTypeOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerTypeStrings[PSCTRL_MAX + 1] PROGMEM = {
	ctrlTypeUnknown,
	ctrlTypeDualShock,
	ctrlTypeDsWireless,
	ctrlTypeGuitHero,
	ctrlTypeOutOfBounds
};


// Controller Protocol
const char ctrlProtoUnknown[] PROGMEM = "Unknown";
const char ctrlProtoDigital[] PROGMEM = "Digital";
const char ctrlProtoDualShock[] PROGMEM = "Dual Shock";
const char ctrlProtoDualShock2[] PROGMEM = "Dual Shock 2";
const char ctrlProtoFlightstick[] PROGMEM = "Flightstick";
const char ctrlProtoNegcon[] PROGMEM = "neGcon";
const char ctrlProtoJogcon[] PROGMEM = "Jogcon";
const char ctrlProtoOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerProtoStrings[PSPROTO_MAX + 1] PROGMEM = {
	ctrlProtoUnknown,
	ctrlProtoDigital,
	ctrlProtoDualShock,
	ctrlProtoDualShock2,
	ctrlProtoFlightstick,
	ctrlProtoNegcon,
	ctrlProtoJogcon,
	ctrlTypeOutOfBounds
};

 
void setup () {
  SwitchControlLibrary();
	fastPinMode (PIN_BUTTONPRESS, OUTPUT);
	
	delay (300);

//	Serial.begin (115200);
//	while (!Serial) {
//		// Wait for serial port to connect on Leonardo boards
//		fastDigitalWrite (PIN_BUTTONPRESS, (millis () / 333) % 2);
//	}
	Serial.println (F("Ready!"));
}
 
void loop () {
	static int8_t slx, sly, sb1, sb2, sbL;
  byte l_x, l_y, l_b1, l_b2, l_bL;
  bool bSendData;
	
	
	if (!haveController) {
		if (psx.begin ()) {
			Serial.println (F("Controller found!"));
			delay (300);
			if (!psx.enterConfigMode ()) {
				Serial.println (F("Cannot enter config mode"));
			} else {
				PsxControllerType ctype = psx.getControllerType ();
				PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
				Serial.print (F("Controller Type is: "));
				Serial.println (PSTR_TO_F (cname));

				if (!psx.enableAnalogSticks ()) {
					Serial.println (F("Cannot enable analog sticks"));
				}
				
				if (!psx.enableAnalogButtons ()) {
					Serial.println (F("Cannot enable analog buttons"));
				}
				
				if (!psx.exitConfigMode ()) {
					Serial.println (F("Cannot exit config mode"));
				}
			}

			psx.read ();		// Make sure the protocol is up to date
			PsxControllerProtocol proto = psx.getProtocol ();
			PGM_BYTES_P pname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerProtoStrings[proto < PSPROTO_MAX ? static_cast<byte> (proto) : PSPROTO_MAX])));
			Serial.print (F("Controller Protocol is: "));
			Serial.println (PSTR_TO_F (pname));

			haveController = true;
		}
	} else {
		if (!psx.read ()) {
			Serial.println (F("Controller lost :("));
			haveController = false;
		} else {
      // buttonSelect = Button::MINUS
      if (psx.getButtonWord () & 0x0001) 
        SwitchControlLibrary().pressButton(Button::MINUS);
      else
        SwitchControlLibrary().releaseButton(Button::MINUS);

      // buttonL3 = Button::LCLICK
      if (psx.getButtonWord () & 0x0002) 
        SwitchControlLibrary().pressButton(Button::LCLICK);
      else
        SwitchControlLibrary().releaseButton(Button::LCLICK);

      // buttonR3 = Button::RCLICK
      if (psx.getButtonWord () & 0x0004) 
        SwitchControlLibrary().pressButton(Button::RCLICK);
      else
        SwitchControlLibrary().releaseButton(Button::RCLICK);

      // buttonStart = Button::PLUS
      if (psx.getButtonWord () & 0x0008) 
        SwitchControlLibrary().pressButton(Button::PLUS);
      else
        SwitchControlLibrary().releaseButton(Button::PLUS);

      // buttonUp = HatButton::UP
      if (psx.getButtonWord () & 0x0010) 
        SwitchControlLibrary().pressHatButton(HatButton::UP);
      else
        SwitchControlLibrary().releaseHatButton(HatButton::UP);

      // buttonRight = HatButton::RIGHT
      if (psx.getButtonWord () & 0x0020) 
        SwitchControlLibrary().pressHatButton(HatButton::RIGHT);
      else
        SwitchControlLibrary().releaseHatButton(HatButton::RIGHT);

      // buttonDown = HatButton::DOWN
      if (psx.getButtonWord () & 0x0040) 
        SwitchControlLibrary().pressHatButton(HatButton::DOWN);
      else
        SwitchControlLibrary().releaseHatButton(HatButton::DOWN);

      // buttonLeft = HatButton::LEFT
      if (psx.getButtonWord () & 0x0080) 
        SwitchControlLibrary().pressHatButton(HatButton::LEFT);
      else
        SwitchControlLibrary().releaseHatButton(HatButton::LEFT);

      // buttonL2 = Button::ZL
      if (psx.getButtonWord () & 0x0100) 
        SwitchControlLibrary().pressButton(Button::ZL);
      else
        SwitchControlLibrary().releaseButton(Button::ZL);

      // buttonR2 = Button::ZR
      if (psx.getButtonWord () & 0x0200) 
        SwitchControlLibrary().pressButton(Button::ZR);
      else
        SwitchControlLibrary().releaseButton(Button::ZR);

      // buttonL1 = Button::L
      if (psx.getButtonWord () & 0x0400) 
        SwitchControlLibrary().pressButton(Button::L);
      else
        SwitchControlLibrary().releaseButton(Button::L);

      // buttonR1 = Button::R
      if (psx.getButtonWord () & 0x0800) 
        SwitchControlLibrary().pressButton(Button::R);
      else
        SwitchControlLibrary().releaseButton(Button::R);

      // buttonTriangle = Button::X
      if (psx.getButtonWord () & 0x1000) 
        SwitchControlLibrary().pressButton(Button::X);
      else
        SwitchControlLibrary().releaseButton(Button::X);

      // buttonCircle = Button::A
      if (psx.getButtonWord () & 0x2000) 
        SwitchControlLibrary().pressButton(Button::A);
      else
        SwitchControlLibrary().releaseButton(Button::A);

      // buttonCross = Button::B
      if (psx.getButtonWord () & 0x4000) 
        SwitchControlLibrary().pressButton(Button::B);
      else
        SwitchControlLibrary().releaseButton(Button::B);

      // buttonSquare = Button::Y
      if (psx.getButtonWord () & 0x8000) 
        SwitchControlLibrary().pressButton(Button::Y);
      else
        SwitchControlLibrary().releaseButton(Button::Y);


		  if ((psx.getProtocol () == PSPROTO_NEGCON) || (psx.getProtocol () != PSPROTO_JOGCON)) {
      //
		    if (psx.getLeftAnalog (l_x, l_y)){
  		    l_b1 = psx.getAnalogButton(6)/2;
	  	    l_b2 = psx.getAnalogButton(7)/2;
  		    l_bL = psx.getAnalogButton(8)/2;
  			  if (l_x != slx || l_b1 != sb1 || l_b2 != sb2 || l_bL != sbL) {
//        	  Serial.print (F(" Neg_analog: x = 0x"));
//        	  Serial.print (l_x,HEX);
//         	  Serial.print (F(", B1 = 0x"));
//            Serial.print (l_b1,HEX);
//         	  Serial.print (F(", B2 = 0x"));
//            Serial.print (l_b2,HEX);
//        	  Serial.print (F(", BL = 0x"));
//            Serial.println (l_bL,HEX);
				    slx = l_x;
				    sb1 = l_b1;
				    sb2 = l_b2;
				    sbL = l_bL;
            
            SwitchControlLibrary().moveLeftStick (l_x,  0x80);
            SwitchControlLibrary().moveRightStick(0x80-l_b1 + l_b2, 0x80 + l_bL);
          }

          if (l_b1 > 0x04)  SwitchControlLibrary().pressButton(Button::ZR);
          if (l_b2 > 0x04)  SwitchControlLibrary().pressButton(Button::ZL);

          SwitchControlLibrary().releaseButton(Button::Y);
          SwitchControlLibrary().releaseButton(Button::B);
          SwitchControlLibrary().releaseButton(Button::L);
        }
      }
      SwitchControlLibrary().sendReport();

    }
	}

  //ネジコンはフレーム待ちしないでも正常に値が取れるので削除
	// Only poll "once per frame" ;)
//	delay (1000 / 60);
}
