#include "RGBAnimator.h"
#include <HardwareSerial.h>
#include <WString.h>
#include <stdlib_noniso.h>
#include <Arduino.h>

RGBAnimator::RGBAnimator(const int* pins)
{
    for(int i=0; i<3; i++)
    {
        pinMode(pins[i], OUTPUT);
        this->pins[i]   = pins[i];//R, G, B
        currentColor[i] = 0;
        targetColor[i]  = 255;
    }
}

void RGBAnimator::setColor(int r, int g, int b)
{
    currentColor[0] = r;
    currentColor[1] = g;
    currentColor[2] = b;
    stepsPerSecond = 0.f;
}

float RGBAnimator::calcColorDistance(char* color1, char* color2)
{
    return (abs((int)color1[0]-(int)color2[0])*abs((int)color1[0]-(int)color2[0])
                + abs((int)color1[1]-(int)color2[1])*abs((int)color1[1]-(int)color2[1])
                + abs((int)color1[2]-(int)color2[2])*abs((int)color1[2]-(int)color2[2]));
}

void RGBAnimator::fade2Color(int r, int g, int b, float timeInSeconds)
{
    targetColor[0] = r;
    targetColor[1] = g;
    targetColor[2] = b;
    stepsPerSecond = calcColorDistance(currentColor, targetColor)/timeInSeconds;
}

void RGBAnimator::onTimer(float deltaSeconds)
{
    currentColor[0] += (char)(stepsPerSecond * deltaSeconds);
    currentColor[1] += (char)(stepsPerSecond * deltaSeconds);
    currentColor[2] += (char)(stepsPerSecond * deltaSeconds);
}

void RGBAnimator::setSpeed(float stepsPerSecond)
{
    this->stepsPerSecond = stepsPerSecond;
}

void RGBAnimator::update()
{   
    currentTime = millis();
    unsigned long deltaTime = currentTime - oldTime;
    if (deltaTime >= minDeltaTime)
    {
        oldTime = currentTime;
    }
    else
    {
        // if no time could be measured -> repeat!
        return;
    }
    
    this->onTimer(((float)deltaTime)/1000.f);

    for(int i=0; i<3; i++)
        analogWrite(pins[i], currentColor[i]);
}