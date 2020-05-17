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

//функция отрисовки окошка с информацией о полётах
void Flights_info_drawer()
{   
    for(int i=0; i<84; i++)
    {
        GoToXY(178,i);
        printf("|");
    }
    GoToXY(190,8);
    printf("Interstellar flights information");
}

//функция отрисовки кораблей
void Spaceships_drawer(int X, int Y)
{
    const char body[5][5] = { "#####",
                              "#   #",
                              "#   #",
                              "#   #",
                              "#####"};
    for(int i=0; i<5; i++)
    {
        for(int j=0; j<5; j++)
        {   
            GoToXY(X+i,Y+j);
            printf("%c", body[i][j]);        
        }
    }
}

//TODO: генерация человечков на станциях
void Passengers(int X, int Y)
{
    LARGE_INTEGER tt;
    QueryPerformanceCounter(&tt);
    srand(tt.LowPart); 
    int min = 3, max = 30; 
    int diff = max-min;
    int passengerAmount = rand()%diff;
    GoToXY(X,Y);
    printf("Flight app: %d", passengerAmount); //выводим количество заявок на перелёты с этой станции
}

//функция отрисовки станций
void Planetary_stations_drawer(int X, int Y)
{
    int station_width = 18;
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

//сами станции (здесь собирается изначальное состояние мира)
void Planetary_stations()
{
    //Станция "Альдебаран"
    GoToXY(5,5); //передвигаем курсор в нужное место
    printf("Aldebaran"); //название станции
    Planetary_stations_drawer(5,8); //рисуем станцию
    Passengers(8,9);
    Spaceships_drawer(25,12); //рисуем корабль
    
    //Станция "Вега"
    GoToXY(130,5); //передвигаем курсор в нужное место
    printf("Vega"); //название станции
    Planetary_stations_drawer(130,8);
    Passengers(133,9);
    Spaceships_drawer(124,12);
    

    //Станция "Земля"
    GoToXY(5,50); //передвигаем курсор в нужное место
    printf("The Earth"); //название станции
    Planetary_stations_drawer(5,53);
    Passengers(8,54);
    Spaceships_drawer(25,57);
    

    //Станция "Сириус"
    GoToXY(130,50); //передвигаем курсор в нужное место
    printf("Sirius"); //название станции
    Planetary_stations_drawer(130,53);
    Passengers(133,54);
    Spaceships_drawer(124,57);
    
}

void main()
{
    system("cls");
    Planetary_stations();
    Flights_info_drawer();
    getchar();
}

