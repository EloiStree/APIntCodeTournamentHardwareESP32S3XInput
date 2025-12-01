
// My name is Eloi stree, this code is a wrapper of XInput.h
// Source XInput: https://github.com/dmadison/ArduinoXInput

// Note that I use Arduino but I am not medior or senior in the topic.
// This code works but is not guaranty to not bug fail or be not optimized.
// Feel free to ping me on Discord if you want to help me improve it.
// https://eloi.page.link/discord
// This code is under beerware license: If you like it, feel free to send me a beer or support my works.
// https://eloi.page.link/beer

#include <XInput.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <math.h>

//COMMENT THIS BEFORE BUILDING on XINPUT
#define EDITOR


const int maxCommandSize = 12;


bool m_targetXInput = false;

//To Tweakable value
bool m_debugModeSerial = false;      // Will send serial message to debug
bool m_wantToSendXboxReport = true;  //To put on false when you debug on none XInput board.

//Arduino Leonardo
const byte m_rxPin = 9; // RX/TX pin to the TTL or HC06
const byte m_txPin = 8; // RX/TX pin to the TTL or HC06

//Adruino MICRO PRO
//const byte m_rxPin = 9;                     // RX/TX pin to the TTL or HC06
//const byte m_txPin = 7;                     // RX/TX pin to the TTL or HC06
SoftwareSerial BTserial(m_rxPin, m_txPin);  // RX //TX
// Note that this code is not design to work without a TTL/HC06
// Note that you can't use classic serial port as it is used to simulate the x360.



bool m_useArrayDebug = false;

const int m_messageBufferMax = 240;
char m_messageBuffer[240];
//PRIVATE
const int JoyMax = 32767;
const int TriggerMax = 255;
// Some temp value used in the code.
bool m_up;
bool m_down;
bool m_right;
bool m_left;
float m_jlh = 0;
float m_jlv = 0;
float m_jrh = 0;
float m_jrv = 0;
bool m_newInputFound;
String m_receivedMessageSerial = "";
String m_receveivedToProcess = "";
String m_debugLine = "--------------------------------------------------------------------------------";
String m_joystickReport;




///////////////////////////////////::::


class CharArrayTarget {

public:
  char* m_array;
  int m_start;
  int m_end;
  int m_lenght;

  CharArrayTarget() {
    m_array = NULL;
    m_start = 0;
    m_end = 0;
    m_lenght = 0;
  }
  // Constructor
  CharArrayTarget(char* array, int start, int end) {
    m_array = array;
    m_start = start;
    m_end = end;
    m_lenght = end - start;
  }
  void set(char* array, int start, int end) {
    m_array = array;
    m_start = start;
    m_end = end;
    m_lenght = end - start;
  }
  int getAsInt(char c) {

    if (c == '0') return 0;
    if (c == '1') return 1;
    if (c == '2') return 2;
    if (c == '3') return 3;
    if (c == '4') return 4;
    if (c == '5') return 5;
    if (c == '6') return 6;
    if (c == '7') return 7;
    if (c == '8') return 8;
    if (c == '9') return 9;
    return 0;
  }

  int powdecimal(int pow) {
    int value = 1;
    if (pow == 0)
      return value;
    for (int i = pow; i > 0; i--)
      value *= 10;
    return value;
  }
  int getAsInt() {
    int value = 0;
    for (int i = 0; i < m_lenght; i++) {
      char c = GetChar(m_lenght - 1 - i);
      // Serial.print(i);
      // Serial.print(" ");
      //  Serial.print(c);
      //  Serial.print(" * ");
      //  Serial.println(String(powdecimal(i)));
      value += getAsInt(c) * powdecimal(i);
    }
    if ( indexofabsolute('-') > -1)
      value = -value;
    //  Serial.print(" R ");
    //  Serial.println(value);
    return value;
  }
  float getAsFloat() {
    int dota = indexofabsolute('.');
    if (dota == -1)
      dota = indexofabsolute(',');
    if (dota == -1)
      return getAsInt();
    int inverse = indexofabsolute('-') > -1;

    //%-21.02
    //4321012


    //Serial.print("\nTest: ");
    //for (int k = m_start; k<=m_end;k++)
    //  Serial.print(m_array[k] );
    //Serial.print("\n");

    double value = 0;
    int j = 0;
    for (int i = dota - 1; i >= m_start; i--) {
      char c = m_array[i];
      //Serial.print(i);
      //Serial.print(" ");
      //Serial.print(j);
      //Serial.print(" ");
      //Serial.print(c);
      //Serial.print(" * ");
      //Serial.println(String(powdecimal(j)));
      value += getAsInt(c) * powdecimal(j);
      j++;
    }
    j = 0;
    for (int i = dota + 1; i <= m_end; i++) {
      char c = m_array[i];
      //Serial.print(i);
      //Serial.print(" ");
      //Serial.print(j);
      //Serial.print(" ");
      //Serial.print(c);
      //Serial.print(" * ");
      //Serial.println(String(powdecimal(j)));
      value += ((double)getAsInt(c)) / ((double)powdecimal(j + 1));
      j++;
    }
    if (inverse)
      value = -value;
    //  Serial.print(" R ");
    //  Serial.println(value);
    return value;
  }


  void trim() {
    while ((m_array[m_start] == ' ' || m_array[m_start] == '\n') && m_start < m_end)
      m_start += 1;

    while ((m_array[m_end] == ' ' || m_array[m_end] == '\n') && m_end > m_start)
      m_end -= 1;

    computerLenght();
  }
  void replace(char toReplace, char by) {
    for (int i = 0; i < maxCommandSize; i++) {
      if (m_array[i] == toReplace)
        m_array[i] = by;
    }

    computerLenght();
  }

  bool endwith(char c) {

    return m_array[m_end] == c;
  }


  int indexofrelative(char c) {
    for (int i = 0; i < m_lenght; i++) {
      if (GetChar(i) == c)
        return i;
    }
    return -1;
  }
  int indexofabsolute(char c) {
    for (int i = m_start; i < m_start + m_lenght; i++) {
      if (m_array[i] == c)
        return i;
    }
    return -1;
  }

  bool startwith(String txt) {
    for (int i = 0; i < txt.length() && i < m_lenght; i++) {
      if (txt[i] != m_array[m_start + i]) return false;
    }
    return true;
  }
  bool startwith(char* txt) {
    for (int i = 0; i < strlen(txt) && i < m_lenght; i++) {
      if (txt[i] != m_array[m_start + i]) return false;
    }
    return true;
  }
  bool startwith(char c0) {

    return m_array[m_start] == c0;
  }
  bool startwith(char c0, char c1) {

    return m_array[m_start + 0] == c0 && m_array[m_start + 1] == c1;
    ;
  }
  bool startwith(char c0, char c1, char c2) {

    return m_array[m_start + 0] == c0 && m_array[m_start + 1] == c1 && m_array[m_start + 2] == c2;
  }
  bool startwith(char c0, char c1, char c2, char c3) {

    return m_array[m_start + 0] == c0 && m_array[m_start + 1] == c1 && m_array[m_start + 2] == c2 && m_array[m_start + 3] == c3;
  }

 bool equals(String txt) {
    if(txt.length() != getArrayLength())
      return false;
    for (int i = 0; i < txt.length() && i < m_lenght; i++) {
      if (txt[i] != m_array[m_start + i]) return false;
    }
    return true;
  }
  bool equals(char* txt) {
    if(strlen(txt) != getArrayLength())
      return false;
    for (int i = 0; i < strlen(txt) && i < m_lenght; i++) {
      if (txt[i] != m_array[m_start + i]) return false;
    }
    return true;
  }
 bool equals(char c0) {
    if(getArrayLength()!=1)
      return false;
    return m_array[m_start] == c0;
  }
  bool equals(char c0, char c1) {

    if(getArrayLength()!=2)
      return false;
    return m_array[m_start + 0] == c0 && m_array[m_start + 1] == c1;
    ;
  }
  bool equals(char c0, char c1, char c2) {
    if(getArrayLength()!=3)
      return false;
    return m_array[m_start + 0] == c0 && m_array[m_start + 1] == c1 && m_array[m_start + 2] == c2;
  }
  bool equals(char c0, char c1, char c2, char c3) {

    if(getArrayLength()!=4)
      return false;
    return m_array[m_start + 0] == c0 && m_array[m_start + 1] == c1 && m_array[m_start + 2] == c2 && m_array[m_start + 3] == c3;
  }

  void removelastchar() {
    m_end -= 1;
    computerLenght();
  }
  void removefirstchar() {
    m_start += 1;
    computerLenght();
  }
  void computerLenght() {

    m_lenght = (m_end - m_start) + 1;
  }

  char* getArrayStart() {
    return m_array[m_start];
  }
  char* getArrayEnd() {
    return m_array[m_end];
  }
  int getArrayLength() {
    return m_lenght;
  }



  char operator[](int index) {
    if (index >= 0 && index < m_lenght) {
      return m_array[m_start + index];
    } else {
      //Serial.println("Index out of bounds");
      return '\0';
    }
  }
  char GetChar(int index) {
    if (index >= 0 && index < m_lenght) {
      return m_array[m_start + index];
    } else {
      //Serial.println("Index out of bounds");
      return '\0';
    }
  }
  void SetChar(int index, char c) {
    if (index >= 0 && index < m_lenght)
      m_array[m_start + index] = c;
  }




  // Get the character at the specified index
  char getCharAtIndex(int index) {
    if (index >= 0 && index < m_array) {
      return m_array[m_start + index];
    } else {
      // Handle out-of-bounds access gracefully
      //BTSerial.println("Index out of bounds");
      // You can return a default value or handle the error as needed
      // Here, we return the null character
      return '\0';
    }
  }
};


class DelayMessage {
public:
  DelayMessage() {
    flush();
    m_timestamp = millis();
    m_hadBedRelay = false;
  }

  void set_command_at(CharArrayTarget command, unsigned long timestamp) {

    for (int i = 0; i < maxCommandSize; i++) {
      if (i < command.getArrayLength())
        m_command[i] = command[i];
      else
        m_command[i] = ' ';
    }
    m_timestamp = timestamp;
    m_hadBedRelay = false;
  }

  void set_command(String command, unsigned long millisecond) {
    for (int i = 0; i < maxCommandSize; i++) {
      if (i < command.length())
        m_command[i] = command[i];
      else
        m_command[i] = ' ';
    }
    m_timestamp = millis() + millisecond;
    m_hadBedRelay = false;
  }

  void set_command_at(String command, unsigned long timestamp) {

    for (int i = 0; i < maxCommandSize; i++) {
      if (i < command.length())
        m_command[i] = command[i];
      else
        m_command[i] = ' ';
    }
    m_timestamp = timestamp;
    m_hadBedRelay = false;
  }

  void set_as_relayed() {
    m_hadBedRelay = true;
  }
  bool had_been_relay() {
    return m_hadBedRelay;
  }


  bool is_time_reach() {
    return millis() >= m_timestamp;
  }

  void flush() {
    for (int i = 0; i < maxCommandSize; i++) {
      m_command[i] = ' ';
    }
  }

  char m_command[14];
  unsigned long m_timestamp;
  bool m_hadBedRelay;
};

const int maxIndex = 40;
DelayMessage delayArray[maxIndex];






///////////////////////////////:



void DisplayArray() {

  if (m_useArrayDebug == false)
    return;
  LogString("------------START-------------");
  LogString("Index, is dealed, is reached, command, timestamp");
  for (int i = 0; i < maxIndex; i++) {

    LogPrintString("\n");
    LogPrintString(String(i));
    LogPrintString(" - ");
    LogPrintString(String(delayArray[i].had_been_relay()));
    LogPrintString(" - ");
    LogPrintString(String(delayArray[i].is_time_reach()));
    LogPrintString(" - ");
    for (int j = 0; j < maxCommandSize; j++)
      LogChar(delayArray[i].m_command[j]);
    LogPrintString(" - ");
    LogPrintString(String(delayArray[i].m_timestamp));
  }
  BTserial.println("-----------END--------------");
}



/////////////////////////////////////////////////////////////////////////////////////:



bool usePrintLog = true;

void add_delay_command(String shortcut, unsigned long msToAdd) {
  for (int i = 0; i < maxIndex; i++) {
    if (delayArray[i].had_been_relay()) {
      delayArray[i].set_command(shortcut, msToAdd);
      return;
    }
  }
}

void add_time_command(String shortcut, unsigned long timestamp) {
  for (int i = 0; i < maxIndex; i++) {
    if (delayArray[i].had_been_relay()) {
      delayArray[i].set_command_at(shortcut, timestamp);
      return;
    }
  }
}
void add_time_command(CharArrayTarget shortcut, unsigned long timestamp) {
  for (int i = 0; i < maxIndex; i++) {
    if (delayArray[i].had_been_relay()) {
      delayArray[i].set_command_at(shortcut, timestamp);
      return;
    }
  }
}

/////////////////////
int charToInt(char c) {
  switch (c) {
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case '0':
      return 0;
    default:
      return -1;
  }
}

/////////////////////





//////////////////////////////////////:::














// Some function

bool StartBy4(String text, char c1, char c2, char c3, char c4) {

  return text[0] == c1 && text[1] == c2 && text[2] == c3 && text[3] == c4;
}
bool StartBy3(String text, char c1, char c2, char c3) {

  return text[0] == c1 && text[1] == c2 && text[2] == c3;
}
bool StartBy2(String text, char c1, char c2) {

  return text[0] == c1 && text[1] == c2;
}
bool EndWith(String text, char c) {

  return text[text.length() - 1] == c;
}

float GetFloatBehindPercent(String text) {
  int start = text.indexOf('%') + 1;
  int end = text.length();
  String integerValue = text.substring(start, end);
  return integerValue.toFloat();
}

CharArrayTarget behindPercentZone = CharArrayTarget();
float GetFloatBehindPercent(CharArrayTarget text) {
  int start = text.indexofabsolute('%') + 1;
  //LogString("% start "+ String(start));
  if (start == -1) return 0;
  behindPercentZone.set(text.m_array, start, text.m_end);
  behindPercentZone.trim();
  return behindPercentZone.getAsFloat();
}


String GetDebugLineReport() {
  m_debugLine[0] = 'l';
  m_debugLine[2] = XInput.getButton(BUTTON_LOGO) ? '1' : '0';
  m_debugLine[3] = ' ';
  m_debugLine[4] = 'a';
  m_debugLine[5] = XInput.getDpad(DPAD_UP) ? '1' : '0';
  m_debugLine[6] = XInput.getDpad(DPAD_DOWN) ? '1' : '0';
  m_debugLine[7] = XInput.getDpad(DPAD_LEFT) ? '1' : '0';
  m_debugLine[8] = XInput.getDpad(DPAD_RIGHT) ? '1' : '0';
  m_debugLine[9] = ' ';
  m_debugLine[10] = 'b';
  m_debugLine[12] = XInput.getButton(BUTTON_A) ? '1' : '0';
  m_debugLine[13] = XInput.getButton(BUTTON_B) ? '1' : '0';
  m_debugLine[14] = XInput.getButton(BUTTON_X) ? '1' : '0';
  m_debugLine[15] = XInput.getButton(BUTTON_Y) ? '1' : '0';
  m_debugLine[16] = ' ';
  m_debugLine[17] = 's';
  m_debugLine[18] = XInput.getButton(BUTTON_LB) ? '1' : '0';
  m_debugLine[19] = XInput.getButton(BUTTON_RB) ? '1' : '0';
  m_debugLine[20] = ' ';
  m_debugLine[22] = 'm';
  m_debugLine[23] = XInput.getButton(BUTTON_BACK) ? '1' : '0';
  m_debugLine[24] = XInput.getButton(BUTTON_START) ? '1' : '0';
  m_debugLine[25] = ' ';
  m_debugLine[26] = 't';
  m_debugLine[27] = XInput.getTrigger(TRIGGER_LEFT) ? '1' : '0';
  m_debugLine[28] = XInput.getTrigger(TRIGGER_RIGHT) ? '1' : '0';
  m_debugLine[29] = ' ';
  m_debugLine[30] = 'j';
  m_debugLine[31] = XInput.getButton(BUTTON_L3) ? '1' : '0';
  m_debugLine[32] = XInput.getButton(BUTTON_R3) ? '1' : '0';
  return m_debugLine;
}
String GetDebugJoystickLineReport() {

  m_joystickReport = "";
  m_joystickReport += " JLH " + String(XInput.getJoystickX(JOY_LEFT));
  m_joystickReport += " JLV " + String(XInput.getJoystickY(JOY_LEFT));
  m_joystickReport += " JRH " + String(XInput.getJoystickX(JOY_RIGHT));
  m_joystickReport += " JRV " + String(XInput.getJoystickY(JOY_RIGHT));
  m_joystickReport += " TL " + String(XInput.getTrigger(TRIGGER_LEFT));
  m_joystickReport += " TR " + String(XInput.getTrigger(TRIGGER_RIGHT));
  return m_joystickReport;
}


String toLowerCase(String str) {
  String lowerCaseStr = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (c >= 'A' && c <= 'Z') {
      c = c + 32;  // Convert uppercase to lowercase
    }
    lowerCaseStr += c;
  }
  return lowerCaseStr;
}

String trim(String str) {
  int startIdx = 0;
  int endIdx = str.length() - 1;
  while (startIdx <= endIdx && str.charAt(startIdx) == ' ') {
    startIdx++;
  }
  while (endIdx >= startIdx && str.charAt(endIdx) == ' ') {
    endIdx--;
  }
  return str.substring(startIdx, endIdx + 1);
}







const int ADC_Max = 1023;  // 10 bit


void OverrideBuffer() {
  for (int i = 0; i < m_messageBufferMax; i++) {
    m_messageBuffer[i] = ';';
  }
}

int GetIndexofLineEnd() {
  for (int i = 0; i < m_messageBufferMax; i++) {
    if (m_messageBuffer[i] == ';')
      return i;
  }
  return -1;
}

void FlushBuffer(char c) {

  for (int i = 0; i < m_messageBufferMax; i++) {

    m_messageBuffer[i] = 'c';
  }
}
void FlushBuffer() {

  for (int i = 0; i < m_messageBufferMax; i++) {

    m_messageBuffer[i] = ';';
  }
}

void SerialDebugDebuff() {

  if (m_debugModeSerial == false)
    return;
  for (int i = 0; i < m_messageBufferMax; i++) {

    Serial.print(m_messageBuffer[i]);
    BTserial.print(m_messageBuffer[i]);
  }
  Serial.println();
  BTserial.println();
}
void Log(CharArrayTarget target, bool useLn) {

  if (m_debugModeSerial == false)
    return;
  for (int i = 0; i < target.getArrayLength(); i++) {

    Serial.print(target[i]);
    BTserial.print(target[i]);
  }
  if (useLn) {
    Serial.println();
    BTserial.println();
  }
}

// At start of the Arduino, we check that the serial is connected.
void setup() {

  XInput.setTriggerRange(0, ADC_Max);
  XInput.setJoystickRange(0, ADC_Max);
  BTserial.begin(9600);
  Serial.begin(9600);
  while (!Serial) { ; }
  //while (!BTserial) { ; }
  delay(500);
  BTserial.println("Serial available !");
  delay(500);
  XInput.setAutoSend(false);
  XInput.begin();
  FlushBuffer();
  SerialDebugDebuff();
  for (int i = 0; i < maxIndex; i++) {

    delayArray[i].set_as_relayed();
  }

  SerialDebugDebuff();
  //CharArrayTarget chunkTarget = CharArrayTarget(m_messageBuffer, 0,3);
  //add_time_command(chunkTarget, millis()+2000);
  //Log(chunkTarget);

  DisplayArray();
}

// void ExecutionLog(String cmd){

//             BTserial.print("Exe|");
//             BTserial.print(cmd);
//             BTserial.print("|");
//             BTserial.println((millis()));
// }
// void ExecutionLog(DelayMessage cmd){

//             BTserial.print("Exe|");
//             BTserial.print(cmd.m_command);
//             BTserial.print("|");
//             BTserial.print((millis()));
//             BTserial.print(" - ");
//             BTserial.println((cmd.m_timestamp));
// }




void Replace(CharArrayTarget message, char toReplace, char by) {
  for (int i = 0; i < message.getArrayLength(); i++) {
    if (message.GetChar(i) == toReplace)
      message.SetChar(i, by);
  }
}

void ToLower(CharArrayTarget message) {
  for (int i = 0; i < message.getArrayLength(); i++) {
    if (message.GetChar(i) == 'A') message.SetChar(i, 'a');
    else if (message.GetChar(i) == 'B') message.SetChar(i, 'b');
    else if (message.GetChar(i) == 'C') message.SetChar(i, 'c');
    else if (message.GetChar(i) == 'D') message.SetChar(i, 'd');
    else if (message.GetChar(i) == 'E') message.SetChar(i, 'e');
    else if (message.GetChar(i) == 'F') message.SetChar(i, 'f');
    else if (message.GetChar(i) == 'G') message.SetChar(i, 'g');
    else if (message.GetChar(i) == 'H') message.SetChar(i, 'h');
    else if (message.GetChar(i) == 'I') message.SetChar(i, 'i');
    else if (message.GetChar(i) == 'J') message.SetChar(i, 'j');
    else if (message.GetChar(i) == 'K') message.SetChar(i, 'k');
    else if (message.GetChar(i) == 'L') message.SetChar(i, 'l');
    else if (message.GetChar(i) == 'M') message.SetChar(i, 'm');
    else if (message.GetChar(i) == 'N') message.SetChar(i, 'n');
    else if (message.GetChar(i) == 'O') message.SetChar(i, 'o');
    else if (message.GetChar(i) == 'P') message.SetChar(i, 'p');
    else if (message.GetChar(i) == 'Q') message.SetChar(i, 'q');
    else if (message.GetChar(i) == 'R') message.SetChar(i, 'r');
    else if (message.GetChar(i) == 'S') message.SetChar(i, 's');
    else if (message.GetChar(i) == 'T') message.SetChar(i, 't');
    else if (message.GetChar(i) == 'U') message.SetChar(i, 'u');
    else if (message.GetChar(i) == 'V') message.SetChar(i, 'v');
    else if (message.GetChar(i) == 'W') message.SetChar(i, 'w');
    else if (message.GetChar(i) == 'X') message.SetChar(i, 'x');
    else if (message.GetChar(i) == 'Y') message.SetChar(i, 'y');
    else if (message.GetChar(i) == 'Z') message.SetChar(i, 'z');
  }
}

void LogString(String serial) {
  if (m_debugModeSerial == false)
    return;
  BTserial.println(serial);
  Serial.println(serial);
}
void LogPrintString(String serial) {
  if (m_debugModeSerial == false)
    return;
  BTserial.print(serial);
  Serial.print(serial);
}
void LogChar(char serial) {
  if (m_debugModeSerial == false)
    return;
  BTserial.print(serial);
  Serial.print(serial);
}
void Log(char* arrayChar, bool useLn) {

  if (m_debugModeSerial == false)
    return;

  for (int i = 0; i < strlen(arrayChar); i++) {
    BTserial.print(arrayChar[i]);
    Serial.print(arrayChar[i]);
  }
  if (useLn) {
    BTserial.println();
    Serial.println();
  }
}


bool Contains(char* command, char c) {
  for (int i = 0; i < maxCommandSize; i++) {
    if (command[i] == c)
      return true;
  }
  return false;
}

void Log(CharArrayTarget toLog, char limiter) {
  if (m_debugModeSerial == false)
    return;
  Log("Log:", false);
  LogChar(limiter);
  for (int i = 0; i < toLog.getArrayLength(); i++) {
    LogChar(toLog.GetChar(i));
  }
  LogChar(limiter);
  LogChar('\n');
  DisplayArray();
}
void Log(CharArrayTarget toLog, char limiter, String debug) {


  if (m_debugModeSerial == false)
    return;
  Log("Log:", false);
  LogChar(limiter);
  for (int i = 0; i < toLog.getArrayLength(); i++) {
    LogChar(toLog.GetChar(i));
  }
  LogChar(limiter);
  LogPrintString(debug);
  LogChar('\n');
  DisplayArray();
}

void R(){
  randomSeed(analogRead(0));
}
float RP(){
  return random(0, 100)/100.0;
}

CharArrayTarget cmd = CharArrayTarget();
void ExecuteWaitingCommand(char* command) {
  cmd.set(command, 0, strlen(command) - 1);
  cmd.trim();


  if (cmd.getArrayLength() == 0)
    return;
  if (cmd.GetChar(0) == ';')
    return;
  if (cmd.GetChar(0) == ' ')
    return;

  if (m_debugModeSerial) {

    Log("Exel:", false);
    LogChar('#');
    for (int i = 0; i < cmd.getArrayLength(); i++) {
      LogChar(cmd.GetChar(i));
    }
    LogChar('#');
    LogChar('\n');
    DisplayArray();
  }


  if (cmd.indexofrelative('%') > -1) {

    float value = GetFloatBehindPercent(cmd);
    R();
    if (m_debugModeSerial)
      Log(cmd, '?', "VALUE:" + String(value));
    if (cmd.startwith('r')) {
           if (cmd.equals('r','j', 'l', 'h')) {  R(); m_jlh = RP(); }
      else if (cmd.equals('r','j', 'l', 'v')) {  R(); m_jlv = RP(); }
      else if (cmd.equals('r','j', 'r', 'h')) {  R(); m_jrh = RP(); }
      else if (cmd.equals('r','j', 'r', 'v')) {  R(); m_jrv = RP(); }
      
      else if (cmd.equals('r','j', 'l')) {  R();m_jlh = RP();  R(); m_jlv = RP(); }
      else if (cmd.equals('r','j', 'r')) {  R();m_jrh = RP();  R(); m_jrv = RP(); }
      else if (cmd.equals('r','j')) { 
      R(); m_jlh = RP();
      R(); m_jlv = RP();
      R(); m_jrh = RP();
      R(); m_jrv = RP();
        }

    }
    if (cmd.startwith('j', 'l', 'h', '%')) {
      m_jlh = (value+1.0)/2.0;
      if (m_debugModeSerial) Log(cmd, '!', "\njlh " + String(m_jlh) + " " + String(m_jlv));
    }
    if (cmd.startwith('j', 'l', 'v', '%')) {
      m_jlv = (value+1.0)/2.0;
      if (m_debugModeSerial) Log(cmd, '!', "\njlv " + String(m_jlh) + " " + String(m_jlv));
    }
    if (cmd.startwith('j', 'r', 'h', '%')) {
      m_jrh = (value+1.0)/2.0;
      if (m_debugModeSerial) Log(cmd, '!', "\njrh " + String(m_jrh) + " " + String(m_jrv));
    }
    if (cmd.startwith('j', 'r', 'v', '%')) {
      m_jrv = (value+1.0)/2.0;
      if (m_debugModeSerial) Log(cmd, '!', "\njrv " + String(m_jrh) + " " + String(m_jrv));
    }

      XInput.setJoystick(JOY_LEFT,(m_jlh * (float)ADC_Max), (m_jlv * (float)ADC_Max));
      XInput.setJoystick(JOY_RIGHT,( m_jrh * (float)ADC_Max),( m_jrv * (float)ADC_Max));
    if (cmd.startwith('t', 'l', '%')) {

      if (m_debugModeSerial) Log(cmd, '!', "\ntl " + String(value));
      //XInput.setTrigger(TRIGGER_LEFT, value );
      XInput.setTrigger(TRIGGER_LEFT,( value * (float)ADC_Max));
    }
    if (cmd.startwith('t', 'r', '%')) {

      if (m_debugModeSerial) Log(cmd, '!', "\ntr " + String(value));
      //XInput.setTrigger(TRIGGER_RIGHT, value );
      XInput.setTrigger(TRIGGER_RIGHT,( value * (float)ADC_Max));
    }
    //100> jlh%1 5000> jlh%-1 5000> jlh%0 ;
    //100> tl%1 5000> tl%-1 5000> tl%0 ;
      if (m_wantToSendXboxReport)
        XInput.send();
  }

  else {
    unsigned long t = millis();  // Get timestamp for comparison
    bool release = true;
    cmd.endwith('\'');
    bool press = true;
    if (cmd.endwith('.')) {
      release = false;
    }
    if (cmd.endwith('\'')) {
      press = false;
    }
    uint8_t button = -1;

    cmd.replace('.', ' ');
    cmd.replace('\'', ' ');
    cmd.trim();

    if (m_debugModeSerial)
      Log(cmd, '!');
    bool dpadChange=false;
    if (cmd.equals('a') || cmd.equals('b', 'd')) {
      button = (BUTTON_A);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_A");
    } else if (cmd.equals('x') || cmd.equals('b', 'l')) {
      button = (BUTTON_X);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_X");
    } else if (cmd.equals('b') || cmd.equals('b', 'r')) {
      button = (BUTTON_B);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_B");
    } else if (cmd.equals('y') || cmd.equals('b', 'u')) {
      button = (BUTTON_Y);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_Y");
    } else if (cmd.equals("start") || cmd.equals('m', 'r')) {
      button = (BUTTON_START);
      if (m_debugModeSerial) Log(cmd, '!', "start");
    } else if (cmd.equals("back") || cmd.equals('m', 'l')) {
      button = (BUTTON_BACK);
      if (m_debugModeSerial) Log(cmd, '!', "back");
    } else if (cmd.equals("xbox") || cmd.equals("logo") || cmd.equals('m', 'c')) {
      button = (BUTTON_LOGO);
      if (m_debugModeSerial) Log(cmd, '!', "xbox");
    }

    else if (cmd.equals("debug on")) {
      m_debugModeSerial = true;
      if (m_debugModeSerial) Log(cmd, '!', "Debug on");
    } else if (cmd.equals("debug off")) {
      m_debugModeSerial = false;
      if (m_debugModeSerial) Log(cmd, '!', "Debug off");
    } else if (cmd.equals("debug")) {
      m_debugModeSerial = !m_debugModeSerial;
      if (m_debugModeSerial) Log(cmd, '!', "Switch debug");
    } else if (cmd.equals('j', 'l') || cmd.equals('l', '3')) {
      button = (BUTTON_L3);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_L3");
    } else if (cmd.equals('j', 'r') || cmd.equals('r', '3')) {
      button = (BUTTON_R3);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_R3");
    }

    else if (cmd.equals('t', 'l') || cmd.equals('l', '2')) {
      button = (TRIGGER_LEFT);
      if (m_debugModeSerial) Log(cmd, '!', "TRIGGER_LEFT");
    } else if (cmd.equals('t', 'r') || cmd.equals('r', '2')) {
      button = (TRIGGER_RIGHT);
      if (m_debugModeSerial) Log(cmd, '!', "TRIGGER_RIGHT");
    }

    else if (cmd.equals('s', 'b', 'l') || cmd.equals('l', '1')) {
      button = (BUTTON_LB);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_LB");
    } else if (cmd.equals('s', 'b', 'r') || cmd.equals('r', '1')) {
      button = (BUTTON_RB);
      if (m_debugModeSerial) Log(cmd, '!', "BUTTON_RB");
    }

    else if (cmd.equals('d') || cmd.equals('a', 'd')) {
      if (press) m_down = true;
      if (release) m_down = false;
      dpadChange=true;
      if (m_debugModeSerial) Log(cmd, '!', "Arrow Down");

    } else if (cmd.equals('l') || cmd.equals('a', 'l')) {
      if (press) m_left = true;
      if (release) m_left = false;
      dpadChange=true;
      if (m_debugModeSerial) Log(cmd, '!', "Arrow Left");
    } else if (cmd.equals('r') || cmd.equals('a', 'r')) {
      if (press) m_right = true;
      if (release) m_right = false;
      dpadChange=true;
      if (m_debugModeSerial) Log(cmd, '!', "Arrow Right");
    } else if (cmd.equals('u') || cmd.equals('a', 'u')) {
      if (press) m_up = true;
      if (release) m_up = false;
      dpadChange=true;
      if (m_debugModeSerial) Log(cmd, '!', "Arrow Up");
    } else if (cmd.equals("release")) {
      XInput.releaseAll();
      dpadChange=true;
      if (m_debugModeSerial)
        Log(cmd, '!', "Release all");
    }

  
  if( dpadChange)
    XInput.setDpad(m_up, m_down, m_left, m_right,false);



    bool sent = false;
    if (press && button > -1) {
      XInput.press(button);
      if (m_wantToSendXboxReport)
        XInput.send();
      sent = true;
    }
    if (release && button > -1) {
      XInput.release(button);
      if (m_wantToSendXboxReport)
        XInput.send();
      sent = true;
    }
    if (!sent)
      XInput.send();


    // if (m_debugModeSerial)
    //    Log(cmd, '!',String(button) + " " + String(press) + " " + String(release));

    // if (m_debugModeSerial) BTserial.print("\nXinput: " + String(button) + " " + String(press) + " " + String(release));


    //ExecutionLog(message);
    if (m_debugModeSerial) {
      BTserial.println(GetDebugLineReport());
      BTserial.println(GetDebugJoystickLineReport());
    }
  }
}

void CheckForExecutable() {
  for (int i = 0; i < maxIndex; i++) {
    if (!delayArray[i].had_been_relay()) {
      if (delayArray[i].is_time_reach()) {
        delayArray[i].set_as_relayed();

        if (m_debugModeSerial)
          Log(delayArray[i].m_command, true);
        ExecuteWaitingCommand(delayArray[i].m_command);
      }
    }
  }
}

void ExecuteCommand(CharArrayTarget mArray) {

  //LogString("> MessageA:");
  //Log(mArray);
  //Replace(mArray,';', ' ');
  Replace(mArray, '\n', ';');
  Replace(mArray, '\r', ';');
  ToLower(mArray);

  if (m_debugModeSerial)
    Log(mArray, false);
  //LogString("Time Clock:"+ String(millis()));

  if (m_debugModeSerial)
    Log(m_messageBuffer, false);

  unsigned long tstart = millis();
  unsigned long msToAdd = 0;

  int start = 0;
  int end = 0;
  CharArrayTarget shortcut = CharArrayTarget(m_messageBufferMax, 0, 0);
  for (int i = 0; i < m_messageBufferMax; i++) {
    start = end;
    if (m_messageBuffer[i] == ' ' || m_messageBuffer[i] == ';') {
      end = i;
      shortcut.set(m_messageBuffer, start, end);
      shortcut.trim();
      if (shortcut.endwith('|')) {
        shortcut.removelastchar();
        int t = shortcut.getAsInt();
        msToAdd = t;
        //LogString("AT "+String(t));
      } else if (shortcut.endwith('>')) {
        shortcut.removelastchar();
        int t = shortcut.getAsInt();
        msToAdd += t;
        //LogString("DELAY "+ String(t));
      } else if (shortcut.endwith('\'')) {
        //LogString("ShortCut release");
        //Log(shortcut);
        add_time_command(shortcut, tstart + msToAdd);
      } else if (shortcut.endwith('.')) {
        //LogString("ShortCut press");
        //Log(shortcut);
        add_time_command(shortcut, tstart + msToAdd);
      } else {
        //LogPrintString("\nShortCut Not managed:");
        //Log(shortcut, false);
        add_time_command(shortcut, tstart + msToAdd);
      }
      // LogString("MS "+ String(msToAdd)+ " Milli "+ String(tstart+msToAdd)+ " Current "+ String(millis()));

      if (m_messageBuffer[i] == ';')
        break;
    }
  }

  DisplayArray();
  CheckForExecutable();

  return;
}



int serialIndex = 0;
CharArrayTarget readSerialArray = CharArrayTarget();

void loop() {



  while (BTserial.available() > 0) {

    char c = BTserial.read();
    m_messageBuffer[serialIndex] = c;
    serialIndex++;
    if (c == '\n' || c == ';') {
      if (m_debugModeSerial)
        Log(m_messageBuffer, true);
      // if (m_debugModeSerial) {
      //   LogString("\nArduino Recieved:\n"+m_receivedMessageSerial);
      // }
      readSerialArray.set(m_messageBuffer, 0, serialIndex);
      ExecuteCommand(readSerialArray);

      DisplayArray();
      m_receveivedToProcess = "";
      m_receivedMessageSerial = "";
      serialIndex = 0;
    }
  }
#ifdef EDITOR
  while (Serial.available() > 0) {

    char c = Serial.read();
    m_messageBuffer[serialIndex] = c;
    serialIndex++;
    if (c == '\n' || c == ';') {

      if (m_debugModeSerial)
        Log(m_messageBuffer, true);
      // if (m_debugModeSerial) {
      //   LogString("\nArduino Recieved:\n"+m_receivedMessageSerial);
      // }
      readSerialArray.set(m_messageBuffer, 0, serialIndex);
      ExecuteCommand(readSerialArray);

      DisplayArray();
      m_receveivedToProcess = "";
      m_receivedMessageSerial = "";
      serialIndex = 0;
    }
  }
#endif

  CheckForExecutable();
}
