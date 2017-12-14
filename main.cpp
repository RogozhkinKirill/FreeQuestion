#include "TXLib.h"
#include <stdio.h>
#include <math.h>

//------------------------------------------------------------

double PI = 3.14159265358979323846;

//------------------------------------------------------------

void DrawCircle(int x , int y , int radius , COLORREF colorIn , COLORREF colorOut);

//------------------------------------------------------------

int main ()
{
    HWND main = txCreateWindow(1300 , 700);

    //Собственные время и частоты системы
    int    periodTime = 150;
    double frequency  = 2 * PI / periodTime;

    //Времы между толчками
    double pushPeriod = 150;

    //Скорости
    double pushVelocity  = 0.01;
    double curVelocity   = 0;

    //Амплитуды
    double amplitude     = 0;
    double lastAmplitude = 0;
    double pushAmplitude = pushVelocity / frequency;
    double pushAmplUp    = -pushAmplitude;

    //Фазы
    double phase       = 0;
    double lastPhase   = 0;

    //Текущие значения
    int dotAmpl   = 0;
    int breakdown = 20;

    txLine(200 , 0   , 200  , 140); //Положение равновесия (ТЕЛА)
    txLine(0   , 600 , 1300 , 600); //Положение равновесия (ТЕЛА)
    txLine(0   , 300 , 1300 , 300); //Отклонение скорости
    txLine(0   , 400 , 1300 , 400);

    txLine(0   , 500 , 1300 , 500);


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



        //Рисуем (1) Отклонение от положения равновесия, (2) скорости от времени
        if (dotAmpl < 1300 * breakdown)
        {
            //(1)
            if (amplitude * sin (time               * frequency - phase) <  100 &&
                amplitude * sin (time               * frequency - phase) > -100 &&
                amplitude * sin ((time - breakdown) * frequency - phase) <  100 &&
                amplitude * sin ((time - breakdown) * frequency - phase) > -100 &&
                dotAmpl % breakdown == 0)
            {
                txSetColor(TX_GREEN);
                txLine ( dotAmpl/breakdown - 1 , 600 + (int)(amplitude * sin ((time - breakdown) * frequency - phase)) ,
                         dotAmpl/breakdown     , 600 + (int)(amplitude * sin ( time              * frequency - phase)));
            }

            //(2)
            if (curVelocity * cos ( time              * frequency - phase) <  100 &&
                curVelocity * cos ( time              * frequency - phase) > -100 &&
                curVelocity * cos ((time - breakdown) * frequency - phase) <  100 &&
                curVelocity * cos ((time - breakdown) * frequency - phase) > -100 &&
                dotAmpl % breakdown == 0)
            {
                txSetColor(TX_GREEN);
                txLine ( dotAmpl/breakdown - 1 , 400 + (int)(curVelocity *
                                                             cos ((time - breakdown) * frequency - phase)) ,
                         dotAmpl/breakdown     , 400 + (int)(curVelocity *
                                                             cos ( time              * frequency - phase)));
            }


            ++dotAmpl;
        }
        else
        {
            txSetColour(TX_BLACK);
            txSetFillColour(TX_BLACK);
            txRectangle(0 , 250 , 1300 , 750);

            txSetColour(TX_WHITE);
            txLine(0 , 600 , 1300 , 600);
            txLine(0 , 300 , 1300 , 300);
            txLine(0 , 400 , 1300 , 400);
            txLine(0 , 500 , 1300 , 500);

            dotAmpl = 0;
        }



        //Остановка движения
        if (GetAsyncKeyState (VK_SPACE))
        {
            DrawCircle (200 + (int)(amplitude * sin (time * frequency - phase)) ,
                        70 , 30 , TX_BLACK , TX_BLACK);

            curVelocity  = 0;

            phase     = 0;

            amplitude     = 0;
        }





        //Математика
        if (time == (int)(10000 * periodTime))
            time = 0;


        //Математика удар вправо
        if (time % (int)pushPeriod == 0 || GetAsyncKeyState (VK_RIGHT))
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


            //Амплитуда только для периодических толчков (переделать под общий случай)
            lastAmplitude = amplitude;
            amplitude     = sqrt (lastAmplitude * lastAmplitude +
                                  pushAmplitude * pushAmplitude +
                                  2 * pushAmplitude * lastAmplitude * cos(frequency * time - lastPhase));



            curVelocity  = amplitude;

        }


        //Математика удар влево
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


            curVelocity += curVelocity * cos (time * frequency - lastPhase) + pushVelocity;


            lastAmplitude = amplitude;
            amplitude     = sqrt (lastAmplitude * lastAmplitude +
                                  pushAmplUp    * pushAmplUp    +
                                  2 * pushAmplUp    * lastAmplitude * cos(frequency * time - lastPhase));
        }

    }




    DrawCircle (200 + (int)(amplitude * sin (time * frequency - phase)) ,
                70 , 30 , TX_CYAN , TX_BLACK);

    txEnd();

    return 0;
}

//------------------------------------------------------------

void DrawCircle(int x , int y , int radius , COLORREF colorIn , COLORREF colorOut)
{

    txSetColor(colorOut);
    txSetFillColor(colorIn);
    txCircle (x , y , radius);

    return;
}
