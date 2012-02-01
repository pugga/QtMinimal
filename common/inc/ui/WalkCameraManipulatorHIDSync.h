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

#include <nvutil/Observer.h>
#include "ui/HumanInterfaceDevice.h"
#include "ui/WalkCameraManipulator.h"

/*! \brief WalkCameraManipulatorHIDSync "synchronizes" the HID input devices to the CursorState of a WalkCameraManipulator.
 *
 *  The WalkCameraManipulator comes with the following HID input mapping: \n\n
 *  <table>
 *  <tr><td> <b>Operation</b></td>    <td><b>MouseButton</b></td>     <td><b>KeyState</b></td></tr>
 *  <tr><td>Walk forward</td>         <td>Mouse_Left</td>              <td></td></tr>
 *  <tr><td>Walk backward</td>        <td>Mouse_Middle</td>            <td></td></tr>
 *  <tr><td>Tilt</td>                 <td>Mouse_Position</td>          <td></td></tr>
 *  <tr><td>Translate</td>            <td>Mouse_Middle</td>            <td>Key_Control</td></tr>
 *  <tr><td>Accelerate</td>           <td>Mouse_Wheel</td>             <td></td></tr>
 *  <tr><td>Decelerate</td>           <td>Mouse_Wheel</td>             <td></td></tr>
 *  </table>
 *
 *  The WalkCameraManipulator can also operate like a "First Person Shooter": \n\n
 *
 *  <table>
 *  <tr><td> <b>Operation</b></td>    <td><b>MouseButton</b></td>     <td><b>KeyState</b></td></tr>
 *  <tr><td>Walk forward</td>         <td></td>                       <td>Key_W</td></tr>
 *  <tr><td>Walk backward</td>        <td></td>                       <td>Key_S</td></tr>
 *  <tr><td>Look</td>                 <td>Mouse_Position</td>         <td></td></tr>
 *  <tr><td>Strafe left</td>          <td></td>                       <td>Key_A</td></tr>
 *  <tr><td>Strafe right</td>         <td></td>                       <td>Key_D</td></tr>
 *  <tr><td>Run (2x speed)</td>       <td></td>                       <td>Key_Shift</td></tr>
 *  </table>
 *  \n
 *  The manipulator automatically connects the eyepoint to the model's surface.  See the \a setCameraHeightAboveTerrain
 *  and \a getCameraHeightAboveTerrain functions for how to manipulate the camera height above the terrain.
 *  This simulates pushing a camera on a tripod through an environment that will always be this height
 *  above the actual terrain.
 *  \remarks Typically the application will want to run the WalkCameraManipulator continuously, passing appropriate
 *  delta-time passage as the 'dt' argument to updateFrame().  If this is not done, then the walk manipulator will only
 *  move when one of the control inputs changes.
 *  \sa Manipulator, WalkCameraManipulatorHIDSync */
class WalkCameraManipulatorHIDSync : public WalkCameraManipulator
{
public:
    /* \brief Constructor
   *  \param sensitivity The vector should be greater than (0,0).  This will set the "look" controls sensitivity
   *  in the X and Y axes.  The sensitivity value is the ratio of window pixels to angular displacement in the walk
   *  manipulator.  A value of 1.0 means 1 window pixel = 1 degree of movement in that axis.  Values closer to zero
   *  will decrease the sensitivity (so a larger mouse movement has less affect on the controls).  The default value
   *  works well for most applications.
   */
    WalkCameraManipulatorHIDSync( const nvmath::Vec2f & sensitivity = nvmath::Vec2f( 0.8f, 0.8f ) );
    virtual ~WalkCameraManipulatorHIDSync();

    void setHID( HumanInterfaceDevice *hid );

    virtual bool updateFrame( float dt );

    /*! \brief Specifies the speed used to perform camera motions.
   * \param speed Speed to set.  The speed is in "units-per-second" where 'units' are database units, and
   * the time reference comes from the argument to updateFrame().
   * \note This speed is cached even when the WalkCameraManipulatorHIDSync is "stopped" (not moving because no buttons are
   * pressed) whereas the Speed stored in the WalkCameraManipulator base class will be 0.0 when "stopped," and nonzero when
   * moving.
   * \remarks The initial speed is set to 1.0
   * \sa getSpeed */
    virtual void setSpeed( float speed );

    /*! \brief Get speed used to perform camera motions.
   *  \return Speed value that has been set.
   *  \sa setSpeed */
    virtual float getSpeed() const;

    void setReverseMouse( bool reverse );
    bool getReverseMouse() const;

private:
    /* Property ids for HID input */
    nvutil::PropertyId PID_Pos;
    nvutil::PropertyId PID_Wheel;
    nvutil::PropertyId PID_ButtonForward;
    nvutil::PropertyId PID_ButtonReverse;

    nvutil::PropertyId PID_KeyForward;
    nvutil::PropertyId PID_KeyReverse;
    nvutil::PropertyId PID_KeyStrafeLeft;
    nvutil::PropertyId PID_KeyStrafeRight;
    nvutil::PropertyId PID_KeyUp;
    nvutil::PropertyId PID_KeyDown;
    nvutil::PropertyId PID_KeyRun;

    HumanInterfaceDevice *m_hid;
    float m_speed;
    bool  m_reverseMouse;
};

inline void WalkCameraManipulatorHIDSync::setReverseMouse( bool rev )
{
    m_reverseMouse = rev;
}

inline bool WalkCameraManipulatorHIDSync::getReverseMouse() const
{
    return m_reverseMouse;
}

inline void WalkCameraManipulatorHIDSync::setSpeed( float speed )
{
    m_speed = speed;
}

inline float WalkCameraManipulatorHIDSync::getSpeed() const
{
    return m_speed;
}
