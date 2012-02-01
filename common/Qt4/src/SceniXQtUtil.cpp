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

#include "SceniXQtUtil.h"

#include <nvsg/TextureHost.h>
#include <nvutil/ReadableObject.h>
#include <nvutil/WritableObject.h>

using namespace nvsg;
using namespace nvutil;

static QImage errorImage;

// Data in QImage::Format_ARGB32 
const static unsigned int rgby[] =
{
  0xFFFF0000,
  0xFF00FF00,
  0xFF0000FF,
  0xFFFFFF00
};

template <typename SrcType>
struct UnsignedIntegerToUnsignedChar
{
public:
  unsigned char operator()( const char *&src ) const
  {
    unsigned char result = *((SrcType*)src) >> ((sizeof(SrcType) * 8) - 8);
    src += sizeof(SrcType);
    return result;
  }
};

template <typename SrcType>
struct IntegerToUnsignedChar
{
public:
  unsigned char operator()( const char *&src ) const
  {
    unsigned char result = std::max((SrcType)0, *((SrcType*)src)) >> ((sizeof(SrcType) * 8) - 8);
    src += sizeof(SrcType);
    return result;
  }
};


template <typename SrcType>
struct FloatToChar
{
public:
  FloatToChar( SrcType maxValue ) : m_scale( 255.0f / maxValue ) {}

  unsigned char operator()( const char *&src ) const
  {
    unsigned char result = static_cast<unsigned char>(*((SrcType*)src) * m_scale);
    src += sizeof(SrcType);
    return result;
  }
protected:
  SrcType m_scale;
};


template <typename Converter>
void convertRGBToRGBA( unsigned char *dst, const char *src, unsigned int numPixels, const Converter &converter)
{
  for (unsigned int pixel = 0;pixel < numPixels;++pixel)
  {
    dst[2] = converter(src);
    dst[1] = converter(src);
    dst[0] = converter(src);
    dst[3] = 255;
    dst += 4;
  }
}

template <typename Converter>
void convertRGBAToRGBA( unsigned char *dst, const char *src, unsigned int numPixels, const Converter &converter)
{
  for (unsigned int pixel = 0;pixel < numPixels;++pixel)
  {
    dst[2] = converter(src);
    dst[1] = converter(src);
    dst[0] = converter(src);
    dst[3] = converter(src);
    dst += 4;
  }
}

template <typename Converter>
void convertBGRToRGBA( unsigned char *dst, const char *src, unsigned int &numPixels, const Converter &converter)
{
  for (unsigned int pixel = 0;pixel < numPixels;++pixel)
  {
    dst[0] = converter(src);
    dst[1] = converter(src);
    dst[2] = converter(src);
    dst[3] = 255;
    dst += 4;
  }
}

template <typename Converter>
void convertBGRAToRGBA( unsigned char *dst, const char *src, unsigned int numPixels, const Converter &converter)
{
  for (unsigned int pixel = 0;pixel < numPixels;++pixel)
  {
    dst[0] = converter(src);
    dst[1] = converter(src);
    dst[2] = converter(src);
    dst[3] = converter(src);
    dst += 4;
  }
}

/** \brief Creates an QImage out of a TextureHost 
**/
QImage createQImage( const nvsg::TextureHostSharedPtr & textureImage, int image, int mipmap )
{
  TextureHostReadLock texImage( textureImage );

  Buffer::DataReadLock buffer( texImage->getPixels( image, mipmap ) );
  const void *srcData = buffer.getPtr();

  int width = texImage->getWidth( image, mipmap );
  int height = texImage->getHeight( image, mipmap );
  unsigned int numPixels = width * height;

  std::vector<unsigned char> tmpData;
  tmpData.resize( width * 4 * height );
  QImage result;

  // determine QImage format
  QImage::Format format = QImage::Format_ARGB32;
  bool supported = true;

  if (texImage->getDepth() != 1)
  {
    supported = false;
  }
  else
  {
    // convert to RGBA
    switch ( texImage->getFormat( image, mipmap ) )
    {
    case Image::IMG_BGR:
      switch (texImage->getType())
      {
      case Image::IMG_UNSIGNED_BYTE:
        convertBGRToRGBA( &tmpData[0], (char*)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned char>() );
        break;
      case Image::IMG_UNSIGNED_SHORT:
        convertBGRToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned short>() );
        break;
      case Image::IMG_UNSIGNED_INT:
        convertBGRToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned int>() );
        break;
      case Image::IMG_BYTE:
        convertBGRToRGBA( &tmpData[0], (char*)srcData, numPixels, IntegerToUnsignedChar<char>() );
        break;
      case Image::IMG_SHORT:
        convertBGRToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<short>() );
        break;
      case Image::IMG_INT:
        convertBGRToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<int>() );
        break;
      case Image::IMG_FLOAT:
        {
          const float *src = static_cast<const float *>(srcData);
          float max = *std::max_element( src, src + 3 * numPixels );
          convertBGRToRGBA( &tmpData[0], (char *)srcData, numPixels, FloatToChar<float>(max) );
        }
        break;
      default:
        supported = false;
      }
      break;
    case Image::IMG_RGB:
      switch (texImage->getType())
      {
      case Image::IMG_UNSIGNED_BYTE:
        convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned char>() );
        break;
      case Image::IMG_UNSIGNED_SHORT:
        convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned short>() );
        break;
      case Image::IMG_UNSIGNED_INT:
        convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned int>() );
        break;
      case Image::IMG_BYTE:
        convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<char>() );
        break;
      case Image::IMG_SHORT:
        convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<short>() );
        break;
      case Image::IMG_INT:
        convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<int>() );
        break;
      case Image::IMG_FLOAT:
        {
          const float *src = static_cast<const float *>(srcData);
          float max = *std::max_element( src, src + 3 * numPixels );
          convertRGBToRGBA( &tmpData[0], (char *)srcData, numPixels, FloatToChar<float>( max ) );
        }
        break;
      default:
        supported = false;
      }
      break;
    case Image::IMG_RGBA:
      switch (texImage->getType())
      {
      case Image::IMG_UNSIGNED_BYTE:
        convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned char>() );
        break;
      case Image::IMG_UNSIGNED_SHORT:
        convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned short>() );
        break;
      case Image::IMG_UNSIGNED_INT:
        convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned int>() );
        break;
      case Image::IMG_BYTE:
        convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<char>() );
        break;
      case Image::IMG_SHORT:
        convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<short>() );
        break;
      case Image::IMG_INT:
        convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<int>() );
        break;
      case Image::IMG_FLOAT:
        {
          const float *src = static_cast<const float *>(srcData);
          float max = *std::max_element( src, src + 4 * numPixels );
          convertRGBAToRGBA( &tmpData[0], (char *)srcData, numPixels, FloatToChar<float>( max ) );
        }
        break;
      default:
        supported = false;
      }
      break;
    case Image::IMG_BGRA:
      switch (texImage->getType())
      {
      case Image::IMG_UNSIGNED_BYTE:
        convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned char>() );
        break;
      case Image::IMG_UNSIGNED_SHORT:
        convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned short>() );
        break;
      case Image::IMG_UNSIGNED_INT:
        convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, UnsignedIntegerToUnsignedChar<unsigned int>() );
        break;
      case Image::IMG_BYTE:
        convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<char>() );
        break;
      case Image::IMG_SHORT:
        convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<short>() );
        break;
      case Image::IMG_INT:
        convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, IntegerToUnsignedChar<int>() );
        break;
      case Image::IMG_FLOAT:
        {
          const float *src = static_cast<const float *>(srcData);
          float max = *std::max_element( src, src + 4 * numPixels );
          convertBGRAToRGBA( &tmpData[0], (char *)srcData, numPixels, FloatToChar<float>( max ) );
        }
        break;
      default:
        supported = false;
      }
      break;
    default:
      supported = false;
      break;
    };
  }
  if( !supported )
  {
    if( errorImage.isNull() )
    {
      errorImage = QImage( reinterpret_cast<const uchar *>(rgby), 2, 2, QImage::Format_ARGB32 );
    }

    result = errorImage;
  }
  else
  {
    // The QImage is mirrored vertically when using the pixelData of a TextureHost. Mirror it. 
    // This way it's also ensured that Qt does manage a copy of the pixel data.
    // DAR BUG: Won't work with Format_RGB888 and width which is NOT a multiple of four!
    // Because: "Constructs an image with the given width, height and format, that uses an existing memory buffer, data. 
    //           The width and height must be specified in pixels, data must be 32-bit aligned, 
    //           and ***each scanline of data in the image must also be 32-bit aligned***."
    result = QImage( reinterpret_cast<const uchar *>( &tmpData[0] ), 
                            texImage->getWidth( image, mipmap ), texImage->getHeight( image, mipmap ), format ).mirrored();
  }

  return result;
}
