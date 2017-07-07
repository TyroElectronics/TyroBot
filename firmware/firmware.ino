/* TyroBot Demo Code
 *  copyright (c) 2017 Tyro Electronics
 *  written by Tyler Spadgenske
 *  GNU General Public License 3.0
 */

#include "TyroBot.h" //include this library whenever using TyroBot

char ssid[] = "spadgenske";
char pass[] = "68mustang";

Adafruit_LIS3DH lis = Adafruit_LIS3DH(); //Setup the Accelerometer
Adafruit_ILI9341 tft = Adafruit_ILI9341(4, 9, 23, 24, -1, 22); //CS, DC, MOSI, CLK, RST, MISO (Setup the display)

TyroBot bot = TyroBot(&tft, &lis); //Create the TyroBot object

void setup()
{
  bot.connectWifi(ssid, pass);
}


void loop()
{
  bot.connectAccel(); //Connect to the accelerometer 
  bot.menu(); //display the default splash screen on startup
  //Any code below runs when "run program" is tapped in TyroBot's menu
  //bot.token = "b8354jvkbG7ZSitd6ygT4z";
  //bot.IFTT("test");
  //bot.shakeHead();
  //bot.waveRightArm(1);
  //bot.waveLeftArm(1);
  delay(10000);
}

