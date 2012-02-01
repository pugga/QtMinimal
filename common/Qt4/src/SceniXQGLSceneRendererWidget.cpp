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

#include "SceniXQGLSceneRendererWidget.h"
#include <nvsg/ViewState.h>

#include <nvutil/DbgNew.h>

using namespace nvgl;
using namespace nvsg;
using namespace nvmath;
using namespace nvtraverser;
using namespace nvutil;

SceniXQGLSceneRendererWidget::SceniXQGLSceneRendererWidget( QWidget *parent, const nvgl::RenderContextGLFormat &format, SceniXQGLWidget *shareWidget )
: SceniXQGLWidget( parent, format, shareWidget )
, m_continuousUpdate( false )
, m_timerID( -1 )
{
  m_appTraverser = new nvtraverser::AppTraverser();
  //
  // MMM - can we remove this stuf??
  //
  //if ( m_appTraverser->isSkinningProcessorSupported( AppTraverser::SKINNING_PROCESSOR_CUDA ) )
  //{
  //  m_appTraverser->setSkinningProcessor( AppTraverser::SKINNING_PROCESSOR_CUDA );
  //}
  //else if ( m_appTraverser->isSkinningProcessorSupported( AppTraverser::SKINNING_PROCESSOR_GPUG80GL ) )
  //{
  //  m_appTraverser->setSkinningProcessor( AppTraverser::SKINNING_PROCESSOR_GPUG80GL );
  //}
 
  m_todTimer.start();
  m_lastTime = m_todTimer.getTime();
}

SceniXQGLSceneRendererWidget::~SceniXQGLSceneRendererWidget()
{
  if( m_timerID != -1 )
  {
    killTimer( m_timerID );
    m_timerID = -1;
  }

  getRenderContext()->makeCurrent();
}

void SceniXQGLSceneRendererWidget::initializeGL()
{
  smart_cast<nvgl::RenderTargetGL>(getRenderTarget())->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  if ( m_manipulator )
  {
    m_manipulator->setRenderTarget( getRenderTarget() );
  }
}

void SceniXQGLSceneRendererWidget::paintGL()
{
  if (m_appTraverser && m_renderer && m_viewState)
  {
    SceneSharedPtr sceneHandle = ViewStateReadLock(m_viewState)->getScene();
    if (sceneHandle)
    {
      // Auto-clip planes are updated by a standard AppTraverser.
      m_appTraverser->apply( m_viewState );

      m_renderer->render( m_viewState, getRenderTarget() );
    }
  }
}

void SceniXQGLSceneRendererWidget::onTimer( float dt )
{
  if ( m_manipulator )
  {
    m_manipulator->updateFrame( dt );
  }

  triggerRepaint();
}

float SceniXQGLSceneRendererWidget::getElapsedTime()
{
  double curTime = m_todTimer.getTime();
  double diffTime = curTime - m_lastTime;

  m_lastTime = curTime;

  // gettime returns time in ms.  convert to s.
  diffTime /= 1000.0;

  return static_cast<float>( diffTime );
}

void SceniXQGLSceneRendererWidget::timerEvent( QTimerEvent * event )
{
  onTimer( getElapsedTime() );
}

void SceniXQGLSceneRendererWidget::triggerRepaint()
{
  update();
}

void SceniXQGLSceneRendererWidget::setContinuousUpdate( bool onOff )
{
  if( onOff != m_continuousUpdate )
  {
    m_continuousUpdate = onOff;
    if( onOff )
    {
      // arg of 0 means signal every time there are no more window events to process
      m_timerID = startTimer(0);
    }
    else
    {
      NVSG_ASSERT( m_timerID != -1 );

      killTimer( m_timerID );
      m_timerID = -1;
    }
  }
}

void SceniXQGLSceneRendererWidget::hidNotify( PropertyId property )
{
  SceniXQGLWidget::hidNotify( property );
  if ( m_manipulator && m_manipulatorAutoRefresh )
  {
    if ( m_manipulator->updateFrame( getElapsedTime() ) )
    {
      update();
    }
  }
}

void SceniXQGLSceneRendererWidget::onManipulatorChanged( Manipulator *manipulator )
{
  if ( manipulator )
  {
    manipulator->setRenderTarget( getRenderTarget() );
  }
}
