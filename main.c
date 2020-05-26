#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

//массив, в котором хранится количество пассажиров, прибывших на каждую станцию. Номер ячейки = номеру станции. Значение ячейки = количеству прибывших на эту станцию пассажиров
int ArrivedPassengers[4] = {0,0,0,0};

//массив массивов. Используется при генерации заявок на полёты. Номер {} ячейки = номеру станции, для которой происходит генерация заявок. 
//Номер ячейки внутри большой ячейки {} = номеру станции, на которую полетят люди со станции в большой ячейке {}. 
//Значение ячейки = количество пассажиров, которые полетят со станции {} на станцию.
int StationFlightsApp[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
//координаты доков каждой станции
int StationCOORD[4][2] = {{16,12}, {38,12}, {16,30}, {38,30}};
//Общее кол-во пассажиров на всех станциях
int passengerAmount = 0;
//курс движения каждого из кораблей
int shipRoute[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
//свободные места в каждом из кораблей
int freeSeats[4] = {4,4,4,4};

//хэндлы
HANDLE hship[4]; //корабли
DWORD shipThreadID[4]; //id кораблей
HANDLE hSem[4],hMtx,hSemSt;

HANDLE hWrMtxSt[4]; //хэндлы станций на запись
HANDLE hReadMtxSt[4]; //хэндлы станций на чтение

int N;
CRITICAL_SECTION csec;

//Добавочные функции
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
//функция генерации случайного числа в заданном диапазоне значений
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

//Функции-рисовальщики
//функция отрисовки станций
void Planetary_stations_drawer(int X, int Y)
{
    int station_width = 9;
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

//функция начальной отрисовки кораблей в доках своих станций
void Spaceships_drawer(int X, int Y)
{
    char *body = "B";
    //если нужно нарисовать кораблик
    GoToXY(X,Y);
    printf("%s", body);      
}

//функция отрисовки движения кораблей
void SpaceshipMovementDrawer(int departureStationCoordX, int departureStationCoordY, int arrivalStationCoordX, int arrivalStationCoordY)
{   
    //тело корабля
    char *body = "*";
    char *bodyDeliter = " ";
    char *bay = "B";

    //далее идёт проверка условий для перемещения корабля по пространству
    //если станции находятся на одном уровне по Y
    if(departureStationCoordY == arrivalStationCoordY)
    {
        //если станция отбытия левее станции прибытия
        if(departureStationCoordX < arrivalStationCoordX)
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordX; i<arrivalStationCoordX; i++)
            {
                //перемещаемся в нужную точку
                GoToXY(departureStationCoordX, departureStationCoordY);
                //стираем то, что в ней было
                printf("%s", bodyDeliter);
                departureStationCoordX++;
                //перемещаемся дальше
                GoToXY(departureStationCoordX, departureStationCoordY);
                //рисуем тело корабля
                printf("%s", body);
                //задержка для создания эффекта "движения"
                Sleep(80);
            }  
        }
        GoToXY(arrivalStationCoordX, arrivalStationCoordY);
        printf("%s", bay);

        //если станция отбытия правее станции прибытия
        if(departureStationCoordX > arrivalStationCoordX)
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordX; i > arrivalStationCoordX; i--)
            {
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", bodyDeliter);
                departureStationCoordX--;
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", body);
                Sleep(80);
            }  
        }
        GoToXY(arrivalStationCoordX, arrivalStationCoordY);
        printf("%s", bay);
    } 
    
    //если станции находятся на одном уровне по Х
    if(departureStationCoordX == arrivalStationCoordX)
    {
        //если станция отбытия выше станции прибытия
        if(departureStationCoordY < arrivalStationCoordY)
        {
           //пока координаты не совпадут
            for(int i = departureStationCoordY; i < arrivalStationCoordY; i++)
            {
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", bodyDeliter);
                departureStationCoordY++;
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", body);
                Sleep(80);
            }
        }
        //если станция отбытия ниже станции прибытия
        else
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordY; i>arrivalStationCoordY; i--)
            {
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", bodyDeliter);
                departureStationCoordY--;
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", body);
                Sleep(80);
            }  
        }
        GoToXY(arrivalStationCoordX, arrivalStationCoordY);
        printf("%s", bay);
    }
    
    //Внештатные ситуации. Движение корабля по этим маршрутам происходит по диагонали
    //маршрут Вега - Земля
    if(departureStationCoordX == 38 && departureStationCoordY == 12 && arrivalStationCoordX == 16 && arrivalStationCoordY == 30)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX -= 3.5;
            departureStationCoordY += 3;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(100);
        }
    }
    GoToXY(arrivalStationCoordX, arrivalStationCoordY);
    printf("%s", bay);
    //маршрут Земля - Вега
    if(departureStationCoordX == 16 && departureStationCoordY == 30 && arrivalStationCoordX == 38 && arrivalStationCoordY == 12)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX += 4;
            departureStationCoordY -= 2.1;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(100);
        }
    }
    GoToXY(arrivalStationCoordX, arrivalStationCoordY);
    printf("%s", bay);
    //Альдебаран - Сириус
    if(departureStationCoordX == 16 && departureStationCoordY == 12 && arrivalStationCoordX == 38 && arrivalStationCoordY == 30)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX += 4;
            departureStationCoordY += 3;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(100);
        }
    }
    //Сириус - Альдебаран
    else 
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX -= 4;
            departureStationCoordY -= 2.1;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(100);
        }
    }
    GoToXY(arrivalStationCoordX, arrivalStationCoordY);
    printf("%s", bay); 
}

void CreateRoute(int departureStationNumber, int shipNumber)
{
    int i=0, k=0;
    //ищет, хочет ли кто-то полететь с текущей станции хоть куда-нибудь
    for(i=0; i<4; i++)
    {
        //проверяет, есть ли хоть 1 пассажир на направление i
        if(StationFlightsApp[departureStationNumber][i] != 0)
        {
            //проверяем, остались ли места в салоне
            if(freeSeats[shipNumber] != 0)
            {
                //следующий цикл реализует следующее: поскольку на 1 направление может быть несколько пассажиров (в т.ч. 4 и более) мы усаживаем в корабль столько человек, сколько хотело
                //на это направление, но не больше 4-х за раз. Если их было 2 - садим сразу 2-х, если 1 - садим 1 и смотрим следующие направления. Этот цикл реализован, чтобы корабль полностью 
                //использовал свой потенциал и "по максимуму" нагружал в себя пассажиров

                //поскольку на 1 направление может быть несколько человек
                for(int j=0; j<4; j++)
                {
                    //если пассажиры на данное направление не закончились
                    if(StationFlightsApp[departureStationNumber][i] != 0)
                    {
                        if(freeSeats[shipNumber] != 0)
                        {
                            freeSeats[shipNumber] -= (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); //пассажиры занимают места
                            StationFlightsApp[departureStationNumber][i] -= (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); //севшие на корабль пассажиры больше не считаются пассажирами, ожидающими перелёт
                            shipRoute[shipNumber][i] += (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); //заполняем маршрут корабля
                        }
                    } 
                }
            }
        }
    }
}

//функция, отвечающая за отрисовку логики передвижения кораблей. В ней вызывается функция перемещения, пока маршрут не будет завершён
int SpaceshipMovementLogic(int departureStationNumber, int shipNumber)
{
    //координаты станции, на которой корабль находится в данный момент
    int currentStation[2] = {0,0};
    currentStation[0] = StationCOORD[departureStationNumber][0]; //X
    currentStation[1] = StationCOORD[departureStationNumber][1]; //Y 
    int i=0;
    int currentStationNumber;

    GoToXY(57,40);
    printf("                     ");
    GoToXY(57,40);
    printf("Seats free: %d", freeSeats[shipNumber]);
    Sleep(2000); 

    if(passengerAmount > 0)
    {
        while(i<4)
        {
            if(shipRoute[shipNumber][i] > 0) 
            {
                SpaceshipMovementDrawer(currentStation[0], currentStation[1], StationCOORD[i][0], StationCOORD[i][1]); //летим
                        
                    //прилетели на новую станцию
                        
                    ArrivedPassengers[i] += shipRoute[shipNumber][i]; //прибавляем новых пассажиров к кол-ву прибывших на станцию
                    freeSeats[shipNumber] += shipRoute[shipNumber][i]; //кол-во свободных мест увеличивается на число высадившихся пассажиров
                    passengerAmount -= shipRoute[shipNumber][i]; //от общего числа неразвезённых пассажиров отнимаем тех, кто уже прилетел, куда хотел
                    shipRoute[shipNumber][i] -= shipRoute[shipNumber][i]; //высаживаем пассажиров на станции

                    //выводим надпись о прибывших пассажирах
                    if(StationCOORD[i][0] == 16) //условие для корректного отображения для станций Альдебаран и Земля
                    {
                        GoToXY(StationCOORD[i][0]-16, StationCOORD[i][1]+7);
                        printf("Arr: %d", ArrivedPassengers[i]);
                    }
                    else
                    {
                        GoToXY(StationCOORD[i][0]+3, StationCOORD[i][1]+7);
                        printf("Arr: %d", ArrivedPassengers[i]);
                    }

                    //отладка
                    GoToXY(57,38);
                    printf("                     ");
                    GoToXY(57,38);
                    printf("Passengers left: %d", passengerAmount);
                    Sleep(100);

                    GoToXY(57,42);
                    printf("                     ");
                    GoToXY(57,42);
                    printf("Dropped off. Seats free: %d", freeSeats[shipNumber]);
                    Sleep(2000); 

                    GoToXY(57,44);
                    printf("                     ");
                    GoToXY(57,44);
                    printf("New pass in. Seats free: %d", freeSeats[shipNumber]);
                    Sleep(2000); 

                    //меняем новые координаты отправления
                    currentStation[0] = StationCOORD[i][0];
                    currentStation[1] = StationCOORD[i][1];

                    //идентифицируем, куда прилетели
                    switch(currentStation[0]) //глядим по Х переменной
                    {
                        //если станция прибытия имеет координату Х = 16
                        case 16:
                        //если её координата Y == 12 - это Альдебаран (ст. 0)
                        if(currentStation[1] == 12) currentStationNumber = 0;
                        //если координата Y == 30 - это Земля (ст. 2)
                        if(currentStation[1] == 30) currentStationNumber = 2;
                        break;
                        //если станция прибытия имеет координату Х = 38
                        case 38:
                        //если её координата Y == 12 - это Вега (ст. 1)
                        if(currentStation[1] == 12) currentStationNumber = 1;
                        //если координата Y == 30 - это Сириус (ст. 3)
                        if(currentStation[1] == 30) currentStationNumber = 3;
                        break;
                    }

                    //если на корабле появились свободные места
                    if(freeSeats[shipNumber] > 0)
                    {
                        for(int k=0; k<4; k++)
                        {
                            if(freeSeats[shipNumber] > 0)
                            {
                                if(StationFlightsApp[currentStationNumber][k] != 0)
                                {
                                    shipRoute[shipNumber][k] += (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                                    StationFlightsApp[currentStationNumber][k] -= (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                                    freeSeats[shipNumber] -= (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                                }
                            }
                        }
                    }
            }
            i++;
        }
    }
    int currSt = currentStationNumber;
}

void TheEnd()
{
    GoToXY(10,22);
    printf("All passengers have been transported!");
    Sleep(15000);
    //exit(0);
}

DWORD Spaceship(LPVOID station)
{
    //вызываем функцию формирования маршрута
    CreateRoute(*(DWORD*)station, *(DWORD*)station);

    int success = 5;
    success = SpaceshipMovementLogic(*(DWORD*)station, *(DWORD*)station);

    if(success != 5)
    {
        while(passengerAmount != 0)
        {
            success = SpaceshipMovementLogic(success, *(DWORD*)station);
        }  
    }
    TheEnd(); 
    return 0;
}
//Логические функции
//функция генерации пассажиров на станциях + распределение пассажиров по направлениям полётов
void Passengers(int X, int Y, int departureStationNumber, int y)
{
    //общее количество пассажиров на конкретной станции
    int passengersOnStation = 0;
    char *departureStationName; //указатель на строку с названием станции, с которой совершается отлёт
    departureStationName = (char*)malloc(15); //выделение памяти под хранение этой строки
    switch(departureStationNumber) //в зависимости от номера станции отправления
    {
        case 0:
        departureStationName = "Aldebaran"; //присваиваем название станции пришедшему в функцию номеру станции
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
            int passengers = Random_number_generator(1,3); //генерируем рандомное кол-во пассажиров, которые куда-то поедут
            StationFlightsApp[departureStationNumber][j] = passengers; //записываем этих пассажиров как желающих поехать на станцию j
            passengerAmount += passengers; //считаем общее кол-во пассажиров
            passengersOnStation += passengers; //считаем общее кол-во пассажиров на станции
        }
        GoToXY(X,Y);
        printf("app: %d", passengersOnStation); //выводим общее кол-во пассажиров на станции в окошко станции
    }
    GoToXY(57,37);
    printf("Passenger amount: %d", passengerAmount);

    //просто линия для очерчивания окошка
    for(int i=0; i<40; i++)
    {
        GoToXY(55,i);
        printf("|");
    }
    //выводим информацию о перелётах в окошко
    GoToXY(57,4);
    printf("Interstellar flights information");
    GoToXY(57,y);
    printf("From %s to Aldebaran: %d", departureStationName, StationFlightsApp[departureStationNumber][0]);
    GoToXY(57,y+1);
    printf("From %s to Vega: %d", departureStationName, StationFlightsApp[departureStationNumber][1]);
    GoToXY(57,y+2);
    printf("From %s to The Earth: %d", departureStationName, StationFlightsApp[departureStationNumber][2]);
    GoToXY(57,y+3);
    printf("From %s to Sirius: %d", departureStationName, StationFlightsApp[departureStationNumber][3]);
}
    
void Planetary_stations()
{
    //Станция "Альдебаран"
    GoToXY(5,5); //передвигаем курсор в нужное место
    printf("Aldebaran"); //название станции
    Planetary_stations_drawer(5,8); //рисуем станцию
    Passengers(7,9,0,7); //генерируем пассажиров
    Spaceships_drawer(16,12); //рисуем корабль

    //Станция "Вега"
    GoToXY(40,5); //передвигаем курсор в нужное место
    printf("Vega"); //название станции
    Planetary_stations_drawer(40,8); //рисуем станцию
    Passengers(42,9,1,14); //генерируем пассажиров
    Spaceships_drawer(38,12); //рисуем корабль

    //Станция "Земля"
    GoToXY(5,25); //передвигаем курсор в нужное место
    printf("The Earth"); //название станции
    Planetary_stations_drawer(5,28); //рисуем станцию
    Passengers(7,29,2,21); //генерируем пассажиров
    Spaceships_drawer(16,30); //рисуем корабль

    //Станция "Сириус"
    GoToXY(40,25); //передвигаем курсор в нужное место
    printf("Sirius"); //название станции
    Planetary_stations_drawer(40,28); //рисуем станцию
    Passengers(42,29,3,28); //генерируем пассажиров
    Spaceships_drawer(38,30); //рисуем корабль
} 

void main()
{
    system("cls");
    Planetary_stations();
    //hSem = CreateSemaphore(NULL, 1, 1, "writing");//создание семафора
    hMtx = CreateMutex(NULL, FALSE, NULL); //создаём мьютекс для записи
    
    for(int i=0; i<4; i++)
    {
        hWrMtxSt[i] = CreateMutex(NULL, FALSE, NULL); 
        //hReadMtxSt[i] = CreateMutex(NULL, FALSE, NULL); 
        hSem[i] = CreateSemaphore(NULL, 1, 1, "writing");//создание семафора
    }

    //генерация нитей с кораблями
    DWORD shipNumber[4] = {0,1,2,3};
    for(int j=0; j<1; j++)
    {
        hship[j] = CreateThread(NULL, 0, Spaceship, &shipNumber[j], 0, &shipThreadID[j]);
    } 
    getchar();
}

