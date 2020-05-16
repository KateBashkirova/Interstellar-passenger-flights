#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

//Функция, отвечающая за перемещение курсора по экрану и вовода текста в нужное место
void GoToXY(const int X,const int Y)
{
    HANDLE OutPutHandle;
    CONSOLE_SCREEN_BUFFER_INFO ScreenBufInfo;
    OutPutHandle=GetStdHandle(STD_OUTPUT_HANDLE); //хэндл вывода на экран
    GetConsoleScreenBufferInfo(OutPutHandle,&ScreenBufInfo);
    ScreenBufInfo.dwCursorPosition.X=X;
    ScreenBufInfo.dwCursorPosition.Y=Y;
    SetConsoleCursorPosition(OutPutHandle,ScreenBufInfo.dwCursorPosition);
}

void Planetary_stations_drawer(int X, int Y)
{
    int station_width = 15;
    char brick = "*"; //из какого символа будут отрисовываться станции
    int i=0,j=0;
    
    for(i=0; i<station_width; i++)
    {
        GoToXY(X,Y);
        printf("*");
        X++;
    }
    for(i=station_width+1; i<station_width*2; i++)
    {
        GoToXY(X,Y);
        printf("*");
        Y++;
    }
    for (i=(station_width*2)+1; i<=station_width*3; i++)
    {
        GoToXY(X,Y);
        printf("*");
        X--;
    }
    for(i=(station_width*3)+1; i<=station_width*4; i++)
    {
        GoToXY(X,Y);
        printf("*");
        Y--;
    } 
}

void Planetary_stations()
{
    //Станция "Альдебаран"
    GoToXY(5,5); //передвигаем курсор в нужное место
    printf("Aldebaran"); //название станции
    Planetary_stations_drawer(5,8);

    //Станция "Вега"
    GoToXY(130,5); //передвигаем курсор в нужное место
    printf("Vega"); //название станции
    Planetary_stations_drawer(130,8);

    //Станция "Земля"
    GoToXY(5,50); //передвигаем курсор в нужное место
    printf("The Earth"); //название станции
    Planetary_stations_drawer(5,53);

    //Станция "Сириус"
    GoToXY(130,50); //передвигаем курсор в нужное место
    printf("Sirius"); //название станции
    Planetary_stations_drawer(130,53);
}

void main()
{
    system("cls");
    Planetary_stations();
    getchar();
}