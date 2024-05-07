#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>


#define U 1
#define D 2
#define L 3
#define R 4       //�ߵ�״̬��U���� ��D���£�L:�� R����
// ANSI��ɫ����
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct SNAKE //�����һ���ڵ�
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

// ��Ϸ�û���־�ṹ��
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
    printf("�������û�����");
    scanf("%s", username);
    printf("���������룺");
    scanf("%s", password);

    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("�޷����ļ���\n");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char savedUsername[20];
        char savedPassword[20];
        sscanf(line, "%s %s", savedUsername, savedPassword);

        if (strcmp(username, savedUsername) == 0 && strcmp(password, savedPassword) == 0) {
            printf("��¼�ɹ���\n");
            fclose(file);
            return;
        }
    }

    printf("�û������������\n");
    fclose(file);
    exit(1);
}
int isUsernameTaken(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("�޷����ļ���\n");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char savedUsername[20];
        sscanf(line, "%s", savedUsername);

        if (strcmp(username, savedUsername) == 0) {
            fclose(file);
            return 1; // �û����Ѵ���
        }
    }

    fclose(file);
    return 0; // �û���������
}
void registerUser() {
    User currentUser;
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("�޷����ļ���\n");
        exit(1);
    }
    do {
        printf("�������û�����");
        scanf("%s", currentUser.username);

        // ����û����Ƿ��Ѵ���
        if (isUsernameTaken(currentUser.username)) {
            printf("�û����Ѵ��ڣ���ʹ�������û�����\n");
        }
    } while (isUsernameTaken(currentUser.username));

    printf("���������룺");
    scanf("%s", currentUser.password);



    fprintf(file, "%s %s\n", currentUser.username, currentUser.password);
    fclose(file);
    printf("ע��ɹ���\n");
    printf("���¼������Ϸ��\n");
    loginUser();
}


//ȫ�ֱ���//
int score = 0, add = 10;//�ܵ÷���ÿ�γ�ʳ��÷֡�
int status, sleeptime = 200;//ÿ�����е�ʱ����
snake* head, * food;//��ͷָ�룬ʳ��ָ��
snake* q;//�����ߵ�ʱ���õ���ָ��
int endgamestatus = 0; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��
char currentUsername[20]; // ȫ�ֱ��������浱ǰ��¼���û���

//����ȫ������//
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

void Pos(int x, int y)//���ù��λ��
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//������ͼ
{
    int i;
    for (i = 0; i < 58; i += 2)//��ӡ���±߿�
    {
        Pos(i, 0);
         printf(ANSI_COLOR_YELLOW "��" ANSI_COLOR_RESET);
        Pos(i, 26);
        printf(ANSI_COLOR_YELLOW "��" ANSI_COLOR_RESET);
    }
    for (i = 1; i < 26; i++)//��ӡ���ұ߿�
    {
        Pos(0, i);
       printf(ANSI_COLOR_YELLOW "��" ANSI_COLOR_RESET);
        Pos(56, i);
         printf(ANSI_COLOR_YELLOW "��" ANSI_COLOR_RESET);
    }
}




void initsnake()//��ʼ������
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��//
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
    while (tail != NULL)//��ͷ��Ϊ���������
    {
        Pos(tail->x, tail->y);
        printf("��");
        tail = tail->next;
    }
}

int biteself()//�ж��Ƿ�ҧ�����Լ�
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
void createfood()//�������ʳ��
{
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while (1)
    {
        food_1->x = rand() % 28 * 2 + 2; // ����ż��λ��
        food_1->y = rand() % 24 + 1;
        q = head;
        int overlap = 0;
        while (q != NULL)
        {
            if (q->x == food_1->x && q->y == food_1->y) // �ж������Ƿ���ʳ���غ�
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
    printf("��");
}*/

int checkOverlap(snake* food_1);

void createfood() {
    srand((unsigned)time(NULL));

    // ����ʳ���ڴ�
    snake* food_1 = (snake*)malloc(sizeof(snake));
    if (food_1 == NULL) {
        fprintf(stderr, "ʳ���ڴ����ʧ��\n");
        exit(EXIT_FAILURE);
    }

    // ѭ��ֱ�����ɲ��������غϵ�ʳ��λ��
    do {
        food_1->x = (rand() % 14) * 4 + 2; // ����ż��λ��
        food_1->y = rand() % 24 + 1;
    } while (checkOverlap(food_1));

    // ��ӡʳ��
    Pos(food_1->x, food_1->y);
    printf("��");

    // ����ʳ��ָ��
    food = food_1;
}

// ���ʳ��λ���Ƿ��������ص�
int checkOverlap(snake* food_1) {
    snake* q = head;
    while (q != NULL) {
        if (q->x == food_1->x && q->y == food_1->y) {
            return 1; // �ص�
        }
        q = q->next;
    }
    return 0; // ���ص�
}



void cantcrosswall()//���ܴ�ǽ
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//��ǰ��,��U,��D,��L,��R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//�����һ����ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //���û��ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)  //��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //�ж��Ƿ��ҧ���Լ�
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//��ͣ
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
        printf("�޷�����Ϸ�û���־�ļ���\n");
        return;
    }

    printf("��Ϸ�û���־��\n");
    printf("ID\t�û���\t��ʼʱ��\t����ʱ��\t�÷�\n");
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

void gamecircle()//������Ϸ
{

    Pos(64, 15);
    printf("���ܴ�ǽ������ҧ���Լ�\n");
    Pos(64, 16);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
    Pos(64, 17);
    printf("F1 Ϊ���٣�F2 Ϊ����\n");
    Pos(64, 18);
    printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
    Pos(64, 20);
    printf("��F5��ʾ��Ϸ�û���־");
    Pos(64, 22);
    printf("***%s������Ϸ��***\n", username);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("�÷֣�%d  ", score);
        Pos(64, 11);
        printf("ÿ��ʳ��÷֣�%d��", add);
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
                    add = 2;//��ֹ����1֮���ټӻ����д�
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
                    add = 1;  //��֤��ͷ�Ϊ1
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

void welcometogame()//��ʼ����
{

    int choice;
    system("cls");


    printf(ANSI_COLOR_YELLOW "******************************************\n");
    printf("*          ��ӭ����̰ʳ����Ϸ��          *\n");
    printf("*                                        *\n");
    printf("*             " ANSI_COLOR_RESET "1. ע��" ANSI_COLOR_YELLOW "                    *\n");
    printf("*             " ANSI_COLOR_RESET "2. ��¼" ANSI_COLOR_YELLOW "                    *\n");
    printf("*                                        *\n");
    printf("*             ��ѡ��" ANSI_COLOR_RESET);
    scanf("%d", &choice);

    if (choice == 1) {
        registerUser();
    } else if (choice == 2) {
        loginUser();
    } else {
        printf("��Ч��ѡ��\n");
        exit(1);
    }

    printf("���������ʼ��Ϸ...\n");
    getchar();
    system("cls");

}
int generateUserID(const char* username) {
    unsigned int hash = 0;
    for (int i = 0; i < strlen(username); i++) {
        hash = 31 * hash + username[i];
    }
    // ����ϣֵӳ�䵽 [100000, 999999] �ķ�Χ��
    return 100000 + (hash % 900000);
}
void endgame()//������Ϸ
{

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("�Բ�����ײ��ǽ�ˡ���Ϸ����!");
    }
    else if (endgamestatus == 2)
    {
        printf("�Բ�����ҧ���Լ��ˡ���Ϸ����!");
    }
    else if (endgamestatus == 3)
    {
        printf("���Ѿ���������Ϸ��");
    }
    Pos(24, 13);
    printf("���ĵ÷���%d\n", score);
    //�������ʱ��
    time_t endTime = time(NULL); // ��ȡ��ǰʱ��
    int duration = difftime(endTime, startTime); // �������ʱ������λ���룩
     // ��¼��Ϸ�û���־
    FILE* file = fopen(logFile, "a");
    if (file == NULL) {
        printf("�޷�����Ϸ�û���־�ļ���\n");
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
    // ��ʾ���µ���Ϸ�û���־
    showGameLogs(username);
    Sleep(1000);
    exit(0);
}


void gamestart()//��Ϸ��ʼ��
{
    system("mode con cols=100 lines=30");
    // ��¼��Ϸ�û���־
    FILE* file = fopen(logFile, "a");
    if (file == NULL) {
      printf("�޷�����Ϸ�û���־�ļ���\n");
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
