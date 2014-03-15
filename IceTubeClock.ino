/*
 *
 *
 */
 
//////////////
// Settings //
//////////////

// Pin-Settings
#define VFDPWR  9 // Power On/Off the VCC for the Tube and the Driver
#define BLANK   7 // Driver: If this is HIGH, the driver sets all Outputs to LOW
#define LOAD    8 // Driver: Loads the data from shift register to output latch
#define CLK    13 // Driver: Shifts in a Bit on rising edge
#define DIN    11 // Driver: Data In (gets shiftet on CLK rising edge)
#define BOOST  10 // PWM-Signal for boost power supply

//////////////
// Mappings //
//////////////

// Decimal numbers to bitmask for the 7-segments (+ decimal Point)
uint8_t number_bitmask[11] = {
  0b11101110, // 0
  0b00100100, // 1
  0b01111010, // 2
  0b01110110, // 3
  0b10110100, // 4
  0b11010110, // 5
  0b11011111, // 6
  0b01100100, // 7
  0b11111110, // 8
  0b11110111, // 9
  0b00000000  // 10 = off
};

// Bitmask for selecting the digits from left to right
uint8_t digit_bitmask[9] = {
  0b00000100, // left most 7-segment digit
  0b00100000,
  0b00010000,
  0b00001000,
  0b01000000,
  0b00000010,
  0b10000000,
  0b00000001, // right most 7-segment digit
  0b00000000  // signs (dot and minus)
};

// Bitmasks for the minus and the dot
uint8_t dot_bitmask   = 0b00000001;
uint8_t minus_bitmask = 0b00010000;

/////////////////
// Global vars //
/////////////////
boolean dot = false;
boolean minus = true;

byte current_digit = 0;
//byte display_value[8] = {10,10,10,10,10,0,0,0};
byte display_value[8] = {0,1,2,3,4,5,6,7};

////////////////////////////////////////////////////////////////////////////////

///////////
// Setup //
///////////
void setup(){
  pinMode(VFDPWR,OUTPUT);
  pinMode(BLANK ,OUTPUT);
  pinMode(LOAD  ,OUTPUT);
  pinMode(CLK   ,OUTPUT);
  pinMode(DIN   ,OUTPUT);
  
  digitalWrite(VFDPWR, LOW); // Enable VFD-Module
  digitalWrite(BLANK , LOW); // Disable blank
  
  // Divide PWM frequency to prevent inductor from singing
  setPwmFrequency(BOOST, 8);
//  analogWrite(BOOST,128);
}

//////////
// Loop //
//////////
void loop(){
  // Set Boost-Value with Poti and display on Tube
  byte val = map(analogRead(A1),0,1023,0,255);
  analogWrite(BOOST,val);
  /*
  display_value[7] = val % 10;
  display_value[6] = (val/10) % 10;
  display_value[5] = val/100;
  */
  
  // Show Value on Tube
  multiplex();
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////
// display values on tube //
////////////////////////////
void multiplex(){
  uint8_t value = 0;
  boolean signs = LOW;
  
  if(current_digit == 9) current_digit = 0;
  
  if(current_digit == 8){
    if(dot) value = value | dot_bitmask;
    if(minus) value = value | minus_bitmask;
    signs = HIGH;
  }else{
    value = number_bitmask[display_value[current_digit]];
  }
  
  digitalWrite(LOAD, LOW);
  
  my_shiftOut(DIN, CLK, digit_bitmask[current_digit]);

  digitalWrite(DIN, signs);
  digitalWrite(CLK, LOW);
  digitalWrite(CLK, HIGH);
  
  digitalWrite(DIN, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(CLK, HIGH);
  
  digitalWrite(DIN, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(CLK, HIGH);
  
  digitalWrite(DIN, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(CLK, HIGH);
  
  my_shiftOut(DIN, CLK, value);

  digitalWrite(LOAD, HIGH);
  
  current_digit++;
}

//////////////
// Changed original shiftOut to use rising edge instead of falling edge
//////////////
void my_shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t val){
  uint8_t i;

  for(i = 0; i < 8; i++){
    digitalWrite(dataPin, !!(val & (1 << (7 - i))));
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
  }
}

//////////////
// Divide PWM Frequency
//////////////
// http://playground.arduino.cc/Code/PwmFrequency#.UySCqdt1uVc
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}