// Copyright NVIDIA Corporation 2010
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

#include <nvutil/Observer.h>
#include "ui/HumanInterfaceDevice.h"
#include "ui/TrackballTransformManipulator.h"


/*! \brief TrackballTransformManipulatorHIDSync "synchronizes" the HID input device to the CursorState of a TrackballTransformManipulator.
 *
 *  This Manipulator drives the CursorState of a TrackballTransformManipulatorHID.
 *
 *  This Manipulator is a special Manipulator that interprets/converts
 *  mouse movement into nvsg::Transform changes. You can rotate the object, pan, and
 *  dolly it in and out. So you can freely move and place an object in your scene.
 *  \n
 *  The TrackballTransformManipulatorHIDSync uses the following HID input mapping: \n\n
 *
 *  <table>
 *  <tr><td> <b>Operation</b></td>             <td><b>MouseButton</b></td>            <td><b>KeyState</b></td></tr>
 *  <tr><td>XY-Rotate</td>                     <td>Mouse_Left</td>                    <td></td></tr>
 *  <tr><td>XY-Rotate - lock major axis</td>   <td>Mouse_Left</td>                    <td>Key_Shift</td></tr>
 *  <tr><td>Pan</td>                           <td>Mouse_Middle</td>                  <td></td></tr>
 *  <tr><td>Pan - lock major axis</td>         <td>Mouse_Middle</td>                  <td>Key_Shift</td></tr>
 *  <tr><td>Dolly</td>                         <td>Mouse_Left + Mouse_Middle</td>     <td></td></tr>
 *  </table>
 *
 *  \n
 *  See the TransformManipulatorViewer tutorial source code on how to incorporate a
 *  TrackballTransformManipulatorHID in your application.
 *  \note Note, that a TrackballTransformManipulatorHID needs a FrustumCamera to work with. The behaviour is
 *  undefined in all other cases.
 *  \n\n
 *  \sa Manipulator, TrackballTransformManipulatorHID */
class TrackballTransformManipulatorHIDSync : public TrackballTransformManipulator
{
public:
    TrackballTransformManipulatorHIDSync( );
    virtual ~TrackballTransformManipulatorHIDSync();

    void setHID( HumanInterfaceDevice *hid );

    virtual bool updateFrame( float dt );

private:
    /* Property ids for HID input */
    nvutil::PropertyId PID_Pos;
    nvutil::PropertyId PID_Rotate;
    nvutil::PropertyId PID_Pan;
    nvutil::PropertyId PID_Control;
    nvutil::PropertyId PID_Wheel;

    HumanInterfaceDevice *m_hid;
};
