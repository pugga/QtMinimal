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

#pragma once

#include "SceniXwxGLCanvas.h"

#include <nvgl/RenderTargetGL.h>
#include <nvgl/RenderContextGLFormat.h>
#include <nvui/SceneRenderer.h>
#include <nvtraverser/AppTraverser.h>
#include <SceniXWidget.h>

class SceniXwxGLSceneRendererCanvas : public SceniXwxGLCanvas, public SceniXSceneRendererWidget
{
public:
  SceniXwxGLSceneRendererCanvas(  wxFrame *parent,
                                  wxWindowID id = wxID_ANY,
                                  int* attribList = 0, 
                                  const wxPoint& pos = wxDefaultPosition,
                                  const wxSize& size = wxDefaultSize,
                                  long style = 0, 
                                  const wxString& name = _T("SceniXwxGLSceneRendererCanvas"),
                                  const wxPalette& palette = wxNullPalette
 );
  virtual ~SceniXwxGLSceneRendererCanvas();

  const nvutil::SmartPtr<nvtraverser::AppTraverser> & getAppTraverser() const;
protected:
  DECLARE_EVENT_TABLE();

  virtual void hidNotify( nvutil::PropertyId property );
  virtual void triggerRepaint();

  virtual void initializeGL();
  virtual void paintGL();

  nvutil::SmartPtr< nvtraverser::AppTraverser > m_appTraverser;
};

