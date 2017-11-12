/* TyroBot v1.0.0
 *  copyright (c) 2017 Tyro Electronics
 *  written by Tyler Spadgenske
 *  GNU General Public License 3.0
 */

#ifndef TyroBot_h
#define TyroBot_h
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Servo.h>

class TyroBot
{
public:
  TyroBot(Adafruit_ILI9341 * tft, Adafruit_LIS3DH * lis);

  //Setup Accelerometer
  Adafruit_LIS3DH * _lis;

  //touchscreen
  bool checkForPress();
  void getPoints(int &y, int &x);

  bool cali;

  int x;
  int y;
  int xVal;
  int yVal;
  int TS_MINX;
  int TS_MINY;
  int TS_MAXX;
  int TS_MAXY;

  int mode;

  //battery variables
  float voltage;
  int batMode;
  long theTime;
  long timeMultiple;

  //self program variables
  uint32_t colors[5];
  int program[8];
  int key;
  int y_coord;

  //FUNctions
  void face();
  void TyroBotMenu();
  void TyroBotWifi(char dbm[]);
  void TyroBotBattery(int level);
  void enterBootloader();
  void batteryLevel();
  void selfProgram();
  void runProgram();
  int upTriangle(int x, int y);
  int downTriangle(int x, int y);
  int leftTriangle(int x, int y);
  int rightTriangle(int x, int y);
  void menu();
  void connectWifi(char ssid[], char pass[]);
  void settings();

  Adafruit_ILI9341 * _tft;

  void forward(int steps);
  void shakeHead();
  void lookLeft();
  void lookRight();
  bool checkAccel();
  void checkForFall();
  void calibrate();
  void liftRightFoot();
  void rightFootForward();
  void liftLeftFoot();
  void footDown();
  void leftFootForward();
  void connectAccel();
  void leftFootBackward();
  void backward(int steps);
  void rightFootBackward();
  void turnRight(int turns);
  void turnLeft(int turns);
  void waveRightArm(int times);
  void waveLeftArm(int times);
  void IFTT(char event[]);

  //Setup Servos
  Servo ll;
  Servo lf;
  Servo rl;
  Servo rf;
  Servo head;
  Servo leftArm;
  Servo rightArm;

  //wifi
  char ip[50];
  char rssi[50];
  bool connection;

  //IFTT
  String token;

private:
  //Servo Pins
  int LEFT_LEG;
  int LEFT_FOOT;
  int RIGHT_LEG;
  int RIGHT_FOOT;
  int HEAD;
  int LEFT_ARM;
  int RIGHT_ARM;

  //Step Constants
  int LIFT_HEIGHT;
  int STEP_DISTANCE;
  int STEP_SPEED;

  int LEFT_TRIM;
  int RIGHT_TRIM;

  //Servo Positions when robot is at rest
  int LL_CENTER;
  int LF_CENTER;
  int RL_CENTER;
  int RF_CENTER;
  int HEAD_CENTER;
  int RIGHT_ARM_CENTER;
  int LEFT_ARM_CENTER;

  //Accelerometer
  int maxX;
  int minX;
  int TIMEOUT;
  bool level;
};
#endif
