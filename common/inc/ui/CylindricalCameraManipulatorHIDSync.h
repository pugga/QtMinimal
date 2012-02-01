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
#include "ui/CylindricalCameraManipulator.h"

/*! \brief CylindricalCameraManipulatorHIDSync "synchronizes" the HID input device to the CursorState of a CylindricalCameraManipulator
 *
 *  The CylindricalCameraManipulatorHIDSyc uses the following HID input mappings:
 *
 *  MMM - NOTE: THIS TABLE NEEDS TO BE UPDATED!!
 *
 *  <table>
 *  <tr><td> <b>Camera Operation</b></td>      <td><b>MouseButton</b></td>            <td><b>KeyState</b></td></tr>
 *  <tr><td>Orbit</td>                         <td>Mouse_Left</td>                    <td></td></tr>
 *  <tr><td>Orbit - lock major axis</td>       <td>Mouse_Left</td>                    <td>Key_Shift</td></tr>
 *  <tr><td>Continuous Orbit</td>              <td>Mouse_Left</td>                    <td>Key_C</td></tr>
 *  <tr><td>Pan</td>                           <td>Mouse_Middle</td>                  <td></td></tr>
 *  <tr><td>Pan - lock major axis</td>         <td>Mouse_Middle</td>                  <td>Key_Shift</td></tr>
 *  <tr><td>Dolly</td>                         <td>Mouse_Left + Mouse_Middle</td>     <td></td></tr>
 *  <tr><td>XY-Rotate</td>                     <td>Mouse_Left</td>                    <td>Key_Control</td></tr>
 *  <tr><td>XY-Rotate - lock major axis</td>   <td>Mouse_Left</td>                    <td>Key_Control + Key_Shift</td></tr>
 *  <tr><td>Z-Roll</td>                        <td>Mouse_Middle</td>                  <td>Key_Control</td></tr>
 *  <tr><td>Zoom (FOV)</td>                    <td>Mouse_Left + Mouse_Middle</td>     <td>Key_Control</td></tr>
 *  <tr><td>Dolly-Zoom (Vertigo)</td>          <td>Mouse_Left + Mouse_Middle</td>     <td>Key_Control + Key_Shift</td></tr>
 *  <tr><td>Set rotation point at cursor</td>  <td></td>                              <td>Key_H</td></tr>
 *  </table>
 *  \sa Manipulator, CylindricalCameraManipulatorHID */
class CylindricalCameraManipulatorHIDSync : public CylindricalCameraManipulator
{
public:
    CylindricalCameraManipulatorHIDSync( );
    virtual ~CylindricalCameraManipulatorHIDSync();

    void setHID( HumanInterfaceDevice *hid );

    virtual bool updateFrame( float dt );

private:
    /* Property ids for HID input */
    nvutil::PropertyId PID_Pos;
    nvutil::PropertyId PID_Orbit;
    nvutil::PropertyId PID_Pan;
    nvutil::PropertyId PID_Shift;
    nvutil::PropertyId PID_Control;
    nvutil::PropertyId PID_HotSpot;
    nvutil::PropertyId PID_Wheel;

    HumanInterfaceDevice *m_hid;
};
