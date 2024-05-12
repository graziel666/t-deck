#include <Wire.h>
#include <TFT_eSPI.h>
#include "utilities.h"
//#include <WiFi.h> 
#include <ctype.h>


#ifndef BOARD_HAS_PSRAM
#error "Detected that PSRAM is not turned on. Please set PSRAM to OPI PSRAM in ArduinoIDE"
#endif

#define LILYGO_KB_SLAVE_ADDRESS 0x55
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define BOARD_POWERON       10
#define BOARD_I2C_SDA       18
#define BOARD_I2C_SCL       8

const uint16_t bufferSize = 64;
char command[bufferSize];
uint8_t commandIndex = 0;

const uint16_t promptHeight = 30;
const uint16_t promptY = 0;
const uint16_t outputY = promptY + promptHeight + 8;

TFT_eSPI tft = TFT_eSPI();


void setup()
{
    Serial.begin(115200);
    Serial.println("T-Deck Keyboard Master");

    pinMode(BOARD_POWERON, OUTPUT);
    digitalWrite(BOARD_POWERON, HIGH);

    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);

    tft.begin();
    tft.setRotation(1);
    tft.invertDisplay(1);
    clearScreen();

    // Display command prompt
    tdeck_begin();
        // Check keyboard
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    if (Wire.read() == -1) {
        while (1) {
            Serial.println("LILYGO Keyboad not online .");
            delay(1000);
        }
    }
}

void loop()
{
    // Read key value from T-Keyboard
    char incoming = 0;
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0)
    {
        incoming = Wire.read();
        if (incoming != (char)0x00)
        {
            Serial.print("keyValue: ");
            Serial.println(incoming);

            if (incoming == '\n' || incoming == '\r')
            {
                // Execute command when Enter key is pressed
                // tft.setCursor(10, tft.getCursorY());
                tft.println();
                tft.setCursor(10, tft.getCursorY());
                tft.print("CMD> ");
                tft.print(incoming);
                //executeCommand();
            }
            else if (incoming == '\b' && commandIndex > 0)
            {
                // Handle backspace by deleting previous character
                commandIndex--;
                command[commandIndex] = '\0';
                tft.fillRect(tft.getCursorX() - 6, tft.getCursorY(), SCREEN_WIDTH, promptHeight, TFT_BLACK);
                tft.setCursor(tft.getCursorX() - 6, tft.getCursorY());
                //clearCommandOutput();
                //printCommandScreen();
            }
            else if (commandIndex < bufferSize - 1)
            {
                // Add incoming character to command buffer
                command[commandIndex] = incoming;
                commandIndex++;
                command[commandIndex] = '\0';
                    
                // Display the character on the TFT display
                tft.print(incoming);
            }
        }
    }
}



//clear screen
void clearScreen()
{
    tft.fillRect(0, promptY + promptHeight, SCREEN_WIDTH, SCREEN_HEIGHT - (promptY + promptHeight), TFT_BLACK);
}

void tdeck_begin(){
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(1);
    tft.setCursor(10, promptY + 8);
    tft.println("QuagDeck v0.1");
    // Clear the command buffer and reset the index
    // memset(command, 0, bufferSize);
    // commandIndex = 0;
    printFirstCommandScreen();
}


void printFirstCommandScreen(){
    tft.setCursor(10, outputY);
    tft.setTextColor(TFT_DARKGREEN);
    tft.setTextSize(1); // Set a smaller font size
    tft.print("CMD> ");
    tft.print(command);
}

char getKeyboardInput()
{
    char incoming = 0;
    Wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (Wire.available() > 0)
    {
        incoming = Wire.read();
        if (incoming != (char)0x00)
        {
            return incoming;
        }
    }
    
}