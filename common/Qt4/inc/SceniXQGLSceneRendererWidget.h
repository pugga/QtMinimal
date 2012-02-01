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

#include <nvtraverser/AppTraverser.h>
#include <nvutil/Timer.h>
#include "SceniXQGLWidget.h"
#include <QTimer>

/************************************************************************/
/* Basic widget to render a scene/viewstate pair using a SceneRenderer. */
/* The SceneRenderer must accept a RenderTargetGL as RenderTarget       */
/************************************************************************/
class SceniXQGLSceneRendererWidget : public SceniXQGLWidget, public SceniXSceneRendererWidget
{
public:
    SceniXQGLSceneRendererWidget(QWidget *parent = 0, const nvgl::RenderContextGLFormat &format = nvgl::RenderContextGLFormat(), SceniXQGLWidget *shareWidget = 0);
    virtual ~SceniXQGLSceneRendererWidget();

    const nvutil::SmartPtr<nvtraverser::AppTraverser> & getAppTraverser() const;

    virtual void triggerRepaint();
    virtual void setContinuousUpdate( bool tf );
    virtual bool getContinuousUpdate() const;

protected:
    virtual void onManipulatorChanged( Manipulator *manipulator );
    // updates manipulator, calls triggerRepaint
    virtual void onTimer( float dt );

    virtual void hidNotify( nvutil::PropertyId property );

    virtual void initializeGL();
    virtual void paintGL();

    nvutil::SmartPtr< nvtraverser::AppTraverser > m_appTraverser;

    float getElapsedTime();

private:
    virtual void timerEvent( QTimerEvent * event );

protected:
    bool m_continuousUpdate;
    int  m_timerID;
    nvutil::Timer m_todTimer;
    double        m_lastTime;
};

inline const nvutil::SmartPtr<nvtraverser::AppTraverser> & SceniXQGLSceneRendererWidget::getAppTraverser() const
{
    return( m_appTraverser );
}

inline bool SceniXQGLSceneRendererWidget::getContinuousUpdate() const
{
    return m_continuousUpdate;
}
