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

#include "ui/WalkCameraManipulatorHIDSync.h"

using namespace nvui;
using namespace nvutil;

WalkCameraManipulatorHIDSync::WalkCameraManipulatorHIDSync( const nvmath::Vec2f & sensitivity )
  : WalkCameraManipulator( sensitivity )
  , PID_Pos(0)
  , PID_Wheel(0)
  , PID_ButtonForward(0)
  , PID_ButtonReverse(0)
  , PID_KeyForward(0)
  , PID_KeyReverse(0)
  , PID_KeyStrafeLeft(0)
  , PID_KeyStrafeRight(0)
  , PID_KeyUp(0)
  , PID_KeyDown(0)
  , PID_KeyRun(0)
  , m_speed(1.f)
  , m_reverseMouse( false )
{
}

WalkCameraManipulatorHIDSync::~WalkCameraManipulatorHIDSync()
{
}

bool WalkCameraManipulatorHIDSync::updateFrame( float dt )
{
  if (!m_hid)
  {
    return false;
  }

  nvmath::Vec2i pos = m_hid->getValue<nvmath::Vec2i>( PID_Pos );
  if( getReverseMouse() && getRenderTarget() )
  {
    pos[2] = getRenderTarget()->getHeight() - pos[2] - 1;
  }

  setCursorPosition( pos );

  setWheelTicks( m_hid->getValue<int>( PID_Wheel ) );

  // set height above terrain
  if( m_hid->getValue<bool>( PID_KeyUp ) )
  {
    setCameraHeightAboveTerrain( getCameraHeightAboveTerrain() + m_sensitivity[1] );
  }
  else if( m_hid->getValue<bool>( PID_KeyDown ) )
  {
    float hat = getCameraHeightAboveTerrain() - m_sensitivity[1];
    if( hat < 0.f )
    {
      hat = 0.f;
    }

    setCameraHeightAboveTerrain( hat );
  }

  bool forward = m_hid->getValue<bool>( PID_ButtonForward ) ||
                 m_hid->getValue<bool>( PID_KeyForward );
  bool reverse = m_hid->getValue<bool>( PID_ButtonReverse ) ||
                 m_hid->getValue<bool>( PID_KeyReverse );

  bool strafeLeft  = m_hid->getValue<bool>( PID_KeyStrafeLeft );
  bool strafeRight = m_hid->getValue<bool>( PID_KeyStrafeRight );

  float run = m_hid->getValue<bool>( PID_KeyRun ) ? 2.0f : 1.0f;

  unsigned int mode = MODE_FREELOOK;
  
  // update speed based on wheel
  m_speed += getWheelTicksDelta() * 0.1f;
  if( m_speed < 0.f )
  {
    m_speed = 0.f;
  }

  if( forward || reverse )
  {
    // set forward, reverse here
    float speed = forward ? m_speed : -m_speed;
    WalkCameraManipulator::setSpeed( speed * run );

    mode |= MODE_WALK;
  }

  if( strafeLeft )
  {
    WalkCameraManipulator::setSpeed( m_speed * run );
    mode |= MODE_STRAFE_LEFT;
  }
  else if( strafeRight )
  {
    WalkCameraManipulator::setSpeed( m_speed * run );
    mode |= MODE_STRAFE_RIGHT;
  }

  // if not moving, set speed to 0
  if( mode == MODE_FREELOOK )
  {
    // stopped
    WalkCameraManipulator::setSpeed( 0.f );
  }

  setMode( mode );

  return WalkCameraManipulator::updateFrame( dt );
}

void WalkCameraManipulatorHIDSync::setHID( HumanInterfaceDevice *hid )
{
  m_hid = hid;

  PID_Pos           = m_hid->getProperty( "Mouse_Position" );
  PID_ButtonForward = m_hid->getProperty( "Mouse_Left" );
  PID_ButtonReverse = m_hid->getProperty( "Mouse_Middle" );
  PID_Wheel         = m_hid->getProperty( "Mouse_Wheel" );

  PID_KeyForward     = m_hid->getProperty( "Key_W" );
  PID_KeyReverse     = m_hid->getProperty( "Key_S" );
  PID_KeyStrafeLeft  = m_hid->getProperty( "Key_A" );
  PID_KeyStrafeRight = m_hid->getProperty( "Key_D" );
  PID_KeyRun         = m_hid->getProperty( "Key_Shift" );

  PID_KeyUp          = m_hid->getProperty( "Key_Q" );
  PID_KeyDown        = m_hid->getProperty( "Key_E" );
}
