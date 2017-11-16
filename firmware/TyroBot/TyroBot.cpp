/* TyroBot v1.0.0
 *  copyright (c) 2017 Tyro Electronics
 *  written by Tyler Spadgenske
 *  GNU General Public License 3.0
 */

#include "Arduino.h"
#include "TyroBot.h"

TyroBot::TyroBot(Adafruit_ILI9341 *tft, Adafruit_LIS3DH *lis)
{
  _tft = tft;
  _lis = lis;

  int mode = 1;
  //battery variables
  float voltage = 0;
  int batMode = 3;
  long theTime = 0;
  long timeMultiple = 1;

  //self program variables
  uint32_t colors[5] = {ILI9341_ORANGE, ILI9341_ORANGE, ILI9341_ORANGE, ILI9341_ORANGE, ILI9341_RED};
  int program[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int key = 0;

  //touchscreen
  x = 0;
  y = 0;
  xVal = 0;
  yVal = 0;
  TS_MINX = 700;
  TS_MINY = 805;
  TS_MAXX = 218;
  TS_MAXY = 155;

  //Servo Pins
  LEFT_LEG = 5;
  LEFT_FOOT = 11;
  RIGHT_LEG = 13;
  RIGHT_FOOT = 10;
  HEAD = 2;
  RIGHT_ARM = 12;
  LEFT_ARM = 6;

  //Step Constants
  LIFT_HEIGHT = 6;//Decrease if steps are too big and the robot is falling over. Increase of the robot is dragging his feet
  STEP_DISTANCE = 25; //Decrease if robot is tipping over or not walking straight increase of robot is taking too tiny steps
  STEP_SPEED = 250; //in milliseconds

  LEFT_TRIM = 50; //If the robot is walking to the left decrease this number in multiples of 5
  RIGHT_TRIM = 140; //If the robot is walking to the right decrease this number in multiples of 5

  //Servo Positions when robot is at rest
  LL_CENTER = 90;
  LF_CENTER = 80;
  RL_CENTER = 110;
  RF_CENTER = 70;
  HEAD_CENTER = 50;
  RIGHT_ARM_CENTER = 90;
  LEFT_ARM_CENTER = 90;

  //Setup Servos
   Servo ll;
   Servo lf;
   Servo rl;
   Servo rf;
   Servo head;
   Servo leftArm;
   Servo rightArm;

  cali = true;

  //Accelerometer
  maxX = 1500;
  minX = -1000;
  level = false;

  //wifi
  char ip[50];
  char rssi[50];
  bool connection = false;

  //IFTT
  char token;
}

void TyroBot::settings() {
  SerialUSB.println("settings");
  _tft->begin();
  _tft->setRotation(1);
  _tft->fillScreen(ILI9341_BLACK);
  //draw text
  _tft->setCursor(10, 10);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(2);
  _tft->println("Settings");
  _tft->drawLine(0, 35, 320, 35, ILI9341_WHITE);
  _tft->setCursor(10, 50);
  _tft->println("IP:");
  _tft->setCursor(55, 50);
  _tft->println(ip);
  _tft->drawLine(0, 75, 320, 75, ILI9341_WHITE);
  _tft->setCursor(10, 90);
  _tft->println("dbm:");
  _tft->setCursor(70, 90);
  _tft->println(rssi);
  _tft->drawLine(0, 115, 320, 115, ILI9341_WHITE);
  _tft->setCursor(10, 130);
  _tft->println("Firmware:");
  _tft->setCursor(140, 130);
  _tft->println("v1.0.0");
  _tft->drawLine(0, 160, 320, 160, ILI9341_WHITE);
  while (1) {
    if (checkForPress() == true) {
      TyroBotMenu();
      return;
    }
  }
}

void TyroBot::connectWifi(char ssid[], char pass[]) {
  _tft->begin();
  _tft->setRotation(1);
  _tft->fillScreen(ILI9341_BLACK);
  //draw text
  _tft->setCursor(5, 5);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(1);
  _tft->println("Starting...");
  SerialUSB.begin(9600); //Open up the serial port for the console
  SerialUSB.println("TyroBot");
  delay(1000);
  Serial.begin(9600); //open up the serial port for the wifi module
  Serial.write(1);
  Serial.write(ssid);
  Serial.write(",");
  Serial.write(pass);
  while (1) {
    if (Serial.available()) {
      delay(100); //allows all serial sent to be received together
      String credentials = Serial.readString();
      if (credentials[0] == 'N') {
        SerialUSB.print("Could not connect to ");
        SerialUSB.println(ssid);
        connection = false;
        rssi[1] = '0';
        ip[0] = 'N';
        ip[1] = 'o';
        ip[2] = ' ';
        ip[3] = 'c';
        ip[4] = 'o';
        ip[5] = 'n';
        ip[6] = 'n';
        ip[7] = 'e';
        ip[8] = 'c';
        ip[9] = 't';
        ip[10] = 'i';
        ip[11] = 'o';
        ip[12] = 'n';
        break;
      }
      else {
        int ind1;
        String rssiStr;
        String ipStr;
        ind1 = credentials.indexOf(',');
        rssiStr = credentials.substring(0, ind1);
        ipStr = credentials.substring(ind1 + 1);
        ipStr.toCharArray(ip, 50);
        rssiStr.toCharArray(rssi, 50);
        SerialUSB.print("IP:");
        SerialUSB.println(ip);
        SerialUSB.print("RSSI:");
        SerialUSB.println(rssi);
        connection = true;
        pinMode(8, OUTPUT);
        digitalWrite(8,1);
        break;
      }
    }
  }
}
void TyroBot::face() {
  _tft->begin();
  _tft->setRotation(2);
  _tft->fillScreen(ILI9341_BLACK);
  _tft->fillCircle(100, 60, 40, ILI9341_ORANGE);
  _tft->fillCircle(100, 250, 40, ILI9341_ORANGE);
}

void TyroBot::TyroBotMenu() {
  _tft->begin();
  _tft->setRotation(1);

  //draw outline boxes
  _tft->fillScreen(ILI9341_BLACK);
  _tft->fillRect(0, 0, 160, 155, ILI9341_BLUE);
  _tft->fillRect(160, 0, 160, 155, ILI9341_RED);
  _tft->fillRect(240, 155, 80, 85, ILI9341_DARKGREY);
  _tft->fillRect(0, 155, 80, 85, ILI9341_WHITE);

  //draw text
  _tft->setCursor(27, 25);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(6);
  _tft->println("</>");
  _tft->setCursor(15, 100);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(2);
  _tft->println("Run Program");

  //draw arrows
  _tft->fillRect(232, 20, 16, 40, ILI9341_WHITE);
  _tft->fillRect(232, 90, 16, 40, ILI9341_WHITE);
  _tft->fillRect(185, 68, 40, 16, ILI9341_WHITE);
  _tft->fillRect(254, 68, 40, 16, ILI9341_WHITE);
  _tft->fillTriangle(240, 12, 256, 28, 224, 28, ILI9341_WHITE);
  _tft->fillTriangle(240, 140, 256, 124, 224, 124, ILI9341_WHITE);
  _tft->fillTriangle(176, 75, 192, 91, 192, 59, ILI9341_WHITE);
  _tft->fillTriangle(302, 75, 286, 91, 286, 59, ILI9341_WHITE);

  //draw settings
  _tft->fillRect(254, 173, 54, 4, ILI9341_BLACK);
  _tft->fillRect(254, 187, 54, 4, ILI9341_BLACK);
  _tft->fillRect(254, 201, 54, 4, ILI9341_BLACK);
  _tft->fillCircle(264, 175, 5, ILI9341_BLACK);
  _tft->fillCircle(294, 189, 5, ILI9341_BLACK);
  _tft->fillCircle(273, 203, 5, ILI9341_BLACK);
  _tft->setCursor(258, 218);
  _tft->setTextColor(ILI9341_BLACK);  _tft->setTextSize(1);
  _tft->println("Settings");

  //draw exit
  _tft->fillCircle(40, 197, 30, ILI9341_RED);
  _tft->setCursor(18, 190);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(2);
  _tft->println("Exit");

  TyroBotWifi(rssi);
  TyroBotBattery(batMode);
}

void TyroBot::TyroBotBattery(int level) {
  _tft->fillRect(185, 180, 30, 50, ILI9341_WHITE);
  _tft->fillRect(190, 172, 20, 15, ILI9341_WHITE);
  _tft->setCursor(175, 231);
  _tft->setTextColor(ILI9341_BLACK);  _tft->setTextSize(1);
  _tft->println("Complete.");
  if (level >= 4) {
    _tft->fillRect(185, 180, 30, 50, ILI9341_GREEN);
    _tft->fillRect(190, 172, 20, 15, ILI9341_GREEN);
    _tft->setCursor(175, 231);
    _tft->setTextColor(ILI9341_GREEN);  _tft->setTextSize(1);
    _tft->println("Complete.");
  }
  if (level >= 3) {
    _tft->fillRect(188, 187, 24, 6, ILI9341_GREEN);
  }
  if (level >= 2) {
    _tft->fillRect(188, 197, 24, 6, ILI9341_GREEN);
  }
  if (level >= 1) {
    _tft->fillRect(188, 207, 24, 6, ILI9341_GREEN);
  }
  if (level > 0) {
    _tft->fillRect(188, 217, 24, 6, ILI9341_GREEN);
  }
  if (level == 0) {
    _tft->fillRect(188, 217, 24, 6, ILI9341_RED);
  }
}

void TyroBot::TyroBotWifi(char dbm[]) {
  SerialUSB.println(dbm[1]);
  if (dbm[1] == '7') {
    _tft->fillRect(90, 220, 6, 10, ILI9341_WHITE);
  }
  if (dbm[1] == '6') {
    _tft->fillRect(101, 210, 6, 20, ILI9341_WHITE);
    _tft->fillRect(90, 220, 6, 10, ILI9341_WHITE);
  }
  if (dbm[1] == '5') {
    _tft->fillRect(112, 200, 6, 30, ILI9341_WHITE);
    _tft->fillRect(101, 210, 6, 20, ILI9341_WHITE);
    _tft->fillRect(90, 220, 6, 10, ILI9341_WHITE);
  }
  if (dbm[1] == '4') {
    _tft->fillRect(123, 190, 6, 40, ILI9341_WHITE);
    _tft->fillRect(112, 200, 6, 30, ILI9341_WHITE);
    _tft->fillRect(101, 210, 6, 20, ILI9341_WHITE);
    _tft->fillRect(90, 220, 6, 10, ILI9341_WHITE);
  }
  if (dbm[1] == '3') {
    _tft->fillRect(134, 180, 6, 50, ILI9341_WHITE);
    _tft->fillRect(123, 190, 6, 40, ILI9341_WHITE);
    _tft->fillRect(112, 200, 6, 30, ILI9341_WHITE);
    _tft->fillRect(101, 210, 6, 20, ILI9341_WHITE);
    _tft->fillRect(90, 220, 6, 10, ILI9341_WHITE);
  }
  if (dbm[1] == '0') {
    _tft->fillRect(134, 180, 6, 50, ILI9341_WHITE);
    _tft->fillRect(123, 190, 6, 40, ILI9341_WHITE);
    _tft->fillRect(112, 200, 6, 30, ILI9341_WHITE);
    _tft->fillRect(101, 210, 6, 20, ILI9341_WHITE);
    _tft->fillRect(90, 220, 6, 10, ILI9341_WHITE);
    _tft->setCursor(90, 195);
    _tft->setTextColor(ILI9341_RED);  _tft->setTextSize(4);
    _tft->println("X");
  }
}

void TyroBot::enterBootloader() {
  _tft->setRotation(1);
  _tft->fillScreen(ILI9341_BLACK);
  _tft->setCursor(20, 70);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(3);
  _tft->println("Bootloader mode");

  for (int i = 0; i < 301; i++) {
    if (checkForPress() == true) {
      TyroBotMenu();
      return;
    }
    delay(10);
  }
  int d = 0;
  Serial.write(d);
}

void TyroBot::batteryLevel() {
  pinMode(A4, INPUT);
  voltage = analogRead(A4);
  voltage = voltage * ((3.3 / 1023.0) * 2);
  if (voltage > 4.19) {
    batMode = 4;
  }
  else if (voltage > 4.0) {
    batMode = 3;
  }
  else if (voltage > 3.8) {
    batMode = 2;
  }
  else if (voltage > 3.6) {
    batMode = 1;
  }
  else {
    batMode = 0;
  }

  theTime = millis() / 1000;
  if (timeMultiple - theTime <= 0) {
    timeMultiple += 60;
    TyroBotBattery(batMode);
  }
}

void TyroBot::selfProgram() {
  uint32_t colors[5] = {ILI9341_GREEN, ILI9341_BLUE, ILI9341_YELLOW, ILI9341_PURPLE, ILI9341_RED};
  int program[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int key = 0;

  _tft->begin();
  _tft->setRotation(1);

  //draw program boxes
  _tft->fillScreen(ILI9341_BLACK);
  for (int i = 0; i <= 4; i++) {
    _tft->fillRect(i * 62 + 12, 182, 50, 50, colors[i]);
  }
  upTriangle(0 * 62 + 37, 197);
  downTriangle(3 * 62 + 37, 197);
  rightTriangle(1 * 62 + 37, 197);
  leftTriangle(2 * 62 + 37, 197);
  _tft->setCursor(273, 190);
  _tft->setTextColor(ILI9341_WHITE);  _tft->setTextSize(5);
  _tft->println("X");

  //START AND BACK BUTTONS
  _tft->fillRect(176, 134, 124, 36, ILI9341_GREEN);
  _tft->fillRect(178, 136, 120, 32, ILI9341_BLACK);
  _tft->setCursor(180, 138);
  _tft->setTextColor(ILI9341_GREEN);  _tft->setTextSize(4);
  _tft->println("Start");

  _tft->fillRect(16, 134, 100, 36, ILI9341_RED);
  _tft->fillRect(18, 136, 96, 32, ILI9341_BLACK);
  _tft->setCursor(20, 138);
  _tft->setTextColor(ILI9341_RED);  _tft->setTextSize(4);
  _tft->println("Home");

  while (1) {
    if (checkForPress() == true) {
      int x = 0;
      int y = 0;
      getPoints(y, x);

      //start and home buttons
      if (x < 106 and x > 70) {
        if (y > 12 and y < 112) {
          TyroBotMenu();
          break;
        }
        if (y > 174 and y < 300) {
          face();
          for (int i = 0; i <= 9; i++) {
            if (program[i] == 1) {
              SerialUSB.println("forward");
              if (program[i + 1] == 1) {
                cali = true;
              }
              else {
                cali = false;
              }
              forward(2);

            }
            else if (program[i] == 2) {
              SerialUSB.println("right");
              turnRight(2);
            }
            else if (program[i] == 3) {
              SerialUSB.println("left");
              turnLeft(2);
            }
            else if (program[i] == 4) {
              SerialUSB.println("backward");
              if (program[i + 1] == 4) {
                cali = true;
              }
              else {
                cali = false;
              }
              backward(2);
            }
          }
          calibrate();
          TyroBotMenu();
          break;
        }
      }

      if (x < 58) {
        if (y > 12 and y < 62) {
          program[key] = 1;
          key += 1;
        }
        else if (y > 74 and y < 124) {
          program[key] = 2;
          key += 1;
        }
        else if (y > 136 and y < 186) {
          program[key] = 3;
          key += 1;
        }
        else if (y > 198 and y < 248) {
          program[key] = 4;
          key += 1;
        }
        else if (y > 256 and y < 306) {
          program[key] = 0;
          key -= 1;
          if (key < 0){
            key = 0;
          }
        }
        if (key > 9) {
          key = 9;
        }

        for (int w = 0; w <= 4; w++) {
          if (program[w] == 0) {
            _tft->fillRect(w * 62 + 12, 12, 50, 50, ILI9341_BLACK);
          }
          if (program[w] == 2) {
            _tft->fillRect(w * 62 + 12, 12, 50, 50, colors[1]);
            rightTriangle(w * 62 + 37, 27);
          }
          if (program[w] == 1) {
            _tft->fillRect(w * 62 + 12, 12, 50, 50, colors[0]);
            upTriangle(w * 62 + 37, 27);
          }
          if (program[w] == 3) {
            _tft->fillRect(w * 62 + 12, 12, 50, 50, colors[2]);
            leftTriangle(w * 62 + 37, 27);
          }
          if (program[w] == 4) {
            _tft->fillRect(w * 62 + 12, 12, 50, 50, colors[3]);
            downTriangle(w * 62 + 37, 27);
          }
        }
        for (int r = 0; r <= 4; r++) {
          if (program[r + 5] == 0) {
            _tft->fillRect(r * 62 + 12, 74, 50, 50, ILI9341_BLACK);
          }
          if (program[r + 5] == 2) {
            _tft->fillRect(r * 62 + 12, 74, 50, 50, colors[1]);
            rightTriangle(r * 62 + 37, 89);
          }
          if (program[r + 5] == 1) {
            _tft->fillRect(r * 62 + 12, 74, 50, 50, colors[0]);
            upTriangle(r * 62 + 37, 89);
          }
          if (program[r + 5] == 3) {
            _tft->fillRect(r * 62 + 12, 74, 50, 50, colors[2]);
            leftTriangle(r * 62 + 37, 89);
          }
          if (program[r + 5] == 4) {
            _tft->fillRect(r * 62 + 12, 74, 50, 50, colors[3]);
            downTriangle(r * 62 + 37, 89);
          }
        }
        delay(300);
      }
    }
  }
}

int TyroBot::upTriangle(int x, int y) {
  _tft->fillTriangle(x, y, x + 16, y + 16, x - 16, y + 16, ILI9341_WHITE);
}

int TyroBot::downTriangle(int x, int y) {
  _tft->fillTriangle(x, y + 21, x + 16, y + 5, x - 16, y + 5, ILI9341_WHITE);
}

int TyroBot::rightTriangle(int x, int y) {
  _tft->fillTriangle(x - 5, y - 5, x + 11, y + 11, x - 5, y + 27, ILI9341_WHITE);
}

int TyroBot::leftTriangle(int x, int y) {
  _tft->fillTriangle(x + 6, y - 5, x - 10, y + 11, x + 6, y + 27, ILI9341_WHITE);
}


void TyroBot::runProgram() {
  face();
}

void TyroBot::menu()
{
  calibrate();
  TyroBotMenu();
  while (1) {
  mode = 1;
  batteryLevel();

  int data = 0;

  //poll touchscreen
  if (checkForPress() == true) {
    int x = 0;
    int y = 0;
    getPoints(x, y);
    SerialUSB.print(x);
    SerialUSB.print(",");
    SerialUSB.println(y);
    if (x < 80 and y < 85) {
      mode = 0;
    }
    if (x < 155 and y > 85) {
      runProgram();
      break;
    }
    if (x > 155 and y > 85) {
      selfProgram();
    }
    if (x > 240 and y < 85) {
      settings();
    }
  }

  //ESP8266 Communication
  if (mode == 0) {
    mode = 1;
    enterBootloader();
  }
  }
}

bool TyroBot::checkForPress() {
  pinMode(A2, OUTPUT);
  digitalWrite(A2, 0);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  pinMode(3, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(3, 1);
  digitalWrite(A3, 1);

  pinMode(A2, OUTPUT);
  pinMode(7, OUTPUT);

  int val = analogRead(A3);
  if (val < 50) {
    return true;
  }
  else {
    return false;
  }
}

void TyroBot::getPoints(int &y, int &x) {
  if (checkForPress()) {

    pinMode(A2, INPUT_PULLUP);

    pinMode(7, INPUT_PULLUP);
    digitalWrite(A2, LOW);
    digitalWrite(7, LOW);

    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);
    pinMode(A3, OUTPUT);
    digitalWrite(A3, LOW);

    xVal = analogRead(A2);

    // Set up the analog pins in preparation for reading the Y value
    // from the touchscreen
    pinMode(3, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);
    digitalWrite(3, LOW);
    digitalWrite(A3, LOW);

    pinMode(A2, OUTPUT);
    digitalWrite(A2, HIGH);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);

    yVal = analogRead(A3);

    // Scale from ~0->1000 to tft.width using the calibration #'s
    x = map(xVal, TS_MINX, TS_MAXX, 0, 240);
    y = map(yVal, TS_MINY, TS_MAXY, 0, 320);
  }
}

void TyroBot::shakeHead() {
  head.attach(HEAD);
  head.write(30);
  delay(500);
  head.write(150);
  delay(500);
  head.write(90);
  delay(500);
  head.detach();
}

void TyroBot::lookRight() {
  head.attach(HEAD);
  head.write(30);
  delay(1000);
  head.detach();
}

void TyroBot::lookLeft() {
  head.attach(HEAD);
  head.write(150);
  delay(1000);
  head.detach();
}

void TyroBot::connectAccel() {
  //Setup Accelerometer
  if (! _lis->begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Could not connect to accelerometer");
  }
  _lis->setRange(LIS3DH_RANGE_4_G);
  _lis->read();
  //SerialUSB.print(_lis->x);
  //SerialUSB.print(",");
 // SerialUSB.print(_lis->y);
}

bool TyroBot::checkAccel() {
  _lis->read();
  if (_lis->x > maxX or _lis->x < minX) {
    return false;
  }
  else {
    return true;
  }
}

void TyroBot::checkForFall() {
  _lis->read();
  if (_lis->y < 3000) {
    SerialUSB.print("I've fallen and i cant get up!");
    calibrate();
    while (1);
  }
}

void TyroBot::calibrate() {
  _lis->read();
  rf.detach();
  ll.detach();
  lf.detach();
  rl.detach();

  ll.attach(LEFT_LEG);
  ll.write(LL_CENTER);
  delay(STEP_SPEED);
  ll.detach();

  lf.attach(LEFT_FOOT);
  lf.write(LF_CENTER);
  delay(STEP_SPEED);
  lf.detach();

  rl.attach(RIGHT_LEG);
  rl.write(RL_CENTER);
  delay(STEP_SPEED);
  rl.detach();

  rf.attach(RIGHT_FOOT);
  rf.write(RF_CENTER);
  delay(STEP_SPEED);
  rf.detach();

  head.attach(HEAD);
  head.write(HEAD_CENTER);
  delay(STEP_SPEED);
  head.detach();

  _lis->read();
}

//Lift right foot off the ground
void TyroBot::liftRightFoot() {
  rf.attach(RIGHT_FOOT);
  lf.attach(LEFT_FOOT);
  rf.write(RF_CENTER - LIFT_HEIGHT*3);
  delay(50);
  lf.write(LF_CENTER - LIFT_HEIGHT*3);
  delay(STEP_SPEED);
}

//Move right foot forward
void TyroBot::rightFootForward() {
  ll.attach(LEFT_LEG);
  rl.attach(RIGHT_LEG);
  ll.write(LL_CENTER + STEP_DISTANCE);
  delay(500);
  rl.write(RL_CENTER + STEP_DISTANCE);
  delay(STEP_SPEED);
}

void TyroBot::liftLeftFoot() {
  //Lift left foot off the ground
  rf.attach(RIGHT_FOOT);
  lf.attach(LEFT_FOOT);
  lf.write(LF_CENTER + LIFT_HEIGHT * 3);
  delay(50);
  rf.write(RF_CENTER + LIFT_HEIGHT * 3);
  delay(STEP_SPEED);
}

void TyroBot::leftFootForward() {
  ll.attach(LEFT_LEG);
  rl.attach(RIGHT_LEG);
  rl.write(RL_CENTER - STEP_DISTANCE);
  delay(500);
  ll.write(LL_CENTER - STEP_DISTANCE);
  delay(STEP_SPEED);
}

void TyroBot::leftFootBackward() {
  ll.attach(LEFT_LEG);
  rl.attach(RIGHT_LEG);
  rl.write(RL_CENTER + STEP_DISTANCE);
  delay(500);
  ll.write(LL_CENTER + STEP_DISTANCE);
  delay(STEP_SPEED);
}

void TyroBot::rightFootBackward() {
  ll.attach(LEFT_LEG);
  rl.attach(RIGHT_LEG);
  ll.write(LL_CENTER - STEP_DISTANCE);
  delay(500);
  rl.write(RL_CENTER - STEP_DISTANCE);
  delay(STEP_SPEED);
}

void TyroBot::footDown() {
  rf.attach(RIGHT_FOOT);
  lf.attach(LEFT_FOOT);
  rf.write(RF_CENTER);
  lf.write(LF_CENTER);
  delay(STEP_SPEED);
}

void TyroBot::forward(int steps) { //Take a step forward
  for (int i = 0; i < steps; i++) {
    liftRightFoot();
    checkForFall();
    rightFootForward();
    checkForFall();
    footDown();
    checkForFall();
    liftLeftFoot();
    checkForFall();
    leftFootForward();
    checkForFall();
    footDown();
    checkForFall();
  }
  if (cali == false) {
  liftRightFoot();
  calibrate();
  delay(STEP_SPEED*2);
  }
}

void TyroBot::backward(int steps) {
  for (int i = 0; i < steps; i++) {
    liftLeftFoot();
    checkForFall();
    leftFootBackward();
    checkForFall();
    footDown();
    checkForFall();
    liftRightFoot();
    checkForFall();
    rightFootBackward();
    checkForFall();
    footDown();
    checkForFall();
  }
  if (cali == false) {
  calibrate();
  delay(STEP_SPEED*2);
  }
}

void TyroBot::turnRight(int turns) {
  for (int i = 0; i < turns; i++) {
    liftLeftFoot();
    rl.attach(RIGHT_LEG);
    rl.write(RL_CENTER - STEP_DISTANCE*2);
    delay(500);
    delay(STEP_SPEED);
    footDown();
    rf.detach();
    lf.detach();
    liftRightFoot();
    rl.write(RL_CENTER);
    checkForFall();
    calibrate();
  }
}

void TyroBot::turnLeft(int turns) {
  for (int i = 0; i < turns; i++) {
    liftRightFoot();
    ll.attach(LEFT_LEG);
    ll.write(LL_CENTER + STEP_DISTANCE*2);
    delay(500);
    delay(STEP_SPEED);
    footDown();
    rf.detach();
    lf.detach();
    liftLeftFoot();
    ll.write(LL_CENTER);
    checkForFall();
    calibrate();
  }
}

void TyroBot::waveRightArm(int times) {
  for (int i = 0; i < times; i++) {
    rightArm.attach(RIGHT_ARM);
    rightArm.write(180);
    delay(500);
    rightArm.write(0);
    delay(1000);
    rightArm.write(RIGHT_ARM_CENTER);
    delay(500);
    rightArm.detach();
  }
}

void TyroBot::waveLeftArm(int times) {
  for (int i = 0; i < times; i++) {
    leftArm.attach(LEFT_ARM);
    leftArm.write(0);
    delay(500);
    leftArm.write(180);
    delay(1000);
    leftArm.write(LEFT_ARM_CENTER);
    delay(500);
    leftArm.detach();
  }
}

void TyroBot::IFTT(char event[]) {
  Serial.write(2);
  char tokenChar[50];
  token.toCharArray(tokenChar, 50);
  Serial.write(tokenChar);
  Serial.write(",");
  Serial.write(event);
}
