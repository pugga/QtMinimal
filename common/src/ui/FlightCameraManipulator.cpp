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


#include <nvsg/Camera.h>
#include <nvsg/ViewState.h>
#include "ui/FlightCameraManipulator.h"

using namespace nvui;
using namespace nvmath;
using namespace nvsg;
using namespace nvutil;

static const UINT AREAOFPEACE = 1;

FlightCameraManipulator::FlightCameraManipulator( const nvmath::Vec2f & sensitivity )
: Manipulator()
, CursorState()
, m_sensitivity( sensitivity )
, m_speed( 0.f )
, m_upVector( 0.f, 1.f, 0.f ) // default to y-up
{
}

FlightCameraManipulator::~FlightCameraManipulator(void)
{
}

bool FlightCameraManipulator::updateFrame( float dt )
{
  bool result = false;
  m_deltaT = dt;

  if( getViewState() && getRenderTarget() )
  {
    result = fly();
  }

  return result; 
}

void FlightCameraManipulator::reset()
{
  resetInput();
}

bool FlightCameraManipulator::fly()
{
  // Since this is more or less a slow helicopter flight we only 
  // rotate around the world axis.

  CameraSharedPtr camera = ViewStateReadLock(getViewState())->getCamera();

  if (camera && fabs( m_speed ) > 0.f )
  {
    CameraWriteLock theCamera(camera);

    // left / right 
    float deltaX = getRenderTarget()->getWidth() * 0.5f - getLastX();
    if ( fabs(deltaX) > AREAOFPEACE )
    {
      // so we start at 0
      (deltaX < 0.f) ? deltaX += AREAOFPEACE : deltaX -= AREAOFPEACE; 

      float alpha = m_deltaT * m_sensitivity[0] * deltaX;
      theCamera->rotate(m_upVector, degToRad(alpha), false);
    }

    // up / down
    float deltaY = getRenderTarget()->getHeight() * 0.5f - getLastY();
    if ( fabs(deltaY) > AREAOFPEACE )
    {
      // so we start at 0
      (deltaY < 0.f) ? deltaY += AREAOFPEACE : deltaY -= AREAOFPEACE;

      // determine the horizontal axis
      Vec3f side = theCamera->getDirection() ^ theCamera->getUpVector();
      side.normalize();
      
      float alpha = m_deltaT * m_sensitivity[1] * deltaY;
      theCamera->rotate(side, degToRad(alpha), false);
    }

    // move into fly direction
    theCamera->setPosition(theCamera->getPosition() + m_deltaT * m_speed * theCamera->getDirection());

    return true;
  }

  return false;
}

