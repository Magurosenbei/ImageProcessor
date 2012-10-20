#ifndef TEXTURE_H
#define TEXTURE_H

class   Texture
{
protected:
    size_t              m_iWidth;
    size_t              m_iHeight;
    unsigned char *     m_pData;
    size_t              m_Type;

    float   *   m_pfData;
public:
    Texture(std::string pszFileName);
    Texture(size_t piWidth, size_t piHeight, size_t pType, unsigned char * pData);

    ~Texture();

    const   size_t          Type()      {   return  m_Type;     }
    const   size_t          Width()     {   return  m_iWidth;   }
    const   size_t          Height()    {   return  m_iHeight;  }

    //  Returns Byte Data
    const   ILubyte *       Data()      {   return  m_pData;    }
    //  Returns Float Data
    const   float   *       FData()     {   return  m_pfData;   }

    //@param    pszFileName :   Must Include Extension
            void            SaveAs(std::string pszFileName);
            float   *       GenerateGrayDataF();
};

#endif

