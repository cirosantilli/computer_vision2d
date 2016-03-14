#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <vector>

#include <irrlicht.h>

#include <robot.h>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;

namespace Fly2D
{
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
                float forwardStep = 0.0f,
                float strafeStep = 0.0f,
                float rotateStep = 0.0f
            );

            float forwardStep; //+forward
            float strafeStep;  //+right
            float rotateStep;  //+right
    };

    class Brain : public ::Brain<BrainInput,BrainOutput>
    {
        public:

            Brain();
            virtual ~Brain();

            /*
            updates brain inner state and current output
            */
            virtual void pushInput(BrainInput* in) = 0;
            virtual BrainOutput getOutput() = 0;
    };

    /*
    brain that goes always straight
    */
    class BrainForward : public Brain
    {
        virtual void pushInput(BrainInput* in);
        virtual BrainOutput getOutput();
    };

    /*
    brain that turns always in circles
    */
    class BrainCircle : public Brain
    {
        virtual void pushInput(BrainInput* in);
        virtual BrainOutput getOutput();
    };

    /*
    for human controlled brain
    */
    class BrainHuman : public Brain
    {
        public:

            BrainHuman();
            virtual ~BrainHuman();

            virtual void pushInput(BrainInput* in);
            virtual BrainOutput getOutput();

            bool forwardDown;
            bool backwardDown;
            bool strafeLeftDown;
            bool strafeRightDown;
            bool rotateLeftDown;
            bool rotateRightDown;
    };

    /*
    IEventReceiver for test human input

    takes only human fly brains
    */
    class ReceiverHuman : public IEventReceiver
    {
        public:

            ReceiverHuman(vector<BrainHuman*> brain);

            virtual bool OnEvent(const SEvent& event);

            int getMaxBrains();

        private:

            vector<BrainHuman*> brains;
    };

    class Robot : public ::RobotTemplate<BrainInput,BrainOutput>
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
            returns a string representation of class
            */
            virtual std::string str();

            vector3df getPosition(){ return camera->getPosition(); }
            vector3df getTarget(){ return camera->getTarget(); }

        private:

            IVideoDriver* driver;
            ISceneNodeAnimatorCollisionResponse* anim;
            vector3df oldpos, oldlookat; //TEST

            const static float forwardStepMax = 0.01;
            const static float strafeStepMax = 0.01;
            const static float rotateStepMax = 1.0;
            float pleasure;

            virtual BrainInput getBrainInput();

            /*
            linear, saturates at max min
            */
            void actuate(BrainOutput* out);
    };
}

#endif
