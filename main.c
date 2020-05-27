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
int passengerAmount = 0;

//курс движения каждого из кораблей
int shipRoute[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

//свободные места в каждом из кораблей
int freeSeats[4] = {4,4,4,4};

//хэндлы
HANDLE hship[4]; //хэндлы кораблей
DWORD  shipThreadID[4]; //id кораблей
HANDLE hPassengersOnStations[4]; //хэндлы пассажиров
DWORD  passengersOnStationsID[4]; //id пассажиров

HANDLE hMtxSt[4]; //хэндлы мьютексов для работы с информацией о пассажирах на каждой станции


//Функция, отвечающая за перемещение курсора по экрану в нужное место
void GoToXY(const int X,const int Y)
{
    HANDLE OutPutHandle;
    CONSOLE_SCREEN_BUFFER_INFO ScreenBufInfo;
    OutPutHandle=GetStdHandle(STD_OUTPUT_HANDLE); 
    GetConsoleScreenBufferInfo(OutPutHandle,&ScreenBufInfo);
    ScreenBufInfo.dwCursorPosition.X=X;
    ScreenBufInfo.dwCursorPosition.Y=Y;
    SetConsoleCursorPosition(OutPutHandle,ScreenBufInfo.dwCursorPosition);
}

//Функция генерации случайного числа в заданном диапазоне значений
int RandomNumberGenerator(int min, int max)
{
    LARGE_INTEGER tt;
    QueryPerformanceCounter(&tt);
    srand(tt.LowPart); 
    double fraction = 1.0 / ((double)(RAND_MAX) + 1.0); 
    //равномерно распределяем случайное число в диапазоне min/max
    int randomNumber = (int)(rand() * fraction * (max - min + 1) + min);
    return randomNumber;
}

//Функция отрисовки станций
void PlanetaryStationsDrawer(int X, int Y)
{
    int station_width = 9; //ширина станции
    char brick = "*"; //из какого символа будут отрисовываться станции
    int i=0,j=0;
    
    //отрисовка прямоугольника станции
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

//Функция начальной отрисовки кораблей в доках своих станций
void SpaceshipsDrawer(int X, int Y)
{
    char *body = "B"; //тело корабля
    GoToXY(X,Y);
    printf("%s", body);      
}

//Функция отрисовки движения кораблей
void SpaceshipMovementDrawer(int departureStationCoordX, int departureStationCoordY, int arrivalStationCoordX, int arrivalStationCoordY)
{   
    char *body = "*"; //тело корабля
    char *bodyDeliter = " ";

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
                GoToXY(departureStationCoordX, departureStationCoordY); //перемещаемся в нужную точку
                printf("%s", bodyDeliter); //стираем то, что в ней было
                departureStationCoordX++; //увеличиваем координату
                GoToXY(departureStationCoordX, departureStationCoordY); //перемещаемся по новым координатам
                printf("%s", body); //рисуем тело корабля
                Sleep(80); //задержка для создания эффекта движения
            }  
        }

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
    //маршрут Альдебаран - Сириус
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
    //маршрут Сириус - Альдебаран
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
}

//Функция генерации маршрута корабля
void CreateRoute(int departureStationNumber, int shipNumber)
{
    int i=0, k=0;
    WaitForSingleObject(hMtxSt[shipNumber], INFINITE); //захватываем мьютекс для работы с информацией о пассажирах на станции
    //для 4-х возможных направлений
    for(i=0; i<4; i++)
    {
        //если на текущей станции есть хоть один человек, желающий полететь на направление i
        if(StationFlightsApp[departureStationNumber][i] != 0)
        {
            //проверяем, остались ли места на корабле
            if(freeSeats[shipNumber] != 0)
            {                
                //поскольку на 1 направление может быть несколько человек, максимально нагружаем корабль
                for(int j=0; j<4; j++)
                {
                    //если пассажиры на данное направление не закончились
                    if(StationFlightsApp[departureStationNumber][i] != 0)
                    {
                        //если остались места
                        if(freeSeats[shipNumber] != 0)
                        {
                            //пассажиры занимают места
                            freeSeats[shipNumber] -= (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); 
                            //севшие на корабль пассажиры больше не считаются пассажирами, ожидающими перелёт
                            StationFlightsApp[departureStationNumber][i] -= (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); 
                            //заполняем маршрут корабля в соответствии с заявками севших в салон пассажиров
                            shipRoute[shipNumber][i] += (StationFlightsApp[departureStationNumber][i]/StationFlightsApp[departureStationNumber][i]); 
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
    if(passengerAmount > 0)
    {
        i = RandomNumberGenerator(0,3); //выбирается случайная станция, на которую полетит корабль
        SpaceshipMovementDrawer(currentStation[0], currentStation[1], StationCOORD[i][0], StationCOORD[i][1]); //корабль летит на эту станцию
                        
        //по прибытии на новую станцию
        WaitForSingleObject(hMtxSt[shipNumber], INFINITE); //захватываем мьютекс для работы с информацией о пассажирах на станции
        ArrivedPassengers[i] += shipRoute[shipNumber][i]; //прибавляем новых пассажиров к кол-ву прибывших на станцию
        freeSeats[shipNumber] += shipRoute[shipNumber][i]; //кол-во свободных мест на корабле увеличивается на число высадившихся пассажиров
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
                   
        //устанавливаем новые координаты отправления (поскольку теперь корабль отправляется с новой станции, а не со своей начальной)
        currentStation[0] = StationCOORD[i][0];
        currentStation[1] = StationCOORD[i][1];

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
        
        //если после высадки пассажиров на корабле появились свободные места
        if(freeSeats[shipNumber] > 0)
        {
            for(int k=0; k<4; k++) //для 4-х возможных направлений
            {
                if(freeSeats[shipNumber] > 0) //если всё ещё не все места заняты
                {
                    if(StationFlightsApp[currentStationNumber][k] != 0) //если на текущей станции есть хоть 1 пассажир на направление i
                    {
                        //добавляем заявку этого пассажира в текущий маршрут корабля
                        shipRoute[shipNumber][k] += (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                        //севший на корабль пассажир больше не считается пассажиром, ожидающим перелёт
                        StationFlightsApp[currentStationNumber][k] -= (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                        //пассажир занимает место
                        freeSeats[shipNumber] -= (StationFlightsApp[currentStationNumber][k]/StationFlightsApp[currentStationNumber][k]);
                    }
                }
            }
        }
    }
    ReleaseMutex(hMtxSt[shipNumber]); //освобождение мьютекса
    int currSt = currentStationNumber; //номер станции, на которой находится корабль (на которую он прилетел)
    return currSt; 
}

//Функция, отвечающая за вызов перемещения корабля со станции на станцию
DWORD Spaceship(LPVOID station)
{
    //вызываем функцию формирования маршрута
    CreateRoute(*(DWORD*)station, *(DWORD*)station);

    int success = 5;
    success = SpaceshipMovementLogic(*(DWORD*)station, *(DWORD*)station); //отправляем корабль в путь
    if(success != 5) //если первый перелёт корабля прошёл успешно, и он прибыл на какую-либо станцию
    {
        //то пока не развезены все подавшие заявку на перелёт пассажиры
        while(passengerAmount != 0)
        {
            //корабль будет летать со станции на станцию
            success = SpaceshipMovementLogic(success, *(DWORD*)station);
        }  
    }

    //когда все пассажиры будут развезены
    GoToXY(10,22);
    printf("All passengers have been transported!"); //сообщить об этом
    return 0;
}

//Функция, генерирующая количество пассажиров на станциях и заявки на полёты
DWORD CreatePassengers(LPVOID station)
{
    int passengersOnStation = 0; //количество пассажиров на станции
    char *departureStationName; //указатель на строку с названием станции, с которой совершается вылет
    departureStationName = (char*)malloc(15); //выделение памяти под хранение этой строки
    
    //генерируем количество пассажиров и заявки на полёты
    for(int j=0; j<4; j++) //для всех 4-х направлений
    {
        //если заполняется ячейка, отвечающая за заявки на перелёт на ту станцию, на которой пассажиры находятся в данный момент
        if(j==*(DWORD*)station) 
        {
            StationFlightsApp[*(DWORD*)station][j] = 0; //значение ячейки приравнивается к 0, т.к. пассажиры не могут полететь на ту станцию, на которой уже находятся
        }
        else 
        {
            int passengers = RandomNumberGenerator(2,10); //генерируем случайное кол-во пассажиров, которые куда-то полетят
            StationFlightsApp[*(DWORD*)station][j] = passengers; //записываем этих пассажиров как желающих полететь на станцию j
            passengerAmount += passengers; //считаем общее кол-во пассажиров, подавших заявки
            passengersOnStation += passengers; //считаем кол-во пассажиров, подавших заявки конкретно с этой станции
        }
    }

    switch(*(DWORD*)station) //в зависимости от номера станции отправления
    {
        case 0:
        departureStationName = "Aldebaran"; //узнаём название станции отправления
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
    //выводим общее количество пассажиров, подавших заявки на перелёт
    GoToXY(57,30);
    printf("Total passenger amount: %d", passengerAmount);

    //просто линия для очерчивания окна
    for(int i=0; i<40; i++)
    {
        GoToXY(55,i);
        printf("|");
    }
    //выводим информацию о перелётах 
    GoToXY(57,4);
    printf("Interstellar flights information");
    int y=6+*(DWORD*)station; int multiplier=*(DWORD*)station; //для корректного отображения списка
    GoToXY(57,y+1+(multiplier*4));
    printf("From %s to Aldebaran: %d", departureStationName, StationFlightsApp[*(DWORD*)station][0]);
    GoToXY(57,y+2+(multiplier*4));
    printf("From %s to Vega: %d", departureStationName, StationFlightsApp[*(DWORD*)station][1]);
    GoToXY(57,y+3+(multiplier*4));
    printf("From %s to The Earth: %d", departureStationName, StationFlightsApp[*(DWORD*)station][2]);
    GoToXY(57,y+4+(multiplier*4));
    printf("From %s to Sirius: %d", departureStationName, StationFlightsApp[*(DWORD*)station][3]);
    GoToXY(85,y+(multiplier*4));
    printf("Total: %d", passengersOnStation); //общее кол-во заявок на перелёты с данной станции
}

//Функция, отвечающая за формирование начального состояния мира
void PlanetaryStations()
{
    //Станция "Альдебаран"
    GoToXY(5,5); //передвигаем курсор в нужное место
    printf("Aldebaran"); //пишем название станции
    PlanetaryStationsDrawer(5,8); //рисуем саму станцию
    SpaceshipsDrawer(16,12); //рисуем корабль в доках станции

    //Станция "Вега"
    GoToXY(40,5); 
    printf("Vega"); 
    PlanetaryStationsDrawer(40,8);
    SpaceshipsDrawer(38,12); 

    //Станция "Земля"
    GoToXY(5,25);
    printf("The Earth"); 
    PlanetaryStationsDrawer(5,28); 
    SpaceshipsDrawer(16,30); 

    //Станция "Сириус"
    GoToXY(40,25); 
    printf("Sirius"); 
    PlanetaryStationsDrawer(40,28); 
    SpaceshipsDrawer(38,30); 
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

    DWORD stationNumber[4] = {0,1,2,3}; //номера станций
    for(int k=0; k<4; k++)
    {
        hPassengersOnStations[k] = CreateThread(NULL, 0, CreatePassengers, &stationNumber[k], 0, &passengersOnStationsID[k]); //создаём нити для станций
        Sleep(15);
    }

    DWORD shipNumber[4] = {0,1,2,3}; //номера кораблей
    for(int j=0; j<4; j++)
    {
        hship[j] = CreateThread(NULL, 0, Spaceship, &shipNumber[j], 0, &shipThreadID[j]); //создаём нити кораблей
    } 
    getchar();
}

