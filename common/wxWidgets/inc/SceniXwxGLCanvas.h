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

#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/glcanvas.h>

#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGL.h>
#include "ui/HumanInterfaceDevice.h"
#include "ui/Manipulator.h"

class SceniXwxGLCanvas : public wxGLCanvas, public HumanInterfaceDevice
{
public:
  SceniXwxGLCanvas( wxFrame *parent,
                    wxWindowID id = wxID_ANY,
                    int* attribList = 0, 
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0, 
                    const wxString& name = _T("SceniXwxGLCanvas"),
                    const wxPalette& palette = wxNullPalette );


  void paint(wxPaintEvent& event);
  void resize( wxSizeEvent &event);
  void keyPressEvent( wxKeyEvent &event );
  void keyReleaseEvent( wxKeyEvent &event );
  void mouseEvent( wxMouseEvent &event );

  nvgl::SmartRenderContextGL getRenderContext() const;
  nvgl::SmartRenderTargetGL  getRenderTarget() const;

  // HID
  virtual unsigned int getNumberOfAxes() const;
  virtual std::string getAxisName( unsigned int axis ) const;
  virtual unsigned int getNumberOfKeys() const;
  virtual std::string getKeyName( unsigned int key ) const;

  REFLECTION_INFO( SceniXwxGLCanvas );
  BEGIN_REFLECT_STATIC_PROPERTIES( SceniXwxGLCanvas )
    ADD_PROPERTY_RO_MEMBER( Mouse_Left, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Mouse_Middle, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Mouse_Right, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Mouse_Position, nvmath::Vec2i, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Mouse_Wheel, int, SEMANTIC_VALUE );
      
    // keyboard
    ADD_PROPERTY_RO_MEMBER( Key_Shift, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Control, bool, SEMANTIC_VALUE );

    ADD_PROPERTY_RO_MEMBER( Key_0, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_1, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_2, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_3, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_4, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_5, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_6, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_7, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_8, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_9, bool, SEMANTIC_VALUE );

    ADD_PROPERTY_RO_MEMBER( Key_A, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_B, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_C, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_D, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_E, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_G, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_H, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_I, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_J, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_K, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_L, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_M, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_N, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_O, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_P, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Q, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_R, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_S, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_T, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_U, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_V, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_W, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_X, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Y, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Z, bool, SEMANTIC_VALUE );

    #if 0 // from qt, not yet fully done
    ADD_PROPERTY_RO_MEMBER( Key_Escape, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Tab, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Backtab, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Backspace, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Return, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Enter, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Insert, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Delete, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Pause, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Print, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_SysReq, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Clear, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Home, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_End, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Left, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Up, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Right, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Down, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_PageUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_PageDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Shift, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Control, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Meta, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Alt, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AltGr, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_CapsLock, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_NumLock, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ScrollLock, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F1, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F2, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F3, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F4, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F5, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F6, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F7, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F8, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F9, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F10, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F11, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F12, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F13, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F14, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F15, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F16, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F17, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F18, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F19, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F20, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F21, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F22, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F23, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F24, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F25, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F26, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F27, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F28, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F29, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F30, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F31, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F32, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F33, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F34, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_F35, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Super_L, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Super_R, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Menu, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hyper_L, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hyper_R, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Help, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Direction_L, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Direction_R, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Space, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Any, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Exclam, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_QuoteDbl, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_NumberSign, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dollar, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Percent, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ampersand, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Apostrophe, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ParenLeft, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ParenRight, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Asterisk, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Plus, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Comma, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Minus, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Period, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Slash, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Colon, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Semicolon, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Less, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Equal, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Greater, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Question, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_At, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BracketLeft, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Backslash, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BracketRight, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AsciiCircum, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Underscore, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_QuoteLeft, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BraceLeft, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Bar, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BraceRight, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AsciiTilde, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_nobreakspace, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_exclamdown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_cent, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_sterling, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_currency, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_yen, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_brokenbar, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_section, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_diaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_copyright, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ordfeminine, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_guillemotleft, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_notsign, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_hyphen, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_registered, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_macron, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_degree, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_plusminus, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_twosuperior, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_threesuperior, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_acute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_mu, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_paragraph, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_periodcentered, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_cedilla, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_onesuperior, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_masculine, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_guillemotright, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_onequarter, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_onehalf, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_threequarters, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_questiondown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Agrave, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Aacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Acircumflex, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Atilde, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Adiaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Aring, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AE, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ccedilla, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Egrave, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Eacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ecircumflex, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ediaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Igrave, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Iacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Icircumflex, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Idiaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ETH, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ntilde, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ograve, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Oacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ocircumflex, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Otilde, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Odiaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_multiply, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ooblique, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ugrave, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Uacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Ucircumflex, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Udiaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Yacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_THORN, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ssharp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_division, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ydiaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Multi_key, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Codeinput, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_SingleCandidate, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MultipleCandidate, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_PreviousCandidate, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Mode_switch, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Kanji, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Muhenkan, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Henkan, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Romaji, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hiragana, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Katakana, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hiragana_Katakana, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Zenkaku, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hankaku, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Zenkaku_Hankaku, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Touroku, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Massyo, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Kana_Lock, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Kana_Shift, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Eisu_Shift, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Eisu_toggle, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Start, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_End, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Hanja, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Jamo, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Romaja, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Jeonja, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Banja, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_PreHanja, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_PostHanja, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangul_Special, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Grave, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Acute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Circumflex, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Tilde, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Macron, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Breve, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Abovedot, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Diaeresis, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Abovering, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Doubleacute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Caron, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Cedilla, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Ogonek, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Iota, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Voiced_Sound, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Semivoiced_Sound, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Belowdot, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Hook, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Dead_Horn, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Back, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Forward, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Stop, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Refresh, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_VolumeDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_VolumeMute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_VolumeUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BassBoost, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BassUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BassDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_TrebleUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_TrebleDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MediaPlay, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MediaStop, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MediaPrevious, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MediaNext, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MediaRecord, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_HomePage, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Favorites, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Search, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Standby, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_OpenUrl, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchMail, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchMedia, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch0, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch1, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch2, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch3, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch4, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch5, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch6, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch7, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch8, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Launch9, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchA, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchB, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchC, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchD, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchE, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LaunchF, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MonBrightnessUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MonBrightnessDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_KeyboardLightOnOff, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_KeyboardBrightnessUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_KeyboardBrightnessDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_PowerOff, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_WakeUp, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Eject, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ScreenSaver, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_WWW, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Memo, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LightBulb, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Shop, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_History, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AddFavorite, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_HotLinks, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BrightnessAdjust, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Finance, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Community, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AudioRewind, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_BackForward, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ApplicationLeft, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ApplicationRight, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Book, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_CD, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Calculator, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ToDoList, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ClearGrab, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Close, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Copy, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Cut, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Display, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_DOS, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Documents, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Excel, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Explorer, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Game, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Go, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_iTouch, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_LogOff, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Market, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Meeting, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MenuKB, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MenuPB, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MySites, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_News, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_OfficeHome, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Option, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Paste, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Phone, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Calendar, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Reply, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Reload, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_RotateWindows, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_RotationPB, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_RotationKB, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Save, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Send, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Spell, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_SplitScreen, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Support, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_TaskPane, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Terminal, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Tools, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Travel, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Video, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Word, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Xfer, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ZoomIn, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ZoomOut, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Away, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Messenger, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_WebCam, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MailForward, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Pictures, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Music, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Battery, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Bluetooth, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_WLAN, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_UWB, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AudioForward, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AudioRepeat, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AudioRandomPlay, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Subtitle, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_AudioCycleTrack, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Time, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hibernate, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_View, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_TopMenu, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_PowerDown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Suspend, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_ContrastAdjust, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_MediaLast, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_unknown, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Call, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Context1, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Context2, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Context3, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Context4, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Flip, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Hangup, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_No, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Select, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Yes, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Execute, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Printer, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Play, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Sleep, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Zoom, bool, SEMANTIC_VALUE );
    ADD_PROPERTY_RO_MEMBER( Key_Cancel, bool, SEMANTIC_VALUE );
    #endif
  END_REFLECT_STATIC_PROPERTIES()

protected:
  DECLARE_EVENT_TABLE();

  bool m_initialized;

  nvgl::SmartRenderTargetGL  m_renderTargetGL;
  nvgl::SmartRenderContextGL m_renderContextGL;

  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL( int width, int height );
private:
  // HID
  struct KeyInfo {
    KeyInfo() : propertyId(0), member(0), name(0) {}
    KeyInfo( nvutil::PropertyId id, bool SceniXwxGLCanvas::*keyMember, const char *keyName ) : propertyId(id), member( keyMember ), name(keyName) {}

    nvutil::PropertyId     propertyId;
    bool SceniXwxGLCanvas::*member;
    const char            *name;
  };

  typedef std::vector<KeyInfo> KeyInfoVector;
  typedef std::map<int, KeyInfo> KeyInfoMap;

  KeyInfoMap m_keyProperties;  //!< Map wx key to propertyId
  KeyInfoVector m_keyInfos;  //!< For indexed access

  // mouse
  bool m_propMouse_Left;
  bool m_propMouse_Middle;
  bool m_propMouse_Right;
  nvmath::Vec2i m_propMouse_Position;
  int  m_propMouse_Wheel;

  nvutil::PropertyId PID_Mouse_Left;
  nvutil::PropertyId PID_Mouse_Middle;
  nvutil::PropertyId PID_Mouse_Right;
  nvutil::PropertyId PID_Mouse_Position;
  nvutil::PropertyId PID_Mouse_Wheel;

  // keyboard
  bool m_propWXK_BACK;
  bool m_propKey_Tab;
  bool m_propKey_Return;
  bool m_propKey_Escape;
  bool m_propKey_Space;
  bool m_propKey_Delete;
  bool m_propWXK_START;
  bool m_propWXK_LBUTTON;
  bool m_propWXK_RBUTTON;
  bool m_propWXK_CANCEL;
  bool m_propWXK_MBUTTON;
  bool m_propWXK_CLEAR;
  bool m_propKey_Shift;
  bool m_propKey_Control;
  bool m_propWXK_MENU;
  bool m_propWXK_PAUSE;
  bool m_propWXK_CAPITAL;
  bool m_propWXK_PRIOR;
  bool m_propWXK_NEXT;
  bool m_propWXK_END;
  bool m_propWXK_HOME;
  bool m_propWXK_LEFT;
  bool m_propWXK_UP;
  bool m_propWXK_RIGHT;
  bool m_propWXK_DOWN;
  bool m_propWXK_SELECT;
  bool m_propWXK_PRINT;
  bool m_propWXK_EXECUTE;
  bool m_propWXK_SNAPSHOT;
  bool m_propWXK_INSERT;
  bool m_propWXK_HELP;
  bool m_propWXK_NUMPAD0;
  bool m_propWXK_NUMPAD1;
  bool m_propWXK_NUMPAD2;
  bool m_propWXK_NUMPAD3;
  bool m_propWXK_NUMPAD4;
  bool m_propWXK_NUMPAD5;
  bool m_propWXK_NUMPAD6;
  bool m_propWXK_NUMPAD7;
  bool m_propWXK_NUMPAD8;
  bool m_propWXK_NUMPAD9;
  bool m_propWXK_MULTIPLY;
  bool m_propWXK_ADD;
  bool m_propWXK_SEPARATOR;
  bool m_propWXK_SUBTRACT;
  bool m_propWXK_DECIMAL;
  bool m_propWXK_DIVIDE;
  bool m_propWXK_F1;
  bool m_propWXK_F2;
  bool m_propWXK_F3;
  bool m_propWXK_F4;
  bool m_propWXK_F5;
  bool m_propWXK_F6;
  bool m_propWXK_F7;
  bool m_propWXK_F8;
  bool m_propWXK_F9;
  bool m_propWXK_F10;
  bool m_propWXK_F11;
  bool m_propWXK_F12;
  bool m_propWXK_F13;
  bool m_propWXK_F14;
  bool m_propWXK_F15;
  bool m_propWXK_F16;
  bool m_propWXK_F17;
  bool m_propWXK_F18;
  bool m_propWXK_F19;
  bool m_propWXK_F20;
  bool m_propWXK_F21;
  bool m_propWXK_F22;
  bool m_propWXK_F23;
  bool m_propWXK_F24;
  bool m_propWXK_NUMLOCK;
  bool m_propWXK_SCROLL;
  bool m_propWXK_PAGEUP;
  bool m_propWXK_PAGEDOWN;
  bool m_propWXK_NUMPAD_SPACE;
  bool m_propWXK_NUMPAD_TAB;
  bool m_propWXK_NUMPAD_ENTER;
  bool m_propWXK_NUMPAD_F1;
  bool m_propWXK_NUMPAD_F2;
  bool m_propWXK_NUMPAD_F3;
  bool m_propWXK_NUMPAD_F4;
  bool m_propWXK_NUMPAD_HOME;
  bool m_propWXK_NUMPAD_LEFT;
  bool m_propWXK_NUMPAD_UP;
  bool m_propWXK_NUMPAD_RIGHT;
  bool m_propWXK_NUMPAD_DOWN;
  bool m_propWXK_NUMPAD_PRIOR;
  bool m_propWXK_NUMPAD_PAGEUP;
  bool m_propWXK_NUMPAD_NEXT;
  bool m_propWXK_NUMPAD_PAGEDOWN;
  bool m_propWXK_NUMPAD_END;
  bool m_propWXK_NUMPAD_BEGIN;
  bool m_propWXK_NUMPAD_INSERT;
  bool m_propWXK_NUMPAD_DELETE;
  bool m_propWXK_NUMPAD_EQUAL;
  bool m_propWXK_NUMPAD_MULTIPLY;
  bool m_propWXK_NUMPAD_ADD;
  bool m_propWXK_NUMPAD_SEPARATOR;
  bool m_propWXK_NUMPAD_SUBTRACT;
  bool m_propWXK_NUMPAD_DECIMAL;
  bool m_propWXK_NUMPAD_DIVIDE;
  bool m_propWXK_WINDOWS_LEFT;
  bool m_propWXK_WINDOWS_RIGHT;
  bool m_propWXK_WINDOWS_MENU;
  bool m_propWXK_COMMAND;
  bool m_propWXK_SPECIAL1;
  bool m_propWXK_SPECIAL2;
  bool m_propWXK_SPECIAL3;
  bool m_propWXK_SPECIAL4;
  bool m_propWXK_SPECIAL5;
  bool m_propWXK_SPECIAL6;
  bool m_propWXK_SPECIAL7;
  bool m_propWXK_SPECIAL8;
  bool m_propWXK_SPECIAL9;
  bool m_propWXK_SPECIAL10;
  bool m_propWXK_SPECIAL11;
  bool m_propWXK_SPECIAL12;
  bool m_propWXK_SPECIAL13;
  bool m_propWXK_SPECIAL14;
  bool m_propWXK_SPECIAL15;
  bool m_propWXK_SPECIAL16;
  bool m_propWXK_SPECIAL17;
  bool m_propWXK_SPECIAL18;
  bool m_propWXK_SPECIAL19;
  bool m_propWXK_SPECIAL20;

  bool m_propKey_0;
  bool m_propKey_1;
  bool m_propKey_2;
  bool m_propKey_3;
  bool m_propKey_4;
  bool m_propKey_5;
  bool m_propKey_6;
  bool m_propKey_7;
  bool m_propKey_8;
  bool m_propKey_9;
  bool m_propKey_A;
  bool m_propKey_B;
  bool m_propKey_C;
  bool m_propKey_D;
  bool m_propKey_E;
  bool m_propKey_F;
  bool m_propKey_G;
  bool m_propKey_H;
  bool m_propKey_I;
  bool m_propKey_J;
  bool m_propKey_K;
  bool m_propKey_L;
  bool m_propKey_M;
  bool m_propKey_N;
  bool m_propKey_O;
  bool m_propKey_P;
  bool m_propKey_Q;
  bool m_propKey_R;
  bool m_propKey_S;
  bool m_propKey_T;
  bool m_propKey_U;
  bool m_propKey_V;
  bool m_propKey_W;
  bool m_propKey_X;
  bool m_propKey_Y;
  bool m_propKey_Z;

protected:
  virtual void hidNotify( nvutil::PropertyId property );

  /*!
    \name Event handlers
    Event handlers for events the wxRenderArea can receive and that have to be
    propagated up to the parent of this window. 
  */
  //@{

  //! Propagate key events to the parent widget
  virtual void PropagateKeyEvent( wxKeyEvent& event )
  { 
    // Make the RenderArea give this event to its parent, even
    // though its not a command event.
    event.ResumePropagation(wxEVENT_PROPAGATE_MAX);    
    event.Skip(); 
  }  
  //! Propagate mouse events to the parent widget
  virtual void PropagateMouseEvent( wxMouseEvent& event )
  { 
    event.ResumePropagation(wxEVENT_PROPAGATE_MAX);    
    event.Skip(); 
  }
  //! Propagate paint events to the parent widget
  virtual void PropagatePaintEvent( wxPaintEvent& event )
  {
    event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
    event.Skip();
  }

  //! Do nothing if the background gets erased, to avoid flashing
  virtual void OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
  {
  }
};



