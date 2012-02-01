#include <iostream>

#include <QApplication>
#include <QKeyEvent>
#include <QTime>
#include <QMessageBox>

#include <nvsg/nvsg.h>
#include <nvrt/RTInit.h>

#include "SceneFunctions.h"
#include "SimpleScene.h"
#include <nvsg/Scene.h>
#include <nvsg/ViewState.h>

#include <nvgl/SceneRendererGL2.h>
#include <nvrt/SceneRendererRTBuiltinGL.h>

#include "SceniXQGLWidget.h"
#include "SceniXQGLSceneRendererWidget.h"

#include "ui/TrackballCameraManipulatorHIDSync.h"

#include "nvtraverser/CombineTraverser.h"
#include "nvtraverser/EliminateTraverser.h"
#include "nvtraverser/UnifyTraverser.h"

#include <nvutil/DbgNew.h>  // enable leak detection

using namespace nvsg;
using namespace nvgl;
using namespace nvui;
using namespace nvrt;
using namespace nvutil;
using namespace nvtraverser;

// Add extra HUD Text rendering support
class QtMinimalWidget : public SceniXQGLSceneRendererWidget
{
public:
    QtMinimalWidget( const RenderContextGLFormat &format );
    virtual ~QtMinimalWidget();

    void keyPressEvent( QKeyEvent *event);
    void screenshot();

protected:
    TrackballCameraManipulatorHIDSync *m_trackballHIDSync;

    QTime           m_time;
};

QtMinimalWidget::QtMinimalWidget( const RenderContextGLFormat &format )
    : SceniXQGLSceneRendererWidget(0, format )
{
    m_trackballHIDSync = new TrackballCameraManipulatorHIDSync( );
    m_trackballHIDSync->setHID( this );
    m_trackballHIDSync->setRenderTarget( getRenderTarget() );
    setManipulator( m_trackballHIDSync );
}

QtMinimalWidget::~QtMinimalWidget()
{
    // Delete SceneRenderer here to cleanup resources before the OpenGL context dies
    setSceneRenderer( 0 );

    // Reset Manipulator
    setManipulator( 0 );
    delete m_trackballHIDSync;
}

void QtMinimalWidget::keyPressEvent( QKeyEvent *event )
{
    SceniXQGLWidget::keyPressEvent( event );

    if (event->text().compare("s") == 0)
    {
        screenshot();
    }

    if ( event->text().compare( "o" ) == 0 )
    {
        optimizeScene( ViewStateReadLock( getViewState() )->getScene(), true, true, CombineTraverser::CT_ALL_TARGETS_MASK
                       , EliminateTraverser::ET_ALL_TARGETS_MASK, UnifyTraverser::UT_ALL_TARGETS_MASK, FLT_EPSILON );
    }

    if (event->text().compare("x") == 0)
    {
        nvgl::RenderContextGLFormat format = getFormat();
        format.setStereo( !format.isStereo() );
        setFormat( format );
    }
}

void QtMinimalWidget::screenshot()
{
    std::string filename = getRenderTarget()->isStereoEnabled() ? "stereo.pns" : "mono.png";
    saveTextureHost( filename, getRenderTarget()->getTextureHost( ) );
}

int runApp( int argc, char *argv[], const std::string &filename, bool stereo, bool raytracing, bool continuous, GLObjectRenderer::CacheMode cacheMode, bool headlight )
{
    QApplication app( argc, argv );

    // Create rendering engine
    SmartSceneRenderer renderer = raytracing ? smart_cast<SceneRenderer>(SceneRendererRTBuiltinGL::create())
                                             : smart_cast<SceneRenderer>(SceneRendererGL2::create());

    // Create a simple scene
    SimpleScene simpleScene;

    // Setup viewstate for the simple scene
    ViewStateSharedPtr viewStateHandle;
    SceneSharedPtr scene = simpleScene.m_sceneHandle;
    if ( !filename.empty() )
    {
        viewStateHandle = loadScene( filename );
    }
    if (!viewStateHandle)
    {
        viewStateHandle  = ViewState::create();
        ViewStateWriteLock(viewStateHandle)->setScene( scene );
    }
    setupDefaultViewState( viewStateHandle );
    if ( headlight )
    {
        ViewStateReadLock vs(viewStateHandle);
        if ( vs && vs->getScene() && !SceneReadLock( vs->getScene() )->containsLight()
             && vs->getCamera() && ( CameraReadLock( vs->getCamera() )->getNumberOfHeadLights() == 0 ) )
        {
            createDefaultHeadLight( vs->getCamera() );
        }
    }

    if ( !raytracing )
    {
        RendererOptionsSharedPtr ro = RendererOptions::create();
        renderer->addRendererOptions( ro );
        RendererOptionsWriteLock(ro)->setValue( GLObjectRenderer::getOptionStringCacheMode(), cacheMode );
        ViewStateWriteLock(viewStateHandle)->setRendererOptions( ro );
    }

    // Setup default OpenGL format descriptor
    // We need to create a default format first to be able to check if a stereo pixelformat is available later.
    // (An unfortunate RenderContextGLFormat.isAvailable() interface due to Linux.)
    RenderContextGLFormat format, changedFormat;

    bool avail = format.isAvailable();

    // create a widget which shows the scene
    QtMinimalWidget w( format );

    avail = format.isAvailable();
    if(w.getFormat().isAvailable())
    {
        changedFormat = w.getFormat();
    }


    if (stereo)
    {
        format.setStereo( stereo );
        if ( !w.setFormat( format ) )  // This automatically checks if the format is available.
        {
            std::cout << "Warning: No stereo pixelformat available." << std::endl;
        }
    }

    w.setViewState( viewStateHandle );
    w.setSceneRenderer( renderer );
    w.setContinuousUpdate( continuous );
    w.resize( 640, 480 );
    w.show();

    // Keep only once reference to the renderer in the widget. This is necessary since the OpenGL resources
    // used by the renderer must be deleted before the window gets destroyed.
    renderer.reset();

    int result = app.exec();

    return result;
}

int QtMinimalAssert( const char *message)
{
#if 0
    // FIXME crashes with assertions on exit
    QMessageBox box;
    box.setText( message );
    box.setIcon( QMessageBox::Critical );
    box.exec();
#else
    std::cerr << message << std::endl;
#endif

    return 0;
}


int main(int argc, char *argv[])
{
    nvsgInitialize( );
#if !defined(NDEBUG)
    nvsg::nvsgSetDebugFlags( nvsg::NVSG_DBG_ASSERT );
    nvutil::SetUserAssertCallback( &QtMinimalAssert );
#endif
    RTInit();

    std::cout << "Usage: QtMinimal [--filename <filename>] [--stereo] [--raytracing] [--cachemode none|vbo|dl] [--continuous] [--headlight]" << std::endl;
    std::cout << "During execution hit 's' for screenshot and 'x' to toggle stereo" << std::endl;
    std::cout << "Stereo screenshots will be saved as side/side png with filename 'stereo.pns'." << std::endl;
    std::cout << "They can be viewed with the 3D Vision Photo Viewer." << std::endl;

    bool stereo = false;
    bool raytracing = false;
    bool continuous = false;
    bool headlight = false;
    std::string filename;
    GLObjectRenderer::CacheMode cacheMode = GLObjectRenderer::CACHEMODE_VBO;

    for (int arg = 0;arg < argc;++arg)
    {
        if ( strcmp( "--filename", argv[arg] ) == 0 )
        {
            ++arg;
            if ( arg < argc )
            {
                filename = argv[arg];
            }
        }
        if ( strcmp( "--stereo", argv[arg] ) == 0 )
        {
            stereo = true;
        }
        if ( strcmp( "--raytracing", argv[arg] ) == 0 && RTIsSupported() )
        {
            raytracing = true;
        }
        if ( strcmp( "--continuous", argv[arg] ) == 0 )
        {
            continuous = true;
        }

        if ( strcmp( "--cachemode", argv[arg] ) == 0 )
        {
            ++arg;
            if ( strcmp( "none", argv[arg] ) == 0)
            {
                cacheMode = GLObjectRenderer::CACHEMODE_UNCACHED;
            }
            if ( strcmp( "dl", argv[arg] ) == 0)
            {
                cacheMode = GLObjectRenderer::CACHEMODE_DL;
            }
            if ( strcmp( "vbo", argv[arg] ) == 0)
            {
                cacheMode = GLObjectRenderer::CACHEMODE_VBO;
            }
        }

        if ( strcmp( "--headlight", argv[arg] ) == 0 )
        {
            headlight = true;
        }
    }

    int result = runApp( argc, argv, filename, stereo, raytracing, continuous, cacheMode, headlight );

    RTShutdown();
    nvsgTerminate();

    return result;
}
