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

#pragma once

#include <QGLContext>
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderContextGLFormat.h>

class SceniXQGLContext : public QGLContext
{
public:
    SceniXQGLContext( const nvgl::RenderContextGLFormat &format);
    virtual ~SceniXQGLContext();

    const nvgl::SmartRenderContextGL & getRenderContext() const;
protected:
    virtual bool create( const QGLContext *shareContext );
    virtual void makeCurrent();
    virtual void doneCurrent();
#if defined( _WIN32 )
    virtual int choosePixelFormat( void *dummyPfd, HDC pdc );
#elif defined(LINUX)
    virtual void *chooseVisual( );
#endif

    nvgl::SmartRenderContextGL  m_context;
    nvgl::RenderContextGLFormat m_format;

#if defined(LINUX)
    nvgl::SmartRenderContextGL m_shareContext; // keep share information on linux until first makeCurrent had been executed
#endif
};
