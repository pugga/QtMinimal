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


#include <nvsg/ViewState.h>
#include <nvmath/Trafo.h>
#include <nvsg/Animation.h>
#include "ui/AnimationManipulator.h"

#include <nvutil/DbgNew.h> // this must be the last include

using namespace nvui;
using namespace nvsg;
using namespace nvutil;
using namespace nvmath;

AnimationManipulator::AnimationManipulator(void)
: m_currentTick(0)
, m_delta(0)
, m_fps( 0.0 )
, m_currentTime( 0.0 )
{
  NVSG_TRACE();
}

AnimationManipulator::~AnimationManipulator(void)
{
  NVSG_TRACE();
}

bool AnimationManipulator::updateFrame( float dt )
{
  NVSG_TRACE();
  bool redrawRequired = false;

  // if fps > 0 then we advance current tick and m_delta appropriately
  if( m_fps > 0.0 )
  {
    m_currentTime += dt;

    unsigned int lastTick = m_currentTick;
    m_currentTick = (unsigned int)( m_currentTime * m_fps );

    m_delta = m_currentTick - lastTick;
  }

  if ( getViewState() )
  {
    ViewStateWriteLock theViewState(getViewState());
    // update the current frame in the ViewState object for all kinds of animations
    redrawRequired |= theViewState->advanceAnimation( m_currentTick, m_delta );
  
    // update the camera animation
    if (m_cameraAnimation && theViewState->getCamera())
    {
      TrafoAnimationWriteLock animation(m_cameraAnimation);
      if ( animation->advance( m_currentTick, m_delta ) )
      {
        CameraWriteLock theCamera(theViewState->getCamera());
        // update the camera animation
        Trafo trafo = animation->getCurrentValue();
        theCamera->setOrientation( trafo.getOrientation() );
        theCamera->setPosition( trafo.getTranslation() );

        redrawRequired = true;
      }
    }
  }
  m_delta = 0;
  return( redrawRequired );
}

void AnimationManipulator::setCameraAnimation( const TrafoAnimationSharedPtr & animation )
{
  NVSG_TRACE();

  m_cameraAnimation = animation;
}

bool AnimationManipulator::isActive() const
{
  NVSG_TRACE();
  if ( m_cameraAnimation )
  {
    // report back active if the animation is running
    return TrafoAnimationReadLock(m_cameraAnimation)->isRunning();
  }

  return false;
}
