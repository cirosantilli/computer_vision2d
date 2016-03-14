#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <vector>

#include <irrlicht/irrlicht.h>
//for IImage and vector3df.
//consider removing this dependency with custom internai image format

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;

const vector3df Y = vector3df(0.f,1.f,0.f); //TODO put in a utils.h

/*
gets current inputs and returns current outputs

can have inner state

cannor perceive all of the Robot's variables such as
absolute positions
*/
template<class IN, class OUT>
class Brain
{
    public:

        Brain(){};
        virtual ~Brain(){};

        /*
        updates brain inner state and current output
        */
        virtual void pushInput(const IN& in) = 0;

        const OUT& getOutput() const { return curOutput; };

    protected:

        OUT curOutput;
};

class Robot
{
    public:

        /*
        gives current input to the brain,
        and updates brain output
        */
        virtual void update() = 0;

        /*
        returns a string representation of class
        */
        virtual std::string str() const = 0;

        ICameraSceneNode* camera;
        //also contains robot inner state: position and target
};

/*
contains all information about the robot,
including for example state information such as absolute position,
which the robot typically does not know about
*/
template<class IN, class OUT>
class RobotTemplate : public Robot
{
    public:

        RobotTemplate();

        RobotTemplate(Brain<IN,OUT>& brain) : brain(brain) {}

        /*
        gives current input to the brain,
        and updates brain output
        */
        virtual void update()
        {
            updateBrainInput();
            brain.pushInput( curBrainInput );
            actuate( brain.getOutput() );
        }


    protected:

        /*
        in general takes into account information
        which is not directly visible by the brain
        */
        virtual void updateBrainInput() = 0;

        /*
        tries to update robot position given brain output.
        does not take collisions into account: this is taken into
        account elsewhere automatically
        */
        virtual void actuate(const OUT& out) = 0;

        Brain<IN,OUT>& brain;
        IN curBrainInput;
};

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
            virtual void pushInput(const BrainInput& in) = 0;
    };

    /*
    brain that goes always straight
    */
    class BrainForward : public Brain
    {
        virtual void pushInput(const BrainInput& in);
    };

    /*
    brain that turns always in circles
    */
    class BrainCircle : public Brain
    {
        virtual void pushInput(const BrainInput& in);
    };

    /*
    for human controlled brain
    */
    class BrainHuman : public Brain
    {
        public:

            BrainHuman();
            virtual ~BrainHuman();

            virtual void pushInput(const BrainInput& in);

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

            int getMaxBrains() const;

        private:

            vector<BrainHuman*> brains;
    };

    class Robot : public ::RobotTemplate<BrainInput,BrainOutput>
    {
        public:

            Robot();

            Robot
            (
                IrrlichtDevice& device,
                IMetaTriangleSelector& meta,
                Brain& brain,
                vector3df pos0,
                vector3df lookat0,
                float collisionRadius
            );

            /*
            returns a string representation of class
            */
            virtual std::string str() const;

            vector3df getPosition(){ return camera->getPosition(); }
            vector3df getTarget(){ return camera->getTarget(); }

        protected:

            IVideoDriver* driver;
            ISceneNodeAnimatorCollisionResponse* anim;
            vector3df oldpos, oldlookat; //TEST

            const static float forwardStepMax;
            const static float strafeStepMax;
            const static float rotateStepMax;

            float pleasure;

            virtual void updateBrainInput();

            /*
            linear, saturates at max min
            */
            void actuate(const BrainOutput& out);
    };
}

#endif
