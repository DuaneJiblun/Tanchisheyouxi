#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>


#define U 1
#define D 2
#define L 3
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右
// ANSI颜色代码
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

typedef struct User
{
    char username[20];
    char password[20];

} User;

// 游戏用户日志结构体
typedef struct {
    int id;
    char username[50];
    time_t startTime;
    int duration;
    int score;
} GameLog;

User currentUser;

const char* logFile = "game_logs.txt";
char username[20];
char password[20];
time_t startTime;
void loginUser() {
    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);

    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("无法打开文件。\n");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char savedUsername[20];
        char savedPassword[20];
        sscanf(line, "%s %s", savedUsername, savedPassword);

        if (strcmp(username, savedUsername) == 0 && strcmp(password, savedPassword) == 0) {
            printf("登录成功！\n");
            fclose(file);
            return;
        }
    }

    printf("用户名或密码错误。\n");
    fclose(file);
    exit(1);
}
int isUsernameTaken(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("无法打开文件。\n");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char savedUsername[20];
        sscanf(line, "%s", savedUsername);

        if (strcmp(username, savedUsername) == 0) {
            fclose(file);
            return 1; // 用户名已存在
        }
    }

    fclose(file);
    return 0; // 用户名不存在
}
void registerUser() {
    User currentUser;
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("无法打开文件。\n");
        exit(1);
    }
    do {
        printf("请输入用户名：");
        scanf("%s", currentUser.username);

        // 检查用户名是否已存在
        if (isUsernameTaken(currentUser.username)) {
            printf("用户名已存在，请使用其他用户名。\n");
        }
    } while (isUsernameTaken(currentUser.username));

    printf("请输入密码：");
    scanf("%s", currentUser.password);



    fprintf(file, "%s %s\n", currentUser.username, currentUser.password);
    fclose(file);
    printf("注册成功！\n");
    printf("请登录进入游戏：\n");
    loginUser();
}


//全局变量//
int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake* head, * food;//蛇头指针，食物指针
snake* q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。
char currentUsername[20]; // 全局变量，保存当前登录的用户名

//声明全部函数//
void Pos();
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();

void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//创建地图
{
    int i;
    for (i = 0; i < 58; i += 2)//打印上下边框
    {
        Pos(i, 0);
         printf(ANSI_COLOR_YELLOW "■" ANSI_COLOR_RESET);
        Pos(i, 26);
        printf(ANSI_COLOR_YELLOW "■" ANSI_COLOR_RESET);
    }
    for (i = 1; i < 26; i++)//打印左右边框
    {
        Pos(0, i);
       printf(ANSI_COLOR_YELLOW "■" ANSI_COLOR_RESET);
        Pos(56, i);
         printf(ANSI_COLOR_YELLOW "■" ANSI_COLOR_RESET);
    }
}




void initsnake()//初始化蛇身
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//从头到为，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()//判断是否咬到了自己
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}
/*
void createfood()//随机出现食物
{
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while (1)
    {
        food_1->x = rand() % 28 * 2 + 2; // 生成偶数位置
        food_1->y = rand() % 24 + 1;
        q = head;
        int overlap = 0;
        while (q != NULL)
        {
            if (q->x == food_1->x && q->y == food_1->y) // 判断蛇身是否与食物重合
            {
                overlap = 1;
                break;
            }
            q = q->next;
        }
        if (!overlap)
        {
            break;
        }
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}*/

int checkOverlap(snake* food_1);

void createfood() {
    srand((unsigned)time(NULL));

    // 分配食物内存
    snake* food_1 = (snake*)malloc(sizeof(snake));
    if (food_1 == NULL) {
        fprintf(stderr, "食物内存分配失败\n");
        exit(EXIT_FAILURE);
    }

    // 循环直到生成不与蛇身重合的食物位置
    do {
        food_1->x = (rand() % 14) * 4 + 2; // 生成偶数位置
        food_1->y = rand() % 24 + 1;
    } while (checkOverlap(food_1));

    // 打印食物
    Pos(food_1->x, food_1->y);
    printf("■");

    // 更新食物指针
    food = food_1;
}

// 检查食物位置是否与蛇身重叠
int checkOverlap(snake* food_1) {
    snake* q = head;
    while (q != NULL) {
        if (q->x == food_1->x && q->y == food_1->y) {
            return 1; // 重叠
        }
        q = q->next;
    }
    return 0; // 不重叠
}



void cantcrosswall()//不能穿墙
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//蛇前进,上U,下D,左L,右R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//如果下一个有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //判断是否会咬到自己
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//暂停
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;

        }

    }
}

void showGameLogs(const char* username) {
    system("cls");
    FILE* file = fopen(logFile, "r");
    if (file == NULL) {
        printf("无法打开游戏用户日志文件。\n");
        return;
    }

    printf("游戏用户日志：\n");
    printf("ID\t用户名\t开始时间\t持续时长\t得分\n");
    printf("------------------------------------------------------\n");

    GameLog log;
    while (fread(&log, sizeof(GameLog), 1, file)) {
        if (strcmp(log.username, username) == 0) {
            printf("%d\t%s\t%s\t\t%d\t%d\n", log.id, log.username, ctime(&log.startTime), log.duration, log.score);
        }
    }

    fclose(file);
    getchar();
    system("cls");
}

void gamecircle()//控制游戏
{

    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);
    printf("按F5显示游戏用户日志");
    Pos(64, 22);
    printf("***%s正在游戏中***\n", username);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到1之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //保证最低分为1
                }
            }
        }
        else if(GetAsyncKeyState(VK_F5))
        {
            showGameLogs(username);
        }
        Sleep(sleeptime);
        snakemove();
    }
}

void welcometogame()//开始界面
{

    int choice;
    system("cls");


    printf(ANSI_COLOR_YELLOW "******************************************\n");
    printf("*          欢迎来到贪食蛇游戏！          *\n");
    printf("*                                        *\n");
    printf("*             " ANSI_COLOR_RESET "1. 注册" ANSI_COLOR_YELLOW "                    *\n");
    printf("*             " ANSI_COLOR_RESET "2. 登录" ANSI_COLOR_YELLOW "                    *\n");
    printf("*                                        *\n");
    printf("*             请选择：" ANSI_COLOR_RESET);
    scanf("%d", &choice);

    if (choice == 1) {
        registerUser();
    } else if (choice == 2) {
        loginUser();
    } else {
        printf("无效的选择。\n");
        exit(1);
    }

    printf("按任意键开始游戏...\n");
    getchar();
    system("cls");

}
int generateUserID(const char* username) {
    unsigned int hash = 0;
    for (int i = 0; i < strlen(username); i++) {
        hash = 31 * hash + username[i];
    }
    // 将哈希值映射到 [100000, 999999] 的范围内
    return 100000 + (hash % 900000);
}
void endgame()//结束游戏
{

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    //计算持续时长
    time_t endTime = time(NULL); // 获取当前时间
    int duration = difftime(endTime, startTime); // 计算持续时长（单位：秒）
     // 记录游戏用户日志
    FILE* file = fopen(logFile, "a");
    if (file == NULL) {
        printf("无法打开游戏用户日志文件。\n");
        return;
    }

    GameLog log;
    log.id = generateUserID(username);
    strcpy(log.username, username);
    log.startTime = startTime;
    log.duration = duration;
    log.score = score;

    fwrite(&log, sizeof(GameLog), 1, file);
    fclose(file);
    getchar();
    system("cls");
    // 显示最新的游戏用户日志
    showGameLogs(username);
    Sleep(1000);
    exit(0);
}


void gamestart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    // 记录游戏用户日志
    FILE* file = fopen(logFile, "a");
    if (file == NULL) {
      printf("无法打开游戏用户日志文件。\n");
      exit(1);
    }
    GameLog log;
    log.id = generateUserID(username);
    strcpy(log.username, username);
    log.startTime = startTime;
    log.duration = 0;
    log.score = 0;
    fwrite(&log, sizeof(GameLog), 1, file);
    fclose(file);

    startTime = time(NULL);
    welcometogame();
    creatMap();
    initsnake();
    createfood();

}

int main()
{
    gamestart();
    gamecircle();
    endgame();

    return 0;
}
