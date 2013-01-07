/*
 
realistic characteristic:
    first person perspective
    collision detection?.

missing realistic features
    - textures
    - large multijoint body
        - currently small spherical punctual body
    - intertia
        gravity
    - lightning
    - soft bodies
        - liquids
        - deformable solides (wires, sheets, masses)
        (still hard to simulate...)
    - control imprecisions

- doubts
    - movement redundancy (foward is redundant with backwards if one can turn,
        only it is faster to go back directly, same for strafe)

- scenarios
    - comunication 
    - invisible causes

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

- animate pleasure sources

#TODO

- make an outside cylinder instead of square

*/
#include <cmath>
#include <iostream>
#include <cstdlib>

#include <irrlicht.h>

#include "robot.h"

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

const bool test = true;       //test keyboard control vs automatic

//window
dimension2d<u32> windowSize = dimension2d<u32>(400, 400);
const bool fullScreen = false;

//robot
vector3df pos0 = vector3df(0,0,-.5f);
vector3df lookat0 = vector3df(0,0,.5f);
ICameraSceneNode* camera;
float rotateSpeed = 100.0f;
float moveSpeed = 0.0005f;
float collisionRadius = 0.01f;
float rotateStep = M_PI;
float moveStep = 0.01f;

int FruitID = 10;

class MyEventReceiver : public IEventReceiver
{
    public:
        virtual bool OnEvent(const SEvent& event)
        {

            //escape quit
            if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            {
                if ( event.KeyInput.PressedDown )
                {

                    if ( event.KeyInput.Key == KEY_KEY_A ){
                        //strafe left
                        vector3df d = Y.crossProduct(
                            camera->getTarget() -
                            camera->getPosition()
                        )*moveStep;
                        camera->setPosition(
                            camera->getPosition() - d
                        );
                        camera->setTarget(
                            camera->getTarget() - d
                        );
                        return true;
                    }

                    if ( event.KeyInput.Key == KEY_KEY_D ){
                        //strafe right
                        vector3df d = Y.crossProduct(
                            camera->getTarget() -
                            camera->getPosition()
                        )*moveStep;
                        camera->setPosition(
                            camera->getPosition() + d
                        );
                        camera->setTarget(
                            camera->getTarget() + d
                        );
                        return true;
                    }

                    if ( event.KeyInput.Key == KEY_KEY_E ){
                        //rotate right
                        vector3df t = vector3df(camera->getTarget());
                        t.rotateXZBy(
                            -rotateStep,
                            camera->getPosition()
                        );
                        camera->setTarget(t);
                        return true;
                    }

                    if ( event.KeyInput.Key == KEY_KEY_Q ){
                        //rotate left
                        vector3df t = vector3df(camera->getTarget());
                        t.rotateXZBy(
                            rotateStep,
                            camera->getPosition()
                        );
                        camera->setTarget(t);
                        return true;
                    }

                    if ( event.KeyInput.Key == KEY_KEY_S ){
                        //move back
                        camera->setPosition(
                            camera->getPosition() -
                            (
                                camera->getTarget() -
                                camera->getPosition()
                            )*moveStep
                        );
                        return true;
                    }

                    else if ( event.KeyInput.Key == KEY_KEY_W ){
                        //move forward
                        camera->setPosition(
                            camera->getPosition() +
                            (
                                camera->getTarget() -
                                camera->getPosition()
                            )*moveStep
                        );
                        return true;
                    }

                    //esc quit
                    else if ( event.KeyInput.Key == KEY_ESCAPE ){
                        exit(EXIT_SUCCESS);
                    }
                }
            }

            //send other events to camera
            if (camera)
                return camera->OnEvent(event);

            return false;
        }
};

int main()
{

//-- device ------------------------------------------------------------//

    MyEventReceiver receiver;
    IrrlichtDevice* device;

    // create device
    if(test)
    {
        device = createDevice(
            EDT_OPENGL,
            windowSize,
            16,
            fullScreen,
            false,
            false,
            &receiver
        );
    } else {
        device = createDevice(
            EDT_OPENGL,
            windowSize,
            16,
            fullScreen,
            false,
            false,
            NULL
        );
    }

    if (device == 0)
        return EXIT_FAILURE; // could not create selected driver.

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();

//-- lights ------------------------------------------------------------//

    //ambient light
    //smgr->setAmbientLight( SColorf(1.0f,1.0f,1.0f,1.0f) );
        //smgr->setAmbientLight( SColorf(.3f,.3f,.3f,1.0f) );

    //diffusive light
        SLight light_data;
        light_data.AmbientColor = SColorf(.3,.3,.3);
        //light_data.Attenuation = vector3df(.3,.3,.3); //Attenuation cte, linear quadratic TODO ??
        light_data.DiffuseColor = SColorf(.0,.0,.0);
        light_data.SpecularColor = SColorf(.0,.0,.0);
        light_data.CastShadows = true;
        light_data.Radius = 100.0f;
        light_data.Type = ELT_DIRECTIONAL;
            //ELT_POINT 	point light, it has a position in space and radiates light in all directions
            //ELT_SPOT    spot light, it has a position in space, a direction, and a limited cone of influence
            //ELT_DIRECTIONAL 	directional light, coming from a direction from an infinite distance 
            
        ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(.5f,.0f,.5f));
        light->setLightData(light_data);

//-- objects ------------------------------------------------------------//

    IMesh* mesh;
    ISceneNode * node;
    float HEIGHT=1000.f, WIDTH=1.f;
    //height between center/sky == height bottom/center
    //large so that scene looks 2d on interactive test mode.
    //on automatic mode, only middle pixel row is taken, so this does not matter

    //outter boundary
        
        //square
            node = smgr->addCubeSceneNode(
                2.f*WIDTH,  // width
                0,      // parent
                -1,     // id
                vector3df(0, 0, 0),               // center
                vector3df(0, 0, 0),               // rotation
                vector3df(1.0f, HEIGHT, 1.0f)*-1.f    // scale. *-1 turns it inside out. to use both faces make two cubes.
            );

        //circle
            //mesh = smgr->getGeometryCreator()->createCylinderMesh(
                //1.f,    //radius
                //1.,     //length
                //50,     //tesselation
                //SColor(0,0,0,255),     //color
                //false,                 //closeTop
                //0.f                    //oblique
            //);
            //node = smgr->addMeshSceneNode(
                //mesh,
                //0,     //ISceneNode * parent
                //-1,    //s32 id
                //vector3df(0, -HEIGHT, 0),            //const core::vector3df & position
                //vector3df(0, 0, 0),                     //const core::vector3df & rotation
                //vector3df(1.0f, 2.f*HEIGHT, 1.0f)      //const core::vector3df & scale
            //);

        node->getMaterial(0).AmbientColor.set(0,0,0,0);
        node->getMaterial(0).DiffuseColor.set(0,0,0,0);
        //node->getMaterial(0).SpecularColor.set(255,255,255,255);
        //node->getMaterial(0).Shininess = 20.0f;
        //node->getMaterial(0).EmissiveColor.set(0,0,0,0);
        //node->setMaterialFlag(EMF_WIREFRAME,true); //wireframe only

    // left cube
        node = smgr->addCubeSceneNode(
            0.2,    // width
            0,      // parent
            -1,     // id
            vector3df(-.3, 0, 0),       // center
            vector3df(0, 0, 0),           // rotation
            vector3df(1.0f, HEIGHT, 1.0f)   // scale
        );
        node->getMaterial(0).AmbientColor.set(0,255,0,0);
        node->getMaterial(0).DiffuseColor.set(0,255,0,0);

    // right cube
        node = smgr->addCubeSceneNode(
            .2f,    // width
            0,      // parent
            -1,     // id
            vector3df(.3, 0, 0),        // center
            vector3df(0, 0, 0),           // rotation
            vector3df(1.0f, HEIGHT, 1.0f)   // scale
        );
        node->getMaterial(0).AmbientColor.set(0,0,255,0);
        node->getMaterial(0).DiffuseColor.set(0,0,255,0);

    //cylinder
        mesh = smgr->getGeometryCreator()->createCylinderMesh(
            .1f,    //radius
            1.,     //length
            50,     //tesselation
            SColor(),              //color
            false,                 //closeTop
            0.f                    //oblique
        );
        node = smgr->addMeshSceneNode(
            mesh,
            0,     //ISceneNode * parent
            -1,    //s32 id
            vector3df(0, -HEIGHT, 0),            //const core::vector3df & position
            vector3df(0, 0, 0),                  //const core::vector3df & rotation
            vector3df(1.0f, 2.*HEIGHT, 1.0f)     //const core::vector3df & scale
        );
        node->getMaterial(0).AmbientColor.set(0,0,0,255);
        node->getMaterial(0).DiffuseColor.set(0,0,0,255);

    //sphere
        //node = smgr->addSphereSceneNode(
            //0.1,     // radius
            //50,     // poly count
            //0,      // parent
            //FruitID,     // id
            //vector3df(0, 0, 0),         // center
            //vector3df(0, 0, 0),           // rotation
            //vector3df(1.0f, 1.0f, 1.0f)   // scale
        //);
        //node->getMaterial(0).AmbientColor.set(0,0,0,255);
        //node->getMaterial(0).DiffuseColor.set(0,0,0,255);
        //node->getMaterial(0).Lighting = true;

//-- camera ------------------------------------------------------------//

    camera = smgr->addCameraSceneNode(
        0,
        vector3df(0,0,0),
        vector3df(0,100,0),
        -1,
        true
    );
    camera->bindTargetAndRotation(true);

    // camera initial position
    device->getCursorControl()->setVisible(true);
    camera->setPosition(pos0);
    camera->setTarget(lookat0);
    camera->setUpVector(vector3df(0,1,0));
    //camera->setAutomaticCulling(EAC_OFF);

    camera->setNearValue(collisionRadius*.99f);
    camera->setFarValue(100.f);

//-- collision ------------------------------------------------------------//

    /* Put everything we want to do collision checking with inside the meta selector. */
    IMetaTriangleSelector * meta = smgr->createMetaTriangleSelector();
    array<ISceneNode *> nodes;
    smgr->getSceneNodesFromType(ESNT_ANY, nodes); // Find all nodes

    for (u32 i=0; i < nodes.size(); ++i)
    {
        ISceneNode * node = nodes[i];
        ITriangleSelector * selector = 0;

        switch(node->getType())
        {
            case ESNT_CUBE:
            case ESNT_ANIMATED_MESH:
            // Because the selector won't animate with the mesh,
            // and is only being used for camera collision, we'll just use an approximate
            // bounding box instead of ((IAnimatedMeshSceneNode*)node)->getMesh(0)
            selector = smgr->createTriangleSelectorFromBoundingBox(node);
            break;

            case ESNT_MESH:
            case ESNT_SPHERE: // Derived from IMeshSceneNode
            selector = smgr->createTriangleSelector(((IMeshSceneNode*)node)->getMesh(), node);
            break;

            case ESNT_TERRAIN:
            selector = smgr->createTerrainTriangleSelector((ITerrainSceneNode*)node);
            break;

            case ESNT_OCTREE:
            selector = smgr->createOctreeTriangleSelector(((IMeshSceneNode*)node)->getMesh(), node);
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

    /*Do the collision checking between camera the and the rest of the world.*/
    ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
        meta,
        camera,
        vector3df(1.f,1.f,1.f)*collisionRadius,     //size of colision ball around camera. default vector3df(30, 60, 30) 
        vector3df(0,0,0),     // gravity
        vector3df(0,0,0));    //eye position
    meta->drop(); // As soon as we're done with the selector, drop it.
    camera->addAnimator(anim);
//    anim->drop();  // And likewise, drop the animator when we're done referring to it.

//-- robot ------------------------------------------------------------//

    //create robot
    camera->setPosition(pos0);
    camera->setTarget(lookat0);
    Robot robot = Robot(driver, camera, anim);

//-- run ------------------------------------------------------------//
	while(device->run())
	{
        if (device->isWindowActive())
        {
            driver->beginScene(true, true, 0);
            smgr->drawAll();
            driver->endScene();
            if(!test)
                robot.update();
            cout << robot.str();
        }
	}
	device->drop();
	return 0;
}
