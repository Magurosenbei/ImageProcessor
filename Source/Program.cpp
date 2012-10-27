#include "Common/Common.h"
#include "Common/Texture.h"
//#include "OGL.h"
#include "Program.h"


Program::Program()
{

}

bool    Program::Initialize(
int     nArgument,
char *  szArgument[])
{
    ilInit();
    iluInit();
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);
    ilEnable(IL_FILE_OVERWRITE);

    //m_pOpenGL = new OGL();
    //m_pOpenGL->Initialize(640, 480);

    return true;
}

bool    Program::Shutdown()
{
    //m_pOpenGL->Shutdown();

    delete m_pMRF;
    //delete m_pOpenGL;
    delete m_pImage1;
    delete m_pImage2;
    return true;
}

void    Program::Run()
{
    m_pImage1 = new Texture("Data/exp0_view5.png");
    m_pImage2 = new Texture("Data/exp2_view5.png");

    size_t  _iImageSize = m_pImage1->Width() * m_pImage1->Height();

    size_t _iAlpha  = 101;
    size_t _iGagueW = 4;

    //  D Matrix
    float *   _pfDMatrix = new float [_iImageSize * _iAlpha]; //   cost[p*nLabels+l]
    //for(size_t i = 0; i < _iImageSize; i++)
    //    _pfDMatrix[i] = new float[_iAlpha];

    float * _pfImageR   = m_pImage1->GenerateGrayDataF();
    float * _pfImageL   = m_pImage2->GenerateGrayDataF();

    float * _pfSMatrix  = new float[_iAlpha * _iAlpha];

    float   _fAlpha     = 0.0f;
    size_t  _iWidth     = m_pImage1->Width();
    size_t  _iHeight    = m_pImage1->Height();
    float   _fImgCal    = 0.0f;
    float   _fImgCalLog = 0.0f;
    float   _fDivisor   = 1.0f /  ( (2 * _iGagueW + 1) * 0.5f * logf(2.0f));
    float   _fFactor    = 1.0f / (_iAlpha - 1);

    printf("\nBuidling DCost\n");
    m_Timer.Start();

//#pragma omp parallel
{
    //#pragma omp parallel for private(_fImgCal, _fImgCalLog, _fAlpha)
    for(size_t y = 0; y < _iHeight; y++)
    {
        for(size_t x = 0; x < _iWidth; x++)
        {
            for(size_t z = 0; z < _iAlpha; z++)
            {
                _fAlpha = static_cast<float>(z) / static_cast<float>(_iAlpha - 1);
                if( y - _iGagueW > 0 &&
                    y + _iGagueW < _iHeight  &&
                    x + _iGagueW < _iWidth   &&
                    x - _iGagueW > 0)
                {
                    _fImgCalLog = 0; _fImgCal = 0;
                    for(size_t j = -_iGagueW; j <= _iGagueW; j++)
                    {
                        for(size_t i = -_iGagueW; i <= _iGagueW; i++)
                        {
                            _fImgCalLog =   _pfImageL[((y + j) * _iWidth) + (x + i)] * _fAlpha;
                            _fImgCalLog +=  _pfImageR[((y + j) * _iWidth) + (x + i)] * (1.0f - _fAlpha);
                            _fImgCal += _fImgCalLog * logf(_fImgCalLog);
                        }
                        //  Summation of WxW around pix(X, Y), with the log of itself
                        _pfDMatrix[((y * _iWidth) + x) * _iAlpha + z] = expf(_fImgCal * _fDivisor);
                        _pfDMatrix[((y * _iWidth) + x) * _iAlpha + z] = std::max(_pfDMatrix[((y * _iWidth) + x) * _iAlpha + z], 0.0f);
                        _pfDMatrix[((y * _iWidth) + x) * _iAlpha + z] = std::min(_pfDMatrix[((y * _iWidth) + x) * _iAlpha + z], 1.0f);
                    }
                }
                else
                    _pfDMatrix[((y * _iWidth) + x) * _iAlpha + z] = 1;
            }
        }
    }
}
    printf("\nEnd Time : %f ms\n", m_Timer.End());
    printf("\nBuidling SCost\n");
    m_Timer.Start();
    //  Compute S Matrix
    //  V(l1,l2) = smooth[l1+numberofLabels*l2] = smooth[l2+numberofLabels*l1]
    //  [X,Y] = meshgrid(1:3,10:14)
    //  S = abs( (X - 1)/(nAlpha - 1) - (Y - 1)/(nAlpha - 1));

    //#pragma omp parallel for
    for(size_t j = 0; j < _iAlpha; j++)
        for(size_t i = 0; i < _iAlpha; i++)
            _pfSMatrix[(j * _iAlpha) + i] = fabsf(_fFactor * i - _fFactor * j);

    printf("\nEnd Time : %f ms", m_Timer.End());
    printf("\nGC Process\n");
    //  Graph Cut
    float _fTime = 0.0f;
    DataCost        *   _pDCost         = new DataCost(_pfDMatrix);
    SmoothnessCost  *   _pSCost         = new SmoothnessCost(_pfSMatrix);
    EnergyFunction  *   _pEnergyFunc    = new EnergyFunction(_pDCost, _pSCost);
    m_pMRF                              = new Swap( m_pImage1->Width(),
                                                    m_pImage1->Height(),
                                                    _iAlpha, _pEnergyFunc);
    m_Timer.Start();
    m_pMRF->initialize();
    m_pMRF->clearAnswer();
    m_pMRF->optimize(0, _fTime);

    MRF::EnergyVal E_smooth = m_pMRF->smoothnessEnergy();
    MRF::EnergyVal E_data   = m_pMRF->dataEnergy();
    printf("\n(Smoothness energy %f, Data Energy %f)\n", E_smooth, E_data);

    printf("\nEnd Time : %f ms %f s", m_Timer.End(), _fTime);
    int _iPrev = 0;
    for (size_t pix = 0; pix < _iImageSize; pix++ )
    {
        if(_iPrev != m_pMRF->getLabel(pix))
        {
            printf("\nLabel of pixel %i is %i", (int)pix, m_pMRF->getLabel(pix));
            _iPrev =  m_pMRF->getLabel(pix);
        }
    }

    /*%% fusion of I1, I2 to create I.
    wMap = (double(labels))/(nAlpha-1);
    hdr = imgL.*wMap + imgR.*(1-wMap);
    I = zeros(m,n,3);
    I(:,:,1) = double(I1(:,:,1)).*wMap + double(I2(:,:,1)).*(1-wMap);
    I(:,:,2) = double(I1(:,:,2)).*wMap + double(I2(:,:,2)).*(1-wMap);
    I(:,:,3) = double(I1(:,:,3)).*wMap + double(I2(:,:,3)).*(1-wMap);
    */
    printf("\nGenerating Image ...");
    unsigned char *   _fNewImgRGB =   new unsigned char[_iImageSize * 3];
    size_t      _iPixIndex  =   0;
    float       _fLabel     =   0.0f;
    for(size_t y = 0; y < _iHeight; y++)
    {
        for(size_t x = 0; x < _iWidth; x++)
        {
            _iPixIndex = (y * _iWidth + x) * 3;

            _fLabel = m_pMRF->getLabel(y * _iWidth + x) * _fFactor;
            _fNewImgRGB[_iPixIndex]      =   _fLabel * m_pImage1->Data()[_iPixIndex]      +   (1.0f - _fLabel) * m_pImage2->Data()[_iPixIndex];
            _fNewImgRGB[_iPixIndex + 1]  =   _fLabel * m_pImage1->Data()[_iPixIndex + 1]  +   (1.0f - _fLabel) * m_pImage2->Data()[_iPixIndex + 1];
            _fNewImgRGB[_iPixIndex + 2]  =   _fLabel * m_pImage1->Data()[_iPixIndex + 2]  +   (1.0f - _fLabel) * m_pImage2->Data()[_iPixIndex + 2];
        }
    }
    Texture::SaveAs("Data/Test.png", _iWidth, _iHeight, IL_RGB, IL_UNSIGNED_BYTE, _fNewImgRGB);

    delete      _pDCost;
    delete      _pSCost;
    delete      _pEnergyFunc;
    delete []   _pfDMatrix;
    delete []   _pfSMatrix;
    delete []   _pfImageL;
    delete []   _pfImageR;

    _pDCost     = NULL;
    _pSCost     = NULL;
    _pEnergyFunc= NULL;
    _pfDMatrix  = NULL;
    _pfSMatrix  = NULL;
    _pfImageL   = NULL;
    _pfImageR   = NULL;
}


int main(int nArgument, char *  szArgument[])
{
    printf("\nProgram Start\n");
    Program _Program;
    for(int i = 0 ; i < 10; i++)
    {
        printf("\nTest Iteration : %i", i);
        _Program.Initialize(nArgument, szArgument);
        _Program.Run();
        _Program.Shutdown();
    }

    printf("\nProgram End\n");
    return 0;
}
