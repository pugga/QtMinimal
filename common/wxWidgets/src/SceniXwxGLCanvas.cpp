// Copyright NVIDIA Corporation 2010-2011
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#include <wx/defs.h>
#include <nvsg/ViewState.h>
#include <nvgl/RenderTargetGLFB.h>
#include "SceniXwxGLCanvas.h"

#include <nvutil/DbgNew.h>

using namespace nvgl;
using namespace nvutil;

INIT_REFLECTION_INFO( SceniXwxGLCanvas );
DERIVE_STATIC_PROPERTIES( SceniXwxGLCanvas, HumanInterfaceDevice );

SceniXwxGLCanvas::SceniXwxGLCanvas( wxFrame *parent, wxWindowID id, int* attribList, const wxPoint& pos, const wxSize& size, long style, const wxString& name, const wxPalette& palette ) 
  : wxGLCanvas( parent, id, attribList, pos, size, style, name, palette )
  , m_initialized( false )
{

  PID_Mouse_Left = getProperty( "Mouse_Left" ); m_propMouse_Left = false;
  PID_Mouse_Middle = getProperty( "Mouse_Middle" ); m_propMouse_Middle = false;
  PID_Mouse_Right = getProperty( "Mouse_Right" ); m_propMouse_Right = false;
  PID_Mouse_Position = getProperty( "Mouse_Position" ); m_propMouse_Position = nvmath::Vec2i(0,0);
  PID_Mouse_Wheel = getProperty( "Mouse_Wheel" ); m_propMouse_Wheel = 0;

#define INIT_KEY(key) m_prop##key = false; m_keyProperties[ key ] = KeyInfo(getProperty(#key), &SceniXwxGLCanvas::m_prop##key, #key); m_keyInfos.push_back( KeyInfo( getProperty(#key), &SceniXwxGLCanvas::m_prop##key, #key ) );
#define INIT_KEY2(key, name) m_prop##name = false; m_keyProperties[ key ] = KeyInfo(getProperty(#key), &SceniXwxGLCanvas::m_prop##name, #name); m_keyInfos.push_back( KeyInfo( getProperty(#key), &SceniXwxGLCanvas::m_prop##name, #name ) );
#define INIT_KEY3(alpha, key) m_prop##key = false; m_keyProperties[ alpha ] = KeyInfo(getProperty( #key ), &SceniXwxGLCanvas::m_prop##key, #key); m_keyInfos.push_back( KeyInfo( getProperty( #key ), &SceniXwxGLCanvas::m_prop##key, #key ) );

    INIT_KEY(WXK_BACK);
    INIT_KEY2(WXK_TAB, Key_Tab);
    INIT_KEY2(WXK_RETURN, Key_Return);
    INIT_KEY2(WXK_ESCAPE, Key_Escape);
    INIT_KEY2(WXK_SPACE, Key_Space);
    INIT_KEY2(WXK_DELETE, Key_Delete);
    INIT_KEY(WXK_START);
    INIT_KEY(WXK_LBUTTON);
    INIT_KEY(WXK_RBUTTON);
    INIT_KEY(WXK_CANCEL);
    INIT_KEY(WXK_MBUTTON);
    INIT_KEY(WXK_CLEAR);
    INIT_KEY2(WXK_SHIFT, Key_Shift);
    INIT_KEY2(WXK_CONTROL, Key_Control);
    INIT_KEY(WXK_MENU);
    INIT_KEY(WXK_PAUSE);
    INIT_KEY(WXK_CAPITAL);
    INIT_KEY(WXK_PRIOR);
    INIT_KEY(WXK_NEXT);
    INIT_KEY(WXK_END);
    INIT_KEY(WXK_HOME);
    INIT_KEY(WXK_LEFT);
    INIT_KEY(WXK_UP);
    INIT_KEY(WXK_RIGHT);
    INIT_KEY(WXK_DOWN);
    INIT_KEY(WXK_SELECT);
    INIT_KEY(WXK_PRINT);
    INIT_KEY(WXK_EXECUTE);
    INIT_KEY(WXK_SNAPSHOT);
    INIT_KEY(WXK_INSERT);
    INIT_KEY(WXK_HELP);
    INIT_KEY(WXK_NUMPAD0);
    INIT_KEY(WXK_NUMPAD1);
    INIT_KEY(WXK_NUMPAD2);
    INIT_KEY(WXK_NUMPAD3);
    INIT_KEY(WXK_NUMPAD4);
    INIT_KEY(WXK_NUMPAD5);
    INIT_KEY(WXK_NUMPAD6);
    INIT_KEY(WXK_NUMPAD7);
    INIT_KEY(WXK_NUMPAD8);
    INIT_KEY(WXK_NUMPAD9);
    INIT_KEY(WXK_MULTIPLY);
    INIT_KEY(WXK_ADD);
    INIT_KEY(WXK_SEPARATOR);
    INIT_KEY(WXK_SUBTRACT);
    INIT_KEY(WXK_DECIMAL);
    INIT_KEY(WXK_DIVIDE);
    INIT_KEY(WXK_F1);
    INIT_KEY(WXK_F2);
    INIT_KEY(WXK_F3);
    INIT_KEY(WXK_F4);
    INIT_KEY(WXK_F5);
    INIT_KEY(WXK_F6);
    INIT_KEY(WXK_F7);
    INIT_KEY(WXK_F8);
    INIT_KEY(WXK_F9);
    INIT_KEY(WXK_F10);
    INIT_KEY(WXK_F11);
    INIT_KEY(WXK_F12);
    INIT_KEY(WXK_F13);
    INIT_KEY(WXK_F14);
    INIT_KEY(WXK_F15);
    INIT_KEY(WXK_F16);
    INIT_KEY(WXK_F17);
    INIT_KEY(WXK_F18);
    INIT_KEY(WXK_F19);
    INIT_KEY(WXK_F20);
    INIT_KEY(WXK_F21);
    INIT_KEY(WXK_F22);
    INIT_KEY(WXK_F23);
    INIT_KEY(WXK_F24);
    INIT_KEY(WXK_NUMLOCK);
    INIT_KEY(WXK_SCROLL);
    INIT_KEY(WXK_PAGEUP);
    INIT_KEY(WXK_PAGEDOWN);
    INIT_KEY(WXK_NUMPAD_SPACE);
    INIT_KEY(WXK_NUMPAD_TAB);
    INIT_KEY(WXK_NUMPAD_ENTER);
    INIT_KEY(WXK_NUMPAD_F1);
    INIT_KEY(WXK_NUMPAD_F2);
    INIT_KEY(WXK_NUMPAD_F3);
    INIT_KEY(WXK_NUMPAD_F4);
    INIT_KEY(WXK_NUMPAD_HOME);
    INIT_KEY(WXK_NUMPAD_LEFT);
    INIT_KEY(WXK_NUMPAD_UP);
    INIT_KEY(WXK_NUMPAD_RIGHT);
    INIT_KEY(WXK_NUMPAD_DOWN);
    INIT_KEY(WXK_NUMPAD_PRIOR);
    INIT_KEY(WXK_NUMPAD_PAGEUP);
    INIT_KEY(WXK_NUMPAD_NEXT);
    INIT_KEY(WXK_NUMPAD_PAGEDOWN);
    INIT_KEY(WXK_NUMPAD_END);
    INIT_KEY(WXK_NUMPAD_BEGIN);
    INIT_KEY(WXK_NUMPAD_INSERT);
    INIT_KEY(WXK_NUMPAD_DELETE);
    INIT_KEY(WXK_NUMPAD_EQUAL);
    INIT_KEY(WXK_NUMPAD_MULTIPLY);
    INIT_KEY(WXK_NUMPAD_ADD);
    INIT_KEY(WXK_NUMPAD_SEPARATOR);
    INIT_KEY(WXK_NUMPAD_SUBTRACT);
    INIT_KEY(WXK_NUMPAD_DECIMAL);
    INIT_KEY(WXK_NUMPAD_DIVIDE);
    INIT_KEY(WXK_WINDOWS_LEFT);
    INIT_KEY(WXK_WINDOWS_RIGHT);
    INIT_KEY(WXK_WINDOWS_MENU);
    INIT_KEY(WXK_COMMAND);
    INIT_KEY(WXK_SPECIAL1);
    INIT_KEY(WXK_SPECIAL2);
    INIT_KEY(WXK_SPECIAL3);
    INIT_KEY(WXK_SPECIAL4);
    INIT_KEY(WXK_SPECIAL5);
    INIT_KEY(WXK_SPECIAL6);
    INIT_KEY(WXK_SPECIAL7);
    INIT_KEY(WXK_SPECIAL8);
    INIT_KEY(WXK_SPECIAL9);
    INIT_KEY(WXK_SPECIAL10);
    INIT_KEY(WXK_SPECIAL11);
    INIT_KEY(WXK_SPECIAL12);
    INIT_KEY(WXK_SPECIAL13);
    INIT_KEY(WXK_SPECIAL14);
    INIT_KEY(WXK_SPECIAL15);
    INIT_KEY(WXK_SPECIAL16);
    INIT_KEY(WXK_SPECIAL17);
    INIT_KEY(WXK_SPECIAL18);
    INIT_KEY(WXK_SPECIAL19);
    INIT_KEY(WXK_SPECIAL20);

    INIT_KEY3( '0', Key_0 );
    INIT_KEY3( '1', Key_1 );
    INIT_KEY3( '2', Key_2 );
    INIT_KEY3( '3', Key_3 );
    INIT_KEY3( '4', Key_4 );
    INIT_KEY3( '5', Key_5 );
    INIT_KEY3( '6', Key_6 );
    INIT_KEY3( '7', Key_7 );
    INIT_KEY3( '8', Key_8 );
    INIT_KEY3( '9', Key_9 );
    INIT_KEY3( 'A', Key_A );
    INIT_KEY3( 'B', Key_B );
    INIT_KEY3( 'C', Key_C );
    INIT_KEY3( 'D', Key_D );
    INIT_KEY3( 'E', Key_E );
    INIT_KEY3( 'F', Key_F );
    INIT_KEY3( 'G', Key_G );
    INIT_KEY3( 'H', Key_H );
    INIT_KEY3( 'I', Key_I );
    INIT_KEY3( 'J', Key_J );
    INIT_KEY3( 'K', Key_K );
    INIT_KEY3( 'L', Key_L );
    INIT_KEY3( 'M', Key_M );
    INIT_KEY3( 'N', Key_N );
    INIT_KEY3( 'O', Key_O );
    INIT_KEY3( 'P', Key_P );
    INIT_KEY3( 'Q', Key_Q );
    INIT_KEY3( 'R', Key_R );
    INIT_KEY3( 'S', Key_S );
    INIT_KEY3( 'T', Key_T );
    INIT_KEY3( 'U', Key_U );
    INIT_KEY3( 'V', Key_V );
    INIT_KEY3( 'W', Key_W );
    INIT_KEY3( 'X', Key_X );
    INIT_KEY3( 'Y', Key_Y );
    INIT_KEY3( 'Z', Key_Z );
}

void SceniXwxGLCanvas::paint(wxPaintEvent& event)
{
  wxPaintDC(this);

  if ( !m_initialized )
  {
    m_initialized = true;

    // attach RenderContextGL
    m_glContext = new wxGLContext(this);
    SetCurrent();
    m_renderContextGL = smart_cast<RenderContextGL>(RenderContextGL::create( RenderContextGL::Attach() ));
    m_renderTargetGL = smart_cast<RenderTargetGL>( RenderTargetGLFB::create( m_renderContextGL ) );
    m_renderTargetGL->setSize( GetSize().GetWidth(), GetSize().GetHeight() );

    initializeGL();
  }

  m_renderContextGL->makeCurrent();

  paintGL();

  //m_renderContextGL->swap();
  SwapBuffers();
}

void SceniXwxGLCanvas::resize( wxSizeEvent & event )
{
  if ( m_initialized )
  {
    m_renderContextGL->makeCurrent();
    int width = event.GetSize().GetWidth();
    int height = event.GetSize().GetHeight();
    m_renderTargetGL->setSize( width, height );
    resizeGL( width, height );
    Refresh();
  }
}


void SceniXwxGLCanvas::initializeGL()
{
}

void SceniXwxGLCanvas::paintGL()
{
}

void SceniXwxGLCanvas::resizeGL( int width, int height)
{
}

nvgl::SmartRenderContextGL SceniXwxGLCanvas::getRenderContext() const
{
  return m_renderContextGL;
}

nvgl::SmartRenderTargetGL  SceniXwxGLCanvas::getRenderTarget() const
{
  return m_renderTargetGL;
}

// HID
void SceniXwxGLCanvas::keyPressEvent( wxKeyEvent &event )
{
  KeyInfoMap::iterator it = m_keyProperties.find( event.GetKeyCode() );
  if ( it != m_keyProperties.end() )
  {
    const KeyInfo &info = it->second;
    if ( this->*info.member == false )
    {
      this->*info.member = true;
      hidNotify( info.propertyId );
    }
  }
  event.Skip();
}

void SceniXwxGLCanvas::keyReleaseEvent( wxKeyEvent &event )
{
  KeyInfoMap::iterator it = m_keyProperties.find( event.GetKeyCode() );
  if ( it != m_keyProperties.end() )
  {
    const KeyInfo &info = it->second;
    if ( this->*info.member == true )
    {
      this->*info.member = false;
      hidNotify( info.propertyId );
    }
  }
  event.Skip();
}

void SceniXwxGLCanvas::mouseEvent( wxMouseEvent &event )
{
  const wxEventType eventType = event.GetEventType();
  if ( eventType == wxEVT_LEFT_DOWN && m_propMouse_Left == false )
  {
    m_propMouse_Left = true;
    hidNotify( PID_Mouse_Left );
  }
  if ( eventType == wxEVT_LEFT_UP && m_propMouse_Left == true )
  {
    m_propMouse_Left = false;
    hidNotify( PID_Mouse_Left );
  }

  if ( eventType == wxEVT_RIGHT_DOWN && m_propMouse_Right == false )
  {
    m_propMouse_Right = true;
    hidNotify( PID_Mouse_Right );
  }
  if ( eventType == wxEVT_RIGHT_UP && m_propMouse_Right == true )
  {
    m_propMouse_Right = false;
    hidNotify( PID_Mouse_Right );
  }

  if ( eventType == wxEVT_MIDDLE_DOWN && m_propMouse_Middle == false )
  {
    m_propMouse_Middle = true;
    hidNotify( PID_Mouse_Middle );
  }
  if ( eventType == wxEVT_MIDDLE_UP && m_propMouse_Middle == true )
  {
    m_propMouse_Middle = false;
    hidNotify( PID_Mouse_Middle );
  }

  if ( eventType == wxEVT_MOTION )
  {
    m_propMouse_Position = nvmath::Vec2i( event.GetX(), event.GetY() );
    hidNotify( PID_Mouse_Position );
  }

  if ( eventType == wxEVT_MOUSEWHEEL )
  {
    // MMM - I fear this may be mouse and/or OS dependent
    m_propMouse_Wheel += event.GetWheelRotation();
    hidNotify( PID_Mouse_Wheel );
  }
}

unsigned int SceniXwxGLCanvas::getNumberOfAxes() const
{
  return 0;
}

std::string SceniXwxGLCanvas::getAxisName( unsigned int axis ) const
{
  return "";
}

unsigned int SceniXwxGLCanvas::getNumberOfKeys() const
{
  return checked_cast<unsigned int>(m_keyInfos.size());
}

std::string SceniXwxGLCanvas::getKeyName( unsigned int key ) const
{
  NVSG_ASSERT( key < m_keyInfos.size() );

  return m_keyInfos[key].name;
}

void SceniXwxGLCanvas::hidNotify( PropertyId property )
{
  notify( property );
}

BEGIN_EVENT_TABLE(SceniXwxGLCanvas, wxGLCanvas)
    EVT_PAINT    (SceniXwxGLCanvas::paint)
    EVT_SIZE     (SceniXwxGLCanvas::resize)
    EVT_KEY_DOWN (SceniXwxGLCanvas::keyPressEvent)
    EVT_KEY_UP   (SceniXwxGLCanvas::keyReleaseEvent)
    EVT_MOUSE_EVENTS(SceniXwxGLCanvas::mouseEvent)
    EVT_ERASE_BACKGROUND(SceniXwxGLCanvas::OnEraseBackground)
END_EVENT_TABLE()
