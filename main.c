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

//координаты доков каждой станции
int StationCOORD[4][2] = {{25,12}, {124,12}, {25,57}, {124,57}};

int successfulArrival = 0;

int SpaceshipOneCurrentStation = 0,
    SpaceshipTwoCurrentStation = 1,
    SpaceshipThreeCurrentStation = 2,
    SpaceshipFourCurrentStation = 4;


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

//функция отрисовки кораблей
void Spaceships_drawer(int X, int Y, int action)
{
    const char body[2][2] = { "##",
                              "##"};
    const char bodyDeliter[2][2] = {"  ", "  "};
    //если нужно нарисовать кораблик
    for(int i=0; i<2; i++)
    {
        for(int j=0; j<2; j++)
        {   
            GoToXY(X+i,Y+j);
            //если нужно нарисовать - рисуем
            if(action = 1)
            {
                printf("%c", body[i][j]);   
            }
            //если нужно стереть - стираем
            else 
            {
                printf("%c", bodyDeliter[i][j]);  
            }     
        }
    }      
}

//TODO: исправить то, что корабль оставляет за собой след, несмотря на вызывающуюся функцию очистки
//функция отрисовки движения кораблей
void SpaceshipMovementDrawer(int departureStationCoordX, int departureStationCoordY, int arrivalStationCoordX, int arrivalStationCoordY)
{
    //если станции находятся на одном уровне по Х
    if(departureStationCoordX == arrivalStationCoordX)
    {
        //проверим, находится станция отбытия выше или ниже станции прибытия
        if(departureStationCoordY > arrivalStationCoordY)
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordY; i < arrivalStationCoordY; i++)
            {
                //будут проводится манипуляции с Y
                int Y = departureStationCoordY;
                //двигаемся - стираем кораблик в старой точке
                Spaceships_drawer(departureStationCoordX, Y, 0);
                Y = Y-i;
                //двигаемся - рисуем новый кораблик в новой точке
                Spaceships_drawer(departureStationCoordX, Y, 1);
            }
        } 
        else
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordY; i < arrivalStationCoordY; i++)
            {
                //будут проводится манипуляции с Y
                int Y = departureStationCoordY;
                //двигаемся - стираем кораблик в старой точке
                Spaceships_drawer(departureStationCoordX, Y, 0);
                Y = Y+i;
                //двигаемся - рисуем новый кораблик в новой точке
                Spaceships_drawer(departureStationCoordX, Y, 1);
            }
        }
    }

    if(departureStationCoordY == arrivalStationCoordY)
    {
        //проверим, находится станция отбытия правее или левее станции прибытия
        if(departureStationCoordX > arrivalStationCoordX)
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordX; i < arrivalStationCoordX; i++)
            {
                //будут проводится манипуляции с X
                int X = departureStationCoordX;
                //двигаемся - стираем кораблик в старой точке
                Spaceships_drawer(X, departureStationCoordY, 0);
                X = X-i;
                //двигаемся - рисуем новый кораблик в новой точке
                Spaceships_drawer(X, departureStationCoordY, 1);
            }
        } 
        else
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordX; i < arrivalStationCoordX; i++)
            {
                //будут проводится манипуляции с X
                int X = departureStationCoordX;
                //двигаемся - стираем кораблик в старой точке
                Spaceships_drawer(X, departureStationCoordY, 0);
                X = X+i;
                //двигаемся - рисуем новый кораблик в новой точке
                Spaceships_drawer(X, departureStationCoordY, 1);
            }
        }
    }

    //TODO: внештатные ситуации
    //внештатные ситуации (маршруты Альдебаран - Сириус, Вега - Земля)
    //проверяем, на какой из станций внештатных ситуаций мы находимся

    //маршрут Альдебаран - Сириус
    if((departureStationCoordX == 25 && departureStationCoordY == 12 && arrivalStationCoordX == 124 && arrivalStationCoordY == 57) || 
       (departureStationCoordX == 124 && departureStationCoordY == 57 && arrivalStationCoordX == 25 && arrivalStationCoordY == 12)) 
    {
        //пока координаты не совпадут
        for(int i = departureStationCoordX; i < arrivalStationCoordX; i++)
        {
            for(int j=departureStationCoordY; j<arrivalStationCoordY; j++)
            {
                //будут проводится манипуляции с X
                int X = departureStationCoordX;
                //будут проводится манипуляции с Y
                int Y = departureStationCoordY;
                //двигаемся - стираем кораблик в старой точке
                Spaceships_drawer(X, Y, 0);
                if(departureStationCoordX == 25)
                {
                    X = X+i;
                    Y = Y+j;
                }
                else
                {
                    X = X-i;
                    Y = Y-j;
                }
                //двигаемся - рисуем новый кораблик в новой точке
                Spaceships_drawer(X, Y, 1);
            }    
        }
    }

    //маршрут Вега - Земля
    if((departureStationCoordX == 124 && departureStationCoordY == 12 && arrivalStationCoordX == 25 && arrivalStationCoordY == 57) || 
       (departureStationCoordX == 25 && departureStationCoordY == 57 && arrivalStationCoordX == 124 && arrivalStationCoordY == 12)) 
    {
        //пока координаты не совпадут
        for(int i = departureStationCoordX; i < arrivalStationCoordX; i++)
        {
            //будут проводится манипуляции с X
            int X = departureStationCoordX;
            //будут проводится манипуляции с Y
            int Y = departureStationCoordY;

            //если едем из Веги
            if(Y == 12)
            {
                //едем вверх
                for(int j=Y; j<20; j++)
                {
                    //двигаемся - стираем кораблик в старой точке
                    Spaceships_drawer(X, Y, 0);
                    Y=Y-j;
                    Spaceships_drawer(X, Y, 1);
                }
                //поворачиваем вправо
                for(int j=X; j<140; j++)
                {
                    //двигаемся - стираем кораблик в старой точке
                    Spaceships_drawer(X, Y, 0);
                    X=X+j;
                    Spaceships_drawer(X, Y, 1);
                }
                //спускаемся вниз
                for(int i=Y; i<70; i++)
                {
                    //двигаемся - стираем кораблик в старой точке
                    Spaceships_drawer(X, Y, 0);
                    Y=Y+i;
                    Spaceships_drawer(X, Y, 1);
                }
                //поворачиваем налево
                for(int j=X; j<124; j++)
                {
                    //двигаемся - стираем кораблик в старой точке
                    Spaceships_drawer(X, Y, 0);
                    X=X-j;
                    Spaceships_drawer(X, Y, 1);
                }
                //поднимаемся наверх - в док
                for(int j=Y; j<12; j++)
                {
                    //двигаемся - стираем кораблик в старой точке
                    Spaceships_drawer(X, Y, 0);
                    Y=Y-j;
                    Spaceships_drawer(X, Y, 1);
                }
            }  
        }
    }
}

//FIXME: не отнимается кол-во человек, когда корабль долетел до первой точки назначения, поэтому он и не двигается!
void SpaceshipMovementLogic(int *shipRoute, int departureStationNumber)
{
    int currentStation[2];
    currentStation[0] = StationCOORD[departureStationNumber][0]; //X
    currentStation[1] = StationCOORD[departureStationNumber][1]; //Y

    int i=0;
    //обрабатываем пришедший массив с курсом движения корабля
    while(i<4)
    {
        //если на направлении есть пассажиры
        if(shipRoute[i] != 0)
        {
            //вызываем функцию перелёта из станции по текущим координатам на станцию прибытия
            SpaceshipMovementDrawer(currentStation[0], currentStation[1], StationCOORD[i][0], StationCOORD[i][1]);
            currentStation[0] = StationCOORD[i][0];
            currentStation[1] = StationCOORD[i][1];
            GoToXY(StationCOORD[i][0], StationCOORD[i][1]);
            printf("Pass arrived: %d", shipRoute[i]);
            shipRoute[i] = 0; //высаживаем пассажиров на станции
            //записываем координаты станции, на которую нужно попасть
            GoToXY(190,45);
            printf("Arrival station coodrs: %d %d", StationCOORD[i][0], StationCOORD[i][1]);
            GoToXY(190,46);
            printf("New dep station coodrs: %d %d", currentStation[0], currentStation[1]);
        }
        i++;      
    }
    //TODO: возврат обратно в станцию отправления
    SpaceshipMovementDrawer(currentStation[0], currentStation[1], StationCOORD[departureStationNumber][0], StationCOORD[departureStationNumber][1]);
}

void Spaceship(int currentStation, int originalStation)
{
    int seats = 4;
    int passengers=0;
    //логика точно такая же, как и у станций, только здесь вместо станций - корабли.
    //т.е. корабль, у него 4 места. Каждая ячейка отвечает за свою станцию, содержимое ячейки - на количество пассажиров В КОРАБЛЕ, желающих поехать на эту станцию
    int shipWorkload[4] = {0,0,0,0}; //загруженность салона корабля
    int i=0;
    //ищет, хочет ли кто-то полететь с текущей станции хоть куда-нибудь
    for(i=0; i<4; i++)
    {
        //если ещё не всех пассажиров развезли - этот цикл нужен для самого последнего развоза
        if(StationFlightsApp[currentStation][i] != 0)
        {
            //проверяем, остались ли места в салоне. If вместо while для тех моментов, когда, например, остался всего 1 пассажир. Он займёт 1 место, 3 останется, но корабль должен полететь, потому что никого больше нет
            if(seats > 0)
            {
                passengers += (StationFlightsApp[currentStation][i]/StationFlightsApp[currentStation][i]); //садим РОВНО 1 пассажира
                seats -= passengers; //пассажиры занимают места
                StationFlightsApp[currentStation][i] -= (StationFlightsApp[currentStation][i]/StationFlightsApp[currentStation][i]); //севшие на корабль пассажиры больше не считаются пассажирами, ожидающими перелёт
                shipWorkload[i] += (StationFlightsApp[currentStation][i]/StationFlightsApp[currentStation][i]); //записываем, что этот пассажир сел в корабль на направление i
            }
        }
    }
    //отладка
    GoToXY(190, 70);
    printf("Amount of pass on the ship: %d", passengers);
    for(int c=0; c<4; c++)
    {
        GoToXY(190, 60+c);
        printf("Route: %d", shipWorkload[c]);
    }

    //полетели
    int *shipRoute = shipWorkload;
    SpaceshipMovementLogic(shipRoute, currentStation);

    
    for(int v=0; v<4; v++)
    {
        GoToXY(190,75+v);
        printf("SFA spaceship: %d", StationFlightsApp[currentStation][v]);
    }
    //если развезли всех пассажиров со станции, и больше никто никуда ехать не хочет - корабль возвращается в доки своей станции
    SpaceshipMovementDrawer(StationCOORD[currentStation][0], StationCOORD[currentStation][1], StationCOORD[originalStation][0], StationCOORD[originalStation][1]);
}


//SFA здесь генерируется верно
//Логические функции
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
            int passengers = Random_number_generator(1,2); //генерируем рандомное кол-во пассажиров, которые куда-то поедут
            StationFlightsApp[departureStationNumber][j] = passengers; //записываем этих пассажиров как желающих поехать на станцию j
            passengerAmount += passengers; //считаем общее кол-во пассажиров
            GoToXY(X,Y);
            printf("Flight app: %d", passengerAmount); //выводим общее кол-во пассажиров на станции в окошко станции
        }
    }
    
    //отладка
    for(int v=0; v<4; v++)
    {
        GoToXY(190, 50+v);
        printf("SFA aldebaran from SFA function: %d", StationFlightsApp[0][v]);
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

void SpaceshipsCalling(int station)
{
    Spaceship(station, station);
}

//сами станции (здесь собирается изначальное состояние мира)
void Planetary_stations()
{
    //Станция "Альдебаран"
    GoToXY(5,5); //передвигаем курсор в нужное место
    printf("Aldebaran"); //название станции
    Planetary_stations_drawer(5,8); //рисуем станцию
    Passengers(8,9,0,7); //генерируем пассажиров
    Spaceships_drawer(25,12,1); //рисуем корабль

    //Станция "Вега"
    GoToXY(130,5); //передвигаем курсор в нужное место
    printf("Vega"); //название станции
    Planetary_stations_drawer(130,8); //рисуем станцию
    Passengers(133,9,1,14); //генерируем пассажиров
    Spaceships_drawer(124,12,1); //рисуем корабль

    //Станция "Земля"
    GoToXY(5,50); //передвигаем курсор в нужное место
    printf("The Earth"); //название станции
    Planetary_stations_drawer(5,53); //рисуем станцию
    Passengers(8,54,2,21); //генерируем пассажиров
    Spaceships_drawer(25,57,1); //рисуем корабль

    //Станция "Сириус"
    GoToXY(130,50); //передвигаем курсор в нужное место
    printf("Sirius"); //название станции
    Planetary_stations_drawer(130,53); //рисуем станцию
    Passengers(133,54,3,28); //генерируем пассажиров
    Spaceships_drawer(124,57,1); //рисуем корабль

    SpaceshipsCalling(0);
}

void main()
{
    system("cls");
    Planetary_stations();
    getchar();
}

