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


#include "ui/WalkCameraManipulator.h"
#include <nvsg/Camera.h>
#include <nvtraverser/RayIntersectTraverser.h>
#include <nvutil/DbgNew.h> // this must be the last include

using namespace nvui;
using namespace nvmath;
using namespace nvsg;
using namespace nvutil;
using namespace nvtraverser;

static const UINT AREAOFPEACE = 1;

WalkCameraManipulator::WalkCameraManipulator( const nvmath::Vec2f & sensitivity )
: Manipulator()
, CursorState()
, m_cameraHeightAboveTerrain(2.f)
, m_sensitivity( sensitivity )
, m_mode( MODE_FREELOOK )
, m_upVector(0.f,1.f,0.f)
, m_saveCameraDirection(0.f,0.f,-1.f)
{
}

WalkCameraManipulator::~WalkCameraManipulator(void)
{
}

bool WalkCameraManipulator::updateFrame( float dt )
{
  m_deltaT = dt;

  // walk, translate and tilt can be combined at 
  // the same time in this manipulator
  bool retval = false;

  if ( getViewState() && getRenderTarget() && getRenderTarget()->getWidth()
                                           && getRenderTarget()->getHeight() )
  {
    // first, set orientation
    // NOTE: Freelook now controls the direction of travel.  All other elements set the position only.
    if( m_mode & MODE_FREELOOK )
    {
      retval |= freeLook();
    }

    // now set position
    // findTerrainPosition will set the position to be current camera pos, or terrain pos (if found)
    findTerrainPosition();

    if( m_mode & MODE_WALK )
    {
      retval |= walk();
    }

    if( m_mode & MODE_TRANSLATE )
    {
      retval |= translate();
    }

    if( m_mode & (MODE_STRAFE_LEFT | MODE_STRAFE_RIGHT) )
    {
      retval |= strafe( (m_mode & MODE_STRAFE_RIGHT) != 0 );
    }

    // now, set the new camera position
    ViewStateReadLock view( getViewState() );
    CameraWriteLock camera( view->getCamera() );
    camera->setPosition( m_currentPosition );
  }
  
  return retval;
}

void WalkCameraManipulator::reset()
{
  resetInput();
  recordPlanarCameraDirection();
}

bool WalkCameraManipulator::findTerrainPosition()
{
  Vec3f camPos;
  Sphere3f bound;

  {
    ViewStateReadLock view( getViewState() );
    CameraReadLock camera( view->getCamera() );

    // in case we don't find any intersections
    m_currentPosition = camPos = camera->getPosition();

    SceneReadLock scene( view->getScene() );
    NodeReadLock node( scene->getRootNode() );
    bound = node->getBoundingSphere();
  }

  // move intersect start point high in the air
  camPos += (bound.getRadius() * m_upVector);
  Vec3f dir = -m_upVector;

  // wasteful?
  SmartPtr<RayIntersectTraverser> rit( new RayIntersectTraverser );

  rit->setViewState( getViewState() );
  rit->setCamClipping( false );
  rit->setViewportSize( getRenderTarget()->getWidth(), getRenderTarget()->getHeight() );
  rit->setRay( camPos, dir );
  rit->apply( ViewStateReadLock( getViewState() )->getScene() );

  if( rit->getNumberOfIntersections() )
  {
    const Intersection & intr = rit->getNearest();

    // get "closest" point
    m_currentPosition = intr.getIsp();

    // place 2 meters above ground - should be scalable  
    m_currentPosition += ( m_cameraHeightAboveTerrain * m_upVector );

    return true;
  }

  return false;
}

bool WalkCameraManipulator::walk()
{
  NVSG_TRACE();

  ViewStateReadLock view( getViewState() );
  CameraSharedPtr camera = view->getCamera();

  if (camera)
  {
    CameraWriteLock theCamera(camera);
    {
      // The walk direction is not the viewing direction of the camera.
      // It is always parallel to the walking plane 
      Vec3f side = theCamera->getDirection()^m_upVector;
      Vec3f walkdir = m_upVector^side;
      walkdir.normalize();

      m_currentPosition += (m_deltaT * m_speed * walkdir);
    }

    // redraw required
    return true;
  }
   
  // no redraw required
  return false;
}

// note: the WalkCameraManipulatorHIDSync does not exercise this method..
bool WalkCameraManipulator::translate() 
{
  ViewStateReadLock theViewState(getViewState());
  CameraSharedPtr camera = theViewState->getCamera();
  if (camera)
  {
    CameraWriteLock theCamera(camera);
    int dx = getCurrentX() - getLastX();
    int dy = getLastY() - getCurrentY();
    if (dx != 0 || dy != 0)
    {
      float stepX = m_deltaT * m_speed * theViewState->getTargetDistance() * float(dx);
      float stepY = m_deltaT * m_speed * theViewState->getTargetDistance() * float(dy);

      Vec3f side = theCamera->getDirection()^m_upVector;
      side.normalize();
      
      NVSG_ASSERT(isNormalized(m_upVector));

      m_currentPosition += (stepX * side + stepY * m_upVector);

      // redraw required
      return true;
    }
  }

  // no redraw required
  return false;
}

bool WalkCameraManipulator::strafe( bool right ) 
{
  ViewStateReadLock theViewState(getViewState());
  CameraSharedPtr camera = theViewState->getCamera();
  if (camera)
  {
    CameraWriteLock theCamera(camera);

    // this gives a vector pointing "right" of camera pos
    Vec3f dir = theCamera->getDirection()^m_upVector;
    dir.normalize();

    dir *= m_speed * m_deltaT;
      
    if( !right )
    {
      dir = -dir;
    }

    m_currentPosition += dir;

    // redraw required
    return true;
  }

  // no redraw required
  return false;
}

bool WalkCameraManipulator::freeLook()
{
  CameraSharedPtr camera = ViewStateReadLock(getViewState())->getCamera();
  if (camera)
  {
    CameraWriteLock theCamera(camera);

    // first, reset to default
    theCamera->setOrientation( m_saveCameraDirection, m_upVector );

    // now, update the position based on that default orientation, where the center of the screen will have
    // us pointing parallel to the ground
    float w2 = getRenderTarget()->getWidth()*0.5f;
    float h2 = getRenderTarget()->getHeight()*0.5f;
    float alpha = m_sensitivity[0] * -180.f * ((getCurrentX() / w2) - 1.f);
    float beta  = m_sensitivity[1] *  -90.f * ((getCurrentY() / h2) - 1.f);
    theCamera->rotate(m_upVector, degToRad(alpha), false);
    theCamera->rotateX(degToRad(beta));

    // redraw required
    return true;
  }

  // no redraw required
  return false;
}

void WalkCameraManipulator::recordPlanarCameraDirection()
{
  if( getViewState() )
  {
    CameraSharedPtr camera = ViewStateReadLock(getViewState())->getCamera();
    if (camera)
    {
      CameraReadLock theCamera(camera);

      // store the vector parallel to the walking plane 
      Vec3f side = theCamera->getDirection()^m_upVector;
      m_saveCameraDirection = m_upVector^side;
      m_saveCameraDirection.normalize();
    }
  }
}

