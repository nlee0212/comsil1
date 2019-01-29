﻿#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;
	
	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();
	//getch();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	move(8,WIDTH+10);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP://you have to do = up down left right 
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		//don't have to
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0&&field[y+i][x+j]==0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
		
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	// think about when the long block is stuck to the wall
	int i, j;

	for(i=0;i<BLOCK_HEIGHT;i++)
	{
		for(j=0;j<BLOCK_WIDTH;j++)
		{
			if(block[currentBlock][blockRotate][i][j])
			{
				if((i+blockY)>=HEIGHT||(i+blockY)<0||(j+blockX)<0||(j+blockX)>=WIDTH)
					return 0;

				if(field[i+blockY][j+blockX]==1)
					return 0;
			}
		}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	switch(command){
	case KEY_UP:
			blockRotate=(blockRotate-1)%4;
			if(blockRotate<0)
				blockRotate=3;
		break;
	case KEY_DOWN:
			blockY--;
		break;
	case KEY_RIGHT:

			blockX--;
		break;
	case KEY_LEFT:
			blockX++;
		break;
	default:
		break;
	}

	DrawField();

	switch(command){
	case KEY_UP:
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
			blockY++;
		break;
	case KEY_RIGHT:

			blockX++;
		break;
	case KEY_LEFT:
			blockX--;
		break;
	default:
		break;
	}


	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);
	move(HEIGHT,WIDTH+10);

}

void BlockDown(int sig){
	// user code
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==1)
	{
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	}

	else
	{
		if(blockY==-1)
			gameOver=1;
		if(CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX))
			AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);

		score+=DeleteLine(field);
	
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;

		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;

		DrawNextBlock(nextBlock);

		DrawField();
		PrintScore(score);
		if(CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX)==1)
			DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	}

	timed_out=0;
}	

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int under=0;

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j])
			{
				f[blockY+i][blockX+j]=1;
				if(f[blockY+i+1][blockX+j]==1||blockY+i+1==HEIGHT)
					under++;
			}
		}
	}
	
	score+=(under*10);
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i, j, k;
	int lflag=0;
	int linecount=0;

	for(i=HEIGHT-1;i>=0;i--)
	{
		for(j=0;j<WIDTH;j++)
		{
			if(f[i][j]==0)
				break;

			if(f[i][j]==1&&j==WIDTH-1)
				lflag=1;
		}

		if(lflag==1)
		{
			linecount++;

			for(j=i;j>0;j--)
			{
				for(k=0;k<WIDTH;k++)
				{
					f[j][k]=f[j-1][k];
				}
			}
			
			for(k=0;k<WIDTH;k++)
				f[0][k]=0;

			lflag=0;
			i++;
		}
	}

	return (linecount*linecount*100);
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	while(y<HEIGHT&&CheckToMove(field,blockID,blockRotate,y+1,x)==1)
		y++;
	DrawBlock(y,x,blockID,blockRotate,'/');
}

void createRankList(){
	FILE *fp=fopen("rank.txt","r");
	int i;
	int myscore;
	char myname[NAMELEN];

	if(fp==NULL)
		{printf("error!");return;}
	else
	{
		fscanf(fp,"%d",&ranknum);
		//printw("%d",ranknum);

		for(i=0;i<ranknum;i++)
		{
			fscanf(fp,"%s", myname);
			fscanf(fp,"%d",&myscore);
			//printw("%s %d",myname,myscore);
			insertnodeatback(myscore,myname,&head);
		}
	}
}

void rank(){
	// user code
	int x=0, y=0;
	int count=0;
	int i;
	int enter=0;
	node* travel;

	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	switch(wgetch(stdscr))
	{
		case '1':
			echo();
			printw("X : ");
			scanw("%d", &x);
			printw("Y : ");
			scanw("%d", &y);
			//printw("%d %d",x,y);	
			noecho();
			printw("       name       |   score   \n");
			printw("---------------------------------\n");

			if(y<x||y<0||x<0||x>ranknum||y>ranknum||head==NULL)
				printw("search failure: no rank in the list\n");

			else if(x==0&&y==0)
			{
				for(travel=head;travel!=NULL;travel=travel->next)
				{
					if(count==5)
						break;
						
					count++;
					//printw("%d", ranknum);
					fflush(stdin);
					printw("%s",travel->name);
					move(7+enter,18);
					enter++;
					printw("| %d\n",travel->score);

				}
			}

			else if(x==0&&y!=0)
			{
				for(travel=head;travel!=NULL;travel=travel->next)
				{
					if(count==y)
						break;

					count++;

					printw("%s",travel->name);
					move(7+enter,18);
					enter++;
					printw("| %d\n",travel->score);
				}
			}

			else if(x!=0&&y==0)
			{
				for(travel=head;travel!=NULL;travel=travel->next)
				{
					if(count==x+5)
						break;
					if(count==ranknum)
						break;
					count++;
					if(count>=x)
					{
					printw("%s",travel->name);
					move(7+enter,18);
					enter++;
					printw("| %d\n",travel->score);
					}
				}
			}

			else if(x!=0&&y!=0)
			{
				for(travel=head;travel!=NULL;travel=travel->next)
				{
					if(count==y)
						break;
					
					count++;
					if(count>=x)
						{
							printw("%s",travel->name);
							move(7+enter,18);
							enter++;
							printw("| %d\n",travel->score);
						}
				}
			}
			getch();
		break;
	}	
}

void writeRankFile(){
	// user code
	FILE* fp = fopen("rank.txt","w");
	fprintf(fp,"%d\n",ranknum);
	int i;
	node* travel=head;

	for(i=0;i<ranknum;i++)
	{
		fprintf(fp,"%s %d\n",travel->name,travel->score);
		travel=travel->next;
	}

	fclose(fp);
}

void newRank(int score){
	// user code
	char yourname[NAMELEN];
	clear();
	echo();
	printw("your name: ");
	scanw("%s",yourname);
	noecho();
	ranknum++;
	insertnodecompare(score,yourname,&head);
	
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y-1,x,blockID,blockRotate,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	if(CheckToMove(field,blockID,blockRotate,y,x)==1){
		DrawBlock(y,x,blockID,blockRotate,' ');
		DrawShadow(y,x,blockID,blockRotate);
	}
}

void insertnodecompare(int score, char name[NAMELEN], node** head)
{
	node* new_node;
	node* prev;
	new_node = (node*)malloc(sizeof(node));
	new_node->score=score;
	strcpy(new_node->name,name);
	//printf("%s\n",name);
	new_node->next=NULL;
	node* travel= *head;

	if(*head==NULL)
	{
		*head=new_node;
		
		return;
	}

	else
	{
		prev=travel;
		if(travel->score<score)
		{
			new_node->next=travel;
			*head=new_node;
			
			return;
		}
		
		while((travel->score >=  score)&&(travel->next != NULL))
		{
			prev=travel;
			travel=travel->next;
		}
		if(travel->next != NULL){
		new_node->next=travel;
		prev->next=new_node;
		}
		else{
		travel -> next = new_node;
		}
		
	}
}

void insertnodeatback(int score, char name[NAMELEN], node** head)
{
	node* new_node = (node*)malloc(sizeof(node));
	node* last = *head;

	new_node->score=score;
	strcpy(new_node->name,name);
	new_node->next=NULL;

	if(*head==NULL)
	{
		*head = new_node;
		
		return;
	}

	while(last->next!=NULL)
	{
		last=last->next;
	}

	last->next=new_node;
	
}

