#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

//массив, в котором хранится количество пассажиров, прибывших на каждую станцию. Номер ячейки = номеру станции. Значение ячейки = количеству прибывших на эту станцию пассажиров.
int ArrivedPassengers[4] = {0,0,0,0};

//массив, использующийся при генерации заявок на полёты. Номер ячейки = номеру станции, для которой происходит генерация заявок. 
//Далее - номер станции, на которую полетят пассажиры. Значение ячейки = количеству пассажиров, которые полетят с одной станции на другую.
int StationFlightsApp[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

//координаты доков каждой станции
int StationCOORD[4][2] = {{16,12}, {38,12}, {16,30}, {38,30}};

//общее кол-во пассажиров на всех станциях
int PassengerAmount = 0;

//курс движения каждого из кораблей
int ShipRoute[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

//свободные места в каждом из кораблей
int FreeSeats[4] = {4,4,4,4};

//хэндлы
HANDLE hShip[4]; //хэндлы кораблей
unsigned long int  shipThreadID[4]; //id кораблей
HANDLE hPassengersOnStations[4]; //хэндлы пассажиров
unsigned long int  passengersOnStationsID[4]; //id пассажиров

HANDLE hMtxSt[4]; //хэндлы мьютексов для работы с информацией о пассажирах на каждой станции
HANDLE hMtxPass[4];
HANDLE hMtxWr;
HANDLE hMtxWrGTNV;

//Функция, отвечающая за перемещение курсора по экрану в нужное место
void GoToXY(const int X,const int Y, char *info)
{
    WaitForSingleObject(hMtxWr, INFINITE);
    HANDLE OutPutHandle;
    CONSOLE_SCREEN_BUFFER_INFO ScreenBufInfo;
    OutPutHandle=GetStdHandle(STD_OUTPUT_HANDLE); 
    GetConsoleScreenBufferInfo(OutPutHandle,&ScreenBufInfo);
    ScreenBufInfo.dwCursorPosition.X=X;
    ScreenBufInfo.dwCursorPosition.Y=Y;
    SetConsoleCursorPosition(OutPutHandle,ScreenBufInfo.dwCursorPosition);
    printf("%s", info);
    ReleaseMutex(hMtxWr);
}

void GoToXYNormalVersion(const int X,const int Y)
{ 
    WaitForSingleObject(hMtxWrGTNV, INFINITE);
    HANDLE OutPutHandle;
    CONSOLE_SCREEN_BUFFER_INFO ScreenBufInfo;
    OutPutHandle=GetStdHandle(STD_OUTPUT_HANDLE); 
    GetConsoleScreenBufferInfo(OutPutHandle,&ScreenBufInfo);
    ScreenBufInfo.dwCursorPosition.X=X;
    ScreenBufInfo.dwCursorPosition.Y=Y;
    SetConsoleCursorPosition(OutPutHandle,ScreenBufInfo.dwCursorPosition);
    ReleaseMutex(hMtxWrGTNV);
}

void ConvertIntToCharAndPrint(char *info, int arg, const int X, const int Y)
{
    char buffer[40];
    sprintf(buffer, "%d", arg); //конвертировать в строку, записать в буфер
    strncat (info, buffer,7); //объединить конвертированное число и строку. Число записать с точностью до 7-ми знаков
    GoToXY(X,Y, info); //вызываем функцию вывода объединённой строки по нужным координатам
}

//Функция генерации случайного числа в заданном диапазоне значений
int RandomNumberGenerator(int min, int max)
{
    LARGE_INTEGER tt;
    QueryPerformanceCounter(&tt); //извлекает текущее значение счётчика высокого разрешения
    srand(tt.LowPart); //используем младшую часть возвращаемого значения
    double fraction = 1.0 / ((double)(RAND_MAX) + 1.0); 
    //равномерно распределяем случайное число в диапазоне min/max
    int randomNumber = (int)(rand() * fraction * (max - min + 1) + min);
    return randomNumber;
}

//Функция отрисовки станций
void PlanetaryStationsDrawer(int X, int Y)
{
    int station_width = 9; //ширина станции
    int i=0,j=0;
    //отрисовка прямоугольника станции
    for(i=0; i<station_width; i++)
    {
        GoToXYNormalVersion(X,Y);
        printf("*");
        X++;
    }
    for(i=station_width+1; i<station_width*2; i++)
    {
        GoToXYNormalVersion(X,Y);
        printf("*");
        Y++;
    }
    for (i=(station_width*2)+1; i<=station_width*3; i++)
    {
        GoToXYNormalVersion(X,Y);
        printf("*");
        X--;
    }
    for(i=(station_width*3)+1; i<=station_width*4; i++)
    {
        GoToXYNormalVersion(X,Y);
        printf("*");
        Y--;
    } 
}

//Функция начальной отрисовки кораблей в доках своих станций
void SpaceshipsDrawer(int X, int Y)
{
    char body[] = "B"; //тело корабля
    GoToXYNormalVersion(X,Y);  
    printf("B");      
}

//Функция отрисовки движения кораблей
void SpaceshipMovementDrawer(int departureStationCoordX, int departureStationCoordY, int arrivalStationCoordX, int arrivalStationCoordY)
{   
    char body[] = "*";
    char bodyDeliter[] = " ";

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
                GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter); //перемещаемся в нужную точку
                departureStationCoordX++; //увеличиваем координату
                GoToXY(departureStationCoordX, departureStationCoordY, body); //перемещаемся по новым координатам
                Sleep(100); //задержка для создания эффекта движения
            }  
        }
        //если станция отбытия правее станции прибытия
        if(departureStationCoordX > arrivalStationCoordX)
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordX; i > arrivalStationCoordX; i--)
            {
                GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
                departureStationCoordX--;
                GoToXY(departureStationCoordX, departureStationCoordY, body);
                Sleep(100);
            } 
        }
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
                GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
                departureStationCoordY++;
                GoToXY(departureStationCoordX, departureStationCoordY, body);
                Sleep(100);
            }
        }
        //если станция отбытия ниже станции прибытия
        else
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordY; i>arrivalStationCoordY; i--)
            {
                GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
                departureStationCoordY--;
                GoToXY(departureStationCoordX, departureStationCoordY, body);
                Sleep(100);
            }  
        }   
    }

    //Внештатные ситуации. Движение корабля по этим маршрутам происходит по диагонали
    //маршрут Вега - Земля
    if(departureStationCoordX == 38 && departureStationCoordY == 12 && arrivalStationCoordX == 16 && arrivalStationCoordY == 30)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
            departureStationCoordX -= 3.5;
            departureStationCoordY += 3;
            GoToXY(departureStationCoordX, departureStationCoordY, body);
            Sleep(150);
        }
    }
    //маршрут Земля - Вега
    if(departureStationCoordX == 16 && departureStationCoordY == 30 && arrivalStationCoordX == 38 && arrivalStationCoordY == 12)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
            departureStationCoordX += 4;
            departureStationCoordY -= 2.1;
            GoToXY(departureStationCoordX, departureStationCoordY, body);
            Sleep(150);
        }
    }
    //маршрут Альдебаран - Сириус
    if(departureStationCoordX == 16 && departureStationCoordY == 12 && arrivalStationCoordX == 38 && arrivalStationCoordY == 30)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
            departureStationCoordX += 4;
            departureStationCoordY += 3;
            GoToXY(departureStationCoordX, departureStationCoordY, body);
            Sleep(150);
        }
    }
    //маршрут Сириус - Альдебаран
    else 
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY, bodyDeliter);
            departureStationCoordX -= 4;
            departureStationCoordY -= 2.1;
            GoToXY(departureStationCoordX, departureStationCoordY, body);
            Sleep(150);
        }
    }
}

//Функция генерации маршрута корабля
void CreateRoute(int departureStationNumber, int shipNumber)
{
    int i=0;
    WaitForSingleObject(hMtxSt[shipNumber], INFINITE); //захватываем мьютекс для работы с информацией о пассажирах на станции
    //для 4-х возможных направлений
    for(i=0; i<4; i++)
    {
        //если на текущей станции есть хоть один человек, желающий полететь на направление i
        if(StationFlightsApp[departureStationNumber][i] != 0)
        {
            //проверяем, остались ли места на корабле
            if(FreeSeats[shipNumber] != 0)
            {                
                //поскольку на 1 направление может быть несколько человек, максимально нагружаем корабль
                for(int j=0; j<4; j++)
                {
                    //если пассажиры на данное направление не закончились
                    if(StationFlightsApp[departureStationNumber][i] != 0)
                    {
                        //если остались места
                        if(FreeSeats[shipNumber] != 0)
                        {
                            //пассажиры занимают места
                            FreeSeats[shipNumber] -= (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); 
                            //севшие на корабль пассажиры больше не считаются пассажирами, ожидающими перелёт
                            StationFlightsApp[departureStationNumber][i] -= (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); 
                            //заполняем маршрут корабля в соответствии с заявками севших в салон пассажиров
                            ShipRoute[shipNumber][i] += (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); 
                        }
                    } 
                }
            }
        }
    }
    ReleaseMutex(hMtxSt[shipNumber]); //освобождаем мьютекс 
}

//Функция, отвечающая за логику передвижения кораблей и изменение их маршрутов в соответствии с садящимися на борт пассажирами
int SpaceshipMovementLogic(int departureStationNumber, int shipNumber)
{
    //координаты станции, на которой корабль находится в данный момент
    int currentStation[2] = {0,0};
    currentStation[0] = StationCOORD[departureStationNumber][0]; //X
    currentStation[1] = StationCOORD[departureStationNumber][1]; //Y 
    int i=0;
    int currentStationNumber; //номер станции, на которой корабль находится в данный момент

    //если ещё не все подавшие заявку на полёт пассажиры развезены по нужным станциям
    if(PassengerAmount > 0)
    {
        i = RandomNumberGenerator(0,3); //выбирается случайная станция, на которую полетит корабль
        SpaceshipMovementDrawer(currentStation[0], currentStation[1], StationCOORD[i][0], StationCOORD[i][1]); //корабль летит на эту станцию
         
        //по прибытии на новую станцию
        WaitForSingleObject(hMtxSt[shipNumber], INFINITE); //захватываем мьютекс для работы с информацией о пассажирах на станции
        //идентифицируем, на какую станцию прилетел корабль
        switch(currentStation[0]) //смотрим по координате X
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
        WaitForSingleObject(hMtxPass[currentStationNumber], INFINITE);
        ArrivedPassengers[i] += ShipRoute[shipNumber][i]; //прибавляем новых пассажиров к кол-ву прибывших на станцию
        FreeSeats[shipNumber] += ShipRoute[shipNumber][i]; //кол-во свободных мест на корабле увеличивается на число высадившихся пассажиров
        PassengerAmount -= ShipRoute[shipNumber][i]; //от общего числа неразвезённых пассажиров отнимаем тех, кто уже прилетел, куда хотел
        ShipRoute[shipNumber][i] -= ShipRoute[shipNumber][i]; //высаживаем пассажиров на станции

        char info[] = "Arr: ";
        char deliter[] = "         ";
        if(StationCOORD[i][0] == 16) //условие для корректного отображения для станций Альдебаран и Земля
        {
            GoToXY(StationCOORD[i][0]-16, StationCOORD[i][1]+7, deliter);
            ConvertIntToCharAndPrint(info, ArrivedPassengers[i], StationCOORD[i][0]-16, StationCOORD[i][1]+7);
        }
        else
        {
            GoToXY(StationCOORD[i][0]+3, StationCOORD[i][1]+7, deliter);
            ConvertIntToCharAndPrint(info, ArrivedPassengers[i], StationCOORD[i][0]+3, StationCOORD[i][1]+7);
        }

        //устанавливаем новые координаты отправления (поскольку теперь корабль отправляется с новой станции, а не со своей начальной)
        currentStation[0] = StationCOORD[i][0];
        currentStation[1] = StationCOORD[i][1];

        //если после высадки пассажиров на корабле появились свободные места
        if(FreeSeats[shipNumber] > 0)
        {
            for(int k=0; k<4; k++) //для 4-х возможных направлений
            {
                if(FreeSeats[shipNumber] > 0) //если всё ещё не все места заняты
                {
                    if(StationFlightsApp[currentStationNumber][k] != 0) //если на текущей станции есть хоть 1 пассажир на направление i
                    {
                        //добавляем заявку этого пассажира в текущий маршрут корабля
                        ShipRoute[shipNumber][k] += (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                        //севший на корабль пассажир больше не считается пассажиром, ожидающим перелёт
                        StationFlightsApp[currentStationNumber][k] -= (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                        //пассажир занимает место
                        FreeSeats[shipNumber] -= (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                    }
                }
            }
        }
    }
    ReleaseMutex(hMtxPass[currentStationNumber]); 
    ReleaseMutex(hMtxSt[shipNumber]); //освобождение мьютекса
    int currSt = currentStationNumber; //номер станции, на которой находится корабль (на которую он прилетел)
    return currSt; 
}

//Функция, отвечающая за вызов перемещения корабля со станции на станцию
void Spaceship (void* station)
{
    //вызываем функцию формирования маршрута
    CreateRoute(*(unsigned long int*)station, *(unsigned long int*)station);

    int newStation = 5;
    newStation = SpaceshipMovementLogic(*(unsigned long int*)station, *(unsigned long int*)station); //отправляем корабль в путь
    if(newStation != 5) //если первый перелёт корабля прошёл успешно, и он прибыл на какую-либо станцию
    {
        //то пока не развезены все подавшие заявку на перелёт пассажиры
        while(PassengerAmount != 0)
        {
            //корабль будет летать со станции на станцию
            newStation = SpaceshipMovementLogic(newStation, *(unsigned long int*)station);
        }  
    }

    //когда все пассажиры будут развезены
    char info[] = "All passengers have been transported!";
    GoToXY(10,22, info); //сообщить об этом
    Sleep(15000);
    exit(0);
}

//Функция, генерирующая количество пассажиров на станциях и заявки на полёты
void CreatePassengers (void* station)
{
    int passengersOnStation = 0; //количество пассажиров на станции
    char *departureStationName; //указатель на строку с названием станции, с которой совершается вылет
    departureStationName = (char*)malloc(15); //выделение памяти под хранение этой строки
    
    //генерируем количество пассажиров и заявки на полёты
    for(int j=0; j<4; j++) //для всех 4-х направлений
    {
        //если заполняется ячейка, отвечающая за заявки на перелёт на ту станцию, на которой пассажиры находятся в данный момент
        if(j==*(unsigned long int*)station) 
        {
            StationFlightsApp[*(unsigned long int*)station][j] = 0; //значение ячейки приравнивается к 0, т.к. пассажиры не могут полететь на ту станцию, на которой уже находятся
        }
        else 
        {
            int passengers = RandomNumberGenerator(2,12); //генерируем случайное кол-во пассажиров, которые куда-то полетят
            StationFlightsApp[*(unsigned long int*)station][j] = passengers; //записываем этих пассажиров как желающих полететь на станцию j
            PassengerAmount += passengers; //считаем общее кол-во пассажиров, подавших заявки
            passengersOnStation += passengers; //считаем кол-во пассажиров, подавших заявки конкретно с этой станции
        }
    }

    switch(*(unsigned long int*)station) //в зависимости от номера станции отправления
    {
        case 0:
        departureStationName = "From Aldebaran"; //узнаём название станции отправления
        break;
        case 1:
        departureStationName = "From Vega";
        break;
        case 2:
        departureStationName = "From The Earth";
        break;
        case 3:
        departureStationName = "From Sirius";
        break;
        default:
        departureStationName = "Error";
        break;
    }

    //выводим общее количество пассажиров, подавших заявки на перелёт
    char info[] = "Total passenger amount: ";
    ConvertIntToCharAndPrint(info, PassengerAmount, 57, 30);
    //просто линия для очерчивания окна
    for(int i=0; i<40; i++)
    {
        char info[] = "|";
        GoToXY(55,i,info);
    }

    //выводим информацию о перелётах 
    char headline[] = "Interstellar flights information";
    GoToXY(57,4,headline);
    int y=6+*(unsigned long int*)station; int multiplier=*(unsigned long int*)station; //для корректного отображения списка
    GoToXYNormalVersion(57,y+1+(multiplier*4));
    printf("From %s to Aldebaran: %d", departureStationName, StationFlightsApp[*(unsigned long int*)station][0]);
    GoToXYNormalVersion(57,y+2+(multiplier*4));
    printf("From %s to Vega: %d", departureStationName, StationFlightsApp[*(unsigned long int*)station][1]);
    GoToXYNormalVersion(57,y+3+(multiplier*4));
    printf("From %s to The Earth: %d", departureStationName, StationFlightsApp[*(unsigned long int*)station][2]);
    GoToXYNormalVersion(57,y+4+(multiplier*4));
    printf("From %s to Sirius: %d", departureStationName, StationFlightsApp[*(unsigned long int*)station][3]);
    GoToXYNormalVersion(85,y+(multiplier*4));
    printf("Total: %d", passengersOnStation); //общее кол-во заявок на перелёты с данной станции 
}

//Функция, отвечающая за формирование начального состояния мира
void PlanetaryStations()
{
    //Станция "Альдебаран"
    char Aldebaran[] = "Aldebaran ";
    GoToXY(5,5,Aldebaran); //передвигаем курсор в нужное место
    PlanetaryStationsDrawer(5,8); //рисуем саму станцию

    //Станция "Вега"
    char Vega[] = "Vega";
    GoToXY(40,5,Vega); 
    PlanetaryStationsDrawer(40,8);

    //Станция "Земля"
    char TheEarth[] = "The Earth";
    GoToXY(5,25, TheEarth);
    PlanetaryStationsDrawer(5,28); 

    //Станция "Сириус"
    char Sirius[] = "Sirius";
    GoToXY(40,25, Sirius); 
    PlanetaryStationsDrawer(40,28); 

} 

//Главная функция
void main()
{
    system("cls"); //очищаем экран
    PlanetaryStations(); //вызываем функцию генерации начального состояния мира
    
    for(int i=0; i<4; i++)
    { 
        hMtxSt[i] = CreateMutex(NULL, FALSE, NULL); //создаём мьютексы для работы с информацией о пассажирах на каждой из 4-х станций
    }

    for(int q=0; q<4; q++)
    {
        hMtxPass[q] = CreateMutex(NULL, FALSE, NULL);
    }

    hMtxWr = CreateMutex(NULL, FALSE, NULL); //мьютекс для блокировки вывода изображения космического корабля
    hMtxWrGTNV = CreateMutex(NULL, FALSE, NULL); //мьютекс для блокировки вывода количества прибывших на станцию пассажиров

    unsigned long int stationNumber[4] = {0,1,2,3}; //номера станций
    for(int k=0; k<4; k++)
    {
        hPassengersOnStations[k] = (HANDLE)_beginthreadex(NULL, 4096, CreatePassengers, &stationNumber[k], 0, 0);
        Sleep(15);
    }

    unsigned long int shipNumber[4] = {0,1,2,3}; //номера кораблей
    for(int j=0; j<4; j++)
    {
        hShip[j] = (HANDLE)_beginthreadex(NULL, 4096, Spaceship, &shipNumber[j], 0, 0);
    } 

    for(int k=0; k<4; k++)
    {
        CloseHandle(hShip[k]);
    }
    getchar();
}

