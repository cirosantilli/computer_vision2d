/** First person, small spherical body (1/XXXX) of smallest dimension, non intertial, no gravity in n-dimentional world with collision detection and realistic light sources?.

Actions are taken based on an utility maximization/learning procedure.

I want to add a minimal amount of:

 - pleasure pain primitives
    hunger, fear, "beauty", sex
 - innate knowledge

 so that the robot can robot can learn to solve rather complicated puzzles only from
 those basis.

 I consider puzzels which rassemble situations as natural as possible:

 - pleasure/pain touch sources.
    eat a fruit (static pleasure source),
    catch and eat an animal(dynamic pain resource)
    be eaten by an animal (dynamic pain source)
    to touch fire (static pain source)

- Animate pleasure sources

*/
#include <iostream>

#include <irrlicht/irrlicht.h>

#include "robot.h"

using namespace irr;

int main()
{

//-- parameters ------------------------------------------------------------//

    // main parameters
    bool test = 1;       // a first person shooter test or not

    core::dimension2d<u32> windowSize;      // window size
    if(test){
        windowSize = core::dimension2d<u32>(600, 400);
    } else {
        windowSize = core::dimension2d<u32>(600, 10);
    }

    core::vector3df initialPos = core::vector3df(0,0,-500);
    core::vector3df initialTarget = core::vector3df(0,0,0);

    int FruitID = 10;

//-- device ------------------------------------------------------------//

    // create device
	IrrlichtDevice *device =
		createDevice(video::EDT_OPENGL, windowSize, 16, false);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

//-- lights ------------------------------------------------------------//

    // add diffusive light
    smgr->addLightSceneNode 	(
            0,                         //parent
            core::vector3df(0, 0, 0),  // position
            video::SColorf(1.0f, 1.0f, 1.0f), //color
            1.0f,     // radius
            -1          // id
	);

    // add ambient light
	smgr->setAmbientLight(video::SColorf(1.0f,1.0f,1.0f,1.0f));

//-- objects ------------------------------------------------------------//

    scene::ISceneNode * node;

    float width = 1000;
    // grey outter boundary
    node = smgr->addCubeSceneNode(
        width,    // width
        0,      // parent
        -1,     // id
        core::vector3df(0, 0, 0),               // center
		core::vector3df(0, 0, 0),               // rotation
		core::vector3df(1.0f, 1.0f, 1.0f)*-1    // scale. *-1 turns it inside out. to use both faces make two cubes.
    );

    node->getMaterial(0).AmbientColor.set(0,0,0,0);
    node->getMaterial(0).DiffuseColor.set(0,0,0,255);
    node->getMaterial(0).Lighting = true;

    // left cube
    node = smgr->addCubeSceneNode(
        100,    // width
        0,      // parent
        -1,     // id
        core::vector3df(-150, 0, 0),        // center
    core::vector3df(0, 0, 0),           // rotation
    core::vector3df(1.0f, 1.0f, 1.0f)   // scale
    );

    node->getMaterial(0).AmbientColor.set(0,255,0,0);
    node->getMaterial(0).DiffuseColor.set(0,0,0,255);
    node->getMaterial(0).Lighting = true;

    // right cube
    node = smgr->addCubeSceneNode(
        100,    // width
        0,      // parent
        -1,     // id
        core::vector3df(150, 0, 0),         // center
    core::vector3df(0, 0, 0),           // rotation
    core::vector3df(1.0f, 1.0f, 1.0f)   // scale
    );

    node->getMaterial(0).AmbientColor.set(0,0,255,0);
    node->getMaterial(0).DiffuseColor.set(0,0,0,255);
    node->getMaterial(0).Lighting = true;

    // right cube
    node = smgr->addCubeSceneNode(
        200,    // width
        0,      // parent
        FruitID,     // id
        core::vector3df(0, 0, 0),         // center
    core::vector3df(0, 0, 0),           // rotation
    core::vector3df(1.0f, 1.0f, 1.0f)   // scale
    );

    node->getMaterial(0).AmbientColor.set(0,0,255,0);
    node->getMaterial(0).DiffuseColor.set(0,0,0,255);
    node->getMaterial(0).Lighting = true;

    // center blue ball
    node = smgr->addSphereSceneNode(
        100,     // radius
        50,     // poly count
        0,      // parent
        FruitID,     // id
        core::vector3df(0, 0, 0),         // center
        core::vector3df(0, 0, 0),           // rotation
        core::vector3df(1.0f, 1.0f, 1.0f)   // scale
    );

    node->getMaterial(0).AmbientColor.set(0,0,255,0);
    node->getMaterial(0).DiffuseColor.set(0,0,255,0);
    node->getMaterial(0).Lighting = true;

//-- camera ------------------------------------------------------------//

    // User controled tries
    scene::ICameraSceneNode* camera;

    if(test){ // FPS camera for tests

        // define controls
        SKeyMap keyMap[8];
                 keyMap[0].Action = EKA_MOVE_FORWARD;
                 keyMap[0].KeyCode = KEY_UP;
                 keyMap[1].Action = EKA_MOVE_FORWARD;
                 keyMap[1].KeyCode = KEY_KEY_W;

                 keyMap[2].Action = EKA_MOVE_BACKWARD;
                 keyMap[2].KeyCode = KEY_DOWN;
                 keyMap[3].Action = EKA_MOVE_BACKWARD;
                 keyMap[3].KeyCode = KEY_KEY_S;

                 keyMap[4].Action = EKA_STRAFE_LEFT;
                 keyMap[4].KeyCode = KEY_LEFT;
                 keyMap[5].Action = EKA_STRAFE_LEFT;
                 keyMap[5].KeyCode = KEY_KEY_A;

                 keyMap[6].Action = EKA_STRAFE_RIGHT;
                 keyMap[6].KeyCode = KEY_RIGHT;
                 keyMap[7].Action = EKA_STRAFE_RIGHT;
                 keyMap[7].KeyCode = KEY_KEY_D;

        camera = smgr->addCameraSceneNodeFPS(
            0,
            100.0f,     //rotate speed
            0.1f,       // move speed
            -1,
            keyMap,
            8,
            false,       // noVertical movement
            3.f          // jump speed
        );
    } else { // autonomous robot camera, no keyboard controls

        camera = smgr->addCameraSceneNode 	(
            0,
            core::vector3df(0,0,0),
            core::vector3df(0,100,0),
            -1,
            true
        );
        camera->bindTargetAndRotation(true);

    }

    // camera initial position
    device->getCursorControl()->setVisible(true);
    camera->setPosition(initialPos);
    camera->setTarget(initialTarget);
    camera->setUpVector(core::vector3df(0,1,0));

//-- collision ------------------------------------------------------------//

    /* Put everything we want to do collision checking with inside the meta selector. */
	scene::IMetaTriangleSelector * meta = smgr->createMetaTriangleSelector();
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // Find all nodes

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i];
		scene::ITriangleSelector * selector = 0;

		switch(node->getType())
		{
		case scene::ESNT_CUBE:
		case scene::ESNT_ANIMATED_MESH:
			// Because the selector won't animate with the mesh,
			// and is only being used for camera collision, we'll just use an approximate
			// bounding box instead of ((scene::IAnimatedMeshSceneNode*)node)->getMesh(0)
			selector = smgr->createTriangleSelectorFromBoundingBox(node);
		break;

		case scene::ESNT_MESH:
		case scene::ESNT_SPHERE: // Derived from IMeshSceneNode
			selector = smgr->createTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
			break;

		case scene::ESNT_TERRAIN:
			selector = smgr->createTerrainTriangleSelector((scene::ITerrainSceneNode*)node);
			break;

		case scene::ESNT_OCTREE:
			selector = smgr->createOctreeTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
			break;

		default:
			// Don't create a selector for this node type
			break;
		}

		if(selector)
		{
			// Add it to the meta selector, which will take a reference to it
			meta->addTriangleSelector(selector);
			// And drop my reference to it, so that the meta selector owns it.
			selector->drop();
		}
	}

    /*Do the collision checking between the and the rest of the world.*/
    scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
        meta,
        camera,
        core::vector3df(1,1,1)*10,     // size of colision ball around camera. cannot be < 1, don't ask me why, but then it passes walls
        core::vector3df(0,0,0),     // gravity
        core::vector3df(0,0,0));    //eye position
    meta->drop(); // As soon as we're done with the selector, drop it.
    camera->addAnimator(anim);
//    anim->drop();  // And likewise, drop the animator when we're done referring to it.

//-- robot ------------------------------------------------------------//

    // create our robot
    //Robot robot;
    //RobotInput robotInput;
    //RobotOutput* robotOutput;

    //core::vector3df curPos;
    //core::vector3df curTarget;
    //core::vector3df normalTarget;
    //core::vector3df vectorSpeed;

    //core::dimension2d<u32> cameraDimension;
    //video::IImage* curCameraImage;
    //bool touchFruit;

//-- run ------------------------------------------------------------//

	while(device->run())
	if (device->isWindowActive())
	{
		driver->beginScene(true, true, 0);
		smgr->drawAll();
		driver->endScene();

        // get input
        //curCameraImage = driver->createScreenShot();
        //cameraDimension = curCameraImage->getDimension();
        //robotInput.image = curCameraImage;

        //touchFruit = anim->collisionOccurred() && anim->getCollisionNode()->getID() == FruitID;
        //robotInput.touchFruit = touchFruit;

        //// update robot state
        //robot.pushInput(&robotInput);

        ////get robot output and act on the world
        //robotOutput = robot.getOutput();

        //curPos = camera->getPosition();
        //curTarget = camera->getTarget();

        //if(!test){ // update target and position
            //curTarget.rotateXZBy(robotOutput->rotation, curPos);
            //normalTarget = (curTarget-curPos).normalize();
            //vectorSpeed = normalTarget*robotOutput->speed;

            //camera->setPosition(curPos + vectorSpeed);
            //camera->setTarget(curTarget + vectorSpeed);
        //}

    //        printf( "%.0f %.0f %.0f %d \n", curPos.X, curPos.Y, curPos.Z, touchFruit );

	}

	device->drop();

	return 0;
}
