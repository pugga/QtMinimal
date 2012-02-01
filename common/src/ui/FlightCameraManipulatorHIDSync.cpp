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

#include "ui/FlightCameraManipulatorHIDSync.h"

using namespace nvui;
using namespace nvutil;

FlightCameraManipulatorHIDSync::FlightCameraManipulatorHIDSync( const nvmath::Vec2f & sensitivity )
  : FlightCameraManipulator( sensitivity )
  , PID_Pos(0)
  , PID_Wheel(0)
  , PID_Forward(0)
  , PID_Reverse(0)
  , m_speed(1.f)
{

}

FlightCameraManipulatorHIDSync::~FlightCameraManipulatorHIDSync()
{
}

bool FlightCameraManipulatorHIDSync::updateFrame( float dt )
{
  if (!m_hid)
  {
    return false;
  }

  setCursorPosition( m_hid->getValue<nvmath::Vec2i>( PID_Pos ) );
  setWheelTicks( m_hid->getValue<int>( PID_Wheel ) );

  bool forward = m_hid->getValue<bool>( PID_Forward );
  bool reverse = m_hid->getValue<bool>( PID_Reverse );
  
  // set speed based on wheel and buttons
  m_speed  += getWheelTicksDelta() * 0.1f;
  if( m_speed < 0.f )
  {
    m_speed = 0.f;
  }

  if( forward || reverse )
  {
    // set forward, reverse here
    FlightCameraManipulator::setSpeed( forward ? m_speed : -m_speed );
  }
  else
  {
    // stopped
    FlightCameraManipulator::setSpeed( 0.f );
  }

  return FlightCameraManipulator::updateFrame( dt );
}

void FlightCameraManipulatorHIDSync::setHID( HumanInterfaceDevice *hid )
{
  m_hid = hid;

  PID_Pos     = m_hid->getProperty( "Mouse_Position" );
  PID_Forward = m_hid->getProperty( "Mouse_Left" );
  PID_Reverse = m_hid->getProperty( "Mouse_Middle" );
  PID_Wheel   = m_hid->getProperty( "Mouse_Wheel" );
}
