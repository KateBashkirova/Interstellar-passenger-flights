#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

//количество общих заявок на перелёты на каждой станции (т.е. )
int AldebaranPassengers = 0,
    VegaPassengers = 0,
    TheEarthPassengers = 0,
    SiriusPassengers = 0;
//массив массивов. Здесь в зависимости от номера станции [4] хранятся ячейки-места (второе [4]), кужа будут распределяться пассажиры
//4 станции, на каждой по 3 возможных варианта полёта (1 не считается, потому что на какой-то пассажиры уже находятся)
int StationFlightsApp[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};


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

int Random_number_generator(int min, int max)
{
    LARGE_INTEGER tt;
    QueryPerformanceCounter(&tt);
    srand(tt.LowPart); 
    double fraction = 1.0 / ((double)(RAND_MAX) + 1.0); 
    // Равномерно распределяем рандомное число в нашем диапазоне min/max
    int randomNumber = (int)(rand() * fraction * (max - min + 1) + min);
    return randomNumber;
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

//функция генерации пассажиров на станциях + распределение пассажиров по направлениям полётов
void Passengers(int X, int Y, int departureStationNumber, int y)
{
    char *departureStationName; //указатель на строку с названием станции, с которой совершается отлёт
    departureStationName = (char*)malloc(15); //выделение памяти под хранение этой строки
    switch(departureStationNumber) //в зависимости от номера станции отправления
    {
        case 0:
        departureStationName = "Aldebaran";
        break;
        case 1:
        departureStationName = "Vega";
        break;
        case 2:
        departureStationName = "The Earth";
        break;
        case 3:
        departureStationName = "Sirius";
        break;
        default:
        departureStationName = "Error";
        break;
    }

    //общее количество пассажиров на станции 
    int passengerAmount = 0;
    //делим пассажиров по станциям
    for(int j=0; j<4; j++)
    {
        //если заполняется колонка пассажиров для станции, на которой эти пассажиры находятся в данный момент
        if(j==departureStationNumber) 
        {
            StationFlightsApp[departureStationNumber][j] = 0; //пассажиры не могут поехать на ту же станцию, на которой уже находятся
        }
        else 
        {
            int passengers = Random_number_generator(5,12); //генерируем рандомное кол-во пассажиров, которые куда-то поедут
            StationFlightsApp[departureStationNumber][j] = passengers; //записываем этих пассажиров как желающих поехать на станцию j
            passengerAmount += passengers; //считаем общее кол-во пассажиров
            GoToXY(X,Y);
            printf("Flight app: %d", passengerAmount); //выводим кол-во пассажиров на станции в окошко станции
        }
    }
    
    //просто линия для очерчивания окошка
    for(int i=0; i<84; i++)
    {
        GoToXY(178,i);
        printf("|");
    }

    //выводим информацию о перелётах в окошко
    GoToXY(190,4);
    printf("Interstellar flights information");
    GoToXY(190,y);
    printf("From %s to Aldebaran: %d", departureStationName, StationFlightsApp[departureStationNumber][0]);
    GoToXY(190,y+1);
    printf("From %s to Vega: %d", departureStationName, StationFlightsApp[departureStationNumber][1]);
    GoToXY(190,y+2);
    printf("From %s to The Earth: %d", departureStationName, StationFlightsApp[departureStationNumber][2]);
    GoToXY(190,y+3);
    printf("From %s to Sirius: %d", departureStationName, StationFlightsApp[departureStationNumber][3]);
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
    Passengers(8,9,0,7);
    Spaceships_drawer(25,12); //рисуем корабль
    
    //Станция "Вега"
    GoToXY(130,5); //передвигаем курсор в нужное место
    printf("Vega"); //название станции
    Planetary_stations_drawer(130,8);
    Passengers(133,9,1,14);
    Spaceships_drawer(124,12);

    //Станция "Земля"
    GoToXY(5,50); //передвигаем курсор в нужное место
    printf("The Earth"); //название станции
    Planetary_stations_drawer(5,53);
    Passengers(8,54,2,21);
    Spaceships_drawer(25,57);

    //Станция "Сириус"
    GoToXY(130,50); //передвигаем курсор в нужное место
    printf("Sirius"); //название станции
    Planetary_stations_drawer(130,53);
    Passengers(133,54,3,28);
    Spaceships_drawer(124,57);
}

void main()
{
    system("cls");
    Planetary_stations();
    getchar();
}

