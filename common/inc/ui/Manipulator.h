// Copyright NVIDIA Corporation 2002-2005
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
/** \file */

#include <nvsgcommon.h>
#include <nvsg/CoreTypes.h>
#include <nvui/RenderTarget.h>
#include <nvmath/Vecnt.h>
#include <map>

/*! \brief Base class for Manipulators.
 *  A Manipulator provides the ability to control a certain object on a user interaction.
 *  This class serves a base class for concrete Manipulators. As an example, a concrete
 *  Manipulator can implement an interactive control of an nvsg::Camera object, where
 *  user commands are passed through an InputHandler and executed by the implementation
 *  through the public Camera API. Same can be implemented for other SceniX core objects.
 *  The SceniX SDK already comes with a set of concrete CameraManipulator objects, like
 *  the TrackballCameraManipulator, the WalkCameraManipulator, and the FlightCameraManipulator.
 *  These objects can be instantiated and configured for your application right away.
 *  Besides the mentioned CameraManipulators, SceniX also provides a TrackballTransformManipulator
 *  as a concrete implementation of a TransformManipulator.
 *  \n\n
 *  Manipulator provides an API to assign RenderTarget and ViewState, which concrete implementations require
 *  to retrieve necessary information to accurately update the controlled objects, like cameras and transforms. */
class Manipulator
{
public:
    Manipulator();
    virtual ~Manipulator();

    /*! \brief Sets this manipulator's ViewState.
   *  \param viewState The ViewState this manipulator should use.
   *  \sa getViewState */
    virtual void setViewState( const nvsg::ViewStateSharedPtr &viewState);

    /*! \brief Gets this manipulator's ViewState.
   *  \return The ViewState this manipulator is using.
   *  \sa setViewState */
    virtual nvsg::ViewStateSharedPtr getViewState() const;

    /*! \brief Sets this manipulator's RenderTarget.
   *  \param renderTarget The RenderTarget this manipulator should use.
   *  \sa getRenderTarget */
    virtual void setRenderTarget( const nvui::SmartRenderTarget &renderTarget );

    /*! \brief Gets this manipulator's RenderTarget.
   *  \return The RenderTarget this manipulator is using.
   *  \sa setRenderTarget */
    virtual nvui::SmartRenderTarget getRenderTarget() const;

    /*! \brief Updates the manipulator's timestamp, and runs the manipulator.
   *  \param dt Delta time passage since this function was called last, in seconds.
   *  \remarks Some manipulators (walk, flight, animation) require this method to be called continuously
   *  when they are active, to update the simulation.
   */
    virtual bool updateFrame( float dt ) = 0;

    /*! \brief Resets the manipulator to defaults.
   * \remarks Resets the manipulator to initial state.
   */
    virtual void reset();

protected:
    nvui::SmartRenderTarget  m_renderTarget;
    nvsg::ViewStateSharedPtr m_viewState;
};

/*! \brief Internal helper class used to manage the "cursor" state.
 *  \remarks Manipulators that use the mouse cursor will use this class to manage its state.
 */
class CursorState
{
public:
    CursorState();
    ~CursorState();

    void setCursorPosition( const nvmath::Vec2i &cursor );
    const nvmath::Vec2i & getCursorPosition() const;
    const nvmath::Vec2i & getLastCursorPosition() const;

    void setWheelTicks( int wheelTicks );
    int  getWheelTicks() const;
    int  getLastWheelTicks() const;
    int  getWheelTicksDelta() const;

    int getLastX() const;
    int getLastY() const;
    int getCurrentX() const;
    int getCurrentY() const;

    void resetInput();

private:
    enum
    {
        IUP_WHEEL    = 0x01,
        IUP_POSITION = 0x02
    };

    nvmath::Vec2i m_cursorPosition;
    nvmath::Vec2i m_lastCursorPosition;
    int           m_wheelTicks;
    int           m_lastWheelTicks;
    unsigned int  m_initialUpdate;
};

inline void CursorState::resetInput()
{
    m_cursorPosition     = nvmath::Vec2i(0,0);
    m_lastCursorPosition = nvmath::Vec2i(0,0);
    m_wheelTicks         = 0;
    m_lastWheelTicks     = 0;
    m_initialUpdate      = IUP_WHEEL | IUP_POSITION;
}

inline CursorState::CursorState()
{
    resetInput();
}

inline CursorState::~CursorState()
{
}

inline int CursorState::getCurrentX() const
{
    return getCursorPosition()[0];
}

inline int CursorState::getCurrentY() const
{
    return getCursorPosition()[1];
}

inline int CursorState::getLastX() const
{
    return getLastCursorPosition()[0];
}

inline int CursorState::getLastY() const
{
    return getLastCursorPosition()[1];
}

inline void CursorState::setCursorPosition( const nvmath::Vec2i &cursor )
{
    if( m_initialUpdate & IUP_POSITION )
    {
        m_lastCursorPosition = cursor;
        m_initialUpdate &= ~IUP_POSITION;
    }
    else
    {
        m_lastCursorPosition = m_cursorPosition;
    }

    m_cursorPosition = cursor;
}

inline const nvmath::Vec2i & CursorState::getCursorPosition() const
{
    return m_cursorPosition;
}

inline const nvmath::Vec2i & CursorState::getLastCursorPosition() const
{
    return m_lastCursorPosition;
}

inline void CursorState::setWheelTicks( int wheelTicks )
{
    if( m_initialUpdate & IUP_WHEEL )
    {
        m_lastWheelTicks = wheelTicks;
        m_initialUpdate &= ~IUP_WHEEL;
    }
    else
    {
        m_lastWheelTicks = m_wheelTicks;
    }

    m_wheelTicks = wheelTicks;
}

inline int  CursorState::getWheelTicks() const
{
    return m_wheelTicks;
}

inline int  CursorState::getLastWheelTicks() const
{
    return m_lastWheelTicks;
}

inline int  CursorState::getWheelTicksDelta() const
{
    return getWheelTicks() - getLastWheelTicks();
}

