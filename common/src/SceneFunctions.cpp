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

#include <SceneFunctions.h>

#include <nvutil/PlugIn.h>
#include <nvsg/PlugInterface.h>
#include <nvsg/PlugInterfaceID.h>
#include <nvutil/Tools.h>
#include <nvsg/ErrorHandling.h>

#include <nvui/RendererOptions.h>

#include <nvsg/Scene.h>
#include <nvsg/ViewState.h>
#include <nvsg/PointLight.h>
#include <nvsg/PerspectiveCamera.h>

// optimize
#include <nvtraverser/AnalyzeTraverser.h>
#include <nvtraverser/CombineTraverser.h>
#include <nvtraverser/DestrippingTraverser.h>
#include <nvtraverser/EliminateTraverser.h>
#include <nvtraverser/IdentityToGroupTraverser.h>
#include <nvtraverser/NormalizeTraverser.h>
#include <nvtraverser/SearchTraverser.h>
#include <nvtraverser/StatisticsTraverser.h>
#include <nvtraverser/TriangulateTraverser.h>
#include <nvtraverser/UnifyTraverser.h>

// picking
#include <nvtraverser/RayIntersectTraverser.h>

#include <nvutil/DbgNew.h>

using namespace std;

using namespace nvsg;
using namespace nvui;
using namespace nvutil;
using namespace nvmath;
using namespace nvtraverser;

namespace nvutil
{

nvsg::ViewStateSharedPtr loadScene( const std::string & filename, const std::vector<std::string> &searchPaths, PlugInCallback *callback )
{
    ViewStateSharedPtr viewState;

    // appropriate search paths for the
    // loader dll and the sample file.
    string modulePath;
    string curDir;
    string dir;
    vector<string> localSearchPaths = searchPaths;

    nvutil::GetCurrentDir(curDir);
    localSearchPaths.push_back(curDir);

    nvutil::GetModulePath(modulePath);
    if ( find( localSearchPaths.begin(), localSearchPaths.end(), modulePath ) == localSearchPaths.end() )
    {
        localSearchPaths.push_back(modulePath);
    }

    // also add the scene path, if it's not the current path (-> dir.empty()) and not yet added
    nvutil::GetDrvAndDirFromPath(filename, dir);
    if ( !dir.empty() && find( localSearchPaths.begin(), localSearchPaths.end(), dir ) == localSearchPaths.end() )
    {
        localSearchPaths.push_back(dir);
    }

    // for supplied models add additional resource paths to make sure
    // that required resources will be found by subsequent loaders
    size_t pos = dir.rfind("media");
    if ( pos != string::npos )
    {
        string nvsgsdk(dir.substr(0, pos));
        localSearchPaths.push_back(nvsgsdk + "media/effects");
        localSearchPaths.push_back(nvsgsdk + "media/textures");
    }

    // receive the unique scene loader plug interface ID from the file extension
    string ext;
    GetFileExtFromPath( filename, ext );
    UPIID piid = nvutil::UPIID( ext.c_str(), nvutil::UPITID( UPITID_SCENE_LOADER, UPITID_VERSION ) );

    nvutil::PlugIn * plug;
    if ( nvutil::getInterface(localSearchPaths, piid, plug) )
    {
        SmartPtr<nvsg::SceneLoader> loader( reinterpret_cast<nvsg::SceneLoader*>(plug) );
        loader->setCallback( callback );

        SceneSharedPtr scene;
        scene = loader->load( filename, localSearchPaths, viewState );
        if ( !scene )
        {
            viewState.reset();
        }
        else
        {
            // create a new viewstate if necessary
            if ( !viewState )
            {
                viewState = ViewState::create();
            }
            ViewStateWriteLock( viewState )->setScene( scene );
        }

        loader->setCallback( NULL );
    }

    return viewState;
}

bool saveScene( const std::string & filename, const nvsg::ViewStateSharedPtr & viewState, PlugInCallback *callback )
{
    bool result = false;
    // define a unique plug-interface ID for SceneLoader
    const UPITID PITID_SCENE_SAVER(UPITID_SCENE_SAVER, UPITID_VERSION);

    string modulePath, dir;
    vector<string> searchPaths;
    GetCurrentDir(dir);
    searchPaths.push_back(dir);

    GetModulePath(modulePath);
    GetDirFromPath(modulePath, dir);
    searchPaths.push_back(dir);

    string ext;
    GetFileExtFromPath(filename, ext);
    UPIID piid = UPIID(ext.c_str(), PITID_SCENE_SAVER);

    PlugIn * plug;

    if ( getInterface( searchPaths, piid, plug ) )
    {
        SceneSaver *ss = reinterpret_cast<SceneSaver *>(plug);
        try
        {
            SceneSharedPtr scene( ViewStateReadLock(viewState)->getScene() ); // DAR HACK Change SceneSaver interface later.
            result = ss->save( scene, viewState, filename );
        }
        catch(const NVSGException& e) // catch unexpected SceniX errors
        {
            std::cerr << "SceniX critical error: " << e.getErrorMessage().c_str() << std::endl;
        }
        catch(...) // catch all others
        {
        }
    }

    return result;
}



bool setupDefaultViewState( const nvsg::ViewStateSharedPtr & viewState )
{
    if (!viewState)
    {
        return false;
    }

    CameraSharedPtr camera;
    SceneSharedPtr scene;
    RendererOptionsSharedPtr options;

    {
        ViewStateReadLock viewStateLock( viewState );
        camera  = viewStateLock->getCamera();
        scene   = viewStateLock->getScene();
        options = viewStateLock->getRendererOptions();
    }

    // the viewstate does not have an active camera, set one
    if ( !camera )
    {
        if( !scene )
        {
            return false;
        }

        SceneReadLock sceneLock(scene);

        // if there are cameras in the scene choose the first one
        if ( sceneLock->getNumberOfCameras() )
        {
            camera = *sceneLock->beginCameras();
        }
        else
        {
            // otherwise create a new one
            camera = nvsg::PerspectiveCamera::create();
            {
                PerspectiveCameraWriteLock cameraLock(camera);
                cameraLock->setName( "ViewCamera" );

                // Make scene fit into the viewport.
                if (sceneLock->getRootNode())
                {
                    Sphere3f sphere( sceneLock->getBoundingSphere() );
                    if ( isPositive(sphere) )
                    {
                        cameraLock->zoom( sphere, float(nvmath::PI_QUARTER) );
                    }
                }
            }
        }
        ViewStateWriteLock(viewState)->setCamera( camera );
    }

    // a viewstate needs a valid renderer options object
    if( !options )
    {
        ViewStateWriteLock(viewState)->setRendererOptions( RendererOptions::create() );
    }

    return true;
}

bool createDefaultHeadLight( const nvsg::CameraSharedPtr & camera, nvmath::Vec3f offset )
{
    if ( camera )
    {
        nvsg::PointLightSharedPtr pointLightHdl = nvsg::PointLight::create();
        {
            PointLightWriteLock pointLight(pointLightHdl);
            pointLight->setAmbientColor( nvmath::Vec3f(1.f, 1.f, 1.f) );
            pointLight->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 1.0f) );
            pointLight->setSpecularColor( nvmath::Vec3f(1.0f, 1.0f, 1.0f) );
            pointLight->setAttenuation( 1.0f, 0.0f, 0.0f );
            pointLight->setPosition( offset );  // Default offset is zero.
        }

        CameraWriteLock(camera)->addHeadLight( pointLightHdl);
    }

    return( !!camera );
}

void optimizeScene( const nvsg::SceneSharedPtr & scene, bool ignoreNames, bool identityToGroup
                    , unsigned int combineFlags, unsigned int eliminateFlags, unsigned int unifyFlags
                    , float epsilon )
{
    if ( identityToGroup )
    {
        SmartPtr<IdentityToGroupTraverser> itgt( new IdentityToGroupTraverser );
        itgt->setIgnoreNames( ignoreNames );
        itgt->apply( scene );
    }

    //  loop over optimizers until nothing changed
#define UNDEFINED_TRAVERSER ~0
#define ELIMINATE_TRAVERSER 0
#define COMBINE_TRAVERSER   1
#define UNIFY_TRAVERSER     2
    int lastModifyingTraverser = UNDEFINED_TRAVERSER;

    bool modified;
    do
    {
        modified = false;
        //  first eliminate redundant/degenerated objects
        if ( eliminateFlags && ( lastModifyingTraverser != ELIMINATE_TRAVERSER ) )
        {
            SmartPtr<EliminateTraverser> et( new EliminateTraverser );
            et->setIgnoreNames( ignoreNames );
            et->setEliminateTargets( eliminateFlags );
            et->apply( scene );
            if ( et->getTreeModified() )
            {
                modified = true;
                lastModifyingTraverser = ELIMINATE_TRAVERSER;
            }
        }

        // second combine compatible objects
        if ( combineFlags  && ( lastModifyingTraverser != COMBINE_TRAVERSER ) )
        {
            SmartPtr<CombineTraverser> ct( new CombineTraverser );
            ct->setIgnoreNames( ignoreNames );
            ct->setCombineTargets( combineFlags );
            ct->apply( scene );
            if ( ct->getTreeModified() )
            {
                modified = true;
                lastModifyingTraverser = COMBINE_TRAVERSER;
            }
        }

        // third unify all equivalent objects
        if ( unifyFlags  && ( lastModifyingTraverser != UNIFY_TRAVERSER ) )
        {
            SmartPtr<UnifyTraverser> ut( new UnifyTraverser );
            ut->setIgnoreNames( ignoreNames );
            ut->setUnifyTargets( unifyFlags );
            if ( unifyFlags & UnifyTraverser::UT_VERTICES )
            {
                ut->setEpsilon( epsilon );
            }
            ut->apply( scene );
            if ( ut->getTreeModified() )
            {
                modified = true;
                lastModifyingTraverser = UNIFY_TRAVERSER;

                if ( unifyFlags & UnifyTraverser::UT_VERTICES )
                {
                    // after unifying vertices we need to re-normalize the normals
                    SmartPtr<NormalizeTraverser> nt( new NormalizeTraverser );
                    nt->apply( scene );
                }
            }
        }
    } while( modified );
}

void optimizeForRaytracing( const nvsg::SceneSharedPtr & scene )
{
    bool ignoreNames = true;

    //  first some preprocessing optimizers
    //  -> no specific order here
    {
        SmartPtr<IdentityToGroupTraverser> tr( new IdentityToGroupTraverser );
        tr->setIgnoreNames( ignoreNames );
        tr->apply( scene );
    }

    {
        SmartPtr<DestrippingTraverser> tr( new DestrippingTraverser );
        tr->apply( scene );
    }

    {
        SmartPtr<TriangulateTraverser> tr( new TriangulateTraverser );
        tr->apply( scene );
    }

    //  loop over optimizers until nothing changed
    bool modified = false;
    float vertexEpsilon = FLT_EPSILON;
    do
    {
        //  first eliminate redundant/degenerated objects
        {
            SmartPtr<EliminateTraverser> tr( new EliminateTraverser );
            tr->setIgnoreNames( ignoreNames );
            tr->setEliminateTargets( EliminateTraverser::ET_ALL_TARGETS_MASK );
            tr->apply( scene );
            modified = tr->getTreeModified();
        }

        // second combine compatible objects
        {
            SmartPtr<CombineTraverser> tr( new CombineTraverser );
            tr->setIgnoreNames( ignoreNames );
            tr->setCombineTargets( CombineTraverser::CT_ALL_TARGETS_MASK );
            tr->apply( scene );
            modified = tr->getTreeModified();
        }
    } while( modified );
}

void optimizeUnifyVertices( const nvsg::SceneSharedPtr & scene )
{
    unsigned int unifySelection = 0;
    unifySelection |= UnifyTraverser::UT_VERTICES;

    SmartPtr<UnifyTraverser> tr( new UnifyTraverser );
    tr->setIgnoreNames( false );
    tr->setUnifyTargets( unifySelection );
    tr->setEpsilon( FLT_EPSILON );
    tr->apply( scene );

    // after unifying vertices we need to re-normalize the normals
    SmartPtr<NormalizeTraverser> nt( new NormalizeTraverser );
    nt->apply( scene );
}

SmartPtr<RayIntersectTraverser> applyPicker( const ViewStateSharedPtr &viewStatePtr, const SmartRenderTarget &renderTarget, int windowX, int windowY )
{
    NVSG_ASSERT( viewStatePtr );
    NVSG_ASSERT( renderTarget );

    ViewStateReadLock viewState( viewStatePtr );

    SmartPtr<RayIntersectTraverser> picker;

    unsigned int windowWidth, windowHeight;
    renderTarget->getSize( windowWidth, windowHeight );

    CameraSharedPtr pCam = viewState->getCamera();
    if ( pCam && isPtrTo<FrustumCamera>(pCam) ) // requires a frustum camera attached to the ViewState
    {
        picker = new RayIntersectTraverser;

        // calculate ray origin and direction from the input point
        Vec3f rayOrigin;
        Vec3f rayDir;

        FrustumCameraReadLock fc(sharedPtr_cast<FrustumCamera>(pCam));
        fc->getPickRay( windowX, windowHeight - 1 - windowY, windowWidth, windowHeight, rayOrigin, rayDir );

        // run the intersect traverser for intersections with the given ray
        picker->setRay( rayOrigin, rayDir );
        picker->setViewportSize( windowWidth, windowHeight );
        picker->apply( viewStatePtr );
    }
    return picker;
}

float getIntersectionDistance( const ViewStateSharedPtr &smartViewState, const SmartRenderTarget &renderTarget, int windowX, int windowY )
{
    float result = -1.0f;
    SmartPtr<RayIntersectTraverser> picker = applyPicker( smartViewState, renderTarget, windowX, windowY );
    if (picker && picker->getNumberOfIntersections() > 0)
    {
        result = picker->getNearest().getDist();
    }

    return result;
}

bool intersectObject( const nvsg::ViewStateSharedPtr &smartViewState, const SmartRenderTarget &renderTarget,
                      unsigned int windowX, unsigned int windowY, nvtraverser::Intersection & result )
{
    SmartPtr<RayIntersectTraverser> picker = applyPicker( smartViewState, renderTarget, windowX, windowY );
    if (picker && picker->getNumberOfIntersections() > 0)
    {
        result = picker->getNearest();
        return true;
    }

    return false;
}

bool saveTextureHost( const std::string & filename, const TextureHostSharedPtr & tih )
{
    string ext;
    GetFileExtFromPath(filename, ext);

    // load the saver plug-in  - this should be configured
    UPIID piid = UPIID(ext.c_str(), UPITID(UPITID_TEXTURE_SAVER, UPITID_VERSION) );

    PlugIn * plug = 0;

    std::string modulePath, dir;
    std::vector<std::string> searchPaths;
    GetCurrentDir(dir);
    searchPaths.push_back(dir);

    GetModulePath(modulePath);
    GetDirFromPath(modulePath, dir);
    searchPaths.push_back(dir);

    //
    // MMM - TODO - Update me for stereo images
    //
    TextureSaver * ts;
    if ( getInterface( searchPaths, piid, plug ) )
    {
        ts = reinterpret_cast<TextureSaver *>(plug);
    }
    else
    {
        // signal some kind of error
        return false;
    }

    bool retval;

    // save the file
    retval = ts->save( tih, filename );

    return retval;
}

bool loadTextureHost( const std::string & filename, TextureHostSharedPtr & tih, const std::vector<std::string> &searchPaths )
{
    tih.reset();
    // appropriate search paths for the loader dll and the sample file.
    string modulePath;
    string curDir;
    string dir;
    vector<string> localSearchPaths = searchPaths;

    nvutil::GetCurrentDir(curDir);
    localSearchPaths.push_back(curDir);

    nvutil::GetModulePath(modulePath);
    if ( find( localSearchPaths.begin(), localSearchPaths.end(), modulePath ) == localSearchPaths.end() )
    {
        localSearchPaths.push_back(modulePath);
    }

    // also add the texture's path, if it's not the current path (-> dir.empty()) and not yet added
    nvutil::GetDrvAndDirFromPath(filename, dir);
    if ( !dir.empty() && find( localSearchPaths.begin(), localSearchPaths.end(), dir ) == localSearchPaths.end() )
    {
        localSearchPaths.push_back(dir);
    }

    // HACK Add SceniX media folders for supplied models add additional resource paths
    // to make sure that required resources will be found by subsequent loaders.
    size_t pos = dir.rfind("media");
    if ( pos != string::npos )
    {
        string nvsgsdk( dir.substr(0, pos) );
        localSearchPaths.push_back(nvsgsdk + "media/effects");
        localSearchPaths.push_back(nvsgsdk + "media/textures");
    }

    string ext;
    GetFileExtFromPath(filename, ext);

    UPIID piid = UPIID( ext.c_str(), UPITID(UPITID_TEXTURE_LOADER, UPITID_VERSION) );

    PlugIn * plug = 0;

    // TODO - Update me for stereo images
    TextureLoader * tls;
    if ( getInterface( localSearchPaths, piid, plug ) )
    {
        tls = reinterpret_cast<TextureLoader *>(plug);
    }
    else
    {
        // signal some kind of error
        return false;
    }

    std::string foundFile;
    if ( FindFileFirst( filename, localSearchPaths, foundFile) ) // lookup the file
    {
        tih = tls->load( foundFile );
    }

    return tih;
}

} // namespace nvutil
