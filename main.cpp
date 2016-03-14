/*
realistic characteristic:

- first person perspective
- collision detection?.

Missing realistic features:

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

Doubts

- movement redundancy (foward is redundant with backwards if one can turn,
    only it is faster to go back directly, same for strafe)

Scenarios:

- comunication
- invisible causes

Actions are taken based on an utility maximization/learning procedure.

I want to add a minimal amount of:

- pleasure pain primitives

    hunger, fear, "beauty", sex

- innate knowledge

So that the robot can robot can learn to solve rather complicated puzzles only from
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
- arbitrary frame rate

    kewords:

        frame rate
        offscreen rendering
        render to texture
        framebuffer
        renderbuffer

        glTexImage2D

    links:

        http://stackoverflow.com/questions/3191978/how-to-use-glut-opengl-to-render-to-a-file
        http://www.songho.ca/opengl/gl_fbo.html
        http://www.opengl.org/wiki/Framebuffer_Object
        http://stackoverflow.com/questions/12157646/how-to-render-offscreen-on-opengl

*/
#include <cmath>
#include <iostream>
#include <cstdlib>

#include <irrlicht/irrlicht.h>

#include "robot.h"

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

static const bool test = false;       //test keyboard control vs automatic

//window
dimension2d<u32> windowSize = dimension2d<u32>(700, 700);

//robot
float rotateSpeed = 100.0f;
float moveSpeed = 0.0005f;
float collisionRadius = 0.01f;
float rotateStep = M_PI;
float moveStep = 0.01f;

int FruitID = 10;

int main()
{
    const int nRobots = 2;
    //given robot numbers will be controlled by humans
    vector<int> humanBrainIds; 
    //humanBrainIds.push_back(1);
    //humanBrainIds.push_back(0);

//-- device ------------------------------------------------------------//

    IrrlichtDevice* device;

    device = createDevice(
        EDT_OPENGL, //driverType
        windowSize,
        16,  //bits
        false,
        false, //stencilbuffer
        false, //vsync
        NULL //receiver
    );
    //advanced device params
        //SIrrlichtCreationParameters params;
        //params.DeviceType = EIDT_CONSOLE;
        //params.DriverType = EDT_OPENGL;
        //params.WindowSize = windowSize;
        //device = createDeviceEx(params);

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

    //left cube
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

    //right cube
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
        //mesh = smgr->getGeometryCreator()->createCylinderMesh(
            //.1f,    //radius
            //1.,     //length
            //50,     //tesselation
            //SColor(),              //color
            //false,                 //closeTop
            //0.f                    //oblique
        //);
        //node = smgr->addMeshSceneNode(
            //mesh,
            //0,     //ISceneNode * parent
            //-1,    //s32 id
            //vector3df(0, -HEIGHT, 0),            //const core::vector3df & position
            //vector3df(0, 0, 0),                  //const core::vector3df & rotation
            //vector3df(1.0f, 2.*HEIGHT, 1.0f)     //const core::vector3df & scale
        //);
        //node->getMaterial(0).AmbientColor.set(0,0,0,255);
        //node->getMaterial(0).DiffuseColor.set(0,0,0,255);

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

//-- robots ------------------------------------------------------------//

    //create robots
    Fly2D::Brain* brains[nRobots];


    //all to a default type
    //for ( int i=0; i<nRobots; i++ )
    //{
        //brains[i] = new Fly2D::BrainForward;
        ////brains[i] = new Fly2D::BrainCircle();
    //}

    brains[0] = new Fly2D::BrainForward;
    //brains[1] = new Fly2D::BrainForward;
    brains[1] = new Fly2D::BrainCircle;

    //decide human control
        vector<Fly2D::BrainHuman*> hBrains;
        for (
            vector<int>::iterator i = humanBrainIds.begin();
            i != humanBrainIds.end();
            ++i
        )
        {
            if ( *i > nRobots )
            {
                cerr << "no such robot: " << *i << endl;
                exit(EXIT_FAILURE);
            }
            delete brains[*i];
            Fly2D::BrainHuman* hBrain = new Fly2D::BrainHuman;
            brains[*i] = hBrain;
            hBrains.push_back(hBrain);
        }
        Fly2D::ReceiverHuman hReceiver = Fly2D::ReceiverHuman( hBrains );
        device->setEventReceiver( &hReceiver );

    Robot* robots[nRobots];
    robots[0] = new Fly2D::Robot( *device, *meta, *brains[0], vector3df(0,0,-0.5f), vector3df(0,0, 0.5f),  0.01 );
    robots[1] = new Fly2D::Robot( *device, *meta, *brains[1], vector3df(0,0,0.5f),  vector3df(0,0, -0.5f), 0.01 );

    meta->drop(); // As soon as we're done with the selector, drop it.

//-- run ------------------------------------------------------------//

    //TEST
    vector3df oldpos, oldtarget;
    //END TEST

    int nFrames = 0;
    ITimer* timer = device->getTimer();
    int t0 = timer->getTime();
    int w = driver->getScreenSize().Width;
    int h = driver->getScreenSize().Height;
    int dh = h/nRobots;
    int observeRobot = 0;

	while(device->run())
	{
        //if (device->isWindowActive()) //only work if window has focus.

        //draw

        driver->setViewPort(rect<s32>(0,0,w,h));
        driver->beginScene(true,true,0);
        for(int i=0; i<nRobots; i++)
        {
            driver->setViewPort(rect<s32>(0,dh*i,w,dh*(i+1)));
            //only part of window gets drawn into

            smgr->setActiveCamera(robots[i]->camera);

            smgr->drawAll(); 
            //draws on window scene of active camera
 
            robots[i]->update();
        }
        driver->endScene();

        //TEST
        //if
        //(
            //robots[observeRobot].getPosition() != oldpos
            //|| robots[observeRobot].getTarget() != oldtarget
        //)
        //oldpos = robots[observeRobot].getPosition();
        //oldtarget = robots[observeRobot].getTarget();

        cout << robots[observeRobot]->str();

        //FPS info
            //cout << "frame no:" << endl << nFrames << endl;;
            //cout << "average fps:" << endl << 1000*nFrames/(float)(timer->getTime()-t0) << endl;
            //nFrames++;
            cout << "fps:" << endl << driver->getFPS() << endl;
        //END TEST
	}
	device->drop();
	return 0;
}
