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

 
#include "ui/Manipulator.h"

// this must be the last include
#include <nvutil/DbgNew.h> 

using namespace nvui;

Manipulator::Manipulator()
{
}

Manipulator::~Manipulator()
{
}

void Manipulator::reset()
{
}

void Manipulator::setRenderTarget( const nvui::SmartRenderTarget &renderTarget )
{
  m_renderTarget = renderTarget;
}

nvui::SmartRenderTarget Manipulator::getRenderTarget() const
{
  return m_renderTarget;
}

void Manipulator::setViewState( const nvsg::ViewStateSharedPtr & vssp )
{
  m_viewState = vssp;
}

nvsg::ViewStateSharedPtr Manipulator::getViewState() const
{
  return m_viewState;
}
