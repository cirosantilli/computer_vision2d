#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <math.h>

#include <irrlicht/irrlicht.h>

using namespace irr;

class RobotInput {

    public:

    RobotInput(){}
    RobotInput(video::IImage* image);

    video::IImage* image;
    bool touchFruit;

    private:

};

class RobotOutput {

    public:

    RobotOutput(){}
    RobotOutput(float speed, float rotation){ this->speed=speed; this->rotation = rotation; }

    float speed;
    float rotation;

};

class Robot {

    public:

    Robot(){}

    // update the robot state acording to current input
    void pushInput(RobotInput* in);

    // get current output
    RobotOutput* getOutput(){return &this->curOutput;}

    private:

    const static float maxMove = 10;
    const static float maxRotate = 5;

    /*actuators: values for next move trial. may not be able to do
    them depending on the environment (cannot cross a wall for example) */
    RobotOutput curOutput;

    float pleasure;

};

#endif
