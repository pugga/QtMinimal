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


#include "ui/TrackballTransformManipulator.h"
#include <nvsg/ViewState.h>
#include <nvsg/Transform.h>
#include <nvsg/FrustumCamera.h>
#include <nvsg/Path.h>

#include <nvutil/DbgNew.h> // this must be the last include

using namespace nvmath;
using namespace nvsg;
using namespace nvui;
using namespace nvutil;

template <typename T>
inline void TrackballTransformManipulator::checkLockAxis(T dx, T dy)
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

TrackballTransformManipulator::TrackballTransformManipulator()
  : Manipulator()
  , CursorState()
  , m_mode( MODE_NONE )
  , m_lockMajorAxis( false )
{
  m_lockAxis[AXIS_X]       = m_lockAxis[AXIS_Y]       = m_lockAxis[AXIS_Z] = false;
  m_activeLockAxis[AXIS_X] = m_activeLockAxis[AXIS_Y] = m_activeLockAxis[AXIS_Z] = false;
}

TrackballTransformManipulator::~TrackballTransformManipulator()
{
}

void TrackballTransformManipulator::reset()
{
  resetInput();
}

bool TrackballTransformManipulator::updateFrame( float dt )
{
  bool result = false;

  if( m_transformPath && getViewState() && getRenderTarget() )
  {
    switch( m_mode )
    {
      case MODE_ROTATE:
        result = rotate();
        break;

      case MODE_PAN:
        result = pan();
        break;

      case MODE_DOLLY:
        result = dolly();
        break;

      default:
        break;
    }
  }

  return result; 
}

bool  TrackballTransformManipulator::pan()
{
  int dxScreen = getCurrentX() - getLastX();
  int dyScreen = getLastY() - getCurrentY();
  if ( dxScreen || dyScreen )
  {
    ViewStateReadLock theViewState( getViewState() );
    NVSG_ASSERT( isPtrTo<FrustumCamera>(theViewState->getCamera()) );
    TransformWeakPtr transform = dynamic_cast<TransformWeakPtr>(m_transformPath->getTail());
    FrustumCameraSharedPtr camera = sharedPtr_cast<FrustumCamera>(theViewState->getCamera());
    if ( camera && transform )
    {
      FrustumCameraReadLock theCamera(camera);
      TransformWriteLock theTransform(transform);

      unsigned int rtWidth = getRenderTarget()->getWidth();
      unsigned int rtHeight = getRenderTarget()->getHeight();
      Vec2f  camWinSize = theCamera->getWindowSize();
      if (    ( 0 < rtHeight ) && ( 0 < rtWidth )
          &&  ( FLT_EPSILON < fabs( camWinSize[0] ) )
          &&  ( FLT_EPSILON < fabs( camWinSize[1] ) ) )
      {
        //  get all the matrices needed here
        Mat44f m2w, w2m;
        m_transformPath->getModelToWorldMatrix(m2w, w2m); // model->world and world->model
        Mat44f w2v = theCamera->getWorldToViewMatrix();   // world->view
        Mat44f v2w = theCamera->getViewToWorldMatrix();   // view->world

        //  center of the object in view coordinates
        Vec4f center = Vec4f( theTransform->getBoundingSphere().getCenter(), 1.0f ) * m2w * w2v;

        //  window size at distance of the center of the object
        Vec2f centerWindowSize = - center[2] / theViewState->getTargetDistance() * camWinSize;

        checkLockAxis(dxScreen, dyScreen);
        if ( m_activeLockAxis[AXIS_X] )
        {
          if ( dxScreen != 0 )
          {
            dyScreen = 0;
          }
          else 
          {
            return false;
          }
        }
        else if ( m_activeLockAxis[AXIS_Y] )
        {
          if ( dyScreen != 0)
          {
            dxScreen = 0;
          }
          else
          {
            return false;
          }
        }

        //  delta in model coordinates
        Vec4f viewCenter( centerWindowSize[0] * dxScreen / rtWidth
                        , centerWindowSize[1] * dyScreen / rtHeight, 0.f, 0.f );
        Vec4f modelDelta = viewCenter * v2w * w2m;
        
        // add the delta to the translation of the transform
        Trafo trafo = theTransform->getTrafo();
        trafo.setTranslation( trafo.getTranslation() + Vec3f( modelDelta ) );
        theTransform->setTrafo( trafo );

        return true;  
      }
    }
  }
  return false;
}

bool TrackballTransformManipulator::rotate()
{
  NVSG_TRACE();
  if ( ( getCurrentX() != getLastX() ) || ( getCurrentY() != getLastY() ) )
  {
    ViewStateReadLock theViewState(getViewState());
    NVSG_ASSERT( isPtrTo<FrustumCamera>(theViewState->getCamera()) );
    TransformWeakPtr transform = dynamic_cast<TransformWeakPtr>(m_transformPath->getTail());
    FrustumCameraSharedPtr camera = sharedPtr_cast<FrustumCamera>(theViewState->getCamera());
    if ( camera && transform )
    {
      FrustumCameraReadLock theCamera(camera);
      TransformWriteLock theTransform(transform);
      unsigned int rtWidth    = getRenderTarget()->getWidth();
      unsigned int rtHeight   = getRenderTarget()->getHeight();
      Vec2f  camWinSize = theCamera->getWindowSize();
      if (    ( 0 < rtHeight ) && ( 0 < rtWidth )
          &&  ( FLT_EPSILON < fabs( camWinSize[0] ) )
          &&  ( FLT_EPSILON < fabs( camWinSize[1] ) ) )
      {
        //  get all the matrices needed here
        Mat44f m2w, w2m, w2v, v2w, v2s, m2v;
        m_transformPath->getModelToWorldMatrix( m2w, w2m ); // model->world and world->model
        w2v = theCamera->getWorldToViewMatrix();            // world->view
        v2w = theCamera->getViewToWorldMatrix();            // view->world
        v2s = theCamera->getProjection();                   // view->screen (normalized)
        m2v = m2w * w2v;

        const Sphere3f& bs = theTransform->getBoundingSphere();

        //  center of the object in view coordinates
        Vec4f centerV = Vec4f( bs.getCenter(), 1.0f ) * m2v;
        NVSG_ASSERT( fabs( centerV[3] - 1.0f ) < FLT_EPSILON );

        //  center of the object in normalized screen coordinates
        Vec4f centerNS = centerV * v2s;
        NVSG_ASSERT( centerNS[3] != 0.0f );
        centerNS /= centerNS[3];

        //  center of the object in screen space
        Vec2f centerS( rtWidth * ( 1 + centerNS[0] ) / 2, rtHeight * ( 1 - centerNS[1] ) / 2 );

        //  move the input points relative to the center 
        //  move the input points absolutely
        //Vec2f last( m_orbitCursor );
        Vec2f last( getLastCursorPosition() );
        Vec2f p0( last[0]    - centerS[0], centerS[1] - last[1] );
        Vec2f p1( getCurrentX() - centerS[0], centerS[1] - getCurrentY() );
        NVSG_ASSERT( p0[0] != p1[0] || p0[1] != p1[1] );

        //  get the scaling (from model to view)
        Vec3f scaling, translation;
        Quatf orientation, scaleOrientation;
        decompose( m2v, translation, orientation, scaling, scaleOrientation );
        float maxScale = std::max( scaling[0], std::max( scaling[1], scaling[2] ) );
        NVSG_ASSERT( FLT_EPSILON < fabs( maxScale ) );

        //  determine the radius in screen space (in the centers depth)
        Vec2f centerWindowSize = - centerV[2] / theViewState->getTargetDistance() * camWinSize;
        float radius = bs.getRadius() * maxScale * rtWidth / centerWindowSize[0];

        //  with p0, p1, and the radius determine the axis and angle of rotation via the Trackball utility
        //  => axis is in view space then
        Vec3f axis;
        float angle;
        m_trackball.setSize( radius );
        m_trackball.apply( p0, p1, axis, angle );
        
        float dx = p1[0]-p0[0];
        float dy = p1[1]-p0[1];

        checkLockAxis(dx, dy);

        if ( m_activeLockAxis[AXIS_X] )
        {
          if ( dx < 0 )
            axis = Vec3f(0.f, -1.f, 0.f);
          else if ( dx > 0)
            axis = Vec3f(0.f, 1.f, 0.f);
          else
            return false;
        }
        else if ( m_activeLockAxis[AXIS_Y] )
        {
          if ( dy < 0 ) 
            axis = Vec3f(1.f, 0.f, 0.f);
          else if ( dy > 0) 
            axis = Vec3f(-1.f, 0.f, 0.f);
          else 
            return false;
        }

        // transform axis back into model space
        axis = Vec3f( Vec4f( axis, 0.0f ) * v2w * w2m );
        axis.normalize();

        //  create the rotation around the center (in model space)
        Trafo trafo;
        trafo.setCenter( bs.getCenter() );
        trafo.setOrientation( Quatf( axis, angle ) );

        //  concatenate this rotation with the current transformation
        trafo.setMatrix( theTransform->getTrafo().getMatrix() * trafo.getMatrix() );

        //  concatenate this rotation with the original transformation
        //trafo.setMatrix( m_matrix * trafo.getMatrix() );

        //  set the current transform
        theTransform->setTrafo( trafo );

        return true;
      }
    }
  }

  return false;
}


bool TrackballTransformManipulator::dolly()
{
  int dyScreen = getLastY() - getCurrentY();
  if( !dyScreen )
  {
    dyScreen = getWheelTicksDelta();
  }

  if ( dyScreen )
  {
    NVSG_ASSERT( isPtrTo<FrustumCamera>(ViewStateReadLock(getViewState())->getCamera()) );
    TransformWeakPtr transform = dynamic_cast<TransformWeakPtr>(m_transformPath->getTail());
    FrustumCameraSharedPtr camera = sharedPtr_cast<FrustumCamera>(ViewStateReadLock(getViewState())->getCamera());
    if ( camera && transform )
    {
      FrustumCameraReadLock theCamera(camera);
      TransformWriteLock theTransform(transform);

      unsigned int rtWidth = getRenderTarget()->getWidth();
      unsigned int rtHeight = getRenderTarget()->getHeight();
      Vec2f  camWinSize = theCamera->getWindowSize();
      if (    ( 0 < rtHeight ) && ( 0 < rtWidth )
          &&  ( FLT_EPSILON < fabs( camWinSize[0] ) )
          &&  ( FLT_EPSILON < fabs( camWinSize[1] ) ) )
      {
        //  get all the matrices needed here
        Mat44f m2w, w2m, w2v, v2w;
        m_transformPath->getModelToWorldMatrix(m2w, w2m);   // model->world and world->model
        w2v = theCamera->getWorldToViewMatrix();               // world->view
        v2w = theCamera->getViewToWorldMatrix();            // view->world

        // transfer mouse delta into view space
        float dyView = camWinSize[1]/rtHeight * dyScreen;

        // transfer the mouse delta vector into the model space
        Vec4f modelDelta = Vec4f( 0.0f, 0.0f, dyView, 0.0f ) * v2w * w2m;

        // minus the delta to the translation of the transform
        // minus, because we want mouse down to move the object into the direction of the user
        Trafo trafo = theTransform->getTrafo();
        trafo.setTranslation( trafo.getTranslation() - Vec3f( modelDelta ) );
        theTransform->setTrafo( trafo );

        return true;
      }
    }
  }
  return false;
}

void TrackballTransformManipulator::setMode( Mode mode )
{
  if ( m_mode != mode && m_viewState )
  {
    // Store cursor position and camera position on mode change. These values are used for absolute updates.
    //m_orbitCursor = m_propCursor;
    //NVSG_ASSERT( getTransformPath() );
    //TransformWeakPtr transform = dynamic_cast<TransformWeakPtr>(getTransformPath()->getTail());
    //NVSG_ASSERT( transform );
    //m_matrix = TransformReadLock( transform )->getTrafo().getMatrix();
    m_mode = mode;
  }
}

void TrackballTransformManipulator::lockAxis( Axis axis )
{
  m_lockAxis[axis] = true;
}

void TrackballTransformManipulator::unlockAxis( Axis axis )
{
  m_lockAxis[axis] = false;
}

void TrackballTransformManipulator::lockMajorAxis( )
{
  m_lockMajorAxis = true;
}

void TrackballTransformManipulator::unlockMajorAxis( )
{
  m_lockMajorAxis = false;
}

void TrackballTransformManipulator::setTransformPath(nvsg::Path * transformPath)
{
  m_transformPath = transformPath;
}

void TrackballTransformManipulator::setTransformPath( const nvutil::SmartPtr<nvsg::Path> & transformPath )
{
  m_transformPath = transformPath;
}

const nvsg::Path * TrackballTransformManipulator::getTransformPath() const
{
  return m_transformPath.get();
}
