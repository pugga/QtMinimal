// Copyright NVIDIA Corporation 2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#include "SceniXwxGLSceneRendererCanvas.h"
#include <nvsg/ViewState.h>

#include <nvutil/DbgNew.h>

using namespace nvgl;
using namespace nvsg;
using namespace nvmath;
using namespace nvtraverser;
using namespace nvutil;

SceniXwxGLSceneRendererCanvas::SceniXwxGLSceneRendererCanvas( wxFrame *frame, wxWindowID id, int* attribList, const wxPoint& pos, const wxSize& size, long style, const wxString& name, const wxPalette& palette )
  : SceniXwxGLCanvas( frame, id, attribList, pos, size, style, name, palette )
{
  m_appTraverser = new nvtraverser::AppTraverser();
  //if ( m_appTraverser->isSkinningProcessorSupported( AppTraverser::SKINNING_PROCESSOR_CUDA ) )
  //{
  //  m_appTraverser->setSkinningProcessor( AppTraverser::SKINNING_PROCESSOR_CUDA );
  //}
  //else if ( m_appTraverser->isSkinningProcessorSupported( AppTraverser::SKINNING_PROCESSOR_GPUG80GL ) )
  //{
  //  m_appTraverser->setSkinningProcessor( AppTraverser::SKINNING_PROCESSOR_GPUG80GL );
  //}
}

SceniXwxGLSceneRendererCanvas::~SceniXwxGLSceneRendererCanvas()
{
}

void SceniXwxGLSceneRendererCanvas::initializeGL()
{
  getRenderTarget()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void SceniXwxGLSceneRendererCanvas::paintGL()
{
  if (m_appTraverser && m_renderer && m_viewState)
  {
    SceneSharedPtr scenesp = ViewStateReadLock(m_viewState)->getScene();
    if( scenesp )
    {
      {
        Vec4f backColor = SceneReadLock(scenesp)->getBackColor();
        getRenderTarget()->setClearColor( backColor[0], backColor[1], backColor[2], backColor[3] );
      }

      // Auto-clip planes are updated by a standard AppTraverser.
      m_appTraverser->setViewState( m_viewState );
      m_appTraverser->apply( scenesp );

      m_renderer->render( m_viewState, getRenderTarget() );
    }
  }
}

void SceniXwxGLSceneRendererCanvas::triggerRepaint()
{
  Refresh();
}

void SceniXwxGLSceneRendererCanvas::hidNotify( PropertyId property )
{
  SceniXwxGLCanvas::hidNotify( property );
  if ( m_manipulator && m_manipulatorAutoRefresh )
  {
    if ( m_manipulator->updateFrame( 0.f ) )
    {
      Refresh();
    }
  }
}

BEGIN_EVENT_TABLE(SceniXwxGLSceneRendererCanvas, SceniXwxGLCanvas)
END_EVENT_TABLE()
