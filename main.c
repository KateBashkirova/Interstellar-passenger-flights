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

int ArrivedPassengers[4] = {0,0,0,0};
//массив массивов. Здесь в зависимости от номера станции [4] хранятся ячейки-места (второе [4]), кужа будут распределяться пассажиры
//4 станции, на каждой по 3 возможных варианта полёта (1 не считается, потому что на какой-то пассажиры уже находятся)
int StationFlightsApp[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

//координаты доков каждой станции
int StationCOORD[4][2] = {{25,12}, {128,12}, {25,57}, {128,57}};

int RouteComplieted = 0;

int SpaceshipOneCurrentStation = 0,
    SpaceshipTwoCurrentStation = 1,
    SpaceshipThreeCurrentStation = 2,
    SpaceshipFourCurrentStation = 4;

int passengerAmount = 0;

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
void Spaceships_drawer(int X, int Y)
{
    char *body = "S";
    //если нужно нарисовать кораблик
    GoToXY(X,Y);
    printf("%s", body);      
}

//FIXME: маршрут Альдебаран - Сириус
//функция отрисовки движения кораблей
void SpaceshipMovementDrawer(int departureStationCoordX, int departureStationCoordY, int arrivalStationCoordX, int arrivalStationCoordY)
{
    GoToXY(190, 35);
    printf("SMD dep X: %d", departureStationCoordX);
    GoToXY(190, 36);
    printf("SMD dep Y: %d", departureStationCoordY);
    GoToXY(190, 37);
    printf("SMD arr X: %d", arrivalStationCoordX);
    GoToXY(190, 38);
    printf("SMD arr Y: %d", arrivalStationCoordY);
    char *body = "S";
    char *bodyDeliter = " ";

    //если станции находятся на одном уровне по Y
    if(departureStationCoordY == arrivalStationCoordY)
    {
        //если станция отбытия левее станции прибытия
        if(departureStationCoordX < arrivalStationCoordX)
        {
            //пока координаты не совпадут
            for(int i = departureStationCoordX; i<arrivalStationCoordX; i++)
            {
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", bodyDeliter);
                departureStationCoordX++;
                GoToXY(departureStationCoordX, departureStationCoordY);
                printf("%s", body);
                Sleep(10);
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
                Sleep(10);
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
                Sleep(25);
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
                Sleep(25);
            }  
        }
    }
    //маршрут Вега - Земля - работает хорошо
    if(departureStationCoordX == 128 && departureStationCoordY == 12 && arrivalStationCoordX == 25 && arrivalStationCoordY == 57)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX -= 6.1;
            departureStationCoordY += 3;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(50);
        }
    }
    //FIXME:
    //маршрут Земля - Вега
    if(departureStationCoordX == 25 && departureStationCoordY == 57 && arrivalStationCoordX == 128 && arrivalStationCoordY == 12)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX += 7;
            departureStationCoordY -= 2.1;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(50);
        }
        //замазываем косяк
        GoToXY(departureStationCoordX, departureStationCoordY);
        printf("*");
    }
    //Альдебаран - Сириус
    if(departureStationCoordX == 25 && departureStationCoordY == 12 && arrivalStationCoordX == 128 && arrivalStationCoordY == 57)
    {
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(190, 50);
            printf("That Aldebaran noncense", bodyDeliter);
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX += 6;
            departureStationCoordY += 3;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(50);
        }
         //замазываем косяк
        GoToXY(departureStationCoordX, departureStationCoordY);
        printf("*");
    }
    //Сириус - Альдебаран
    else 
    {
        //FIXME:
        //пока координаты не совпадут
        while(departureStationCoordX != arrivalStationCoordX && departureStationCoordY != arrivalStationCoordY)
        {
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", bodyDeliter);
            departureStationCoordX -= 6.1;
            departureStationCoordY -= 2.1;
            GoToXY(departureStationCoordX, departureStationCoordY);
            printf("%s", body);
            Sleep(50);
        }
         //замазываем косяк
        GoToXY(departureStationCoordX, departureStationCoordY);
        printf("*");
    }     
}

//FIXME: возврат на станцию отправления. 
void SpaceshipMovementLogic(int *shipRoute, int departureStationNumber)
{
    int currentStation[2] = {0,0}; ;
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
            //идентифицируем, откуда приехали
            char *departureStationName; //указатель на строку с названием станции
            departureStationName = (char*)malloc(15); //выделение памяти под хранение этой строки
            switch(departureStationNumber)
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
            }
            //массив с прибывшими на станции пассажирами с других станций
            ArrivedPassengers[i] += shipRoute[i];
            //выводим надпись о прибывших пассажирах
            if(StationCOORD[i][0] == 25) //условие для корректного отображения для станций Альдебаран и Земля
            {
                //FIXME: почему-то после того, как кол-во прибывших пассажиров дорастает до 4, следующие будут к нему не прибавляться, а отниматься
                //GoToXY(StationCOORD[i][0]-25, StationCOORD[i][1]+15+departureStationNumber);
                //printf("Arrived from %s: %d", departureStationName, ArrivedPassengers[i]);
                GoToXY(StationCOORD[i][0]-25, StationCOORD[i][1]+15);
                printf("Arrived: %d", ArrivedPassengers[i]);
            }
            else
            {
                //GoToXY(StationCOORD[i][0]+2, StationCOORD[i][1]+15+departureStationNumber);
                //printf("Arrived from %s: %d", departureStationName, ArrivedPassengers[i]);
                GoToXY(StationCOORD[i][0]+2, StationCOORD[i][1]+15);
                printf("Arrived: %d", ArrivedPassengers[i]);
            }
            shipRoute[i] -= ArrivedPassengers[i]; //высаживаем пассажиров на станции
            passengerAmount -= ArrivedPassengers[i];
            GoToXY(190,46);
            printf("Dep station coodrs: %d %d", currentStation[0], currentStation[1]);
            //меняем новые координаты отправления
            currentStation[0] = StationCOORD[i][0];
            currentStation[1] = StationCOORD[i][1];
            //записываем координаты станции, на которую нужно попасть
            GoToXY(190,45);
            printf("Arrival station coodrs: %d %d", StationCOORD[i][0], StationCOORD[i][1]);
            GoToXY(190,48);
            printf("New dep station coodrs: %d %d", currentStation[0], currentStation[1]);
            GoToXY(190,50+i);
            printf("Passengers left: %d", passengerAmount);
        }
        i++;      
    }
    //флексим
    int newCurrentStation;
    if (currentStation[0] == 25)
    {
        if(currentStation[1] == 12) newCurrentStation = 0;
        else newCurrentStation = 2;
    }
    if(currentStation[0] == 128)
    {
        if(currentStation[1] == 12) newCurrentStation = 1;
        else newCurrentStation = 3;
    }
    //с этой строчкой кораблик будет постоянно летать по станциям, но в какой-то момент программа крашнется
    //Spaceship(newCurrentStation, departureStationNumber);
    //возврат обратно в станцию отправления
    //SpaceshipMovementDrawer(currentStation[0], currentStation[1], StationCOORD[departureStationNumber][0], StationCOORD[departureStationNumber][1]);
}

int Spaceship(int currentStation, int originalStation)
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
            //проверяем, остались ли места в салоне. If вместо while для тех моментов, когда, например, остался всего 1 пассажир. Он займёт 1 место, 3 останется, но корабль должен полететь, 
            //потому что никого больше нет
            if(seats > 0)
            {
                //следующий цикл реализует следующее: поскольку на 1 направление может быть несколько пассажиров (в т.ч. 4 и более) мы усаживаем в корабль столько человек, сколько хотело
                //на это направление. Если их было 2 - садим сразу 2-х, если 1 - садим 1 и смотрим следующие направления. Этот цикл реализован, чтобы корабль полностью использовал
                //свой потенциал и "по максимуму" нагружал в себя пассажиров

                //поскольку на 1 направление может быть несколько человек
                for(int j=0; j<4; j++)
                {
                    //если пассажиры на данное направление не закончились
                    if(StationFlightsApp[currentStation][i] != 0)
                    {
                        if(seats != 0)
                        {
                            passengers += (StationFlightsApp[currentStation][i]/StationFlightsApp[currentStation][i]); //садим РОВНО 1 пассажира
                            seats -= 1; //пассажиры занимают места
                            StationFlightsApp[currentStation][i] -= (StationFlightsApp[currentStation][i]/StationFlightsApp[currentStation][i]); //севшие на корабль пассажиры больше не считаются пассажирами, ожидающими перелёт
                            shipWorkload[i] += (StationFlightsApp[currentStation][i]/StationFlightsApp[currentStation][i]); //записываем, что этот пассажир сел в корабль на направление i
                        }
                    } 
                }
            }
        }
    }
    //отладка
    GoToXY(190, 70);
    printf("Amount of pass on the ship: %d", passengers);
    /*for(int c=0; c<4; c++)
    {
        GoToXY(190, 60+c);
        printf("Route: %d", shipWorkload[c]);
    }*/

    //полетели
    int *shipRoute = shipWorkload;
    SpaceshipMovementLogic(shipRoute, currentStation);

    //отладка
    /*for(int v=0; v<4; v++)
    {
        GoToXY(190,75+v);
        printf("SFA on station: %d", StationFlightsApp[currentStation][v]);
    } */
    //если развезли всех пассажиров со станции, и больше никто никуда ехать не хочет - корабль возвращается в доки своей станции
    //SpaceshipMovementDrawer(StationCOORD[currentStation][0], StationCOORD[currentStation][1], StationCOORD[originalStation][0], StationCOORD[originalStation][1]);
    return 1;
}


//SFA здесь генерируется верно
//Логические функции
//функция генерации пассажиров на станциях + распределение пассажиров по направлениям полётов
void Passengers(int X, int Y, int departureStationNumber, int y)
{
    int passengersOnStation = 0;
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
    //int passengerAmount = 0;
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
            int passengers = Random_number_generator(2,12); //генерируем рандомное кол-во пассажиров, которые куда-то поедут
            StationFlightsApp[departureStationNumber][j] = passengers; //записываем этих пассажиров как желающих поехать на станцию j
            passengerAmount += passengers; //считаем общее кол-во пассажиров
            passengersOnStation += passengers; //считаем общее кол-во пассажиров на станции
        }
        GoToXY(X,Y);
        printf("Flight app: %d", passengersOnStation); //выводим общее кол-во пассажиров на станции в окошко станции
    }
    
    //отладка
    /*for(int v=0; v<4; v++)
    {
        GoToXY(190, 50+v);
        printf("SFA aldebaran from SFA function: %d", StationFlightsApp[0][v]);
    }*/

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
    int success = Spaceship(station, station);
    if(success == 1)
    {
        while(passengerAmount != 0)
        {
            int st = Random_number_generator(0,3);
            Spaceship(st,st);
        }
    }
}
    

//сами станции (здесь собирается изначальное состояние мира)
void Planetary_stations()
{
    //Станция "Альдебаран"
    GoToXY(5,5); //передвигаем курсор в нужное место
    printf("Aldebaran"); //название станции
    Planetary_stations_drawer(5,8); //рисуем станцию
    Passengers(8,9,0,7); //генерируем пассажиров
    Spaceships_drawer(25,12); //рисуем корабль

    //Станция "Вега"
    GoToXY(130,5); //передвигаем курсор в нужное место
    printf("Vega"); //название станции
    Planetary_stations_drawer(130,8); //рисуем станцию
    Passengers(133,9,1,14); //генерируем пассажиров
    Spaceships_drawer(128,12); //рисуем корабль

    //Станция "Земля"
    GoToXY(5,50); //передвигаем курсор в нужное место
    printf("The Earth"); //название станции
    Planetary_stations_drawer(5,53); //рисуем станцию
    Passengers(8,54,2,21); //генерируем пассажиров
    Spaceships_drawer(25,57); //рисуем корабль

    //Станция "Сириус"
    GoToXY(130,50); //передвигаем курсор в нужное место
    printf("Sirius"); //название станции
    Planetary_stations_drawer(130,53); //рисуем станцию
    Passengers(133,54,3,28); //генерируем пассажиров
    Spaceships_drawer(128,57); //рисуем корабль

    SpaceshipsCalling(0);

    //проверка на то, остались ли ещё неразвезённые пассажиры
 /*   int k=0;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(StationFlightsApp[i][j] != 0) k++;
        }
    }
    if(k != 0) 
    {
        while(k > 0)
        {
            SpaceshipsCalling(0);
        }
    } */
} 

void main()
{
    system("cls");
    Planetary_stations();
    getchar();
}

