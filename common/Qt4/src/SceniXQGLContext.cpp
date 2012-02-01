// Copyright NVIDIA Corporation 2009-2011
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#if defined(LINUX)
#include <QX11Info>
#endif

#include <QApplication>
#include <SceniXQGLContext.h>

#include <nvutil/DbgNew.h>

using namespace nvgl;
using namespace nvutil;

SceniXQGLContext::SceniXQGLContext( const RenderContextGLFormat &format)
: QGLContext( QGLFormat() ) // qglformat will be set automatically after create
, m_format( format )
, m_context( 0 )
{
}

SceniXQGLContext::~SceniXQGLContext()
{
  if (m_context)
  {
    m_context->makeNoncurrent();
    m_context->notifyDestroy();
  }
}

bool SceniXQGLContext::create(const QGLContext *shareContext)
{
  bool result = QGLContext::create( shareContext );

#if defined(WIN32)
  if (result)
  {
    QGLContext::makeCurrent();
    const SceniXQGLContext *sharedContext = dynamic_cast<const SceniXQGLContext*>( shareContext );
    m_context = smart_cast<RenderContextGL>( RenderContextGL::create( RenderContextGL::Attach( sharedContext ? sharedContext->m_context : 0) ) );
  }
#elif defined(LINUX)
  m_shareContext = shareContext ? dynamic_cast<const SceniXQGLContext*>( shareContext )->m_context : 0;
#endif

  return result;
}

#if defined( _WIN32 )
int SceniXQGLContext::choosePixelFormat( void *dummyPfd, HDC pdc ) 
{
  return m_format.getPixelFormat();
}
#endif

#if defined(LINUX)
void *SceniXQGLContext::chooseVisual( )
{
  GLXFBConfig fbconfig = m_format.getGLXFBConfig( QX11Info::display(), QX11Info::appScreen() );
  XVisualInfo *vInfo = glXGetVisualFromFBConfig( QX11Info::display(), fbconfig);
  return reinterpret_cast<void*>(vInfo);
}
#endif

void SceniXQGLContext::makeCurrent()
{
  QGLContext::makeCurrent();
#if defined(LINUX)
  // FIXME Qt sometimes delays context creation until makeCurrent is called the first time. Attach here and find a better solution.
  if (!m_context)
  {
    m_context = smart_cast<RenderContextGL>( RenderContextGL::create( RenderContextGL::Attach( m_shareContext ) ) );
    m_shareContext.reset();
  }
#endif
  if (m_context)
  {
    m_context->makeCurrent();
  }
}

void SceniXQGLContext::doneCurrent()
{
  // Mind the order. Qt releases the HDC in doneCurrent().
  if (m_context)
  {
    m_context->makeNoncurrent();
  }
  QGLContext::doneCurrent();
}

const SmartRenderContextGL & SceniXQGLContext::getRenderContext() const
{
  return m_context;
}
