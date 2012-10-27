#ifndef TEXTURE_H
#define TEXTURE_H

class   Texture
{
protected:
    size_t              m_iWidth;
    size_t              m_iHeight;
    unsigned char *     m_pData;
    size_t              m_Type;

    float         *     m_pfData;       //  Gray Scale
public:
    Texture(std::string pszFileName);
    ~Texture();

    const   size_t          Type()      {   return  m_Type;     }
    const   size_t          Width()     {   return  m_iWidth;   }
    const   size_t          Height()    {   return  m_iHeight;  }

    //  Returns Byte Data
    const   ILubyte *       Data()      {   return  m_pData;    }
    //  Returns Float Data
    const   float   *       FData()     {   return  m_pfData;   }

    //@param    pszFileName :   Must Include Extension
    //@param    piWidth     :   Width in pixels
    //@param    piHeight    :   Height in pixels
    //@param    pType       :   RGB / RGBA
    //@param    pFmt        :   Data type of pData
    //@param    pData       :   Pointer to pixel memory
    //@throws   Exception   :   Failed to create file or set image data to DevIL
    static  void            SaveAs(std::string pszFileName, size_t piWidth, size_t piHeight, size_t pType, size_t pFmt, void * pData);
    //@return               :   Array of Floats in Gray Scale of Width * Height
            float   *       GenerateGrayDataF();
};

#endif

