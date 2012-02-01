// Copyright NVIDIA Corporation 2002-2009
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#include "ui/CylindricalCameraManipulator.h"

#include "SceneFunctions.h"
#include <nvtraverser/RayIntersectTraverser.h>
#include <nvsg/Camera.h>
#include <nvsg/PerspectiveCamera.h>

#include <nvutil/DbgNew.h> // this must be the last include

using namespace nvui;
using namespace nvsg;
using namespace nvmath;
using namespace nvutil;

template <typename T>
inline void CylindricalCameraManipulator::checkLockAxis(T dx, T dy)
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


CylindricalCameraManipulator::CylindricalCameraManipulator()
: Manipulator()
, CursorState()
, m_lockMajorAxis( false )
, m_speed(0.001f)
{
  m_lockAxis[0] = m_lockAxis[1] = m_lockAxis[2] = false;
  m_activeLockAxis[0] = m_activeLockAxis[1] = m_activeLockAxis[2] = false;
}

CylindricalCameraManipulator::~CylindricalCameraManipulator()
{
}

void CylindricalCameraManipulator::reset()
{
  resetInput();
}

void CylindricalCameraManipulator::setMode( Mode mode )
{
  if ( getMode() != mode && getViewState() )
  {
    // Store cursor position and camera position on mode change. These values are being used for non-incremental updates.
#if 0
    m_orbitCursor = getCursorPosition();
    ViewStateReadLock viewState(m_viewState);
    CameraReadLock camera(viewState->getCamera());
    m_orbitCameraPosition = camera->getPosition();
    m_orbitCameraOrientation = camera->getOrientation();
#endif

    m_mode = mode;
  }
}

bool CylindricalCameraManipulator::updateFrame( float dt )
{
  bool result = false;

  if ( getViewState() && getRenderTarget() )
  {
    switch ( getMode() )
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

  // NOTE: we are missing:
 // New custom operations!
 //else if (isSetPivot())
 // {
 //   result = setPivot();
 // }
 // else if (isSetFocus())
 // {
 //   result = setFocus();
 // }

  return result;
}

bool CylindricalCameraManipulator::pan()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateReadLock theViewState(viewState);
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

        if( m_activeLockAxis[ AXIS_X ] )
        {
          if(dx != 0)
          {
            stepY = 0;
          }
          else
          {
            return false;
          }
        }
        else if( m_activeLockAxis[ AXIS_Y ] )
        {
          if(dy != 0)
          {
            stepX = 0;
          }
          else
          {
            return false;
          }
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

// Dolly without moving the focus point for easier use with depth of field.
// The minimum focus distance is clamped to DOFCamera::m_focalLength * NEAREST_FOCUS_SCALE.
// Use RMB and Ctrl+RMB to change the focus distance.
bool CylindricalCameraManipulator::dolly()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateWriteLock theViewState(viewState);

    if ( isPtrTo<PerspectiveCamera>(theViewState->getCamera()) )
    {
      PerspectiveCameraSharedPtr camera( sharedPtr_cast<PerspectiveCamera>(theViewState->getCamera()) );
      PerspectiveCameraWriteLock theCamera(camera);

      int dy = getCurrentY() - getLastY();
      float multiplier = 1.0f;

#if 0
      if ( m_flags & NVSG_WHEELING )
      {
        // take accumulated deltas if user is wheeling
        dy = getWheelTicks();
         
        // Temporary speedup/slowdown of mouse wheel dolly operation.
        if (getKeyState() & NVSG_SHIFT) // Speedup has precedence.
        {
          multiplier = 4.0f;
        }
        else if (getKeyState() & NVSG_CONTROL)
        {
          multiplier = 0.25f;
        }
      }
#else
      // see if we are wheeling
      if ( getWheelTicksDelta() )
      {
        // take accumulated deltas if user is wheeling
        dy = getWheelTicksDelta();
         
        // MMM - implement speed in sync
#if 0
        // Temporary speedup/slowdown of mouse wheel dolly operation.
        if (getKeyState() & NVSG_SHIFT) // Speedup has precedence.
        {
          multiplier = 4.0f;
        }
        else if (getKeyState() & NVSG_CONTROL)
        {
          multiplier = 0.25f;
        }
#endif
      }
#endif

      if (dy != 0)
      {
        float targetDistance = theViewState->getTargetDistance();
        float step = multiplier * m_speed * targetDistance * float(dy);

        NVSG_ASSERT( isNormalized(theCamera->getDirection()) );
        theCamera->setPosition(theCamera->getPosition() + step * theCamera->getDirection());

        // This is an arbitrarily chosen minimum distance, assumed to be 1 unit == 1 cm.  This
        // may not work in very tiny environments.
        theCamera->setFocusDistance(std::max(theCamera->getFocusDistance() - step, 1.0f));

        theViewState->setTargetDistance(targetDistance - step);

        // redraw required
        return true;
      }
    }
  }

  // no redraw required
  return false;
}


bool CylindricalCameraManipulator::zoom()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateWriteLock theViewState(viewState);
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

bool CylindricalCameraManipulator::dollyZoom()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateWriteLock theViewState(viewState);
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

// Roll free rotate (Ctrl+LMB drag)
bool CylindricalCameraManipulator::rotate()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateReadLock theViewState(viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      float halfWndX = float(getRenderTarget()->getWidth())  * 0.5f;
      float halfWndY = float(getRenderTarget()->getHeight()) * 0.5f;

      Vec2f p0( (float(getLastX()) - halfWndX)  / halfWndX
              , (float(halfWndY) - getLastY())  / halfWndY);

      Vec2f p1( (float(getCurrentX()) - halfWndX)  / halfWndX
              , (float(halfWndY) - getCurrentY())  / halfWndY);

      if (p0 != p1)
      {
        float dx = p1[0] - p0[0];
        float dy = p1[1] - p0[1];

        CameraWriteLock theCamera(camera);

        theCamera->rotate( Vec3f(0.0f, 1.0f, 0.0f), -dx, false); // world relative
        theCamera->rotate( Vec3f(1.0f, 0.0f, 0.0f),  dy, true ); // camera relative

        // redraw required
        return true;
      }
    }
  }
  // no redraw required
  return false;
}


// Roll camera around z axis (Ctrl+MMB drag)
bool CylindricalCameraManipulator::roll()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateReadLock theViewState(viewState);
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

        // redraw required
        return true;
      }
    }
  }
  // no redraw required
  return false;
}

// Roll free orbit (LMB drag)
bool CylindricalCameraManipulator::orbit()
{
  NVSG_ASSERT(getRenderTarget()->getWidth());
  NVSG_ASSERT(getRenderTarget()->getHeight());

  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateReadLock theViewState(viewState);
    CameraSharedPtr camera = theViewState->getCamera();
    if (camera)
    {
      float halfWndX = float(getRenderTarget()->getWidth())  * 0.5f;
      float halfWndY = float(getRenderTarget()->getHeight()) * 0.5f;

      int lastX = getLastX();
      int lastY = getLastY();
      int currentX = getCurrentX();
      int currentY = getCurrentY();
      
#if 0
      if ( m_flags & NVSG_SPIN )
      {
        lastX = m_startSpinX;
        lastY = m_startSpinY;
        currentX = m_currentSpinX;
        currentY = m_currentSpinY;
      }
#endif

      Vec2f p0( (float(lastX) - halfWndX)  / halfWndX
              , (float(halfWndY) - lastY)  / halfWndY);

      Vec2f p1( (float(currentX) - halfWndX)  / halfWndX
              , (float(halfWndY) - currentY)  / halfWndY);

      if (p0 != p1)
      {
        float dx = p1[0] - p0[0];
        float dy = p1[1] - p0[1];

        CameraWriteLock theCamera(camera);

        theCamera->orbit( Vec3f(0.0f, 1.0f, 0.0f), theViewState->getTargetDistance(),  dx * PI_HALF, false ); // world relative
        theCamera->orbit( Vec3f(1.0f, 0.0f, 0.0f), theViewState->getTargetDistance(), -dy * PI_HALF, true  ); // camera relative

        // redraw required
        return true;
      }
    }
  }

  // no redraw required
  return false;
}

bool CylindricalCameraManipulator::lookAt()
{
  NVSG_ASSERT(getRenderTarget()->getWidth());
  NVSG_ASSERT(getRenderTarget()->getHeight());

  bool needsRedraw = false;
  float hitDistance = 0.0f;
  Vec3f rayOrigin;
  Vec3f rayDir;

  ViewStateSharedPtr viewStateHdl = getViewState();
  if ( viewStateHdl )
  {
    ViewStateReadLock viewState(viewStateHdl);

    if ( isPtrTo<FrustumCamera>(viewState->getCamera()) )
    {
      FrustumCameraSharedPtr cameraHdl( sharedPtr_cast<FrustumCamera>(viewState->getCamera()) );
      nvutil::SmartPtr<nvtraverser::RayIntersectTraverser> picker( new nvtraverser::RayIntersectTraverser );

      // calculate ray origin and direction from the input point
      int vpW = getRenderTarget()->getWidth();
      int vpH = getRenderTarget()->getHeight();
      int pkX = getCurrentX();           // at mouse-up, not mouse-down
      int pkY = vpH - 1 - getCurrentY(); // pick point is lower-left-relative

      FrustumCameraReadLock(cameraHdl)->getPickRay(pkX, pkY, vpW, vpH, rayOrigin, rayDir);

      // run the intersect traverser for intersections with the given ray
      picker->setRay(rayOrigin, rayDir);
      picker->setViewState(viewStateHdl);
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
    ViewStateWriteLock viewState(viewStateHdl);
    viewState->setTargetDistance(hitDistance);

    CameraWriteLock camera(viewState->getCamera());
    camera->setPosition(rayOrigin);
    camera->setDirection(rayDir);
  }

  return needsRedraw;
}

bool CylindricalCameraManipulator::setPivot()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateReadLock theViewState(viewState);
    if ( isPtrTo<PerspectiveCamera>(theViewState->getCamera()) )
    {
      PerspectiveCameraSharedPtr camera( sharedPtr_cast<PerspectiveCamera>(theViewState->getCamera()) );
      PerspectiveCameraWriteLock theCamera(camera);

      int vpW = getRenderTarget()->getWidth();
      int vpH = getRenderTarget()->getHeight();

      if (vpW && vpH)
      {
        nvutil::SmartPtr<nvtraverser::RayIntersectTraverser> picker( new nvtraverser::RayIntersectTraverser );

        int pkX = getCurrentX();           // at mouse-up, not mouse-down
        int pkY = vpH - 1 - getCurrentY(); // pick point is lower-left-relative
        Vec3f rayOrigin;
        Vec3f rayDir;

        theCamera->getPickRay(pkX, pkY, vpW, vpH, rayOrigin, rayDir);

        // run the intersect traverser for intersections with the given ray
        picker->setRay(rayOrigin, rayDir);
        picker->setViewState(viewState);
        picker->setViewportSize(vpW, vpH);
        picker->apply(theViewState->getScene());

        if (picker->getNumberOfIntersections() > 0)
        {
          float distance = picker->getNearest().getDist();

          if (0.0f < distance)
          {
            Vec3f pivotOld(theCamera->getPosition() + theCamera->getDirection() * theViewState->getTargetDistance());
            Vec3f pivotNew(rayOrigin + rayDir * distance);

            // Change the camera position instead of direction to make this pivot point setting roll free.
            theCamera->move(pivotNew - pivotOld, false); // world space
            
            // Also set the focus distance!
            // Do not focus nearer than lens' lower focus limit.
            distance = nvmath::distance( theCamera->getPosition(), pivotNew);
            theCamera->setFocusDistance(std::max(0.1f, distance));

            // redraw required
            return true;
          }
        }
      }
    }
  }

  // no redraw required
  return false;
}

bool CylindricalCameraManipulator::setFocus()
{
  ViewStateSharedPtr viewState = getViewState();
  if ( viewState )
  {
    ViewStateReadLock theViewState(viewState);
    
    if ( isPtrTo<PerspectiveCamera>(theViewState->getCamera()) )
    {
      PerspectiveCameraSharedPtr camera( sharedPtr_cast<PerspectiveCamera>(theViewState->getCamera()) );
      PerspectiveCameraWriteLock theCamera(camera);

      int vpW = getRenderTarget()->getWidth();
      int vpH = getRenderTarget()->getHeight();
      
      if (vpW && vpH)
      {
        nvutil::SmartPtr<nvtraverser::RayIntersectTraverser> picker( new nvtraverser::RayIntersectTraverser );

        int pkX = getCurrentX();           // at mouse-up, not mouse-down
        int pkY = vpH - 1 - getCurrentY(); // pick point is lower-left-relative
        Vec3f rayOrigin;
        Vec3f rayDir;

        theCamera->getPickRay(pkX, pkY, vpW, vpH, rayOrigin, rayDir);

        // run the intersect traverser for intersections with the given ray
        picker->setRay(rayOrigin, rayDir);
        picker->setViewState(viewState);
        picker->setViewportSize(vpW, vpH);
        picker->apply(theViewState->getScene());

        if (picker->getNumberOfIntersections() > 0)
        {
          float distance = picker->getNearest().getDist();
          if (0.0f < distance)
          {
            // Only set the focus distance, pivot point remains the same.
            // Do not focus nearer than lens' lower focus limit.
            theCamera->setFocusDistance(std::max(0.1f, distance));
          }
          // redraw required
          return true;
        }
      }
    }
  }
  // no redraw required
  return false;
}

void CylindricalCameraManipulator::lockAxis( Axis axis )
{
  m_lockAxis[axis] = true;
}

void CylindricalCameraManipulator::unlockAxis( Axis axis )
{
  m_lockAxis[axis] = false;
}

void CylindricalCameraManipulator::lockMajorAxis( )
{
  m_lockMajorAxis = true;
}

void CylindricalCameraManipulator::unlockMajorAxis( )
{
  m_lockMajorAxis = false;
}

