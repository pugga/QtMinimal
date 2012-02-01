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
#include "ui/Manipulator.h"

/*! \brief Simulate flight like camera-mouse interaction.
 *  \remarks This manipulator is a special Manipulator that interprets/converts CursorState movement
 *  into flight just like camera movements. The user can simulate flying forward, flying backward,
 *  flying curves, increasing speed, and decreasing speed.\n
 *
 *  CursorState is updated by the derived class FlightCameraManipulatorSync, and therefore users
 *  will probably not use this class directly.
 *
 *  \sa Manipulator, FlightCameraManipulatorSync */
class FlightCameraManipulator : public Manipulator, public CursorState
{
public:
    /*! \brief Constructor .
     *  \param sensitivity The vector should be greater than (0,0).  This will set the flight controls sensitivity
     *  in the X and Y axes.  The sensitivity value is the ratio of window pixels to angular velocity in the flight
     *  manipulator.  A value of 1.0 means 1 window pixel results in an angular velocity of 1 degree per second on that axis.
     *  Values closer to zero will decrease the sensitivity (so a larger mouse movement has less affect on the controls).
     *  The default value works well for most applications.
     *  \remarks Initialize the FlightCameraManipulator object.  The world up direction is taken from the camera.
     *  If the world up is the default (0.f, 1.f, 0.f), you are flying in the x-z-plane and the ceiling/heaven
     *  is in the y-direction. */
    FlightCameraManipulator( const nvmath::Vec2f & sensitivity = nvmath::Vec2f( 0.2f, 0.2f ) );

    //! Default destructor
    virtual ~FlightCameraManipulator();

    /*! \brief Specifies the speed used to perform camera motions.
     * \param speed Speed to set.  The speed is in "units-per-second" where 'units' are database units, and
     * the time reference comes from the argument to updateFrame().
     * \remarks The initial speed is set to 0.0
     * \sa getSpeed */
    virtual void setSpeed( float speed );

    /*! \brief Get speed used to perform camera motions.
     *  \return Speed value that has been set.
     *  \sa setSpeed */
    virtual float getSpeed() const;

    /*! \brief Communicates the world up-vector to the Manipulator.
     * \param up Indicates the world up-vector.
     * \remarks In particular for implementing walk or fly operations it is essential
     * for the manipulator to know the world's up vector to manipulate the camera
     * orientation according.
     * \remarks The initial up vector is (0,1,0) or Y-up.
     * \sa getUpVector */
    void setUpVector( const nvmath::Vec3f & up );

    /*! \brief Retrieves the world up-vector.
     * \return The world up-vector.
     * \sa setUpVector */
    const nvmath::Vec3f & getUpVector() const;

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

protected:
    /*! \brief Do the fly.
    *  \return This function returns true when a redraw is needed.
    *  \remarks Implementation of the fly functionality.*/
    virtual bool fly();

protected:
    // Properties
    nvmath::Vec2f m_sensitivity;
    float m_speed;
    float m_deltaT;
    nvmath::Vec3f m_upVector;
};

inline void FlightCameraManipulator::setSpeed( float spd )
{
    m_speed = spd;
}

inline float FlightCameraManipulator::getSpeed() const
{
    return m_speed;
}

inline void FlightCameraManipulator::setUpVector( const nvmath::Vec3f & up )
{
    m_upVector = up;
}

inline const nvmath::Vec3f & FlightCameraManipulator::getUpVector() const
{
    return m_upVector;
}
