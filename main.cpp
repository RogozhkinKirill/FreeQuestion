#include "TXLib.h"
#include "DirectTX.cpp"


#include <stdio.h>
#include <math.h>

//------------------------------------------------------------

double PI = 3.14159265358979323846;

size_t SIN = 1;
size_t COS = 2;


//------------------------------------------------------------

void DrawCircle     (int x , int y , int radius , COLORREF colorIn , COLORREF colorOut);

void DrawPush       (int time , double amplitude , int line);

void DrawSinusoidal (int xCord     , int time   , double phase     , double frequency ,
                     int amplitude , int line  , int     breakdown , size_t law);



void ReBuildScreen  (int sizeWinX , int sizeWinY  , int velLine ,
                     int pushLine , int objCentrX , int objCentrY , int* dotAmpl);



void PrintInfo      (double  velocity , double amplitude  ,  double phase ,
                     double  frequency , double periodTime , double pushPeriod ,
                     size_t  sizeWinX  , size_t sizeWinY   , double pushAmpl ,
                     double  pushVel   , int    breakdown);

void DrawMainInfo   (int sizeWinX , int sizeWinY , COLORREF background , COLORREF symbol);
void DrawPlus       (int cordX    , int cordY    , COLORREF background , COLORREF symbol);
void DrawMinus      (int cordX    , int cordY    , COLORREF background , COLORREF symbol);


void ScanInfo       (int* periodTime , double* pushPeriod , double* pushVelocity ,
                     int* breakdown  , double* phase      , double* ampl);



void CalcPhase      (int     time      , double  amplitude , double pushAmplitude ,
                     double* lastPhase , double* phase     , double frequency);

void CalcAmplitude  (int time             , double* amplitude , double* lastAmplitude ,
                     double pushAmplitude , double phase      , double frequency);


void ChangingParam (size_t sizeWinX    , size_t  sizeWinY  , double* phase     ,
                    double* pushPeriod , int*    periodTime , double* pushAmpl  ,
                    double* pushVel    , int* breakdown     , double  frequency , int* time);

//------------------------------------------------------------

int main ()
{
    //Собственные время и частоты системы
    int    periodTime = 0;
    double frequency  = 0;

    //Времы между толчками
    double pushPeriod = 0;

    //Скорости
    double pushVelocity;
    double curVelocity = 0;
    double pushVelUp   = 0;

    //Текущие значения
    int dotAmpl   = 0;
    int breakdown = 0;

    //Амплитуды
    double amplitude     = 0;
    double lastAmplitude = 0;
    double pushAmplitude = 0;
    double pushAmplUp    = 0;

    //Фазы
    double phase       = 0;
    double lastPhase   = 0;

    //Размеры окно
    size_t sizeWinX = 1350;
    size_t sizeWinY = 700;


    //Положение равновесия шара
    int objCentrX = sizeWinX / 4;
    int objCentrY = 70;


    ScanInfo (&periodTime , &pushPeriod , &pushVelocity , &breakdown , &phase , &amplitude);


    //Создаем окно для рисования
    DirectTXCreateWindow(sizeWinX , sizeWinY , "FreeQuestion... [Esc] to exit");


    if (periodTime != 0 || pushAmplitude != 0 || frequency != 0)
    {
        frequency     = 2 * PI / periodTime;
        pushAmplitude = pushVelocity / frequency;
        pushAmplUp    = -pushAmplitude;

        pushVelUp     = - pushVelocity;
    }
    else
    {
        frequency = 0;
        pushAmplitude = 0;
    }


    //Скорость
    int velLine  = sizeWinY - 320;

    //Толчки
    int pushLine = sizeWinY - 490;


    //Рисуем основу
    ReBuildScreen (sizeWinX , sizeWinY  , velLine ,
                   pushLine , objCentrX , objCentrY , &dotAmpl);

    DrawMainInfo (sizeWinX , sizeWinY , TX_WHITE , TX_BLACK);

    //область изменений
    RECT areaChanging = { sizeWinX / 2 , 0 , sizeWinX - 220 , 140};

    txBegin();

    int curPeriod = 0;
    int time = 0;

    //Рисование
    for (; !GetAsyncKeyState (VK_ESCAPE); ++time)
    {

        txSleep (0);

        //Области изменения Значений
        if (txMouseButtons() == 1 && In (txMousePos(), areaChanging))
        {
            double lastFre = frequency;
            txSleep (60);
            ChangingParam (sizeWinX     , sizeWinY    , &phase         ,
                           &pushPeriod   , &periodTime , &pushAmplitude ,
                           &pushVelocity , &breakdown  , frequency      , &dotAmpl);

            frequency     = 2 * PI / periodTime;

            DrawCircle (objCentrX + (int)(lastAmplitude * sin ((time - 1) * lastFre - lastPhase)) ,
                        objCentrY , 30 , TX_BLACK , TX_BLACK);

            while (txMouseButtons() == 1)
            {

            }
        }


        if (abs(amplitude * sin (time * frequency - phase)) < (sizeWinX - 30) / 4.0)
            DrawCircle (objCentrX + (int)(amplitude * sin ((time - 1) * frequency - phase)) ,
                        objCentrY , 30 , TX_BLACK , TX_BLACK);


        //Рисуем положение равновесия
        txSetColor (TX_WHITE);
        txLine (objCentrX , 0 , objCentrX , 140);


        if (abs(amplitude * sin (time * frequency - phase)) < (sizeWinX - 30) / 4.0)
            DrawCircle (objCentrX + (int)(amplitude * sin (time * frequency - phase)) ,
                        objCentrY , 30 , TX_CYAN , TX_BLACK);



        //Рисуем (1) Отклонение от положения равновесия, (2) скорости от времени, (3) толчка
        if (dotAmpl < sizeWinX * breakdown)
        {
            //(1)
            if (abs (amplitude * sin (time               * frequency - phase)) <  100 &&
                abs (amplitude * sin ((time - breakdown) * frequency - phase)) <  100 &&
                dotAmpl % breakdown == 0)
            {
                DrawSinusoidal (dotAmpl / breakdown , time            , phase     , frequency ,
                                amplitude           , sizeWinY - 100  , breakdown , SIN);
            }

            //(2)
            if (abs (curVelocity * cos ( time              * frequency - phase)) <  100 &&
                abs (curVelocity * cos ((time - breakdown) * frequency - phase)) <  100 &&
                dotAmpl % breakdown == 0)
            {
                DrawSinusoidal (dotAmpl / breakdown , time     , phase     , frequency ,
                                curVelocity         , velLine  , breakdown , COS);
            }

            //(3) вправо
            if ((GetAsyncKeyState (VK_RIGHT) || time % (int)pushPeriod == 0) &&
                !GetAsyncKeyState (VK_LEFT ))
            {
                if (pushVelocity * 40 < 50)
                {
                    DrawPush (dotAmpl / breakdown , -pushVelocity * 40 , pushLine);
                }
                else
                {
                    DrawPush (dotAmpl / breakdown , -49.0 , pushLine);
                }

            }

            //(3) влево
            if (GetAsyncKeyState (VK_LEFT))
            {
                if (pushVelocity * 40 < 50)
                {
                    DrawPush (dotAmpl / breakdown , -pushVelUp * 40 , pushLine);
                }
                else
                {
                    DrawPush (dotAmpl / breakdown , 49.0 , pushLine);
                }
            }


            ++dotAmpl;
        }
        else
        {
            ReBuildScreen (sizeWinX , sizeWinY  , velLine ,
                           pushLine , objCentrX , objCentrY , &dotAmpl);
        }


        //Пишем информацию
        PrintInfo (curVelocity / frequency , amplitude  , phase         ,
                   frequency               , periodTime , pushPeriod    ,
                   sizeWinX                , sizeWinY   , pushAmplitude ,
                   pushVelocity            , breakdown);


        //Остановка движения
        if (GetAsyncKeyState (VK_SPACE))
        {
            if (abs(amplitude * sin (time * frequency - phase)) < (sizeWinX - 30) / 4.0)
                DrawCircle (objCentrX + (int)(amplitude * sin (time * frequency - phase)) ,
                            objCentrY , 30 , TX_BLACK , TX_BLACK);

            curVelocity = 0;

            phase = 0;

            amplitude = 0;
        }


        //Сон
        if (GetAsyncKeyState ('S'))
        {
            bool i = 0;
            do
            {
                txSleep (100);

                if (GetAsyncKeyState ('S'))
                    ++i;

                if (txMouseButtons() == 1 && In (txMousePos(), areaChanging))
                {
                    double lastFre = frequency;
                    ChangingParam (sizeWinX     , sizeWinY    , &phase         ,
                                   &pushPeriod   , &periodTime , &pushAmplitude ,
                                   &pushVelocity , &breakdown  , frequency      , &dotAmpl);

                    frequency     = 2 * PI / periodTime;

                    DrawCircle (objCentrX + (int)(lastAmplitude * sin ((time - 1) * lastFre - lastPhase)) ,
                                objCentrY , 30 , TX_BLACK , TX_BLACK);


                    //Пишем информацию
                    PrintInfo (curVelocity / frequency , amplitude  , phase         ,
                               frequency               , periodTime , pushPeriod    ,
                               sizeWinX                , sizeWinY   , pushAmplitude ,
                               pushVelocity            , breakdown);


                    while (txMouseButtons() == 1)
                    {

                    }
                }

            } while (i == 0);
        }


        //Математика

        if (time == (10000 * periodTime))
            time = 0;


        //Математика: удар вправо
        if ((time % (int)pushPeriod == 0 || GetAsyncKeyState (VK_RIGHT)) &&
            !GetAsyncKeyState (VK_LEFT)   && pushAmplitude != 0)
        {
            curPeriod++;

            CalcPhase (time      , amplitude , pushAmplitude ,
                       &lastPhase , &phase    , frequency);

            CalcAmplitude (time , &amplitude , &lastAmplitude , pushAmplitude , lastPhase , frequency);

            curVelocity  = amplitude;

        }


        //Математика: удар влево
        if (GetAsyncKeyState (VK_LEFT) && pushAmplUp != 0)
        {
            curPeriod++;

            CalcPhase (time      , amplitude , pushAmplUp ,
                       &lastPhase , &phase    , frequency);

            CalcAmplitude (time       , &amplitude , &lastAmplitude ,
                           pushAmplUp , lastPhase  , frequency);

            curVelocity  = amplitude;
        }

    }


    if (abs(amplitude * sin (time * frequency - phase)) < (sizeWinX - 30) / 4.0)
        DrawCircle (objCentrX + (int)(amplitude * sin ((time - 1) * frequency - phase)) ,
                    objCentrY , 30 , TX_BLACK , TX_BLACK);
    if (abs(amplitude * sin (time * frequency - phase)) < (sizeWinX - 30) / 4.0)
        DrawCircle (objCentrX + (int)(amplitude * sin (time       * frequency - phase)) ,
                    objCentrY , 30 , TX_CYAN , TX_BLACK);


    txEnd();

    return 0;
}

//------------------------------------------------------------
//Рисуем круг

void DrawCircle(int x , int y , int radius , COLORREF colorIn , COLORREF colorOut)
{
    txSetColor     (colorOut);
    txSetFillColor (colorIn);

    txCircle (x , y , radius);

    return;
}


//------------------------------------------------------------
// Рисуем толчки

void DrawPush (int time , double amplitude , int line)
{

    txSetColour     (TX_GREEN);
    txSetFillColour (TX_GREEN);
    txRectangle (time - 1 , line ,
                 time     , line - amplitude);
}

//------------------------------------------------------------
// Рисеум синусоиду

void DrawSinusoidal (int xCord     , int time      , double phase , double frequency ,
                     int amplitude , int    line   , int breakdown , size_t law)
{
    txSetColor(TX_GREEN);

    if (law == COS)
    {
        txLine (xCord - 1 , line + (int) (amplitude *
                                          cos((time - breakdown) * frequency - phase)) ,
                xCord     , line + (int) (amplitude *
                                          cos( time              * frequency - phase)));
    }

    if (law == SIN)
    {
        txLine (xCord - 1 , line + (int) (amplitude *
                                          sin((time - breakdown) * frequency - phase)) ,
                xCord     , line + (int) (amplitude *
                                          sin( time              * frequency - phase)));
    }
}

//------------------------------------------------------------

void ReBuildScreen (int sizeWinX , int sizeWinY  , int velLine ,
                    int pushLine , int objCentrX , int objCentrY , int* dotAmpl)
{
    txSetColour     (TX_BLACK);
    txSetFillColour (TX_BLACK);
    txRectangle     (0 , 160 , sizeWinX , sizeWinY);

    txSetColour(TX_WHITE);

    //Скорость
    txTextOut(0   , velLine - 110 , "Velocity" );
    txLine   (0   , velLine - 100 , sizeWinX , velLine - 100);
    txLine	 (0   , velLine       , sizeWinX , velLine); //Отклонение скорости
    txLine   (0   , velLine + 100 , sizeWinX , velLine + 100);

    //Отклонение
    txTextOut(0         , sizeWinY - 210 , "Deviation");
    txLine   (objCentrX , objCentrY - 70 , objCentrX , objCentrY + 70); //Положение равновесия (ТЕЛА)
    txLine   (0         , sizeWinY - 100 , sizeWinX  , sizeWinY - 100); //Положение равновесия (ТЕЛА)
    txLine   (0         , sizeWinY - 200 , sizeWinX  , sizeWinY - 200);


    //Толчки
    txTextOut(0 , pushLine - 60 , "Pushes");
    txLine   (0 , pushLine - 50 , sizeWinX , pushLine - 50);
    txLine   (0 , pushLine + 50 , sizeWinX , pushLine + 50);
    txLine   (0 , pushLine      , sizeWinX , pushLine);
    txLine   (0 , pushLine - 70 , sizeWinX , pushLine - 70);


    txLine   (sizeWinX / 2 , objCentrY - 70 , sizeWinX / 2 , objCentrY + 70);

    *dotAmpl = 0;
}

//------------------------------------------------------------

void PrintInfo (double velocity , double amplitude  , double phase      ,
                double frequency , double periodTime , double pushPeriod ,
                size_t sizeWinX  , size_t sizeWinY   , double pushAmpl   ,
                double  pushVel  , int  breakdown)
{
    char velStr[16];
    char ampStr[16];
    char phsStr[16];
    char freStr[16];
    char pTmStr[16];
    char pshStr[16];
    char pAmStr[16];
    char pVeStr[16];
    char brkStr[16];


    sprintf (velStr , "%.5f" , velocity);
    sprintf (ampStr , "%.5f" , amplitude);
    sprintf (phsStr , "%.5f" , phase / (2 * PI) * 360);
    sprintf (freStr , "%.5f" , frequency);
    sprintf (pTmStr , "%.5f" , periodTime);
    sprintf (pshStr , "%.5f" , pushPeriod);
    sprintf (pAmStr , "%.5f" , pushAmpl);
    sprintf (pVeStr , "%.5f" , pushVel);
    sprintf (brkStr , "%d" , breakdown);

    txSetColor     (TX_BLACK);
    txSetFillColor (TX_BLACK);
    txRectangle    (sizeWinX - 130 , 0 , sizeWinX , 120);

    txSetColor (TX_WHITE);
    txTextOut  (sizeWinX - 100 , 5    , velStr);
    txTextOut  (sizeWinX - 100 , 25   , ampStr);
    txTextOut  (sizeWinX - 100 , 45   , phsStr);
    txTextOut  (sizeWinX - 100 , 65   , freStr);
    txTextOut  (sizeWinX - 100 , 85   , pTmStr);
    txTextOut  (sizeWinX - 100 , 105  , pshStr);

    txSetColor (TX_BLACK);
    txRectangle   (sizeWinX - 290 , 0 , sizeWinX - 370 , 120);

    txSetColor (TX_WHITE);
    txTextOut  (sizeWinX - 355 , 5    , pAmStr);
    txTextOut  (sizeWinX - 355 , 25   , pVeStr);
    txTextOut  (sizeWinX - 355 , 45   , brkStr);
}

//------------------------------------------------------------

void DrawMainInfo   (int sizeWinX , int sizeWinY ,  COLORREF background , COLORREF symbol)
{
    txTextOut (sizeWinX - 210 , 5   , "Velocity");
    txTextOut (sizeWinX - 210 , 25  , "Amplitude");
    txTextOut (sizeWinX - 210 , 45  , "Phase");
    txTextOut (sizeWinX - 210 , 65  , "Frequency");
    txTextOut (sizeWinX - 210 , 85  , "Own  period");
    txTextOut (sizeWinX - 210 , 105 , "Push period");

    txTextOut (sizeWinX - 485 , 5   , "push amplitude");
    txTextOut (sizeWinX - 485 , 25  , "push velocity");
    txTextOut (sizeWinX - 485 , 45  , "breakdown");


    //Рисуем значки "+" & "-"

    DrawMinus (sizeWinX - 225 , 50   , background , symbol);
    DrawMinus (sizeWinX - 225 , 90   , background , symbol);
    DrawMinus (sizeWinX - 225 , 110  , background , symbol);

    DrawMinus (sizeWinX - 500 , 10   , background , symbol);
    DrawMinus (sizeWinX - 500 , 30   , background , symbol);
    DrawMinus (sizeWinX - 500 , 50   , background , symbol);


    DrawPlus  (sizeWinX - 245 , 50   , background , symbol);
    DrawPlus  (sizeWinX - 245 , 90   , background , symbol);
    DrawPlus  (sizeWinX - 245 , 110  , background , symbol);

    DrawPlus  (sizeWinX - 520 , 10   , background , symbol);
    DrawPlus  (sizeWinX - 520 , 30   , background , symbol);
    DrawPlus  (sizeWinX - 520 , 50   , background , symbol);
}

//-----------------------------------------------------------

void DrawPlus (int cordX , int cordY , COLORREF background , COLORREF symbol)
{
    txSetColour     (background);
    txSetFillColour (background);
    txRectangle     (cordX - 4 , cordY - 4 , cordX + 4 , cordY + 4);

    txSetColour     (symbol);
    txSetFillColour (symbol);
    txRectangle     (cordX - 3 , cordY - 1 , cordX + 3 , cordY + 1);
    txRectangle     (cordX - 1 , cordY - 3 , cordX + 1 , cordY + 3);
}

//------------------------------------------------------------

void DrawMinus (int cordX , int cordY , COLORREF background , COLORREF symbol)
{
    txSetColour     (background);
    txSetFillColour (background);
    txRectangle     (cordX - 4 , cordY - 4 , cordX + 4 , cordY + 4);

    txSetColour     (symbol);
    txSetFillColour (symbol);
    txRectangle     (cordX - 3 , cordY - 1 , cordX + 3 , cordY + 1);
}

//------------------------------------------------------------
//Считаем фазу

void CalcPhase (int     time      , double  amplitude , double pushAmplitude ,
                double* lastPhase , double* phase     , double frequency)
{
    *lastPhase = *phase;
    *phase     = atan2 (amplitude     * sin(*phase) +
                        pushAmplitude * sin(frequency * time)
            ,
                        amplitude     * cos(*phase) +
                        pushAmplitude * cos(frequency * time));

}

//------------------------------------------------------------
//Считаем амплитуду

void CalcAmplitude  (int time             , double* amplitude , double* lastAmplitude ,
                     double pushAmplitude , double phase      , double frequency)
{
    *lastAmplitude = *amplitude;
    *amplitude     = sqrt (*lastAmplitude *  (*lastAmplitude) +
                           pushAmplitude *    pushAmplitude  +
                           2 * pushAmplitude *  (*lastAmplitude) * cos(frequency * time - phase));
}

//------------------------------------------------------------

void ScanInfo (int* periodTime , double* pushPeriod , double* pushVelocity ,
               int* breakdown  , double* phase      , double* ampl)
{
    int i = 0;

    while (i == 0)
    {
        printf ("Print own oscillation period\n");
        scanf  ("%d" , periodTime);

        printf ("Print time between shocks\n");
        scanf  ("%lf" , pushPeriod);

        printf ("Print amplitude of shock\n");
        scanf  ("%lf" , pushVelocity);

        printf ("Print breakdown\n");
        scanf  ("%d" , breakdown);

        printf ("Print 1 if you want change\nPrint 0 if you want save current (%f)\n" , *phase);

        int ans = 0;
        scanf ("%d" , &ans);

        if (ans)
        {
            int grad = 0;
            printf ("In degrees(1) or in radians(0)\n");
            scanf  ("%d" , &grad);
            printf ("Print phase\n");

            if (!grad)
            {
                scanf  ("%lf" , phase);
            }
            else
            {
                double res = 0.0;
                scanf ("%lf" , &res);

                *phase = res / 180 * PI;
            }
        }

        printf ("Print positive amplitude\n");
        scanf  ("%lf" , ampl);

        printf ("%d %f %f %d %f %f\n" , *periodTime , *pushPeriod , *pushVelocity , *breakdown , *phase , *ampl);

        printf ("\nInformation scanned correct, did not it?(yes - 1, no - 0)\n");
        scanf  ("%d" , &i);
    }
}

//------------------------------------------------------------

void ChangingParam (size_t sizeWinX   ,  size_t  sizeWinY , double* phase    ,
                    double* pushPeriod , int* periodTime   , double* pushAmpl ,
                    double* pushVel    , int* breakdown    , double  frequency , int* time)
{
    static RECT phsPlus  = {sizeWinX - 250 , 45  , sizeWinX - 240 , 55};
    static RECT ownPlus  = {sizeWinX - 250 , 85  , sizeWinX - 240 , 95};
    static RECT pshPlus  = {sizeWinX - 250 , 105 , sizeWinX - 240 , 115};

    static RECT pAmPlus  = {sizeWinX - 525 , 5   , sizeWinX - 515 , 15};
    static RECT pVePlus  = {sizeWinX - 525 , 25  , sizeWinX - 515 , 35};
    static RECT brkPlus  = {sizeWinX - 525 , 45  , sizeWinX - 515 , 55};



    static RECT phsMinus = {sizeWinX - 230 , 45  , sizeWinX - 220 , 55};
    static RECT ownMinus = {sizeWinX - 230 , 85  , sizeWinX - 220 , 95};
    static RECT pshMinus = {sizeWinX - 230 , 105 , sizeWinX - 220 , 115};

    static RECT pAmMinus = {sizeWinX - 505 , 5   , sizeWinX - 495 , 15};
    static RECT pVeMinus = {sizeWinX - 505 , 25  , sizeWinX - 495 , 35};
    static RECT brkMinus = {sizeWinX - 505 , 45  , sizeWinX - 495 , 55};



    if (txMouseButtons() == 1 && In (txMousePos() , phsPlus))
        *phase += 2 * PI / 360;

    if (txMouseButtons() == 1 && In (txMousePos() , ownPlus))
        ++(*periodTime);

    if (txMouseButtons() == 1 && In (txMousePos() , pshPlus))
        ++(*pushPeriod);

    if (txMouseButtons() == 1 && In (txMousePos() , pAmPlus) && *pushAmpl  < 2.5)
    {
        *pushAmpl += 0.05;
        *pushVel   = *pushAmpl * frequency;
    }

    if (txMouseButtons() == 1 && In (txMousePos() , pVePlus) && *pushVel   < 2.5)
    {
        *pushVel  += 0.05;
        *pushAmpl  = *pushVel / frequency;
    }

    if (txMouseButtons() == 1 && In (txMousePos() , brkPlus) && *breakdown < 10)
    {
        ++(*breakdown);

        *time *= (*time) * *breakdown / (*breakdown - 1);
    }




    if (txMouseButtons() == 1 && In (txMousePos() , phsMinus))
        *phase -= 2 * PI / 360;

    if (txMouseButtons() == 1 && In (txMousePos() , ownMinus))
        --(*periodTime);

    if (txMouseButtons() == 1 && In (txMousePos() , pshMinus))
        --(*pushPeriod);

    if (txMouseButtons() == 1 && In (txMousePos() , pAmMinus) && *pushAmpl  > -2.5)
    {
        *pushAmpl -= 0.05;
        *pushVel   = *pushAmpl * frequency;
    }

    if (txMouseButtons() == 1 && In (txMousePos() , pVeMinus) && *pushVel   > -2.5)
    {
        *pushVel  -= 0.05;
        *pushAmpl  = *pushVel / frequency;
    }

    if (txMouseButtons() == 1 && In (txMousePos() , brkMinus) && *breakdown > 1)
    {
        --(*breakdown);

        *time = (*time) * *breakdown / (*breakdown + 1);
    }
}

