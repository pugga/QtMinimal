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

#include <nvui/SceneRenderer.h>
#include "ui/Manipulator.h"

class SceniXSceneRendererWidget
{
public:
    SceniXSceneRendererWidget();
    virtual ~SceniXSceneRendererWidget();

    void setViewState( const nvsg::ViewStateSharedPtr &viewStateHandle );
    nvsg::ViewStateSharedPtr getViewState() const;

    void setSceneRenderer( const nvui::SmartSceneRenderer &sceneRenderer );
    nvui::SmartSceneRenderer getSceneRenderer() const;

    void setManipulator( Manipulator *manipulator );
    Manipulator * getManipulator() const;
    /** \brief Enable automatic refresh when sending events to the manipulator **/
    void setManipulatorAutoRefresh( bool autoRefresh );

    virtual void triggerRepaint() = 0;
protected:
    virtual void onViewStateChanged( const nvsg::ViewStateSharedPtr &viewStateHandle );
    virtual void onSceneRendererChanged( const nvui::SmartSceneRenderer &sceneRenderer ) {}
    virtual void onManipulatorChanged( Manipulator *manipulator ) {}

    nvui::SmartSceneRenderer m_renderer;
    nvsg::ViewStateSharedPtr m_viewState;

    // Manipulator
    Manipulator * m_manipulator;
    bool          m_manipulatorAutoRefresh;
};
