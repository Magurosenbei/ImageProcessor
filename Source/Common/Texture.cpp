#include "Common.h"
#include "Texture.h"

Texture::Texture(std::string pszFileName)
{
    ILuint  _iImageId   = 0;
    size_t  _iImageSize = 0;

    _iImageId = ilGenImage();
    ilBindImage(_iImageId);
    WriteCommandLine("\nLoading Image %s", pszFileName.c_str());
    VERIFY( ilLoadImage(pszFileName.c_str() ) == IL_TRUE, "Texture: Unable to load file!");

    switch(ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL))
    {
        case 3:
        ASSERT(  ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE) == IL_TRUE  , "Texture: Unable to load file!");
		m_Type = IL_RGB;
        break;

        case 4:
        ASSERT(  ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) == IL_TRUE  , "Texture: Unable to load file!");
        m_Type = IL_RGBA;
        break;

        default:
        ASSERT(0, "Texture: Unable to load file!");
        break;
    }
    m_iHeight   = ilGetInteger(IL_IMAGE_HEIGHT);
    m_iWidth    = ilGetInteger(IL_IMAGE_WIDTH);

    _iImageSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);

    m_pData     =   new unsigned char[_iImageSize];
    m_pfData    =   new float[_iImageSize];
    memcpy(m_pData, ilGetData(), _iImageSize);

    for(size_t i = 0; i < _iImageSize; i++)
        m_pfData[i] = ((float)m_pData[i]) / 255.0f;

    ilDeleteImage(_iImageId);
}

Texture::~Texture()
{
    delete [] m_pData;
    delete [] m_pfData;

    m_pData     =   NULL;
    m_pfData    =   NULL;
}

void Texture::SaveAs(std::string pszFileName, size_t piWidth, size_t piHeight, size_t pType, size_t pFmt, void * pData)
{
    ILuint  _iImageId   = 0;

    _iImageId = ilGenImage();
    ilBindImage(_iImageId);

    ASSERT(  ilTexImage(piWidth,    piHeight, 0, 3, pType, pFmt, pData) == IL_TRUE  , "Texture: Unable to set data!");
    VERIFY(  ilSaveImage(pszFileName.c_str()) == IL_TRUE  , "Texture: Unable to save Image!");

    ilDeleteImage(_iImageId);
}

float * Texture::GenerateGrayDataF()
{
    float   *   _pData      =  new float[m_iHeight * m_iWidth];
    size_t      _iImageSize =   m_iHeight * m_iWidth;

    //Y = .2126 * R^gamma + .7152 * G^gamma + .0722 * B^gamma
    for(size_t i = 0; i < _iImageSize; i ++)
    {
        _pData[i] += m_pfData[i * 3] * 0.2126f;
        _pData[i] += m_pfData[i * 3 + 1] * 0.7152f;
        _pData[i] += m_pfData[i * 3 + 2] * 0.0722f;
    }
    return      _pData;
}
