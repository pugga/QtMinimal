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
#include <nvui/RenderTarget.h>
#include <nvutil/Trackball.h>
#include <nvmath/Vecnt.h>
#include <nvmath/Quatt.h>
#include <nvutil/Reflection.h>
#include "ui/Manipulator.h"

/*! \brief Simulates a trackball-like camera mouse interaction.
 *
 *  This Manipulator uses the CursorState to drive a certain camera operations like
 *  orbit, pan, dolly, rotate, roll, and zoom.
 *
 *  CursorState is updated by the derived class TrackballCameraManipulatorSync, and therefore users
 *  will probably not use this class directly.
 *
 *  \sa Manipulator, TrackballCameraManipulatorSync */
class TrackballCameraManipulator : public Manipulator, public CursorState
{
public:

    enum Mode
    {
        MODE_NONE = 0
        , MODE_ORBIT
        , MODE_PAN
        , MODE_ROTATE_XY
        , MODE_DOLLY
        , MODE_ROLL_Z
        , MODE_ZOOM_FOV
        , MODE_ZOOM_DOLLY
        , MODE_LOOKAT
    };

    enum Axis
    {
        AXIS_X = 0
        , AXIS_Y = 1
        , AXIS_Z = 2
    };

    /*! \brief Default constructor.
   *  \remarks Initialize the TrackballCameraManipulator object.*/
    TrackballCameraManipulator();

    /*! \brief Default destructor. */
    virtual ~TrackballCameraManipulator();

    virtual bool updateFrame( float dt );
    virtual void reset();

    void setMode( Mode mode );
    void lockAxis( Axis axis );
    void unlockAxis( Axis axis );

    void lockMajorAxis();
    void unlockMajorAxis();

protected:
    void onUpdate( const nvutil::Subject *subject, nvutil::PropertyId property, const nvutil::Subject::SmartPayload &payload );

    nvutil::Trackball m_trackball;       //!< Trackball object that does all the calculations

    Mode m_mode;

    // Properties
    nvmath::Vec2i m_startSpin;

    nvmath::Vec2i m_orbitCursor;
    nvmath::Quatf m_orbitCameraOrientation;
    nvmath::Vec3f m_orbitCameraPosition;

protected:
    float m_speed;

    /* Better to have bool m_lock[3]? */
    bool m_lockAxis[3];       // requested locks by user
    bool m_activeLockAxis[3]; // current active locks
    bool m_lockMajorAxis;     // true if major axis should be locked

    /*! \brief Performs the orbit operation.
   *  \return This function returns true when a redraw is needed. */
    virtual bool orbit();

    /*! \brief Performs the pan operation.
   *  \return This function returns true when a redraw is needed. */
    virtual bool pan();

    /*! \brief Performs the dolly operation.
   *  \return This function returns true when a redraw is needed. */
    virtual bool dolly();

    /*! \brief Performs the zoom operation.
   *  \return This function returns true when a redraw is needed. */
    virtual bool zoom();

    /*! \brief Performs a dolly together with a zoom to mimic a 'vertigo effect'
   *  \return This function returns true when a redraw is needed.
   *  \remarks This mimics a so called 'vertigo effect'.
   *  The effect is achieved by using the setting of a zoom lens to adjust the
   *  angle of view (often referred to as field of view) while the camera dollies
   *  (or moves) towards or away from the subject in such a way as to keep the subject
   *  the same size in the frame throughout. */
    virtual bool dollyZoom();

    /*! \brief Performs a XY-rotation.
   *  \return This function returns true when a redraw is needed. */
    virtual bool rotate();

    /*! \brief Performs a Z-rotation.
  *  \return This function returns true when a redraw is needed. */
    virtual bool roll();

    /*! \brief Performs a look-at operation.
   *  \return This function returns true when a redraw is needed. */
    virtual bool lookAt();

private:
    template<typename T>
    void checkLockAxis(T dx, T dy);
};

