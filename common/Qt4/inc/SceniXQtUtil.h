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

#include <QImage>

#include <nvutil/SmartPtr.h>
#include <nvsg/CoreTypes.h>

/** \brief Creates an QImage out of a TextureHost
    \param textureImage The TextureHost which should be used as source for the QImage.
           Currently the following formats are supported
           <pre>IMG_RGB, IMG_RGBA, IMG_BGR, IMG_BGRA</pre>
    \param image Image to use from \a textureImage
    \param mipmap Mipmap to use from \a textureImage
    \return A QImage created out of the pixel data from \a textureImage
**/
QImage createQImage( const nvsg::TextureHostSharedPtr &textureImage, int image = 0, int mipmap = 0 );

