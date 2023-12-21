#include<stdlib.h>
#include<time.h>
#include<conio.h>
#include "Game.h"
#include"Block.h"
#include<stdio.h>
#include<mmsystem.h>
#include<fstream>
#pragma comment(lib, "winmm.lib")
#define RECORD_FILE "record.txt"

const int maxlevel = 5;//最大关卡数
const int SPEED[maxlevel+1] = { 0,500,300,150,100,80 };//各关的下落速度

Game::Game(int rows, int cols, int leftdis, int updis, int blocksize)
{
	//把参数赋给成员
	this->blocksize = blocksize;
	this->rows = rows;
	this->cols = cols;
	this->updis = updis;
	this->leftdis = leftdis;

	//铺地图，开始都为-1表示空
	for (int i = 0; i <= rows; i++) {
		vector<int>rowmap;
		for (int j = 0; j <= cols; j++)
			rowmap.push_back(-1);
		map.push_back(rowmap);
	}
}

void Game::init()//初始化
{
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);//背景音乐循环播放

	gameover = 0;
	win = 0;//初始默认结束方式是失败
	score = 0;
	level = 1;
	linecnt = 0;
	delay = SPEED[level];//设置初始速度
	srand(time(NULL));

	//加载需要的图片
	loadimage(&background, "res/bg2.png",480,480,1);
	loadimage(&winimg, "res/win.png", 150, 100, 1);
	loadimage(&loseimg, "res/over.png", 150, 100, 1);

	initgraph(480, 480);//开窗口

	//初始化可以用来重开，因此把地图清空
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			map[i][j] = -1;

	//读取历史最高分
	ifstream file(RECORD_FILE);
	file >> highestSocre;
	file.close();
}

void Game::play()
{
	init();

	//生成当前块和下一个块
	nextblock = new Block;
	curblock = nextblock;
	nextblock = new Block;

	int timer = 0;
	//主循环
	while (1) {
		keyEvent();//读取键盘输入

		timer += getDelay();//计算延时

		//延时到了时限就刷屏
		if (timer > delay){
			timer = 0;
			update = 1;//更新
			drop();//下落一格
		}

		//更新标志位为1时更新
		if (update) {
			update = 0;//更新标志位反转
			clearline();//清行
			updateWindow();//更新屏幕
		}
		//结束标志位为1时结束
		if (gameover) {
			saveScore();//保存得分
			displayOver();//展示结束界面
			system("pause");//暂停
			init();//按任意键结束暂停，重开一局
		}
	}
}

int Game::getDelay()//获取延迟
{
	static unsigned long long lasttime = 0;//静态变量，计算从上次调用到现在的延迟
	unsigned long long curtime = GetTickCount();//获取当前时刻
	//第一次调用
	if (lasttime == 0) {
		lasttime = curtime;
		return 0;
	}
	else {
		int ret = curtime - lasttime;//计算延迟
		lasttime = curtime;//保存当前时刻
		return ret;
	}
}

void Game::updateWindow()//更新画面
{
	putimage(0, 0, &background);//显示背景，不知道为啥要在这里每次都刷新

	BeginBatchDraw();
	IMAGE** imgs = Block::getimgs();//获取几个格子的原始图片
	for(int i=0;i<rows;i++)
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == -1)continue;//空格不显示
			int x = j * blocksize + leftdis;
			int y = i * blocksize + updis;//计算坐标
			putimage(x, y, imgs[map[i][j]]);//根据颜色和坐标显示
		}

	curblock->paint(leftdis, updis);//显示当前空中的格子
	nextblock->paint(400, 150);//显示下一个格子

	showscore();//展示成绩
	EndBatchDraw();

}

void Game::keyEvent()
{
	bool rotateflag = 0;//旋转标志位置零
	int dx=0;//水平位移置零
	unsigned char t;
	if (_kbhit()) {
		t = _getch();//读一个字符
		//如果是224，说明是上下左右四个键
		if (t == 224) {
			t = _getch();//再读一个，判断到底是哪个
			if (t == 72)rotateflag = 1;//上旋转
			else if (t == 80)delay = SPEED[5];//下加速
			else if (t == 75)dx = -1;//左往左一格
			else if (t == 77)dx = 1;//→往右一格
		}
	}
	//旋转
	if (rotateflag) {
		rotate();
		update = 1;//旋转后立即更新
	}
	//位移
	if (dx != 0) {
		floatmove(dx);
		update = 1;//有位移也立即更新
	}
}

void Game::clearline()//清行
{
	int k = rows - 1;
	int lines = 0;//清除行数还原为0
	for (int i = rows - 1; i >= 0; i--) {
		int cnt = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j] != -1)cnt++;//统计当前行方块数
			map[k][j] = map[i][j];//用当前行覆盖结果指针的最后一行
		}
		if (cnt < cols)k--;//如果这一行不都是格子，结果指针往后一位，不覆盖这一行，相当于不动
		else lines++;//不都是格子，就结果指针不动，接下来覆盖这一行，同时记录这次消掉的行数
	}
	//如果消掉了，更新分数
	if (lines) {
		linecnt += lines;//更新总消行数
		int reward[4] = { 10,30,50,80 };//消掉不同行数得分不同

		score += reward[lines - 1];
		if (score > highestSocre)highestSocre = score;//更新最高分

		level = score / 100 + 1;//根据分数判断关卡
		if (level > maxlevel)gameover = 1,win=1;//如果过了最后一关，结束

		update = 1;//更新屏幕

		mciSendString("play res/xiaochu2.mp3", 0, 0, 0);//播放消行音效
	}
}

void Game::drop()//下落
{
	backblock = *curblock;//备份
	curblock->drop();//下落
	
	//如果下落不合法
	if (!validblock(map)) {
		delay = SPEED[level];//更新速度
		backblock.solidarity(map);//固化当前块
		delete curblock;//删掉当前块
		curblock = nextblock;//把下一个块赋给当前块
		nextblock = new Block;//创建下一个块
		checkover();//检查新的当前块是否会游戏结束
	}
}

bool Game::validblock(const vector<vector<int>>& map)//检查当前块是否合法，传入地图 
{
	point* loc = curblock->getloc();//获取当前块的坐标
	for (int i = 0; i < 4; i++) {
		if (loc[i].col < 0 || loc[i].row < 0 || 
			loc[i].row >= rows || loc[i].col >= cols)//出界
			return 0;
		if (map[loc[i].row][loc[i].col] != -1)return 0;//重叠都不合法
	}
	return 1;
}

void Game::floatmove(int dis)//水平移动，传入方向
{
	backblock = *curblock;//机制同下落，先备份再操作
	curblock->floatmove(dis);
	if (!validblock(map)) {
		*curblock = backblock;//如果新的位置不合法，还原
	}
}

void Game::rotate()//旋转
{
	if (curblock->getblocktype() == 6)return;//正方形不能旋转
	backblock = *curblock;//备份
	curblock->rotate();//操作
	if (!validblock(map)) {
		*curblock = backblock;//操作后不合法，还原
	}
}

void Game::showscore()//展示分数
{
	char scoretxt[32];
	sprintf_s(scoretxt, sizeof(scoretxt), "%d", score);//传入成绩

	setcolor(RGB(180, 180, 180));//白色

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 25;//字大小
	f.lfWidth = 15;
	f.lfQuality = ANTIALIASED_QUALITY;
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));//字体
	settextstyle(&f);

	outtextxy(340, 390, scoretxt);//输出成绩

	sprintf_s(scoretxt, sizeof(scoretxt), "%d", level);
	outtextxy(70, 395, scoretxt);//输出关卡数

	sprintf_s(scoretxt, sizeof(scoretxt), "%d", linecnt);
	outtextxy(70, 440, scoretxt);//输出输出总消行数

	sprintf_s(scoretxt, sizeof(scoretxt), "%d", highestSocre);
	outtextxy(340, 440, scoretxt);//输出最高分
}

void Game::checkover()//结束判断
{
	gameover = !validblock(map);//当前块没进行操作就不合法，也就是没位置了，结束
}

void Game::saveScore()//保存分数
{
	ofstream file(RECORD_FILE);//保存最高分到文件里
	file << highestSocre;
	file.close();
}

void Game::displayOver()
{
	//如果是通关了
	if (win) {
		putimage(150, 150, &winimg);//显示胜利图片
		mciSendString("play res/win.mp3", 0, 0, 0);//播放胜利音效
	}
	//如果是失败了
	else {
		putimage(150, 150, &loseimg);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
}
