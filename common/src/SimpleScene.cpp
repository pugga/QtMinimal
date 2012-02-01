// Copyright NVIDIA Corporation 2009-2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#include "SimpleScene.h"

#include "SceneFunctions.h"
#include "nvsg/TextureHost.h"
#include "nvsg/TextureAttribute.h"

#include "nvsg/BlendAttribute.h"
#include "nvsg/CgFx.h"
#include "nvsg/DirectedLight.h"
#include "nvsg/Face.h"
#include "nvsg/FaceAttribute.h"
#include "nvsg/GeoNode.h"
#include "nvsg/Material.h"
#include "nvsg/Node.h"
#include "nvsg/Scene.h"
#include "nvsg/StateSet.h"
#include "nvsg/Transform.h"
#include "nvsg/Triangles.h"
#include "nvmath/nvmath.h"
#include "nvmath/Vecnt.h"
#include "MeshGenerator.h"
#include "nvutil/Tools.h"
#include "nvutil/DbgNew.h" // this must be the last include

#include <QApplication>
#include <QtCore/QDir>

using namespace nvsg;
using namespace nvmath;
using namespace nvutil;


SimpleScene::SimpleScene()
{  
    m_sceneHandle = Scene::create();

    WritableObject<Scene> theScene(m_sceneHandle);

    theScene->setBackColor(  Vec4f( 71.0f/255.0f, 111.0f/255.0f, 0.0f, 1.0f ) );

    // Create cube (or other shape)
//    m_drawable = createQuadSet(1,1);
//    m_drawable = createQuadStrip(10);
//    m_drawable = createTriSet(1,1);
    //m_drawable = createTriFan(50);
    //m_drawable = createTriStrip(10,1);
      m_drawable = createCube();
    //m_drawable = createTetrahedron();
    //m_drawable = createOctahedron();
    //m_drawable = createDodecahedron();
    //m_drawable = createIcosahedron();
//    m_drawable = createSphere(32,16);
//    m_drawable = createTorus(64,32);
//    m_drawable = createTessellatedPlane(1);
    //  m_drawable = createTessellatedBox(10);

    // Create BlendAttribute to render the transparent materials correctly.
    BlendAttributeSharedPtr blendAttribute = BlendAttribute::create();
    {
        BlendAttributeWriteLock ba( blendAttribute );
        ba->setBlendFunctions( BF_SOURCE_ALPHA, BF_ONE_MINUS_SOURCE_ALPHA );
    }

    // Render front and back faces of the transparent objects and do two sided ligting on them.
    FaceAttributeSharedPtr faceAttribute = FaceAttribute::create();
    {
        FaceAttributeWriteLock fa( faceAttribute );
        fa->setCullMode( false, false );
        fa->setTwoSidedLighting( true );
    }

    // Create four materials/textures
    StateSetSharedPtr stateSet[4];

    stateSet[0] = createDefinedMaterial( Vec3f( 0.0f, 0.0f, 0.0f ),  // ambientColor,
                                         Vec3f( 1.0f, 1.0f, 1.0f ),  // diffuseColor,
                                         Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
                                         20.0f,                      // specularExponent,
                                         Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
                                         1.0f,                       // opacity,
                                         0.5f,                       // reflectivity,
                                         1.0f );                     // indexOfRefraction


    {
        StateSetWriteLock ss( stateSet[0] );
        // No BlendAttribute or FaceAttribute needed for the opaque material.
        ss->setName( "White Material" );
    }

    stateSet[1] = createDefinedMaterial( Vec3f( 0.0f, 0.0f, 0.0f ),  // ambientColor
                                         Vec3f( 1.0f, 0.0f, 0.0f ),  // diffuseColor
                                         Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor
                                         40.0f,                      // specularExponent
                                         Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
                                         0.7f,                       // opacity
                                         0.3f,                       // reflectivity
                                         1.33f );  // Water          // indexOfRefraction
    {
        StateSetWriteLock ss( stateSet[1] );
        ss->addAttribute( blendAttribute );
        ss->addAttribute( faceAttribute );
        ss->setName( "Red Material" );
    }

    stateSet[2] = createDefinedMaterial( Vec3f( 0.0f, 0.0f, 0.0f ),  // ambientColor
                                         Vec3f( 0.0f, 1.0f, 0.0f ),  // diffuseColor
                                         Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor
                                         60.0f,                      // specularExponent
                                         Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
                                         0.5f,                       // opacity
                                         0.3f,                       // reflectivity
                                         1.45f );  // Glass          // indexOfRefraction
    {
        StateSetWriteLock ss( stateSet[2] );
        ss->addAttribute( blendAttribute );
        ss->addAttribute( faceAttribute );
        ss->setName( "Green Material" );
    }

    stateSet[3] = createDefinedMaterial( Vec3f( 0.0f, 0.0f, 0.0f ),  // ambientColor
                                         Vec3f( 0.0f, 0.0f, 1.0f ),  // diffuseColor
                                         Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor
                                         80.0f,                      // specularExponent
                                         Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
                                         0.3f,                       // opacity
                                         0.3f,                       // reflectivity
                                         1.7f );                     // indexOfRefraction
    {
        StateSetWriteLock ss( stateSet[3] );
        ss->addAttribute( blendAttribute );
        ss->addAttribute( faceAttribute );
        ss->setName( "Blue Material" );
    }

    /*
  stateSet[0] = createTexture();
  stateSet[1] = createTexture();
  stateSet[2] = createTexture();
  stateSet[3] = createTexture();
  */

    // create textures from the file
    std::vector<std::string> searchPaths;
    QString appPath = QApplication::applicationDirPath();
    QString imagePath = appPath + QString("../../../../images");
    imagePath = QDir::cleanPath(imagePath);

    searchPaths.push_back(QDir::toNativeSeparators(appPath).toStdString());
    searchPaths.push_back(QDir::toNativeSeparators(imagePath).toStdString());

    stateSet[0] = createTextureFromFile(std::string("test.png"), searchPaths);
    stateSet[1] = createTextureFromFile(std::string("test.png"), searchPaths);
    stateSet[2] = createTextureFromFile(std::string("test.png"), searchPaths);
    stateSet[3] = createTextureFromFile(std::string("test.png"), searchPaths);

    // Create four GeoNodes.
    GeoNodeSharedPtr node[4];
    for ( int i=0 ; i<4 ; i++ )
    {
        node[i] = GeoNode::create();
        WritableObject<GeoNode> wgn( node[i] );
        wgn->addDrawable( stateSet[i], m_drawable );
    }

    GeoNodeWriteLock( node[0] )->setName( "White Object" );
    GeoNodeWriteLock( node[1] )->setName( "Red Object" );
    GeoNodeWriteLock( node[2] )->setName( "Green Object" );
    GeoNodeWriteLock( node[3] )->setName( "Blue Object" );

    // Create four transforms. Cube coordinates are in the range [-1, 1], set them 3 units apart.
    m_transformHandle[0] = createTransform( node[0] );
    {
//        TransformWriteLock( m_transformHandle[0] )->setName( "White Object Transform" );
        TransformWriteLock tswl(m_transformHandle[0]);
        tswl->setName("White Object Transform");
        tswl->setScaling(Vec3f(1.0f, 1.0f, 2.0f));
    }

    m_transformHandle[1] = createTransform( node[1] , Vec3f( 3.0f, 0.0f, 0.0f ) );
    TransformWriteLock( m_transformHandle[1] )->setName( "Red Object Transform" );

    m_transformHandle[2] = createTransform( node[2] , Vec3f( 0.0f, 3.0f, 0.0f ) , Quatf( Vec3f( 0.0f, 1.0f, 0.0f ), 10.0f) );
    TransformWriteLock( m_transformHandle[2] )->setName( "Green Object Transform" );

    m_transformHandle[3] = createTransform( node[3] , Vec3f( 0.0f, 0.0f, 3.0f ) , Quatf( Vec3f( 0.0f, 0.0f, 1.0f ), 20.0f) );
    TransformWriteLock( m_transformHandle[3] )->setName( "Blue Object Transform" );

    // Create a point light
    m_pointLight = createPointLight( Vec3f( 5.0f, 8.0f, 6.0f ) );

    // Create the root
    GroupSharedPtr groupHdl = Group::create();
    {
        WritableObject<Group> wg(groupHdl);
        for ( int i=0 ; i<4 ; i++ )
        {
            wg->addChild( m_transformHandle[i] );
        }
        wg->addLightSource( m_pointLight );
        wg->setName( "Root Node" );
    }


    theScene->setRootNode( groupHdl );
}


SimpleScene::~SimpleScene()
{
}

