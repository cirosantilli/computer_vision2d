#ifndef ROBOT_H
#define ROBOT_H

#include <irrlicht.h>
//for IImage and vector3df.
//consider removing this dependency with custom internai image format

#include <iostream>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;

const vector3df Y = vector3df(0.f,1.f,0.f); //TODO put in a utils.h

class BrainInput
{
    public:

        BrainInput
        (
            IImage* image = NULL,
            bool collisionOccurred = false,
            float collisionAngle = 0.f
        );

        IImage* image;
        bool collisionOccurred;
        float collisionAngle;

        bool touchFruit;
};

class BrainOutput
{
    public:

        BrainOutput
        (
            float forwardStep = 0.f,
            float strafeStep = 0.f,
            float rotateStep = 0.f
        );

        float forwardStep; //+forward
        float strafeStep; //+right
        float rotateStep; //+ right
};

/*
encapsulates only things the robot can percieve,
including its memory.

for example, typically excludes absolute positions,
which the robot does not know about
*/
class Brain
{

    public:

        Brain();
        virtual ~Brain();

        /*
        updates brain output from a given input
        */
        virtual void pushInput(BrainInput* in) = 0;

        BrainOutput* getOutput();

    protected:

        BrainOutput curOutput;

};

/*
one possible brain algorithm

create a new class for each new algorithm
*/
class RobotBrain : public Brain
{
    public:

        RobotBrain();
        virtual ~RobotBrain();

        virtual void pushInput(BrainInput* in);
};

/*
for human controlled test runs
*/
class HumanBrain : public Brain
{
    public:

        HumanBrain();
        virtual ~HumanBrain();

        virtual void pushInput(BrainInput* in);

        bool forwardDown;
        bool backwardDown;
        bool strafeLeftDown;
        bool strafeRightDown;
        bool rotateLeftDown;
        bool rotateRightDown;

};

/*IEventReceiver for test human input*/
class HumanReceiver : public IEventReceiver
{
    public:

        HumanReceiver(HumanBrain* brain);

        virtual bool OnEvent(const SEvent& event);

    private:

        HumanBrain* brain;

};

/*
contains all information about the robot,
including for example state information such as absolute position,
which the robot typically does not know about
*/
class Robot
{

    public:

        Robot();

        Robot
        (
            IrrlichtDevice* device,
            IMetaTriangleSelector* meta,
            Brain* brain,
            vector3df pos0,
            vector3df lookat0,
            float collisionRadius
        );

        /*
        gives current input to the brain,
        and updates brain output
        */
        void update();

        /*
        returns a string representation of class
        */
        std::string str();

        vector3df getPosition(){ return camera->getPosition(); }
        vector3df getTarget(){ return camera->getTarget(); }

    private:

        IVideoDriver* driver;
        ICameraSceneNode* camera; //also contains robot inner state: position and target
        ISceneNodeAnimatorCollisionResponse* anim;
        vector3df oldpos, oldlookat; //TEST
        Brain* brain;
        BrainInput in; //last brain input

        const static float forwardStepMax = 10;
        const static float strafeStepMax = 10;
        const static float rotateStepMax = 10;
        float pleasure;

        /*
        tries to update robot position given brain output.
        does not take collisions into account: this is taken into
        account elsewhere automatically
        */
        void actuate(BrainOutput* out);

        /*
        decides what will be the brain input
        from all the possibly extra information that describes
        the robot state
        */
        BrainInput getBrainInput();

};

#endif
