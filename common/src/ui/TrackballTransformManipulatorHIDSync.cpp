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

#include "ui/TrackballTransformManipulatorHIDSync.h"

using namespace nvui;
using namespace nvutil;

TrackballTransformManipulatorHIDSync::TrackballTransformManipulatorHIDSync( )
  : TrackballTransformManipulator()
  , PID_Pos(0)
  , PID_Rotate(0)
  , PID_Pan(0)
  , PID_Control(0)
  , PID_Wheel(0)
{
}

TrackballTransformManipulatorHIDSync::~TrackballTransformManipulatorHIDSync()
{
}

bool TrackballTransformManipulatorHIDSync::updateFrame( float dt )
{
  if (!m_hid)
  {
    return false;
  }

  bool rotate  = m_hid->getValue<bool>( PID_Rotate );
  bool pan     = m_hid->getValue<bool>( PID_Pan );
  bool control = m_hid->getValue<bool>( PID_Control );

  if ( control )
  {
    lockMajorAxis();
  }
  else
  {
    unlockMajorAxis();
  }

  setCursorPosition( m_hid->getValue<nvmath::Vec2i>( PID_Pos ) );
  setWheelTicks( m_hid->getValue<int>( PID_Wheel ) );

  TrackballTransformManipulator::Mode mode = TrackballTransformManipulator::MODE_NONE;

  if ( rotate && pan || getWheelTicksDelta() )
  {
    mode = TrackballTransformManipulator::MODE_DOLLY;
  }
  else if ( !rotate && pan )
  {
    mode = TrackballTransformManipulator::MODE_PAN;
  }
  else if ( rotate && !pan )
  {
    mode = TrackballTransformManipulator::MODE_ROTATE;
  }

  setMode( mode );

  return TrackballTransformManipulator::updateFrame( dt );
}

void TrackballTransformManipulatorHIDSync::setHID( HumanInterfaceDevice *hid )
{
  m_hid = hid;

  PID_Pos     = m_hid->getProperty("Mouse_Position" );
  PID_Rotate  = m_hid->getProperty("Mouse_Left" );
  PID_Pan     = m_hid->getProperty("Mouse_Middle" );
  PID_Wheel   = m_hid->getProperty("Mouse_Wheel");
  PID_Control = m_hid->getProperty("Key_Control");
}
