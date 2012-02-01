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

#include "ui/TrackballCameraManipulatorHIDSync.h"

using namespace nvui;
using namespace nvutil;

TrackballCameraManipulatorHIDSync::TrackballCameraManipulatorHIDSync( )
  : TrackballCameraManipulator()
  , PID_Pos(0)
  , PID_Orbit(0)
  , PID_Pan(0)
  , PID_Shift(0)
  , PID_Control(0)
  , PID_HotSpot(0)
{
}

TrackballCameraManipulatorHIDSync::~TrackballCameraManipulatorHIDSync()
{
}

bool TrackballCameraManipulatorHIDSync::updateFrame( float dt )
{
  if (!m_hid)
  {
    return false;
  }

  if ( m_hid->getValue<bool>( PID_Shift ) )
  {
    lockMajorAxis();
  }
  else
  {
    unlockMajorAxis();
  }

  bool orbit = m_hid->getValue<bool>( PID_Orbit );
  bool pan = m_hid->getValue<bool>( PID_Pan );
  bool control = m_hid->getValue<bool>( PID_Control );
  bool shift = m_hid->getValue<bool>( PID_Shift );
  bool hotspot = m_hid->getValue<bool>( PID_HotSpot );

  setCursorPosition( m_hid->getValue<nvmath::Vec2i>( PID_Pos ) );
  setWheelTicks( m_hid->getValue<int>( PID_Wheel ) );

  TrackballCameraManipulator::Mode mode = TrackballCameraManipulator::MODE_NONE;
  if ( hotspot )
  {
    mode = TrackballCameraManipulator::MODE_LOOKAT;
  }
  else 
  {
    if ( orbit && pan && control && shift )  // vertigo
    {
      mode = TrackballCameraManipulator::MODE_ZOOM_DOLLY;
    }
    if ( orbit && pan && control && !shift )  // zoom
    {
      mode = TrackballCameraManipulator::MODE_ZOOM_FOV;
    }
    if ( orbit && pan && !control && !shift )  // dolly
    {
      mode = TrackballCameraManipulator::MODE_DOLLY;
    }
    else if ( orbit && !pan && !control && !shift )
    {
      mode = TrackballCameraManipulator::MODE_ORBIT;
    }
    else if ( !orbit && pan && !control && !shift )
    {
      mode = TrackballCameraManipulator::MODE_PAN;
    }
    else if ( !orbit && pan && control && !shift )
    {
      mode = TrackballCameraManipulator::MODE_ROLL_Z;
    }
    else if ( orbit && !pan && control && !shift )
    {
      mode = TrackballCameraManipulator::MODE_ROTATE_XY;
    }
  }

  setMode( mode );

  return TrackballCameraManipulator::updateFrame( dt );
}

void TrackballCameraManipulatorHIDSync::setHID( HumanInterfaceDevice *hid )
{
  m_hid = hid;

  PID_Pos = m_hid->getProperty( "Mouse_Position" );
  PID_Orbit = m_hid->getProperty( "Mouse_Left" );
  PID_Pan = m_hid->getProperty( "Mouse_Middle" );
  PID_Shift = m_hid->getProperty( "Key_Shift" );
  PID_Control = m_hid->getProperty("Key_Control");
  PID_HotSpot = m_hid->getProperty("Key_H");
  PID_Wheel = m_hid->getProperty("Mouse_Wheel");
}
