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

#pragma once
/** \file */

#include <nvsgcommon.h>
#include <nvsg/CoreTypes.h>

#include "ui/Manipulator.h"

/*! \brief This Manipulator controls the camera animation.
 *  \par Namespace: nvui
 *  \remarks This Manipulator is a special Manipulator that sets the animation data (frame number)
 *  in the current nvsg::ViewState and nvsg::Camera objects.
 *  \n
 *  If you use this manipulator, your application must use a mechanism
 *  such as using OnIdle() that calls updateFrame(). If updateFrame() returns true,
 *  you should call triggerRedraw of the render area so the updated frame will be drawn.
 *  You will find some code samples in the description of the Manipulator class. Also,
 *  the SimpleViewer tutorial source code shows how to use a manipulator with MFC,
 *  the ConsoleViewer tutorial source code shows the details on how to use a manipulator, and
 *  the ManipSampleViewer tutorial source code shows how to derive a new Manipulator class.
 *  \sa Manipulator*/
class AnimationManipulator : public Manipulator 
{
public:
    /*! \brief Default constructor.
     *  \remarks Initialize the AnimationManipulator object.*/
    AnimationManipulator();

    /*! \brief Default Destructor
     *  \remarks The destructor cleans up the AnimationManipulator object and takes care of
     *  incrementing the reference count of the nvsg::Animation<nvmath::Trafo> object. */
    virtual ~AnimationManipulator();

    /*! \brief Set the camera animation.
     *  \param animation Pointer to an Animation object. NULL is a valid value to reset the
     *  camera animation.
     *  \remarks This function sets the current camera animation. If there is already a valid
     *  camera animation defined in this Manipulator the reference count of the old
     *  camera animation object will be decremented, and the reference count of the new
     *  camera animation object will be incremented.\n
     *  One can use NULL to force the Manipulator to not use any camera animation.
     *  \sa nvsg::Animation */
    void setCameraAnimation( const nvsg::TrafoAnimationSharedPtr & animation );

    /*! \brief Jump to the next animation frame.
     *  \remarks Simply set the counter to the next frame in the row.
     *  If the last frame is reached and this function is called, the frame counter will
     *  be set back to the first frame of the animation. So the application can loop
     *  through the animation by simply calling nextFrame.
     *  \sa previousFrame */
    void nextFrame();

    /*! \brief Jump to the previous animation frame.
     *  \remarks Simply set the counter to the previous frame in the row.
     *  If the first frame is reached and this function is called, the frame counter will
     *  be set back to the last frame of the animation. So the application can loop
     *  through the animation by simply calling previousFrame.
     *  \sa nextFrame */
    void previousFrame();

    /*! \brief Updates the manipulator's timestamp, and runs the manipulator.
     *  \param dt Delta time passage since this function was called last, in seconds.
     *  \remarks If FramesPerSecond is set to 0, then the manipulator operates in the older "frame-based"
     *  mode where nextFrame and previousFrame will advance the animation.  Otherwise if FPS > 0 then the
     *  animation will advance based on the deltaT provided.
     *  \sa setAnimationFramesPerSecond, nextFrame, previousFrame */
    virtual bool updateFrame( float dt );

    /*! \brief Checks to see if this manipulator is animating.
     *  \return true if the manipulator is animating, false otherwise.
     **/
    bool isActive() const;

    /*! \brief Sets this manipulator's time-based animation Frame Rate
     *  \param fps The number of frames per second to advance the scene's animations.  If this value is
     *  set to 0, then the manipulator operates in the older "frame-based" mode where nextFrame and previousFrame
     *  will advance the animation as before.
     *  \sa getAnimationFramesPerSecond, nextFrame, previousFrame */
    void setAnimationFramesPerSecond( double fps );

    /*! \brief Gets this manipulator's time-based animation Frame Rate
     *  \return The number of frames per second used to advance the scene's animations.
     *  \sa setAnimationFramesPerSecond, nextFrame, previousFrame */
    double getAnimationFramesPerSecond() const;
    
protected:
    // we have different kinds of animations: camera animation,
    // skin / bone animation transformation animation ...
    nvsg::TrafoAnimationSharedPtr m_cameraAnimation;  //!< Camera animation information.
    unsigned int                  m_currentTick;      //!< Wall clock tick (actually, frame number)
    int                           m_delta;            //!< Steps to do on next/previous.
    double                        m_fps;              //!< Frames per sec
    double                        m_currentTime;      //!< current time
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inlines
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline void AnimationManipulator::nextFrame()
{
    NVSG_TRACE();
    m_currentTick++;
    m_delta++;
}

inline void AnimationManipulator::previousFrame()
{
    NVSG_TRACE();
    m_currentTick++;
    m_delta--;
}

inline void AnimationManipulator::setAnimationFramesPerSecond( double fps )
{
    m_fps = fps;
}

inline double AnimationManipulator::getAnimationFramesPerSecond() const
{
    return m_fps;
}

