// Copyright NVIDIA Corporation 2009-2011
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#include <QResizeEvent>
#ifdef LINUX
#include <QX11Info>
#endif

#include "SceniXQGLContext.h"
#include "SceniXQGLWidget.h"

#include <nvgl/RenderTargetGLFB.h>

#include <nvutil/DbgNew.h>


using namespace nvgl;
using namespace nvui;
using namespace nvsg;
using namespace nvutil;

INIT_REFLECTION_INFO( SceniXQGLWidget )
DERIVE_STATIC_PROPERTIES( SceniXQGLWidget, HumanInterfaceDevice )

SceniXQGLWidget::SceniXQGLWidget( QWidget *parent, const RenderContextGLFormat &glFormat, SceniXQGLWidget *shareWidget )
: QWidget( parent )
{

  m_glWidget = new SceniXQGLWidgetPrivate( this, glFormat, shareWidget ? shareWidget->m_glWidget : 0 );

  // This is here to make sure that the m_context has been created under Linux. 
  // See comment in SceniXQGLContext::makeCurrent().
  m_glWidget->makeCurrent();

  setAttribute(Qt::WA_PaintOnScreen);
  setMouseTracking( true );

  PID_Mouse_Left = getProperty( "Mouse_Left" ); 
  m_propMouse_Left = false;

  PID_Mouse_Middle = getProperty( "Mouse_Middle" ); 
  m_propMouse_Middle = false;

  PID_Mouse_Right = getProperty( "Mouse_Right" ); 
  m_propMouse_Right = false;

  PID_Mouse_Position = getProperty( "Mouse_Position" );
  m_propMouse_Position = nvmath::Vec2i(0,0); 

  PID_Mouse_Wheel = getProperty("Mouse_Wheel"); 
  m_propMouse_Wheel = 0;

#define INIT_KEY(key) m_prop##key = false; m_keyProperties[ Qt::key ] = KeyInfo(getProperty(#key), &SceniXQGLWidget::m_prop##key, #key); m_keyInfos.push_back( KeyInfo( getProperty(#key), &SceniXQGLWidget::m_prop##key, #key ) );

  INIT_KEY(Key_Escape);
  INIT_KEY(Key_Tab);
  INIT_KEY(Key_Backtab);
  INIT_KEY(Key_Backspace);
  INIT_KEY(Key_Return);
  INIT_KEY(Key_Enter);
  INIT_KEY(Key_Insert);
  INIT_KEY(Key_Delete);
  INIT_KEY(Key_Pause);
  INIT_KEY(Key_Print);
  INIT_KEY(Key_SysReq);
  INIT_KEY(Key_Clear);
  INIT_KEY(Key_Home);
  INIT_KEY(Key_End);
  INIT_KEY(Key_Left);
  INIT_KEY(Key_Up);
  INIT_KEY(Key_Right);
  INIT_KEY(Key_Down);
  INIT_KEY(Key_PageUp);
  INIT_KEY(Key_PageDown);
  INIT_KEY(Key_Shift);
  INIT_KEY(Key_Control);
  INIT_KEY(Key_Meta);
  INIT_KEY(Key_Alt);
  INIT_KEY(Key_AltGr);
  INIT_KEY(Key_CapsLock);
  INIT_KEY(Key_NumLock);
  INIT_KEY(Key_ScrollLock);
  INIT_KEY(Key_F1);
  INIT_KEY(Key_F2);
  INIT_KEY(Key_F3);
  INIT_KEY(Key_F4);
  INIT_KEY(Key_F5);
  INIT_KEY(Key_F6);
  INIT_KEY(Key_F7);
  INIT_KEY(Key_F8);
  INIT_KEY(Key_F9);
  INIT_KEY(Key_F10);
  INIT_KEY(Key_F11);
  INIT_KEY(Key_F12);
  INIT_KEY(Key_F13);
  INIT_KEY(Key_F14);
  INIT_KEY(Key_F15);
  INIT_KEY(Key_F16);
  INIT_KEY(Key_F17);
  INIT_KEY(Key_F18);
  INIT_KEY(Key_F19);
  INIT_KEY(Key_F20);
  INIT_KEY(Key_F21);
  INIT_KEY(Key_F22);
  INIT_KEY(Key_F23);
  INIT_KEY(Key_F24);
  INIT_KEY(Key_F25);
  INIT_KEY(Key_F26);
  INIT_KEY(Key_F27);
  INIT_KEY(Key_F28);
  INIT_KEY(Key_F29);
  INIT_KEY(Key_F30);
  INIT_KEY(Key_F31);
  INIT_KEY(Key_F32);
  INIT_KEY(Key_F33);
  INIT_KEY(Key_F34);
  INIT_KEY(Key_F35);
  INIT_KEY(Key_Super_L);
  INIT_KEY(Key_Super_R);
  INIT_KEY(Key_Menu);
  INIT_KEY(Key_Hyper_L);
  INIT_KEY(Key_Hyper_R);
  INIT_KEY(Key_Help);
  INIT_KEY(Key_Direction_L);
  INIT_KEY(Key_Direction_R);
  INIT_KEY(Key_Space);
  INIT_KEY(Key_Any);
  INIT_KEY(Key_Exclam);
  INIT_KEY(Key_QuoteDbl);
  INIT_KEY(Key_NumberSign);
  INIT_KEY(Key_Dollar);
  INIT_KEY(Key_Percent);
  INIT_KEY(Key_Ampersand);
  INIT_KEY(Key_Apostrophe);
  INIT_KEY(Key_ParenLeft);
  INIT_KEY(Key_ParenRight);
  INIT_KEY(Key_Asterisk);
  INIT_KEY(Key_Plus);
  INIT_KEY(Key_Comma);
  INIT_KEY(Key_Minus);
  INIT_KEY(Key_Period);
  INIT_KEY(Key_Slash);
  INIT_KEY(Key_0);
  INIT_KEY(Key_1);
  INIT_KEY(Key_2);
  INIT_KEY(Key_3);
  INIT_KEY(Key_4);
  INIT_KEY(Key_5);
  INIT_KEY(Key_6);
  INIT_KEY(Key_7);
  INIT_KEY(Key_8);
  INIT_KEY(Key_9);
  INIT_KEY(Key_Colon);
  INIT_KEY(Key_Semicolon);
  INIT_KEY(Key_Less);
  INIT_KEY(Key_Equal);
  INIT_KEY(Key_Greater);
  INIT_KEY(Key_Question);
  INIT_KEY(Key_At);
  INIT_KEY(Key_A);
  INIT_KEY(Key_B);
  INIT_KEY(Key_C);
  INIT_KEY(Key_D);
  INIT_KEY(Key_E);
  INIT_KEY(Key_F);
  INIT_KEY(Key_G);
  INIT_KEY(Key_H);
  INIT_KEY(Key_I);
  INIT_KEY(Key_J);
  INIT_KEY(Key_K);
  INIT_KEY(Key_L);
  INIT_KEY(Key_M);
  INIT_KEY(Key_N);
  INIT_KEY(Key_O);
  INIT_KEY(Key_P);
  INIT_KEY(Key_Q);
  INIT_KEY(Key_R);
  INIT_KEY(Key_S);
  INIT_KEY(Key_T);
  INIT_KEY(Key_U);
  INIT_KEY(Key_V);
  INIT_KEY(Key_W);
  INIT_KEY(Key_X);
  INIT_KEY(Key_Y);
  INIT_KEY(Key_Z);
  INIT_KEY(Key_BracketLeft);
  INIT_KEY(Key_Backslash);
  INIT_KEY(Key_BracketRight);
  INIT_KEY(Key_AsciiCircum);
  INIT_KEY(Key_Underscore);
  INIT_KEY(Key_QuoteLeft);
  INIT_KEY(Key_BraceLeft);
  INIT_KEY(Key_Bar);
  INIT_KEY(Key_BraceRight);
  INIT_KEY(Key_AsciiTilde);
  INIT_KEY(Key_nobreakspace);
  INIT_KEY(Key_exclamdown);
  INIT_KEY(Key_cent);
  INIT_KEY(Key_sterling);
  INIT_KEY(Key_currency);
  INIT_KEY(Key_yen);
  INIT_KEY(Key_brokenbar);
  INIT_KEY(Key_section);
  INIT_KEY(Key_diaeresis);
  INIT_KEY(Key_copyright);
  INIT_KEY(Key_ordfeminine);
  INIT_KEY(Key_guillemotleft);
  INIT_KEY(Key_notsign);
  INIT_KEY(Key_hyphen);
  INIT_KEY(Key_registered);
  INIT_KEY(Key_macron);
  INIT_KEY(Key_degree);
  INIT_KEY(Key_plusminus);
  INIT_KEY(Key_twosuperior);
  INIT_KEY(Key_threesuperior);
  INIT_KEY(Key_acute);
  INIT_KEY(Key_mu);
  INIT_KEY(Key_paragraph);
  INIT_KEY(Key_periodcentered);
  INIT_KEY(Key_cedilla);
  INIT_KEY(Key_onesuperior);
  INIT_KEY(Key_masculine);
  INIT_KEY(Key_guillemotright);
  INIT_KEY(Key_onequarter);
  INIT_KEY(Key_onehalf);
  INIT_KEY(Key_threequarters);
  INIT_KEY(Key_questiondown);
  INIT_KEY(Key_Agrave);
  INIT_KEY(Key_Aacute);
  INIT_KEY(Key_Acircumflex);
  INIT_KEY(Key_Atilde);
  INIT_KEY(Key_Adiaeresis);
  INIT_KEY(Key_Aring);
  INIT_KEY(Key_AE);
  INIT_KEY(Key_Ccedilla);
  INIT_KEY(Key_Egrave);
  INIT_KEY(Key_Eacute);
  INIT_KEY(Key_Ecircumflex);
  INIT_KEY(Key_Ediaeresis);
  INIT_KEY(Key_Igrave);
  INIT_KEY(Key_Iacute);
  INIT_KEY(Key_Icircumflex);
  INIT_KEY(Key_Idiaeresis);
  INIT_KEY(Key_ETH);
  INIT_KEY(Key_Ntilde);
  INIT_KEY(Key_Ograve);
  INIT_KEY(Key_Oacute);
  INIT_KEY(Key_Ocircumflex);
  INIT_KEY(Key_Otilde);
  INIT_KEY(Key_Odiaeresis);
  INIT_KEY(Key_multiply);
  INIT_KEY(Key_Ooblique);
  INIT_KEY(Key_Ugrave);
  INIT_KEY(Key_Uacute);
  INIT_KEY(Key_Ucircumflex);
  INIT_KEY(Key_Udiaeresis);
  INIT_KEY(Key_Yacute);
  INIT_KEY(Key_THORN);
  INIT_KEY(Key_ssharp);
  INIT_KEY(Key_division);
  INIT_KEY(Key_ydiaeresis);
  INIT_KEY(Key_Multi_key);
  INIT_KEY(Key_Codeinput);
  INIT_KEY(Key_SingleCandidate);
  INIT_KEY(Key_MultipleCandidate);
  INIT_KEY(Key_PreviousCandidate);
  INIT_KEY(Key_Mode_switch);
  INIT_KEY(Key_Kanji);
  INIT_KEY(Key_Muhenkan);
  INIT_KEY(Key_Henkan);
  INIT_KEY(Key_Romaji);
  INIT_KEY(Key_Hiragana);
  INIT_KEY(Key_Katakana);
  INIT_KEY(Key_Hiragana_Katakana);
  INIT_KEY(Key_Zenkaku);
  INIT_KEY(Key_Hankaku);
  INIT_KEY(Key_Zenkaku_Hankaku);
  INIT_KEY(Key_Touroku);
  INIT_KEY(Key_Massyo);
  INIT_KEY(Key_Kana_Lock);
  INIT_KEY(Key_Kana_Shift);
  INIT_KEY(Key_Eisu_Shift);
  INIT_KEY(Key_Eisu_toggle);
  INIT_KEY(Key_Hangul);
  INIT_KEY(Key_Hangul_Start);
  INIT_KEY(Key_Hangul_End);
  INIT_KEY(Key_Hangul_Hanja);
  INIT_KEY(Key_Hangul_Jamo);
  INIT_KEY(Key_Hangul_Romaja);
  INIT_KEY(Key_Hangul_Jeonja);
  INIT_KEY(Key_Hangul_Banja);
  INIT_KEY(Key_Hangul_PreHanja);
  INIT_KEY(Key_Hangul_PostHanja);
  INIT_KEY(Key_Hangul_Special);
  INIT_KEY(Key_Dead_Grave);
  INIT_KEY(Key_Dead_Acute);
  INIT_KEY(Key_Dead_Circumflex);
  INIT_KEY(Key_Dead_Tilde);
  INIT_KEY(Key_Dead_Macron);
  INIT_KEY(Key_Dead_Breve);
  INIT_KEY(Key_Dead_Abovedot);
  INIT_KEY(Key_Dead_Diaeresis);
  INIT_KEY(Key_Dead_Abovering);
  INIT_KEY(Key_Dead_Doubleacute);
  INIT_KEY(Key_Dead_Caron);
  INIT_KEY(Key_Dead_Cedilla);
  INIT_KEY(Key_Dead_Ogonek);
  INIT_KEY(Key_Dead_Iota);
  INIT_KEY(Key_Dead_Voiced_Sound);
  INIT_KEY(Key_Dead_Semivoiced_Sound);
  INIT_KEY(Key_Dead_Belowdot);
  INIT_KEY(Key_Dead_Hook);
  INIT_KEY(Key_Dead_Horn);
  INIT_KEY(Key_Back);
  INIT_KEY(Key_Forward);
  INIT_KEY(Key_Stop);
  INIT_KEY(Key_Refresh);
  INIT_KEY(Key_VolumeDown);
  INIT_KEY(Key_VolumeMute);
  INIT_KEY(Key_VolumeUp);
  INIT_KEY(Key_BassBoost);
  INIT_KEY(Key_BassUp);
  INIT_KEY(Key_BassDown);
  INIT_KEY(Key_TrebleUp);
  INIT_KEY(Key_TrebleDown);
  INIT_KEY(Key_MediaPlay);
  INIT_KEY(Key_MediaStop);
  INIT_KEY(Key_MediaPrevious);
  INIT_KEY(Key_MediaNext);
  INIT_KEY(Key_MediaRecord);
  INIT_KEY(Key_HomePage);
  INIT_KEY(Key_Favorites);
  INIT_KEY(Key_Search);
  INIT_KEY(Key_Standby);
  INIT_KEY(Key_OpenUrl);
  INIT_KEY(Key_LaunchMail);
  INIT_KEY(Key_LaunchMedia);
  INIT_KEY(Key_Launch0);
  INIT_KEY(Key_Launch1);
  INIT_KEY(Key_Launch2);
  INIT_KEY(Key_Launch3);
  INIT_KEY(Key_Launch4);
  INIT_KEY(Key_Launch5);
  INIT_KEY(Key_Launch6);
  INIT_KEY(Key_Launch7);
  INIT_KEY(Key_Launch8);
  INIT_KEY(Key_Launch9);
  INIT_KEY(Key_LaunchA);
  INIT_KEY(Key_LaunchB);
  INIT_KEY(Key_LaunchC);
  INIT_KEY(Key_LaunchD);
  INIT_KEY(Key_LaunchE);
  INIT_KEY(Key_LaunchF);
  INIT_KEY(Key_MonBrightnessUp);
  INIT_KEY(Key_MonBrightnessDown);
  INIT_KEY(Key_KeyboardLightOnOff);
  INIT_KEY(Key_KeyboardBrightnessUp);
  INIT_KEY(Key_KeyboardBrightnessDown);
  INIT_KEY(Key_PowerOff);
  INIT_KEY(Key_WakeUp);
  INIT_KEY(Key_Eject);
  INIT_KEY(Key_ScreenSaver);
  INIT_KEY(Key_WWW);
  INIT_KEY(Key_Memo);
  INIT_KEY(Key_LightBulb);
  INIT_KEY(Key_Shop);
  INIT_KEY(Key_History);
  INIT_KEY(Key_AddFavorite);
  INIT_KEY(Key_HotLinks);
  INIT_KEY(Key_BrightnessAdjust);
  INIT_KEY(Key_Finance);
  INIT_KEY(Key_Community);
  INIT_KEY(Key_AudioRewind);
  INIT_KEY(Key_BackForward);
  INIT_KEY(Key_ApplicationLeft);
  INIT_KEY(Key_ApplicationRight);
  INIT_KEY(Key_Book);
  INIT_KEY(Key_CD);
  INIT_KEY(Key_Calculator);
  INIT_KEY(Key_ToDoList);
  INIT_KEY(Key_ClearGrab);
  INIT_KEY(Key_Close);
  INIT_KEY(Key_Copy);
  INIT_KEY(Key_Cut);
  INIT_KEY(Key_Display);
  INIT_KEY(Key_DOS);
  INIT_KEY(Key_Documents);
  INIT_KEY(Key_Excel);
  INIT_KEY(Key_Explorer);
  INIT_KEY(Key_Game);
  INIT_KEY(Key_Go);
  INIT_KEY(Key_iTouch);
  INIT_KEY(Key_LogOff);
  INIT_KEY(Key_Market);
  INIT_KEY(Key_Meeting);
  INIT_KEY(Key_MenuKB);
  INIT_KEY(Key_MenuPB);
  INIT_KEY(Key_MySites);
  INIT_KEY(Key_News);
  INIT_KEY(Key_OfficeHome);
  INIT_KEY(Key_Option);
  INIT_KEY(Key_Paste);
  INIT_KEY(Key_Phone);
  INIT_KEY(Key_Calendar);
  INIT_KEY(Key_Reply);
  INIT_KEY(Key_Reload);
  INIT_KEY(Key_RotateWindows);
  INIT_KEY(Key_RotationPB);
  INIT_KEY(Key_RotationKB);
  INIT_KEY(Key_Save);
  INIT_KEY(Key_Send);
  INIT_KEY(Key_Spell);
  INIT_KEY(Key_SplitScreen);
  INIT_KEY(Key_Support);
  INIT_KEY(Key_TaskPane);
  INIT_KEY(Key_Terminal);
  INIT_KEY(Key_Tools);
  INIT_KEY(Key_Travel);
  INIT_KEY(Key_Video);
  INIT_KEY(Key_Word);
  INIT_KEY(Key_Xfer);
  INIT_KEY(Key_ZoomIn);
  INIT_KEY(Key_ZoomOut);
  INIT_KEY(Key_Away);
  INIT_KEY(Key_Messenger);
  INIT_KEY(Key_WebCam);
  INIT_KEY(Key_MailForward);
  INIT_KEY(Key_Pictures);
  INIT_KEY(Key_Music);
  INIT_KEY(Key_Battery);
  INIT_KEY(Key_Bluetooth);
  INIT_KEY(Key_WLAN);
  INIT_KEY(Key_UWB);
  INIT_KEY(Key_AudioForward);
  INIT_KEY(Key_AudioRepeat);
  INIT_KEY(Key_AudioRandomPlay);
  INIT_KEY(Key_Subtitle);
  INIT_KEY(Key_AudioCycleTrack);
  INIT_KEY(Key_Time);
  INIT_KEY(Key_Hibernate);
  INIT_KEY(Key_View);
  INIT_KEY(Key_TopMenu);
  INIT_KEY(Key_PowerDown);
  INIT_KEY(Key_Suspend);
  INIT_KEY(Key_ContrastAdjust);
  INIT_KEY(Key_MediaLast);
  INIT_KEY(Key_unknown);
  INIT_KEY(Key_Call);
  INIT_KEY(Key_Context1);
  INIT_KEY(Key_Context2);
  INIT_KEY(Key_Context3);
  INIT_KEY(Key_Context4);
  INIT_KEY(Key_Flip);
  INIT_KEY(Key_Hangup);
  INIT_KEY(Key_No);
  INIT_KEY(Key_Select);
  INIT_KEY(Key_Yes);
  INIT_KEY(Key_Execute);
  INIT_KEY(Key_Printer);
  INIT_KEY(Key_Play);
  INIT_KEY(Key_Sleep);
  INIT_KEY(Key_Zoom);
  INIT_KEY(Key_Cancel);
#undef INIT_KEY
}

SceniXQGLWidget::~SceniXQGLWidget()
{
}

void SceniXQGLWidget::initializeGL()
{
}

void SceniXQGLWidget::resizeGL(int width, int height)
{
}

void SceniXQGLWidget::paintGL()
{
}

void SceniXQGLWidget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent( event );
  m_glWidget->resize( event->size() );
}

const SmartRenderContextGL & SceniXQGLWidget::getRenderContext() const
{
  return m_glWidget->getRenderContext();
}

const SmartRenderTargetGL & SceniXQGLWidget::getRenderTarget() const
{
  return m_glWidget->getRenderTarget();
}

/**************************/
/* SceniXQGLWidgetPrivate */
/**************************/
SceniXQGLWidget::SceniXQGLWidgetPrivate::SceniXQGLWidgetPrivate( SceniXQGLWidget *parent, const nvgl::RenderContextGLFormat &glFormat, QGLWidget *shareWidget )
  : QGLWidget( new SceniXQGLContext( glFormat ), parent, shareWidget )
  , m_renderTargetGL( 0 )
{
}

SceniXQGLWidget::SceniXQGLWidgetPrivate::~SceniXQGLWidgetPrivate()
{
  m_renderTargetGL.reset();
}

const SmartRenderContextGL & SceniXQGLWidget::SceniXQGLWidgetPrivate::getRenderContext() const
{
  const SceniXQGLContext *ctx = dynamic_cast<const SceniXQGLContext*>(context());
  NVSG_ASSERT(ctx);
  return ctx->getRenderContext();
}

const SmartRenderTargetGL & SceniXQGLWidget::SceniXQGLWidgetPrivate::getRenderTarget() const
{
  return m_renderTargetGL;
}

void SceniXQGLWidget::SceniXQGLWidgetPrivate::initializeGL()
{
  m_renderTargetGL = nvutil::smart_cast<RenderTargetGL>(RenderTargetGLFB::create( getRenderContext() ));

  SceniXQGLWidget *scenixParent = dynamic_cast<SceniXQGLWidget*>(parentWidget());
  if (scenixParent)
  {
    scenixParent->initializeGL();
  }
}

void SceniXQGLWidget::SceniXQGLWidgetPrivate::resizeGL( int width, int height )
{
  NVSG_ASSERT( m_renderTargetGL );
  m_renderTargetGL->setSize( width, height );

  SceniXQGLWidget *scenixParent = dynamic_cast<SceniXQGLWidget*>(parentWidget());
  if (scenixParent)
  {
    scenixParent->resizeGL( width, height );
  }
}

void SceniXQGLWidget::SceniXQGLWidgetPrivate::paintGL()
{
  SceniXQGLWidget *scenixParent = dynamic_cast<SceniXQGLWidget*>(parentWidget());
  if (scenixParent)
  {
    scenixParent->paintGL();
  }
}

bool SceniXQGLWidget::event( QEvent *event )
{
  switch ( event->type() )
  {
  case QEvent::UpdateRequest:
    m_glWidget->update();
  break;
  case QEvent::MouseTrackingChange:
    m_glWidget->setMouseTracking( hasMouseTracking() );
  break;
  }

  return QWidget::event( event );
}


QPaintEngine *SceniXQGLWidget::paintEngine() const
{
  return 0;
}

bool SceniXQGLWidget::setFormat( const nvgl::RenderContextGLFormat &format )
{
  bool valid = true;

  if ( format != getFormat() )
  {
    valid = format.isAvailable();
    if (valid) 
    {
      // create a new gl viewport with the new format
      SceniXQGLWidgetPrivate *oldWidget = m_glWidget;

      m_glWidget = new SceniXQGLWidgetPrivate( this, format, m_glWidget );
      m_glWidget->move(0, 0);
      m_glWidget->resize( width(), height() );
      m_glWidget->show();
      m_glWidget->setMouseTracking( hasMouseTracking() );

      // notify derived classes that the format has changed and a new RenderTarget is available.
      onRenderTargetChanged( oldWidget->getRenderTarget(), m_glWidget->getRenderTarget() );

      // delete the old viewport
      delete oldWidget;
    }
  }
  return valid;
}

nvgl::RenderContextGLFormat SceniXQGLWidget::getFormat() const
{
  return getRenderContext()->getFormat();
}


// HID
void SceniXQGLWidget::keyPressEvent( QKeyEvent *event )
{
  if ( !event->isAutoRepeat() )
  {
    KeyInfoMap::iterator it = m_keyProperties.find( static_cast<Qt::Key>(event->key()) );
    if ( it != m_keyProperties.end() )
    {
      const KeyInfo &info = it->second;
      if ( this->*info.member == false )
      {
        this->*info.member = true;
        hidNotify( info.propertyId );
      }
    }
  }
  event->ignore();
}

void SceniXQGLWidget::keyReleaseEvent( QKeyEvent *event )
{
  if ( !event->isAutoRepeat() )
  {
    KeyInfoMap::iterator it = m_keyProperties.find( static_cast<Qt::Key>(event->key()) );
    if ( it != m_keyProperties.end() )
    {
      const KeyInfo &info = it->second;
      if ( this->*info.member == true )
      {
        this->*info.member = false;
        hidNotify( info.propertyId );
      }
    }
  }
  event->ignore();
}

PropertyId SceniXQGLWidget::getQtMouseButtonProperty( Qt::MouseButton button ) const
{
  switch ( button )
  {
  case Qt::LeftButton:
    return PID_Mouse_Left;
  case Qt::RightButton:
    return PID_Mouse_Right;
  case Qt::MidButton:
    return PID_Mouse_Middle;
  default:
    return 0;
  }
}

void SceniXQGLWidget::mousePressEvent( QMouseEvent *event )
{
  PropertyId propButton = getQtMouseButtonProperty( event->button() );
  if ( propButton )
  {
    if ( getValue<bool>( propButton ) == false )
    {
      setValue<bool>( propButton, true );
      hidNotify( propButton );
    }
  }
  event->ignore();
}

void SceniXQGLWidget::mouseReleaseEvent( QMouseEvent *event )
{
  PropertyId propButton = getQtMouseButtonProperty( event->button() );
  if ( propButton )
  {
    if ( getValue<bool>( propButton ) == true )
    {
      setValue<bool>( propButton, false );
      hidNotify( propButton );
    }
  }
  event->ignore();
}

void SceniXQGLWidget::mouseMoveEvent( QMouseEvent *event )
{
  QPoint pos = event->pos();
  if ( m_propMouse_Position[0] != pos.x() || m_propMouse_Position[1] != pos.y() )
  {
    m_propMouse_Position[0] = pos.x();
    m_propMouse_Position[1] = pos.y();
    hidNotify( PID_Mouse_Position );
  }
  event->ignore();
}

void SceniXQGLWidget::wheelEvent( QWheelEvent *event )
{
  if ( event->orientation() == Qt::Vertical )
  {
    // accumulate here so that wheel is absolute, like position
    m_propMouse_Wheel += event->delta();
    hidNotify( PID_Mouse_Wheel );
  }
  event->ignore();
}

void SceniXQGLWidget::hidNotify( PropertyId property )
{
  notify( property );
}

unsigned int SceniXQGLWidget::getNumberOfAxes() const
{
  return 0;
}

std::string SceniXQGLWidget::getAxisName( unsigned int axis ) const
{
  return "";
}

unsigned int SceniXQGLWidget::getNumberOfKeys() const
{
  return checked_cast<unsigned int>(m_keyInfos.size());
}

std::string SceniXQGLWidget::getKeyName( unsigned int key ) const
{
  NVSG_ASSERT( key < m_keyInfos.size() );

  return m_keyInfos[key].name;
}

void SceniXQGLWidget::triggerRepaint()
{
  update();
}

void SceniXQGLWidget::onRenderTargetChanged( const nvgl::SmartRenderTargetGL &oldTarget, const nvgl::SmartRenderTargetGL &newTarget)
{
}

