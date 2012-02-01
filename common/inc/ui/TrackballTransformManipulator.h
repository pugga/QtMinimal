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
#include <nvutil/Trackball.h>
#include <nvmath/Matnnt.h>
#include "ui/Manipulator.h"

namespace nvsg
{
class Path;
}

/*! \brief Simulate a trackball like object interaction.
 *
 *  This Manipulator uses the CursorState to drive a transform to do operations like
 *  pan, dolly, and rotate.
 *
 *  CursorState is updated by the derived class TrackballCameraManipulatorSync, and therefore users
 *  will probably not use this class directly.
 *
 *  \sa Manipulator, TrackballTransformManipulatorSync */
class TrackballTransformManipulator : public Manipulator, public CursorState
{
public:

    enum Mode
    {
        MODE_NONE = 0
        , MODE_PAN
        , MODE_ROTATE
        , MODE_DOLLY
    };

    enum Axis
    {
        AXIS_X = 0
        , AXIS_Y = 1
        , AXIS_Z = 2
    };

    /*! \brief Default constructor.
   *  \remarks Initialize the TrackballTransformManipulator object.*/
    TrackballTransformManipulator(void);

    /*! \brief Default destructor. */
    virtual ~TrackballTransformManipulator(void);

    /*! \brief Updates the manipulator's timestamp, and runs the manipulator.
   *  \param dt Delta time passage since this function was called last, in seconds.
   *  \remarks This function should be called continuously when this manipulator is active to update the
   *  ViewState's camera using the current flight model.
   */
    virtual bool updateFrame( float dt );

    /*! \brief Resets the manipulator to defaults.
   * \remarks Resets the manipulator to initial state.
   */
    virtual void reset();

    /*! \brief Set the mode this manipulator is operating in.
   *  \remarks The manipulator's mode determines how it updates the object upon every timestep. This
   *  Manipulator supports the following list of modes:
   *
   *  MODE_PAN     Translate the objects below the transform in the XZ plane.
   *  MODE_ROTATE  Rotate the object using a trackball style based on the object's bounding volume.
   *  MODE_DOLLY   Translate the object toward or away from the viewer.
   *
   *  \sa getMode */
    void setMode( Mode mode );

    /*! \brief Get the mode this manipulator is operating in.
   *  \return The manipulator's current mode.
   *  \sa setMode */
    Mode getMode() const;

    void lockAxis( Axis axis );
    void unlockAxis( Axis axis );

    void lockMajorAxis();
    void unlockMajorAxis();

    /*! \brief Set the nvsg::Path to the nvsg::Transform node.
   *  \param transformPath Complete path to the nvsg::Transform node. Null is a valid value to
   *  disconnect the TransformManipulator from the controlled object.
   *  \remarks Attach the Manipulator to the desired nvsg::Transform in the tree by providing
   *  a complete nvsg::Path from the root node to the nvsg::Transform node.\n
   *  This class takes care of incrementing and decrementing the reference count of the provided
   *  nvsg::Path object.
   *  \n\n
   *  The application is responsible to make sure that the nvsg::Path stays
   *  valid during the usage of the Manipulator.
   *  \sa getTransformPath */
    void setTransformPath(nvsg::Path * transformPath);

    /*! \brief Set the nvsg::Path to the nvsg::Transform node.
   *  \param transformPath Complete path to the nvsg::Transform node. Null is a valid value to
   *  disconnect the TransformManipulator from the controlled object.
   *  \remarks Attach the Manipulator to the desired nvsg::Transform in the tree by providing
   *  a complete nvsg::Path from the root node to the nvsg::Transform node.\n
   *  This class takes care of incrementing and decrementing the reference count of the provided
   *  nvsg::Path object.
   *  \n\n
   *  The application is responsible to make sure that the nvsg::Path stays
   *  valid during the usage of the Manipulator.
   *  \sa getTransformPath */
    void setTransformPath( const nvutil::SmartPtr<nvsg::Path> & transformPath );

    /*! \brief Get the nvsg::Path to the nvsg::Transform that currently is under control.
   *  \return Path to the controlled nvsg::Transform. If the TransformManipulator is not
   *  connected this function returns NULL.
   *  \remarks  The application is responsible to make sure that the nvsg::Path stays
   *  valid during the usage of the Manipulator.
   *  \sa setTransformPath */
    const nvsg::Path * getTransformPath() const;

protected:
    nvutil::Trackball m_trackball;  // Trackball object that does all the calculations
    nvutil::SmartPtr<nvsg::Path>      m_transformPath;  //!< Complete nvsg::Path to the nvsg::Transform node.

    // new interface
    Mode m_mode;

    /* Better to have bool m_lock[3]? */
    bool m_lockAxis[3];       // requested locks by user
    bool m_activeLockAxis[3]; // current active locks
    bool m_lockMajorAxis;     // true if major axis should be locked

protected:
    /*! \brief Do the pan.
  *  \return This function returns true when a redraw is needed.
  *  \remarks Implementation of the pan functionality.*/
    virtual bool pan();

    /*! \brief Do the dolly.
  *  \return This function returns true when a redraw is needed.
  *  \remarks Implementation of the dolly functionality.*/
    virtual bool dolly();

    /*! \brief Do the rotate.
  *  \return This function returns true when a redraw is needed.
  *  \remarks Implementation of the rotate functionality. */
    virtual bool rotate();

private:
    template<typename T>
    void checkLockAxis(T dx, T dy);
};

inline TrackballTransformManipulator::Mode TrackballTransformManipulator::getMode() const
{
    return m_mode;
}
