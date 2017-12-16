#include "TXLib.h"


#include <stdio.h>
#include <math.h>

//------------------------------------------------------------

double PI = 3.14159265358979323846;

size_t SIN = 1;
size_t COS = 2;


//------------------------------------------------------------

void DrawCircle (int x , int y , int radius , COLORREF colorIn , COLORREF colorOut);

void DrawPush (int time , double amplitude , int line);

void DrawSinusoidal (int xCord     , int time   , double phase     , double frequency ,
                     int amplitude , int line  , int     breakdown , size_t law);

void ReBuildScreen (int sizeWinX , int sizeWinY , int velLine , int pushLine , int* dotAmpl);

void PrintInfo (double velocity , double amplitude , double phase , size_t sizeWinX , size_t sizeWinY);

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

    printf ("Print own oscillation period\n");
    scanf  ("%d" , &periodTime);

    printf ("Print time between shocks\n");
    scanf  ("%lf" , &pushPeriod);

    printf ("Print amplitude of shock\n");
    scanf  ("%lf" , &pushVelocity);

    printf ("Print breakdown\n");
    scanf  ("%d" , &breakdown);

    printf ("%d %f %f %d\n" , periodTime , pushPeriod , pushVelocity , breakdown);



    scanf ("%lf" , &frequency);
    size_t sizeWinX = 1350;
    size_t sizeWinY = 700;

    HWND main = txCreateWindow(sizeWinX , sizeWinY);

    //Амплитуды
    double amplitude     = 0;
    double lastAmplitude = 0;
    double pushAmplitude = 0;
    double pushAmplUp    = 0;

    //Фазы
    double phase       = 0;
    double lastPhase   = 0;



    if (!periodTime == 0 || !pushAmplitude == 0)
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
    txTextOut(0   , velLine - 110 , "Velocity");
    txLine   (0   , velLine - 100 , sizeWinX , velLine - 100);
    txLine	 (0   , velLine       , sizeWinX , velLine); //Отклонение скорости
    txLine   (0   , velLine + 100 , sizeWinX , velLine + 100);

    //Отклонение
    txTextOut(0   , sizeWinY - 210 , "Deviation");
    txLine   (200 , 0              , 200      , 140); //Положение равновесия (ТЕЛА)
    txLine   (0   , sizeWinY - 100 , sizeWinX , sizeWinY - 100); //Положение равновесия (ТЕЛА)
    txLine   (0   , sizeWinY - 200 , sizeWinX , sizeWinY - 200);

    //Толчки
    int pushLine = sizeWinY - 490;
    txTextOut(0 , pushLine - 60 , "Pushes");
    txLine   (0 , pushLine - 50 , sizeWinX , pushLine - 50);
    txLine   (0 , pushLine + 50 , sizeWinX , pushLine + 50);
    txLine   (0 , pushLine       , sizeWinX , pushLine);
    txLine   (0 , pushLine - 70 , sizeWinX , pushLine - 70);


    txBegin();

    int curPeriod = 0;
    int time = 0;
    for (; !GetAsyncKeyState (VK_ESCAPE); ++time)
    {
        //Рисование

        txSleep (0);
        DrawCircle (200 + (int)(amplitude * sin ((time - 1) * frequency - phase)) ,

                    70 , 30 , TX_BLACK , TX_BLACK);

        //Рисуем положение равновесия
        txSetColor (TX_WHITE);
        txLine (200 , 0 , 200 , 140);


        DrawCircle (200 + (int)(amplitude * sin (time * frequency - phase)) ,
                    70 , 30 , TX_CYAN , TX_BLACK);



        //Рисуем (1) Отклонение от положения равновесия, (2) скорости от времени, (3) толчка
        if (dotAmpl < sizeWinX * breakdown)
        {
            //(1)
            if (amplitude * sin (time               * frequency - phase) <  100 &&
                amplitude * sin (time               * frequency - phase) > -100 &&
                amplitude * sin ((time - breakdown) * frequency - phase) <  100 &&
                amplitude * sin ((time - breakdown) * frequency - phase) > -100 &&
                dotAmpl % breakdown == 0)
            {
                DrawSinusoidal (dotAmpl / breakdown , time            , phase     , frequency ,
                                amplitude           , sizeWinY - 100  , breakdown , SIN);
            }

            //(2)
            if (curVelocity * cos ( time              * frequency - phase) <  100 &&
                curVelocity * cos ( time              * frequency - phase) > -100 &&
                curVelocity * cos ((time - breakdown) * frequency - phase) <  100 &&
                curVelocity * cos ((time - breakdown) * frequency - phase) > -100 &&
                dotAmpl % breakdown == 0)
            {
                DrawSinusoidal (dotAmpl / breakdown , time     , phase     , frequency ,
                                curVelocity         , velLine  , breakdown , COS);
            }

            //(3) вправо
            if(((time % (int)pushPeriod == 0 && pushVelocity * 40 < 50) || GetAsyncKeyState (VK_RIGHT)) &&
               !GetAsyncKeyState (VK_LEFT))
            {
                DrawPush (dotAmpl / breakdown , -pushVelocity , pushLine);
            }
            else	if(time % (int)pushPeriod == 0 && pushVelocity * 40 >= 50)
            {
                DrawPush (dotAmpl / breakdown , -49.0 , pushLine);
            }

            //(3) влево
            if(pushVelocity * 40 < 50 && GetAsyncKeyState (VK_LEFT))
            {
                DrawPush (dotAmpl / breakdown , -pushVelUp , pushLine);
            }
            else	if(time % (int)pushPeriod == 0 && pushVelocity * 40 >= 50)
            {
                DrawPush (dotAmpl / breakdown , 49.0 , pushLine);
            }


            ++dotAmpl;
        }
        else
        {
            ReBuildScreen (sizeWinX , sizeWinY , velLine , pushLine , &dotAmpl);
        }


        //Пишем информацию
        PrintInfo (curVelocity / frequency , amplitude , phase , sizeWinX , sizeWinY);


        //Остановка движения
        if (GetAsyncKeyState (VK_SPACE))
        {
            DrawCircle (200 + (int)(amplitude * sin (time * frequency - phase)) ,
                        70 , 30 , TX_BLACK , TX_BLACK);

            curVelocity = 0;

            phase = 0;

            amplitude = 0;
        }


        while (GetAsyncKeyState ('S'))
            Sleep (50);


        //Математика
        if (time == (10000 * periodTime))
            time = 0;


        //Математика: удар вправо
        if ((time % (int)pushPeriod == 0 || GetAsyncKeyState (VK_RIGHT)) && !GetAsyncKeyState (VK_LEFT))
        {
            curPeriod++;

            lastPhase = phase;
            if (amplitude     * cos(phase)    +
                pushAmplitude * sin(frequency * time) != 0)
            {
                phase     = atan2 ((amplitude     * sin(phase) +
                                    pushAmplitude * sin(frequency * time))
                        ,
                                   (amplitude     * cos(phase) +
                                    pushAmplitude * cos(frequency * time)));
            }
            else
                phase = 0;


            lastAmplitude = amplitude;
            amplitude     = sqrt (lastAmplitude * lastAmplitude +
                                  pushAmplitude * pushAmplitude +
                                  2 * pushAmplitude * lastAmplitude * cos(frequency * time - lastPhase));


            curVelocity  = amplitude;

        }


        //Математика: удар влево
        if (GetAsyncKeyState (VK_LEFT))
        {
            curPeriod++;

            lastPhase = phase;
            if (amplitude  * cos(phase)    +
                pushAmplUp * sin(frequency * time) != 0)
            {
                phase     = atan2 ((amplitude  * sin(phase) +
                                    pushAmplUp * sin(frequency * time))
                        ,
                                   (amplitude  * cos(phase) +
                                    pushAmplUp * cos(frequency * time)));
            }
            else
                phase = 0;



            lastAmplitude = amplitude;
            amplitude     = sqrt (lastAmplitude * lastAmplitude +
                                  pushAmplUp    * pushAmplUp    +
                                  2 * pushAmplUp    * lastAmplitude * cos(frequency * time - lastPhase));


            curVelocity  = amplitude;
        }

    }




    DrawCircle (200 + (int)(amplitude * sin (time * frequency - phase)) ,
                70 , 30 , TX_CYAN , TX_BLACK);

    txEnd();

    return 0;
}

//------------------------------------------------------------
//Рисуем круг

void DrawCircle(int x , int y , int radius , COLORREF colorIn , COLORREF colorOut)
{

    txSetColor(colorOut);
    txSetFillColor(colorIn);
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
                 time     , line - amplitude * 60);
}

//------------------------------------------------------------
// Рисеум синусоиду

void DrawSinusoidal (int xCord     , int time      , double phase , double frequency ,
                     int amplitude , int    line  , int breakdown    , size_t law)
{
    txSetColor(TX_GREEN);

    if (law == COS)
    {
        txLine(xCord - 1 , line + (int) (amplitude *
                                         cos((time - breakdown) * frequency - phase)) ,
               xCord     , line + (int) (amplitude *
                                         cos( time              * frequency - phase)));
    }

    if (law == SIN)
    {
        txLine(xCord - 1 , line + (int) (amplitude *
                                         sin((time - breakdown) * frequency - phase)) ,
               xCord     , line + (int) (amplitude *
                                         sin( time              * frequency - phase)));
    }
}

//------------------------------------------------------------

void ReBuildScreen (int sizeWinX , int sizeWinY , int velLine , int pushLine , int* dotAmpl)
{
    txSetColour    (TX_BLACK);
    txSetFillColour(TX_BLACK);
    txRectangle    (0 , pushLine - 150 , sizeWinX , sizeWinY);

    txSetColour(TX_WHITE);

    //Скорость
    txTextOut(0   , velLine - 110 , "Velocity" );
    txLine   (0   , velLine - 100 , sizeWinX , velLine - 100);
    txLine	 (0   , velLine       , sizeWinX , velLine); //Отклонение скорости
    txLine   (0   , velLine + 100 , sizeWinX , velLine + 100);

    //Отклонение
    txTextOut(0   , sizeWinY - 210 , "Deviation");
    txLine   (200 , 0              , 200      , 140); //Положение равновесия (ТЕЛА)
    txLine   (0   , sizeWinY - 100 , sizeWinX , sizeWinY - 100); //Положение равновесия (ТЕЛА)
    txLine   (0   , sizeWinY - 200 , sizeWinX , sizeWinY - 200);


    //Толчки
    txTextOut(0 , pushLine - 60 , "Pushes");
    txLine   (0 , pushLine - 50 , sizeWinX , pushLine - 50);
    txLine   (0 , pushLine + 50 , sizeWinX , pushLine + 50);
    txLine   (0 , pushLine      , sizeWinX , pushLine);
    txLine   (0 , pushLine - 70 , sizeWinX , pushLine - 70);


    *dotAmpl = 0;
}

//------------------------------------------------------------

void PrintInfo (double velocity , double amplitude , double phase , size_t sizeWinX , size_t sizeWinY)
{
    char velStr[16];
    char ampStr[16];
    char phsStr[16];


    sprintf (velStr , "%.5f" , velocity);
    sprintf (ampStr , "%.5f" , amplitude);
    sprintf (phsStr , "%.5f" , phase);

    txSetColor     (TX_BLACK);
    txSetFillColor (TX_BLACK);
    txRectangle    (sizeWinX - 150 , 0 , sizeWinX , 60);

    txSetColor (TX_WHITE);
    txTextOut  (sizeWinX - 100 , 10  , velStr);
    txTextOut  (sizeWinX - 100 , 30 , ampStr);
    txTextOut  (sizeWinX - 100 , 50 , phsStr);
}
