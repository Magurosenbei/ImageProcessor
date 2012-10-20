#ifndef PROGRAM_H
#define PROGRAM_H


class   Program
{
    System_Timer    m_Timer;
    MRF         *   m_pMRF;
    //OGL         *   m_pOpenGL;

    Texture     *   m_pImage1;
    Texture     *   m_pImage2;

    Texture     *   m_pGenImage;
public:
    Program();

    bool    Initialize( int nArgument,
                        char *  szArgument[]);

    bool    Shutdown();

    void    Run();
};


#endif
