#include "robot.h"

RobotInput::RobotInput(video::IImage* image)
{
    this->image = image;
}


void Robot::pushInput(RobotInput* in){

    video::IImage* image = in->image;

    int width = image->getDimension().Width;
    int center[2] = {width/2,image->getDimension().Height/2};

    irr::video::SColor centerPx = image->getPixel(center[0],center[1]);

//    printf( "%d %d %d \n", centerPx.getRed(), centerPx.getGreen(), centerPx.getBlue());

    pleasure = 0.0;

    // sight pleasure. seeing good things makes you happy.
    for(int i=0; i<width; i++){
        irr::video::SColor curPx = image->getPixel(i,center[1]);
        float green = curPx.getGreen()/255.0f;
        float red = curPx.getRed()/255.0f;
        float x = ((float)(i-center[0]))/((float)(2.0*width));
        this->pleasure += green*exp(-x*x*100);                          // exponential from center pleasure function
    }
    pleasure = pleasure/width;  // average all. maximum is 2\int_0^1 exp() < 1

    if(in->touchFruit)
        pleasure += 10;

    printf("%f\n",this->pleasure);

    curOutput = RobotOutput(5e-1,5e-1);

}
