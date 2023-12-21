#include<graphics.h>
#include "Block.h"
#include<stdlib.h>
#include<vector>
using namespace std;

IMAGE* Block::imgs[7] = { NULL };//原始方块，初始为空
int Block::size = 19;//设置方块大小


Block::Block()
{
	//如果原始块是空的，先导入原始块
	if (imgs[0] == NULL) {
		IMAGE tmp;
		loadimage(&tmp, "res/tiles.png",152,19,1);//将七个原始块的图片导入

		SetWorkingImage(&tmp);
		for (int i = 0; i < 7; i++) {
			imgs[i] = new IMAGE;//申请空间
			getimage(imgs[i], i * size, 0, size, size);//切割图片放进原始块数组
		}
		SetWorkingImage();
	}
	int blocks[7][4] = {
		1,3,5,7,
		2,4,5,7,
		3,5,4,6,
		3,5,4,7,
		2,3,5,7,
		3,5,7,6,
		2,3,4,5
	};//设置各种块的形状

	blocktype = rand() % 7;//随机选一种

	for (int i = 0; i < 4; i++) {
		int v = blocks[blocktype][i];//用方块形状换算出在地图中坐标
		loc[i].col = v % 2;
		loc[i].row = v / 2;
	}

	img = imgs[blocktype];//导入这个类型方块的图片
}

void Block::drop()//下落
{
	for (int i = 0; i < 4; i++)
		loc[i].row++;//行数加一
}

void Block::floatmove(int dis)//水平位移
{
	for (int i = 0; i < 4; i++)
		loc[i].col += dis;//列数加上水平位移数
}

void Block::rotate()//旋转
{
	point center = loc[1];//取旋转中心
	for (int i = 0; i < 4; i++) {
		point tmp = loc[i];//保存原始坐标
		loc[i].col = center.col - tmp.row + center.row;//旋转
		loc[i].row = center.row + tmp.col - center.col;
	}
}

void Block::paint(int leftdis, int updis)//画出方块
{
	for (int i = 0; i < 4; i++) {
		int x = leftdis + loc[i].col * size;//用地图内坐标加上左边界位置，算出屏幕上坐标
		int y = updis + loc[i].row * size;
		putimage(x, y, img);//画出方块
	}
}

IMAGE** Block::getimgs()//传递原始块数组
{
	return imgs;
}

point* Block::getloc()//传递坐标数组
{
	return loc;
}

void Block::solidarity(vector<vector<int>>& map)//固化
{
	for (int i = 0; i < 4; i++) {
		map[loc[i].row][loc[i].col] = blocktype;//在地图上标记方块类型
	}
}

Block& Block::operator=(const Block& other)
{
	if (this == &other)return *this;//相同就返回自己

	this->blocktype = other.blocktype;//复制类型

	for (int i = 0; i < 4; i++) {
		this->loc[i] = other.loc[i];//复制坐标
	}
	return *this;
}

int Block::getblocktype()//获取类型
{
	return blocktype;
}


