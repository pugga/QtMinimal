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


#include "ui/TrackballCameraManipulator.h"

#include <nvtraverser/RayIntersectTraverser.h>
#include <nvsg/FrustumCamera.h>
#include <nvutil/DbgNew.h> // this must be the last include

using namespace nvui;
using namespace nvsg;
using namespace nvmath;
using namespace nvtraverser;
using namespace nvutil;

template <typename T>
inline void TrackballCameraManipulator::checkLockAxis(T dx, T dy)
{
  if ( m_lockMajorAxis )
  {
    if ( !(m_lockAxis[AXIS_X] | m_lockAxis[AXIS_Y]) )
    {
       m_activeLockAxis[AXIS_X] = abs(dx)>abs(dy);
       m_activeLockAxis[AXIS_Y] = abs(dx)<abs(dy);
    }
  }
  else
  {
    m_activeLockAxis[AXIS_X] = m_lockAxis[AXIS_X];
    m_activeLockAxis[AXIS_Y] = m_lockAxis[AXIS_Y];
  }
}

TrackballCameraManipulator::TrackballCameraManipulator()
: Manipulator()
, CursorState()
, m_mode(MODE_NONE)
, m_speed( 0.001f )
, m_lockMajorAxis( false )
{
  m_lockAxis[AXIS_X]       = m_lockAxis[AXIS_Y]       = m_lockAxis[AXIS_Z]       = false;
  m_activeLockAxis[AXIS_X] = m_activeLockAxis[AXIS_Y] = m_activeLockAxis[AXIS_Z] = false;
}

TrackballCameraManipulator::~TrackballCameraManipulator()
{
}

void TrackballCameraManipulator::reset()
{
  resetInput();
}

bool TrackballCameraManipulator::updateFrame( float dt )
{
  NVSG_TRACE();
  bool result = false;

  if ( getViewState() && getRenderTarget() )
  {
    switch ( m_mode )
    {
    case MODE_ORBIT:
      result = orbit();
      break;

    case MODE_PAN:
      result = pan();
      break;

    case MODE_ROTATE_XY:
      result = rotate();
      break;

    case MODE_DOLLY:
      result = dolly();
      break;

    case MODE_ROLL_Z:
      result = roll();
      break;

    case MODE_ZOOM_FOV:
      result = zoom();
      break;

    case MODE_ZOOM_DOLLY:
      result = dollyZoom();
      break;

    case MODE_LOOKAT:
      result = lookAt();
      break;

    default:
      break;
    }

    if ( getWheelTicksDelta() && (m_mode != MODE_DOLLY) )
    {
      result = dolly();
    }
  }

  return result;
}

bool TrackballCameraManipulator::pan()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);
  if ( m_viewState )
  {
    ViewStateReadLock theViewState(m_viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      CameraWriteLock theCamera(camera);
      int dx = getLastX() - getCurrentX();
      int dy = getCurrentY() - getLastY();

      if (dx != 0 || dy != 0)
      {

        checkLockAxis(dx, dy);

        float stepX = m_speed * theViewState->getTargetDistance() * float(dx);
        float stepY = m_speed * theViewState->getTargetDistance() * float(dy);

        if( m_activeLockAxis[AXIS_X] )
        {
          if(dx!=0)
            stepY =0;
          else
            return false;
        }
        else if( m_activeLockAxis[AXIS_Y] )
        {
          if(dy!=0)
            stepX =0;
          else
            return false;
        }

        // construct the camera movement plane
        Vec3f side = theCamera->getDirection()^theCamera->getUpVector();
        side.normalize();

        NVSG_ASSERT(isNormalized(theCamera->getUpVector()));
        theCamera->setPosition(theCamera->getPosition() + stepX * side + stepY * theCamera->getUpVector());

        // redraw required
        return true;
      }
    }
  }

  // no redraw required
  return false;
}

bool TrackballCameraManipulator::dolly()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);
  if ( m_viewState )
  {
    ViewStateWriteLock theViewState(m_viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      CameraWriteLock theCamera(camera);

      int dy = getWheelTicksDelta();
      if (!dy)
      {
        dy = getCurrentY()-getLastY();
      }

      if (dy != 0)
      {
        float targetDistance = theViewState->getTargetDistance();
        float step = m_speed * targetDistance * float(dy);

        if ( (targetDistance - step) <= 0.0f )
        {
          // don't move to a position that either is on the target point or behind it
          return false;
        }

        NVSG_ASSERT( isNormalized(theCamera->getDirection()));
        theCamera->setPosition(theCamera->getPosition()  + step * theCamera->getDirection());

        theViewState->setTargetDistance(targetDistance - step);
        // redraw required
        return true;
      }
    }
  }

  // no redraw required
  return false;
}

bool TrackballCameraManipulator::zoom()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);
  if ( m_viewState )
  {
    ViewStateWriteLock theViewState( m_viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      CameraWriteLock theCamera(camera);
      int dy = getCurrentY()-getLastY();
      if (dy != 0)
      {
        float targetDistance = theViewState->getTargetDistance();
        float step =  m_speed * targetDistance * float(dy);

        NVSG_ASSERT( isNormalized(theCamera->getDirection()));
        theCamera->zoom(targetDistance / (targetDistance-step));
        // redraw required
        return true;
      }
    }
  }
  // no redraw required
  return false;
}

bool TrackballCameraManipulator::dollyZoom()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);
  if ( m_viewState )
  {
    ViewStateWriteLock theViewState(m_viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      CameraWriteLock theCamera(camera);

      int dy = getCurrentY()-getLastY();
      if (dy != 0)
      {
        float targetDistance = theViewState->getTargetDistance();
        float step =  m_speed * targetDistance * float(dy);

        NVSG_ASSERT( isNormalized(theCamera->getDirection()));
        theCamera->setPosition(theCamera->getPosition()  + step * theCamera->getDirection());
        theCamera->zoom(targetDistance / (targetDistance-step));

        theViewState->setTargetDistance(targetDistance - step);
        // redraw required
        return true;
      }
    }
  }

  // no redraw required
  return false;
}

bool TrackballCameraManipulator::rotate()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);
  if ( m_viewState )
  {
    ViewStateReadLock theViewState(m_viewState);

    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      float halfWndX = float(getRenderTarget()->getWidth())  * 0.5f;
      float halfWndY = float(getRenderTarget()->getHeight()) * 0.5f;

      int lastX = m_orbitCursor[0];
      int lastY = m_orbitCursor[1];
      {
        CameraWriteLock theCamera(camera);
        theCamera->setPosition( m_orbitCameraPosition );
        theCamera->setOrientation( m_orbitCameraOrientation );
      }

      Vec2f p0( (float(lastX) - halfWndX)  / halfWndX
              , (float(halfWndY) - lastY)  / halfWndY);

      Vec2f p1( (float(getCurrentX()) - halfWndX)  / halfWndX
              , (float(halfWndY) - getCurrentY())  / halfWndY);

      if (p0 != p1)
      {
        float dx = p1[0] - p0[0];
        float dy = p1[1] - p0[1];

        checkLockAxis(dx, dy);

        Vec3f axis;
        Vec2f m = p1-p0;
        axis = Vec3f(m[1], -m[0],0.0f);
        axis.normalize();

        if( m_activeLockAxis[AXIS_X] )
        {
          if(dx>0)
            axis = Vec3f(0.0f,-1.0f,0.0f);
          else if(dx<0)
            axis = Vec3f(0.0f,1.0f,0.0f);
          else
            return false;
        }
        else if( m_activeLockAxis[AXIS_Y] )
        {
          if(dy>0)
            axis = Vec3f(1.0f,0.0f,0.0f);
          else if(dy<0)
            axis = Vec3f(-1.0f,0.0f,0.0f);
          else
            return false;
        }
        CameraWriteLock theCamera(camera);
        theCamera->rotate( axis, nvmath::distance(p0,p1) );
      }
      // redraw required
      return true;
    }
  }
  // no redraw required
  return false;
}

bool TrackballCameraManipulator::roll()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);
  if ( m_viewState )
  {
    ViewStateReadLock theViewState(m_viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      float halfWndX = float(getRenderTarget()->getWidth())  * 0.5f;
      float halfWndY = float(getRenderTarget()->getHeight()) * 0.5f;

      Vec2f p0( (float(getLastX()) - halfWndX) / halfWndX
              , (float(halfWndY) - getLastY()) / halfWndY);

      Vec2f p1( (float(getCurrentX()) - halfWndX) / halfWndX
              , (float(halfWndY) - getCurrentY()) / halfWndY);

      if (p0 != p1)
      {
        // The z-coordinate sign of the p1 x p0 cross product controls the direction.
        float r = distance(p0, p1) * sign(p1[0] * p0[1] - p0[0] * p1[1]);

        CameraWriteLock theCamera(camera);
        theCamera->rotate( Vec3f(0.0f, 0.0f, 1.0f), r ); // camera relative
      }
      // redraw required
      return true;
    }
  }
  // no redraw required
  return false;
}

bool TrackballCameraManipulator::orbit()
{
  NVSG_TRACE();

  if ( m_viewState )
  {
    ViewStateReadLock theViewState(m_viewState);

    NVSG_ASSERT(getRenderTarget());
    NVSG_ASSERT(getRenderTarget()->getWidth());
    NVSG_ASSERT(getRenderTarget()->getHeight());

    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      float halfWndX = float(getRenderTarget()->getWidth())  * 0.5f;
      float halfWndY = float(getRenderTarget()->getHeight()) * 0.5f;

      int lastX = m_orbitCursor[0];
      int lastY = m_orbitCursor[1];
      {
        CameraWriteLock theCamera(camera);
        theCamera->setPosition( m_orbitCameraPosition );
        theCamera->setOrientation( m_orbitCameraOrientation );
      }
      
      int currentX = getCurrentX();
      int currentY = getCurrentY();
      
      Vec2f p0( (float(lastX) - halfWndX)  / halfWndX
              , (float(halfWndY) - lastY)  / halfWndY);

      Vec2f p1( (float(currentX) - halfWndX)  / halfWndX
              , (float(halfWndY) - currentY)  / halfWndY);

      if (p0 != p1)
      {

        float dx = p1[0] - p0[0];
        float dy = p1[1] - p0[1];

        checkLockAxis(dx, dy);

        Vec3f axis;
        float angle;
        m_trackball.apply(p0, p1, axis, angle);

        if ( m_activeLockAxis[AXIS_X] )
        {
          if(dx>0)
            axis = Vec3f(0.0f,1.0f,0.0f);
          else if(dx<0)
            axis = Vec3f(0.0f,-1.0f,0.0f);
          else
            return false;
        }
        else if( m_activeLockAxis[AXIS_Y] )
        {
          if(dy>0)
            axis = Vec3f(-1.0f,0.0f,0.0f);
          else if(dy<0)
            axis = Vec3f(1.0f,0.0f,0.0f);
          else
            return false;
        }

        CameraWriteLock theCamera(camera);
        theCamera->orbit( axis, theViewState->getTargetDistance(), angle );

        // redraw required
        return true;
      }
    }
  }

  // no redraw required
  return false;
}

bool TrackballCameraManipulator::lookAt()
{
  NVSG_TRACE();
  NVSG_ASSERT(m_viewState);

  bool needsRedraw = false;
  float hitDistance = 0.0f;
  Vec3f rayOrigin;
  Vec3f rayDir;

  if ( m_viewState )
  {
    ViewStateReadLock viewState(m_viewState);

    NVSG_ASSERT(getRenderTarget());
    NVSG_ASSERT(getRenderTarget()->getWidth());
    NVSG_ASSERT(getRenderTarget()->getHeight());

    CameraSharedPtr cameraHdl = viewState->getCamera();
    if (cameraHdl && isPtrTo<FrustumCamera>(cameraHdl) )
    {
      SmartPtr<RayIntersectTraverser> picker( new nvtraverser::RayIntersectTraverser );

      // calculate ray origin and direction from the input point
      int vpW = getRenderTarget()->getWidth();
      int vpH = getRenderTarget()->getHeight();
      int pkX = getCurrentX();       // at mouse-up, not mouse-down
      int pkY = vpH - getCurrentY(); // pick point is lower-left-relative

      FrustumCameraReadLock(sharedPtr_cast<FrustumCamera>(cameraHdl))->getPickRay(pkX, pkY, vpW, vpH, rayOrigin, rayDir);

      // run the intersect traverser for intersections with the given ray
      picker->setRay(rayOrigin, rayDir);
      picker->setViewState(m_viewState);
      picker->setViewportSize(vpW, vpH);
      picker->apply(viewState->getScene());

      if (picker->getNumberOfIntersections() > 0)
      {
        needsRedraw = true;
        hitDistance = picker->getNearest().getDist();
      }
    }
  }

  if(needsRedraw)
  {
    ViewStateWriteLock viewState(m_viewState);
    viewState->setTargetDistance(hitDistance);

    CameraWriteLock camera(viewState->getCamera());
    camera->setPosition(rayOrigin);
    camera->setDirection(rayDir);
  }

  return needsRedraw;
}

void TrackballCameraManipulator::setMode( Mode mode )
{
  if ( m_mode != mode && m_viewState )
  {
    // Store cursor position and camera position on mode change. These values are being used for non-incremental updates.
    m_orbitCursor = getCursorPosition();
    ViewStateReadLock viewState(m_viewState);
    CameraReadLock camera(viewState->getCamera());
    m_orbitCameraPosition = camera->getPosition();
    m_orbitCameraOrientation = camera->getOrientation();

    m_mode = mode;
  }
}

void TrackballCameraManipulator::lockAxis( Axis axis )
{
  m_lockAxis[axis] = true;
}

void TrackballCameraManipulator::unlockAxis( Axis axis )
{
  m_lockAxis[axis] = false;
}

void TrackballCameraManipulator::lockMajorAxis( )
{
  m_lockMajorAxis = true;
}

void TrackballCameraManipulator::unlockMajorAxis( )
{
  m_lockMajorAxis = false;
}

