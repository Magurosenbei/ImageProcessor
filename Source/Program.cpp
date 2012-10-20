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
    float   _fDivisor   = 1.0f /  ( (2 * _iGagueW + 1) * 0.5f * log(2.0f));

    printf("\nBuidling DCost\n");
    m_Timer.Start();

#pragma omp parallel
{
    #pragma omp parallel for private(_fImgCal, _fImgCalLog, _fAlpha)
    for(size_t y = 0; y < m_pImage1->Height(); y++)
    {
        for(size_t x = 0; x < m_pImage1->Width(); x++)
        {
            for(size_t z = 0; z < _iAlpha; z++)
            {
                _fAlpha = static_cast<float>(z - 1) / static_cast<float>(_iAlpha - 1);
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
                            _fImgCalLog =   _fAlpha * _pfImageL[((y + j) * x) + (x + i)] +
                                            (1.0f - _fAlpha) * _pfImageR[((y + j) * x) + (x + i)];
                            _fImgCal += _fImgCalLog + log(_fImgCalLog);
                        }
                        //cost[p*nLabels+l]
                        //_pfDMatrix[((y * x) + x)][z] = exp(_fImgCal) * _fDivisor;
                        _pfDMatrix[((y * x) + x) * _iAlpha + z] = exp(_fImgCal) * _fDivisor;
                    }
                }
                else
                    _pfDMatrix[((y * x) + x) * _iAlpha + z] = 1;//_pfDMatrix[(y * x) + x][z] = 1;
            }
        }
    }
}
    printf("\nEnd Time : %f ms\n", m_Timer.End());
    printf("\nBuidling SCost\n");
    m_Timer.Start();
    //  Compute S Matrix
    //  V(l1,l2) = smooth[l1+numberofLabels*l2] = smooth[l2+numberofLabels*l1]
    /*
    [X,Y] = meshgrid(1:3,10:14)
    X =
         1     2     3
         1     2     3
         1     2     3
         1     2     3
         1     2     3

    Y =
        10    10    10
        11    11    11
        12    12    12
        13    13    13
        14    14    14
    */
    //S = abs( (X - 1)/(nAlpha - 1) - (Y - 1)/(nAlpha - 1));
    {
        float       _fFactor    = 1.0f / (_iAlpha - 1);
        //float  *    _ftX        =   new float[_iAlpha * _iAlpha];
        //float  *    _ftY        =   new float[_iAlpha * _iAlpha];
        //#pragma omp parallel for
        for(size_t j = 0; j < _iAlpha; j++)
        {
            for(size_t i = 0; i < _iAlpha; i++)
            {
                //_ftX[((j * i) + i)] = i;
                //_ftY[((j * i) + i)] = j;
                _pfSMatrix[(j * _iAlpha) + i] = fabsf(_fFactor * i - _fFactor * j);
                //_pfSMatrix[(i * _iAlpha) + j] = _pfSMatrix[(j * _iAlpha) + i];
            }
        }
        /*for(size_t j = 0; j < _iAlpha; j++)
        {
            for(size_t i = 0; i < _iAlpha; i++)
            {
                _pfSMatrix[(j * _iAlpha) + i] = fabsf(_fFactor * _ftX[((j * i) + i)] - _fFactor * _ftY[((j * i) + i)]);
                _pfSMatrix[(i * _iAlpha) + j] = _pfSMatrix[(j * _iAlpha) + i];
                //_pfSMatrix[((j * i) + i)] = fabsf(_ftX[(j * _iAlpha) + i] - _ftY[(i * _iAlpha) + j]);
            }
        }*/
    }
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
    m_pMRF->optimize(5, _fTime);

    MRF::EnergyVal E_smooth = m_pMRF->smoothnessEnergy();
    MRF::EnergyVal E_data   = m_pMRF->dataEnergy();
    printf("\n(Smoothness energy %f, Data Energy %f)\n", E_smooth, E_data);


    printf("\nEnd Time : %f ms %f s", m_Timer.End(), _fTime);
    for (size_t pix = 0; pix < _iImageSize; pix++ )
        printf("\nLabel of pixel %i is %i", (int)pix, m_pMRF->getLabel(pix));



    /*%% fusion of I1, I2 to create I.
    wMap = (double(labels))/(nAlpha-1);
    hdr = imgL.*wMap + imgR.*(1-wMap);
    I = zeros(m,n,3);
    I(:,:,1) = double(I1(:,:,1)).*wMap + double(I2(:,:,1)).*(1-wMap);
    I(:,:,2) = double(I1(:,:,2)).*wMap + double(I2(:,:,2)).*(1-wMap);
    I(:,:,3) = double(I1(:,:,3)).*wMap + double(I2(:,:,3)).*(1-wMap);
    figure, imshow(uint8(round(I))),title('Fused Color Img');
    imwrite(uint8(round(I)),'I.png','png');
    p1 = imhist(imgL)./numel(imgL); p2 = imhist(imgR)./numel(imgR); p3 = imhist(hdr)./numel(hdr);
    p1(p1==0) = 1; p2(p2==0) = 1;p3(p3==0) = 1;
    avgInfVal1 = sum(-p1.*log(p1)); avgInfVal2 = sum(-p2.*log(p2)); avgInfVal3 = sum(-p3.*log(p3));
    fprintf(1,'Information val left->%f right->%f fused->%f\n',avgInfVal1,avgInfVal2,avgInfVal3);
    */

    /*Texture *   _NTex = new Texture(m_pImage1->Width(), m_pImage1->Height(), m_pImage1->Type(),
                                    _pfDMatrix);

    _NTex->SaveAs("Data/Test.png");*/

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
    _Program.Initialize(nArgument, szArgument);
    _Program.Run();
    _Program.Shutdown();
    printf("\nProgram End\n");
    return 0;
}
