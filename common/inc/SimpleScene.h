// Copyright NVIDIA Corporation 2009
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

#include "nvsg/nvsgapi.h" // NVSG storage-class defines
#include "nvsg/CoreTypes.h"
#include <nvutil/SmartPtr.h>

/* This class defines a simple scene with a set of drawables.
*/

class SimpleScene
{
public:
    SimpleScene();
    virtual ~SimpleScene();

    nvsg::SceneSharedPtr      m_sceneHandle;
    nvsg::MaterialSharedPtr   m_materialHandle[4];  // one material for each cube
    nvsg::TransformSharedPtr  m_transformHandle[4]; // one transform for each cube
    nvsg::PointLightSharedPtr m_pointLight;         // one light
    nvsg::DrawableSharedPtr   m_drawable;           // the drawable attached to the transforms
};
