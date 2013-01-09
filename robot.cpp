#include <iostream> //TEST
#include <sstream> //TEST

#include "robot.h"

using namespace std;

Brain::Brain(){}
Brain::~Brain(){}

BrainInput::BrainInput
(
    IImage* image,
    bool collisionOccurred,
    float collisionAngle
) :
    image(image),
    collisionOccurred(collisionOccurred),
    collisionAngle(collisionAngle)
{}

BrainOutput::BrainOutput(
    float forwardStep,
    float strafeStep,
    float rotateStep
) :
    forwardStep(forwardStep),
    strafeStep(strafeStep),
    rotateStep(rotateStep)
{}

RobotBrain::RobotBrain(){}
RobotBrain::~RobotBrain(){}

void RobotBrain::pushInput(BrainInput* in)
{
    curOutput = BrainOutput(.01f,.00f,0.f); //EST

    //int width = image->getDimension().Width;
    //int center[2] = {width/2,image->getDimension().Height/2};

    //irr::video::SColor centerPx = image->getPixel(center[0],center[1]);

    //printf( "%d %d %d \n", centerPx.getRed(), centerPx.getGreen(), centerPx.getBlue());

    //pleasure = 0.0;

    //// sight pleasure. seeing good things makes you happy.
    //for(int i=0; i<width; i++){
        //irr::video::SColor curPx = image->getPixel(i,center[1]);
        //float green = curPx.getGreen()/255.0f;
        //float red = curPx.getRed()/255.0f;
        //float x = ((float)(i-center[0]))/((float)(2.0*width));
        //this->pleasure += green*exp(-x*x*100);                          // exponential from center pleasure function
    //}
    //pleasure = pleasure/width;  // average all. maximum is 2\int_0^1 exp() < 1

    //if(in->touchFruit)
        //pleasure += 10;
}

BrainOutput* Brain::getOutput() { return &curOutput; }

HumanBrain::HumanBrain() :
        forwardDown(false),
        backwardDown(false),
        strafeLeftDown(false),
        strafeRightDown(false),
        rotateLeftDown(false),
        rotateRightDown(false)
{
}

HumanBrain::~HumanBrain(){}

HumanReceiver::HumanReceiver(
    HumanBrain* brain,
    ICameraSceneNode* camera
):brain(brain), camera(camera)
{
}

bool HumanReceiver::OnEvent(const SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT)
    {
        bool isDown;
        if ( event.KeyInput.PressedDown )
            isDown = true;
        else
            isDown = false;

        if ( event.KeyInput.Key == KEY_KEY_A ){
            //strafe left
            brain->strafeLeftDown = isDown;
            return true;
        }

        if ( event.KeyInput.Key == KEY_KEY_D ){
            //strafe right
            brain->strafeRightDown = isDown;
            return true;
        }

        if ( event.KeyInput.Key == KEY_KEY_E ){
            //rotate right
            brain->rotateRightDown = isDown;
            return true;
        }

        if ( event.KeyInput.Key == KEY_KEY_Q ){
            //rotate left
            brain->rotateLeftDown = isDown;
            return true;
        }

        if ( event.KeyInput.Key == KEY_KEY_S ){
            //move back
            brain->backwardDown = isDown;
            return true;
        }

        else if ( event.KeyInput.Key == KEY_KEY_W ){
            //move forward
            brain->forwardDown = isDown;
            return true;
        }

        else if ( event.KeyInput.Key == KEY_ESCAPE ){
            //quit
            exit(EXIT_SUCCESS);
        }

    }

    //send other events to camera
    if (camera)
        return camera->OnEvent(event);

    return false;
}

void HumanBrain::pushInput(BrainInput* in)
{
    float forward=0.f, strafe=0.f, rotate=0.f;

    float forwardStep = .01f; //TEST: on a real continuous test, frame rate must be fixed!
    float strafeStep = forwardStep; //TEST: on a real continuous test, frame rate must be fixed!
    float rotateStep = 2.f; //TEST: on a real continuous test, frame rate must be fixed!

    if(backwardDown)
        forward -= forwardStep;
    if(forwardDown)
        forward += forwardStep;
    if(strafeLeftDown)
        strafe -= strafeStep;
    if(strafeRightDown)
        strafe += strafeStep;
    if(rotateLeftDown)
        rotate -= rotateStep;
    if(rotateRightDown)
        rotate += rotateStep;

    curOutput = BrainOutput(forward,strafe,rotate);
}

Robot::Robot(
    IVideoDriver* driver,
    ICameraSceneNode* camera,
    ISceneNodeAnimatorCollisionResponse* anim,
    Brain* brain
) :
    driver(driver),
    camera(camera),
    anim(anim),
    brain(brain)
{}

BrainInput Robot::getBrainInput()
{
    //calculate relative collision angle
    vector3df pos = camera->getPosition();
    vector3df lookat = camera->getTarget();
    vector3df col = anim->getCollisionPoint();
    vector3df v1 = (col-pos);
    vector3df v2 = (lookat-pos);
    float collisionAngle = acos(v1.dotProduct(v2)/(v1.getLength()*v2.getLength()));
    if( (v1.crossProduct(v2)).dotProduct(Y) < 0 ) //decide quadrant
        collisionAngle = -collisionAngle;

    //touchFruit = anim->collisionOccurred() && anim->getCollisionNode()->getID() == FruitID;
    //robotInput.touchFruit = touchFruit;

    //make input
    return BrainInput(
        driver->createScreenShot(),
        anim->collisionOccurred(),
        collisionAngle
    );
}

void Robot::update()
{
    in = getBrainInput();
    brain->pushInput(&in);
    actuate(brain->getOutput());
}

void Robot::actuate(BrainOutput* out)
{
    vector3df dx =
    //forward
    (
        camera->getTarget() -
        camera->getPosition()
    )*out->forwardStep
    +
    //strafe
    Y.crossProduct(
        camera->getTarget() -
        camera->getPosition()
    )*out->strafeStep;

    camera->setPosition(
        camera->getPosition() + dx
    );
    camera->setTarget(
        camera->getTarget() + dx
    );

    //rotation
    vector3df t = vector3df(camera->getTarget());
    t.rotateXZBy(
        out->rotateStep,
        camera->getPosition()
    );
    camera->setTarget(t);
}

std::string Robot::str()
{
    stringstream oss;
    vector3df pos = camera->getPosition();
    vector3df lookat = camera->getTarget();

    oss << "pos:";
    oss << endl;
    oss << pos.X;
    oss << endl;
    oss << pos.Z;
    oss << endl;

    oss << "lookat:";
    oss << endl;
    oss << lookat.X;
    oss << endl;
    oss << lookat.Z;
    oss << endl;

    /*rgb print
        oss << "rgb:";
        oss << endl;
        IImage* img = driver->createScreenShot();
        for(int i=0; i<img->getDimension().Width; i++){
            SColor c = img->getPixel(i,0);
            oss << c.getRed();
            oss << " ";
            oss << c.getGreen();
            oss << " ";
            oss << c.getBlue();
            oss << endl;
        }
        oss << endl;
    */
    if(in.collisionOccurred)
    {
        //oss << "collision:";
        //oss << endl;
        //oss << "point";
        //oss << endl;
        //oss << col.X;
        //oss << endl;
        //oss << col.Z;
        //oss << endl;
        oss << "collision angle:";
        oss << endl;
        oss << in.collisionAngle;
        oss << endl;
    }
    oss << endl;

    return oss.str();
}
