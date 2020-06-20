#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include <Windows.h>
#define XMAX 40
#define YMAX 20
#define WALL '#'
#define FLOOR ' '
#define PLAYER '\1'
#define STAR '*'

void cls(){
	for(int i = 0 ; i < 40 ; i ++)
		puts("");
}

void gotoxy(int x, int y){
	COORD c = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setColor(WORD w){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), w);
}

struct User{
	char username[20], password[25];
	User(){}
	User(char username[], char password[]){
		strcpy(this->username, username);
		strcpy(this->password, password);
	}
}user[100];

int countUser = 0;

struct Session{
	User user;
	Session(){}
	Session(char username[], char password[]){
		this->user = User(username, password);
	}
}session;

struct Player{
	int x, y, score;
	Player(){}
	Player(int x, int y){
		this->x = x;
		this->y = y;
		this->score = 0;
	}
}player;

struct Map{
	char node[25][50];
	int block;
	bool isVisit[25][50], isTrap[25][50];
	Map(){}
	Map(char node[][50], int block){
		for(int i = 0 ; i < YMAX ; i++){
			for(int j = 0 ; j < XMAX ; j++){
				this->node[i][j] = node[i][j];
				this->isTrap[i][j] = false;
				(this->node[i][j] == WALL) ? isVisit[i][j] = true : isVisit[i][j] = false;
			}
		}
		this->block = block;
	}
}map;

struct Score{
	char username[20];
	int score;
	Score(){}
	Score(char username[], int score){
		strcpy(this->username, username);
		this->score = score;
	}
}score[50];

int countScore = 0;

struct UserController{
	bool printError(char error[]){
		puts(error);
		return false;
	}
	bool checkUsername(char username[]){
		if(strlen(username) < 5 || strlen(username) > 10)
			return printError("Username must be between 5 - 10 characters");
		for(int i = 0 ; i < strlen(username) ; i++)
			if(!isalnum(username[i]))
				return printError("Username must be only contain alphabetic and numeric");
		for(int i = 0 ; i < countUser ; i++)
			if(strcmpi(user[i].username, username) == 0)
				return printError("Username already taken");
		return true;
	}
	bool checkPassword(char password[]){
		int countAlphabetic = 0, countNumeric = 0;
		if(strlen(password) < 8 || strlen(password) > 20)
			return printError("Password must be between 8 - 20 characters");
		for(int i = 0 ; i < strlen(password) ; i++){
			if(isalpha(password[i]))
				countAlphabetic++;
			if(isdigit(password[i]))
				countNumeric++;
		}
		if(countAlphabetic == 0 || countNumeric == 0)
			return printError("Password must contain alphabetic and numeric");
		return true;
	}
	void readFile(){
		FILE *u = fopen("user.txt", "r");
		if(u){
			while(fscanf(u, "%[^#]#%[^\n]\n", &user[countUser].username, &user[countUser].password) != EOF)
				countUser++;
			fclose(u);
		}
	}
	void saveFile(){
		FILE *u = fopen("user.txt", "w");
		for(int i = 0 ; i < countUser ; i++)
			fprintf(u, "%s#%s\n", user[i].username, user[i].password);
		fclose(u);
	}
	void registerUser(char username[], char password[]){
		user[countUser] = User(username, password);
		countUser++;
		saveFile();
	}
	bool loginUser(char username[], char password[]){
		for(int i = 0 ; i < countUser ; i++){
			if(strcmp(user[i].username, username) == 0 && strcmp(user[i].password, password) == 0){
				session = Session(username, password);	
				return true;
			}
		}
		printf("Invalid username or password");
		getch();
		fflush(stdin);
		return false;
	}
}uc;

struct ScoreController{
	void readFile(){
		countScore = 0;
		FILE *s = fopen("score.txt", "r");
		if(s){
			while(fscanf(s, "%[^#]#%d\n", &score[countScore].username, &score[countScore].score) != EOF)
				countScore++;
			fclose(s);
		}
	}
	void saveFile(){
		sorting();
		FILE *s = fopen("score.txt", "w");
		for(int i = 0 ; i < countScore ; i++){
			if(i > 9) break;
			fprintf(s, "%s#%d\n", score[i].username, score[i].score);
		}
		fclose(s);
		readFile();
	}
	void sorting(){
		for(int i = 0 ; i < countScore ; i++){
			for(int j = 0 ; j < countScore - 1 ; j++){
				if(score[j].score < score[j + 1].score){
					struct Score temp = score[j];
					score[j] = score[j + 1];
					score[j + 1] = temp;
				}
			}
		}
	}
}sc;

struct PlayerController{
	void generateStar(){
		int x, y;
		do{
			x = rand() % XMAX;
			y = rand() % YMAX;
		}while(map.node[y][x] == WALL || map.isTrap[y][x] || map.node[y][x] == PLAYER);
		gotoxy(x, y);
		setColor(0x0E);
		printf("%c", STAR);
		setColor(0x0F);
		map.node[y][x] = STAR;
		gotoxy(player.x, player.y);
	}
	void fillPlayer(){
		gotoxy(player.x, player.y);
		setColor(0x0B);
		printf("%c", PLAYER);
		setColor(0x0F);
	}
	bool move(int key){
		if(key == 119){
			if(map.node[player.y - 1][player.x] != WALL && (player.y - 1) != 0){
				gotoxy(player.x, player.y);
				printf("%c", FLOOR);
				player.y--;
				fillPlayer();
			}
		}else if(key == 115){
			if(map.node[player.y + 1][player.x] != WALL && (player.y + 1) != (YMAX - 1)){
				gotoxy(player.x, player.y);
				printf("%c", FLOOR);
				player.y++;
				fillPlayer();
			}
		}else if(key == 97){
			if(map.node[player.y][player.x - 1] != WALL && (player.x  - 1) != 0){
				gotoxy(player.x, player.y);
				printf("%c", FLOOR);
				player.x--;
				fillPlayer();
			}
		}else if(key == 100){
			if(map.node[player.y][player.x + 1] != WALL && (player.x + 1) != (XMAX - 1)){
				gotoxy(player.x, player.y);
				printf("%c", FLOOR);
				player.x++;
				fillPlayer();
			}
		}
		if(map.isTrap[player.y][player.x]){
			gotoxy(player.x, player.y);
			printf("%c", 2);
			Sleep(500);
			gotoxy(player.x, player.y);
			setColor(0x0C);
			printf("X");
			gotoxy(45, 16);
			printf("Opps, you stepped on a trap");
			gotoxy(45, 17);
			printf("You Lose !!!");
			setColor(0x0F);
			score[countScore] = Score(session.user.username, player.score);
			countScore++;
			sc.saveFile();
			getchar();
			return true;
		}else if(map.node[player.y][player.x] == STAR){
			map.node[player.y][player.x] = PLAYER;
			generateStar();
			map.node[player.y][player.x] = FLOOR;			
			player.score++;			
			gotoxy(45, 13);
			printf("Score : %d", player.score);
		}
		return false;
	}
}pc;

struct MapController{
	void readFile(){
		char file[20], node[25][50];
		int block = 0;
		strcpy(file, session.user.username);
		strcat(file, "_map.txt");
		FILE *m = fopen(file, "r");
		if(!m){
			for(int i = 0 ; i < YMAX ; i++)
				for(int j = 0 ; j < XMAX ; j++)
					(i == 0 || j == 0 || i == YMAX - 1 || j == XMAX - 1) ? node[i][j] = WALL : node[i][j] = FLOOR;
			block = 50;
		}else{
			int index = 0;
			while(!feof(m)){
				(index < 20) ? fscanf(m, "%[^\n]\n", &node[index]) : fscanf(m, "%d\n", &block);
				index++;
			}
		}
		map = Map(node, block);
	}
	void saveFile(){
		char file[20];
		strcpy(file, session.user.username);
		strcat(file, "_map.txt");
		FILE *m = fopen(file, "w");
		for(int i = 0 ; i < YMAX ; i++){
			for(int j = 0 ; j < XMAX ; j++)
				fprintf(m, "%c", map.node[i][j]);
			fprintf(m, "\n");
		}
		fprintf(m, "\n%d", map.block);
		fclose(m);
	}
	void printMap(){
		for(int i = 0 ; i < YMAX ; i++){
			for(int j = 0 ; j < XMAX ; j++){
				if(map.node[i][j] == STAR){
					setColor(0x0E);
					printf("%c", STAR);
				}else{
					setColor(0x0F);	
					printf("%c", map.node[i][j]);
				}
			}
			puts("");
		}
	}
	bool moveCursor(){
		bool check;
		(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_DOWN)) ? check = true : check = false;
		return check;
	}
	void movePosition(int &x, int &y){
		bool move = moveCursor();
		if(move){
			if(GetAsyncKeyState(VK_LEFT)){
				if((x - 1) != 0)
					x--;
			}else if(GetAsyncKeyState(VK_RIGHT)){
				if((x + 1) != (XMAX - 1))
					x++;
			}else if(GetAsyncKeyState(VK_UP)){
				if((y - 1) != 0)
					y--;
			}else if(GetAsyncKeyState(VK_DOWN)){
				if((y + 1) != (YMAX - 1))
					y++;
			}
			getch();
			fflush(stdin);
		}
	}
	void fillMap(int x, int y, int key){
		if(key == 32){
			if(map.node[y][x] == FLOOR && map.block > 0){
				map.node[y][x] = WALL;
				map.isVisit[y][x] = true;
				map.block--;
				gotoxy(x, y);
				printf("%c", WALL);
			}else if(map.node[y][x] == WALL){
				map.node[y][x] = FLOOR;
				map.isVisit[y][x] = false;
				map.block++;
				gotoxy(x, y);
				printf("%c", FLOOR);
			}
			for(int i = 0 ; i < 22 ; i++){
				gotoxy(i, 20);
				printf("%c", FLOOR);
			}
			gotoxy(0, 20);
			printf("Block remaining : %d", map.block);
		}
	}
	void floodFill(int x, int y){
		if(map.node[y][x] != WALL){ 
			if(x > 0 && y > 0 && x < XMAX && y < YMAX && !map.isVisit[y][x]){
				map.isVisit[y][x] = true;
				floodFill(x - 1, y);
				floodFill(x + 1, y);
				floodFill(x, y - 1);
				floodFill(x, y + 1);
			}
		}
		return;
	}
	bool checkMap(){
		for(int i = 0 ; i < YMAX ; i++)
			for(int j = 0 ; j < XMAX ; j++) 
				if(!map.isVisit[i][j])
					return false; 
		return true;
	}
	bool checkFloodFill(){
		int x, y;
		do{
			x = rand() % XMAX;
			y = rand() % YMAX;
		}while(map.node[y][x] == WALL);
		floodFill(x, y);
		return checkMap();
	}
	void backStatus(){
		for(int i = 0 ; i < YMAX ; i++)
			for(int j = 0 ; j < XMAX ; j++)
				if(map.node[i][j] != WALL)
					map.isVisit[i][j] = false; 
	}
	bool floodFill(){
		bool check = checkFloodFill();
		backStatus();
		if(check){
			saveFile();
			gotoxy(48, 9);
			printf("Success save new map !!!");
			Sleep(1500);
			return true;
		}
		gotoxy(44, 9);
		setColor(0x0C);
		printf("Flood Fill Validation Error !!!");
		Sleep(1500);
		setColor(0x0F);
		for(int i = 42 ; i < 75 ; i++){
			gotoxy(i, 9);
			printf("%c", FLOOR);
		}
		return false; 
	}
	void generateTrap(){
		for(int i = 0 ; i < 10 ; i++){
			int x, y;
			do{
				x = rand() % XMAX;
				y = rand() % YMAX;
			}while(map.node[y][x] == WALL || map.isTrap[y][x]);
			map.isTrap[y][x] = true;
		}
	}
	void generatePlayer(){
		int x, y;
		do{
			x = rand() % XMAX;
			y = rand() % YMAX;
		}while(map.node[y][x] == WALL || map.isTrap[y][x]);
		map.node[y][x] = PLAYER;
		player = Player(x, y);
	}
	void generateStar(){
		int x, y;
		do{
			x = rand() % XMAX;
			y = rand() % YMAX;
		}while(map.node[y][x] == WALL || map.isTrap[y][x] || map.node[y][x] == PLAYER);
		map.node[y][x] = STAR;
	}
	void viewInfo(){
		gotoxy(45, 4);
		printf("Move Player");
		gotoxy(50, 8);
		printf("%c", 1);
		gotoxy(50, 6);
		printf("w");
		gotoxy(47, 8);
		printf("a");
		gotoxy(53, 8);
		printf("d");
		gotoxy(50, 10);
		printf("s");
		gotoxy(45, 13);
		printf("Score : %d", player.score);
	}
	void generate(){
		generateTrap();
		generatePlayer();
		generateStar();
		printMap();
		viewInfo();
		pc.fillPlayer();
		map.node[player.y][player.x] = FLOOR;
	}
}mc;

void playGame(){
	system("cls");
	bool valid = true;
	mc.generate();
	while(valid){
		int key = getch();
		fflush(stdin);
		bool check = pc.move(key);
		if(check){
			mc.readFile();
			valid = false;
		}
	}
}

void createMap(){
	system("cls");
	int x, y, key;
	bool valid = true;
	x = y = 1;
	mc.printMap();
	gotoxy(42, 2);
	printf("Information:");
	gotoxy(42, 3);
	printf("- Use arrow to move cursor position");
	gotoxy(42, 4);
	printf("- Press 'space' to add or remove wall");
	gotoxy(42, 5);
	printf("- Press 'esc' to save map");
	gotoxy(0, 20);
	printf("Block remaining : %d", map.block);
	do{
		gotoxy(x, y);
		key = getch();
		fflush(stdin);
		mc.movePosition(x, y);
		mc.fillMap(x, y, key);
		if(key == 27)
			if(mc.floodFill())
				valid = false;
	}while(valid);
}

void viewScore(){
	system("cls");
	cls();
	if(countScore > 0){
		puts("       +==+==++==+==+");
		puts("       | High Score |");
		puts("       +==+==++==+==+\n");
		puts("+-----+------------+-------+");
		printf("| %2s. | %-10s | %-5s |\n", "No", " Username", "Score");
		puts("+-----+------------+-------+");
		for(int i = 0 ; i < countScore ; i++)
			printf("| %2d. | %-10s | %5d |\n", (i + 1), score[i].username, score[i].score);
		puts("+-----+------------+-------+");
	}else
		printf("No score available");
	getch();
	fflush(stdin);
}

void menuGame(){
	setColor(0x0F);
	gotoxy(45, 6);
	printf("1. Play Game");
	gotoxy(45, 7);
	printf("2. Create Map");
	gotoxy(45, 8);
	printf("3. High Score");
	gotoxy(45, 9);
	printf("4. Logout");
	gotoxy(45, 10);
	printf(">> ");
}

void menu(){
	int choose = 0;
	mc.readFile();
	do{
		system("cls");
		mc.printMap();
		menuGame();
		choose = getch();
		fflush(stdin);
		switch(choose){
		case 49: 
			playGame();
			break;
		case 50: 
			createMap();
			break;
		case 51: 
			viewScore();
			break;
		}
	}while(choose != 52);
}

void doLogin(){
	char username[100], password[100];
	do{
		system("cls");
		cls();
		do{
			printf("Input your username [input \"exit\" to return main menu]: ");
			gets(username); fflush(stdin);
			if(strlen(username) == 0)
				uc.printError("Username must be filled");
			else if(strcmp(username, "exit") == 0)
				return;
		}while(strlen(username) == 0);
		do{
			printf("Input your password: ");
			gets(password); fflush(stdin);
			if(strlen(password) == 0)
				uc.printError("Password must be filled");
		}while(strlen(password) == 0);
	}while(!uc.loginUser(username, password));
	printf("Welcome, %s !", session.user.username);
	getch();
	fflush(stdin);
	menu();
}

void doRegister(){
	char username[100], password[100], confirm[100];
	system("cls");
	cls();
	do{
		printf("Input your username: ");
		gets(username); fflush(stdin);
	}while(!uc.checkUsername(username));
	do{
		printf("Input your password: ");
		gets(password); fflush(stdin);
	}while(!uc.checkPassword(password));
	do{
		printf("Input your confirm password: ");
		gets(confirm); fflush(stdin);
		if(strcmp(confirm, password) != 0){
			puts("Confirm password not same with password");
		}
	}while(strcmp(confirm, password) != 0);
	printf("Success register new account");
	uc.registerUser(username, password);
	getch();
	fflush(stdin);
}

void mainLogo(){
	puts("         ___________      __"); Sleep(50);                                               
	puts("        /   _____/  \\    /  \\ ____   ____             _____ _____    ____"); Sleep(50);
	puts("        \\_____  \\\\   \\/\\/   // __ \\_/ __ \\   ______  /     \\\\__  \\  /    \\"); Sleep(50);
	puts("        /        \\\\        /\\  ___/\\  ___/  /_____/ |  Y Y  \\/ __ \\|   |  \\"); Sleep(50);
	puts("       /_______  / \\__/\\  /  \\___  >\\___  >         |__|_|  (____  /___|  /"); Sleep(50);
	puts("               \\/       \\/       \\/     \\/                \\/     \\/     \\/"); Sleep(50);
	puts("\n"); Sleep(50);
}

void exitLogo(){
	system("cls");
	cls();
	puts("\t      ___.   .__                     __               __"); Sleep(50);  
	puts("\t      \\_ |__ |  |  __ __   ____     |__|____    ____ |  | __"); Sleep(50);
	puts("\t       | __ \\|  | |  |  \\_/ __ \\    |  \\__  \\ _/ ___\\|  |/ /"); Sleep(50);
	puts("\t       | \\_\\ \\  |_|  |  /\\  ___/    |  |/ __ \\\\  \\___|    <"); Sleep(50);
	puts("\t       |___  /____/____/  \\___  >\\__|  (____  /\\___  >__|_ \\"); Sleep(50);
	puts("\t           \\/                 \\/\\______|    \\/     \\/     \\/"); Sleep(50);
	puts("\n"); Sleep(50);
	puts("\t                ____  ________           ________"); Sleep(50);  
	puts("\t               /_   |/  _____/           \\_____  \\"); Sleep(50);
	puts("\t                |   /   __  \\    ______   /  ____/"); Sleep(50);
	puts("\t                |   \\  |__\\  \\  /_____/  /       \\"); Sleep(50);
	puts("\t                |___|\\_____  /           \\_______ \\"); Sleep(50);
	puts("\t                           \\/                    \\/"); Sleep(50);
	puts("\n"); Sleep(50);
	printf("\t     %c Encourage One Another To Improve Our Full Potential %c", 1, 1); Sleep(50);
	puts("\n\n"); Sleep(50);
}

void mainMenu(){
	int choose = 0;
	uc.readFile();
	sc.readFile();
	do{
		system("cls");
		cls();
		mainLogo();
		puts("1. Login");
		puts("2. Register");
		puts("3. Exit");
		printf(">> ");
		choose = getch();
		fflush(stdin);
		switch(choose){
		case 49: doLogin(); break;
		case 50: doRegister(); break;
		}
	}while(choose != 51);
	exitLogo();
	system("pause");
}

void SetConsolePosition(){
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	SetWindowPos(GetConsoleWindow(), 0, desktop.right / 4, desktop.bottom / 4 , 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void init(){
	srand(time(NULL));
	system("mode 82,22");
	setColor(0x0F);
	SetConsoleTitle("SWee-man by SW16-2");
	SetConsolePosition();
}

int main(){
	init();
	mainMenu();
	return EXIT_SUCCESS;
}