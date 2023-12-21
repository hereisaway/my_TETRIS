#include<vector>
#include<graphics.h>
#include"Block.h"
using namespace std;

#ifndef GAME_H
#define GAME_H

class Game
{
public:
	Game(int rows, int cols, int leftdis, int updis, int blocksize);
	void init();//初始化
	void play();//主逻辑

private:
	int getDelay();//获取延迟
	void updateWindow();//更新画面
	void keyEvent();//键盘输入
	void clearline();//清行
	void drop();//下落
	bool validblock(const vector<vector<int>>& map);//判断当前块是否合法
	void floatmove(int dis);//水平移动
	void rotate();//旋转
	void showscore();//展示分数
	void checkover();//判断结束
	void saveScore();//保存成绩
	void displayOver();//展示结束界面
private:
	bool update;//更新标志
	int delay;//
	int rows;//行数
	int cols;//列数
	int leftdis;//左边界到窗口左侧距离
	int updis;//上边界到窗口上侧距离
	int blocksize;//方块大小
	vector<vector<int>>map;//地图
	IMAGE background;//背景图片
	IMAGE loseimg;//失败图片
	IMAGE winimg;//胜利图片
	Block* curblock;//当前块，用指针，都指向几个原始方块，减少开销
	Block* nextblock;//下一个块
	Block backblock;//备份块
	int score;//成绩
	int level;//关卡数
	int linecnt;//总消除行数
	int highestSocre;//最高分
	bool gameover;//结束标志位
	bool win;//胜利标志位，1是胜利，0是失败
};

#endif // !GAME_H


