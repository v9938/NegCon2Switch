////////////////////////////////////////////////////////////////////////
//
// PSX NEJI Controller to Switch Joystick Sample
// Copyright 2025 @V9938
//	
//	25/06/07 V0.1		1st version
//	25/06/07 V0.2		fix I/II/L Analog Value , change library NintendoSwitchControlLibrary.
//
// This program requires 
// "PsxNewLib”
// https://github.com/SukkoPera/PsxNewLib
//
// "NintendoSwitchControlLibrary"
// https://github.com/lefmarna/NintendoSwitchControlLibrary
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
// DX筐体モードだと、右スティックにシフトが来るのでこの配置だと使えないです。
// SD筐体モードでお楽しみください。
////////////////////////////////////////////////////////////////////////

#include <PsxControllerHwSpi.h>
#include <DigitalIO.h>
#include <NintendoSwitchControlLibrary.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

// ネジコンアナログ値の補正
#define NEG_CALIB 1.2

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

const byte hatValue[0x10] = {
	Hat::NEUTRAL,
	Hat::UP,
	Hat::RIGHT,
  Hat::UP_RIGHT,
	Hat::DOWN,
	Hat::NEUTRAL,
	Hat::DOWN_RIGHT,
	Hat::NEUTRAL,
	Hat::LEFT,
	Hat::UP_LEFT,
	Hat::NEUTRAL,
	Hat::NEUTRAL,
	Hat::DOWN_LEFT,
	Hat::NEUTRAL,
	Hat::NEUTRAL,
	Hat::NEUTRAL
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
  byte l_x_tmp;
  bool bSendData;
  byte hat_num;
	
	
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

      // hat_switch
      SwitchControlLibrary().pressHatButton(hatValue[(psx.getButtonWord () & 0x00f0)>>4]);

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

            // 各々のアナログ値がちょっと鈍い感じもあるので少しだけ補正する
            if (l_x > 0x80) {
              l_x_tmp = (l_x - 0x7f) * NEG_CALIB;
              if (l_x_tmp > 0x7f) l_x_tmp = 0x7f;
              l_x = 0x80 + l_x_tmp;
            }else{
              l_x_tmp = (0x80 - l_x) * NEG_CALIB;
              if (l_x_tmp > 0x80) l_x_tmp = 0x80;
              l_x = 0x80 - l_x_tmp;
            }
            l_b1 = l_b1 * NEG_CALIB;
            if (l_b1 > 0x80) l_b1 = 0x80;
            l_b2 = l_b2 * NEG_CALIB;
            if (l_b2 > 0x7f) l_b2 = 0x7f;
            l_bL = l_bL * NEG_CALIB;
            if (l_bL > 0x7f) l_bL = 0x7f;

            SwitchControlLibrary().moveLeftStick (l_x,  0x80);
            SwitchControlLibrary().moveRightStick(0x80 + l_bL, 0x80-l_b1 + l_b2);
          }

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
