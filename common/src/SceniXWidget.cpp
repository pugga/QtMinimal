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

#include <SceniXWidget.h>
#include <nvui/RendererOptions.h>

using namespace nvsg;
using namespace nvui;

SceniXSceneRendererWidget::SceniXSceneRendererWidget()
    : m_manipulatorAutoRefresh( true )
    , m_manipulator( 0 )
{

}

SceniXSceneRendererWidget::~SceniXSceneRendererWidget()
{
    if ( m_manipulator )
    {
        m_manipulator->setViewState( 0 );
        m_manipulator->setRenderTarget( 0 );
    }
    if ( m_renderer )
    {
        m_renderer->setRenderTarget( nullptr );
    }
}

void SceniXSceneRendererWidget::setViewState( const nvutil::SmartPtr<nvsg::ViewStateHandle> &viewState )
{
    if ( viewState != m_viewState )
    {
        m_viewState = viewState;
        onViewStateChanged( m_viewState );
        triggerRepaint();
    }
}

ViewStateSharedPtr SceniXSceneRendererWidget::getViewState() const
{
    return m_viewState;
}

void SceniXSceneRendererWidget::setSceneRenderer( const nvui::SmartSceneRenderer &sceneRenderer )
{
    if ( m_renderer != sceneRenderer)
    {
        m_renderer = sceneRenderer;
        onSceneRendererChanged( m_renderer );
        triggerRepaint();
    }
}

nvui::SmartSceneRenderer SceniXSceneRendererWidget::getSceneRenderer() const
{
    return m_renderer;
}

void SceniXSceneRendererWidget::setManipulator( Manipulator *manipulator )
{
    if ( m_manipulator != manipulator )
    {
        m_manipulator = manipulator;
        if ( m_manipulator )
        {
            m_manipulator->setViewState( m_viewState );
            m_manipulator->reset();
        }
        onManipulatorChanged( m_manipulator );
    }
}

Manipulator * SceniXSceneRendererWidget::getManipulator() const
{
    return m_manipulator;
}

void SceniXSceneRendererWidget::onViewStateChanged( const ViewStateSharedPtr &viewState )
{
    if ( m_manipulator )
    {
        m_manipulator->setViewState( viewState );
    }
}
