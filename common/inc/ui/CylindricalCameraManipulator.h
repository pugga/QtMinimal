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

#pragma once
/** \file */

#include <nvsgcommon.h>
#include "ui/Manipulator.h"
#include <nvutil/Trackball.h>

class CylindricalCameraManipulator : public Manipulator, public CursorState
{
public:
    CylindricalCameraManipulator();
    virtual ~CylindricalCameraManipulator();

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

    void setMode( Mode mode );
    Mode getMode() const;

    void setSpeed( float speed );
    float getSpeed() const;

    virtual bool updateFrame( float dt );
    virtual void reset();

    void lockAxis( Axis axis );
    void unlockAxis( Axis axis );

    void lockMajorAxis();
    void unlockMajorAxis();

protected:

    unsigned int m_flags;
    nvutil::Trackball m_trackball;       //!< Trackball object that does all the calculations
    int m_startSpinX, m_startSpinY;
    int m_currentSpinX, m_currentSpinY;

    /* Better to have bool m_lock[3]? */
    bool m_lockAxis[3];       // requested locks by user
    bool m_activeLockAxis[3]; // current active locks
    bool m_lockMajorAxis;     // true if major axis should be locked

    float m_speed;

protected:

    bool orbit();     // Custom roll free orbit.
    bool pan();
    bool dolly();     // Custom dolly keeping the focus point until reaching camera minimum focus distance.
    bool zoom();
    bool dollyZoom();
    bool rotate();    // Custom roll free rotate.
    bool roll();
    bool lookAt();

    bool setPivot();  // Custom operation 0 (also sets the focus point)
    bool setFocus();  // Custom operation 1

protected:
    Mode m_mode;

private:
    template<typename T> void checkLockAxis(T dx, T dy);
};

inline CylindricalCameraManipulator::Mode CylindricalCameraManipulator::getMode() const
{
    return m_mode;
}

inline float CylindricalCameraManipulator::getSpeed() const
{
    return m_speed;
}

