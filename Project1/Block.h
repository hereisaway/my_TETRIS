#include<vector>
using namespace std;
#ifndef BLOCK_H
#define BLOCK_H
struct point {
	int row, col;
};

class Block
{
public:
	Block();
	void drop();//下落
	void floatmove(int dis);//水平移动
	void rotate();//旋转
	void paint(int leftborder, int upborder);//显示方块
	static IMAGE** getimgs();//传递原始块图片
	point* getloc();//获取坐标，坐标是私有成员，需要接口给别的类使用
	void solidarity(vector<vector<int>>& map);//固化，传地图引用
	Block& operator=(const Block& other);//重载Block的复制
	int getblocktype();//获取方块类型
private:
	int blocktype;//方块类型
	point loc[4];//四个格子的坐标
	IMAGE* img;//这四个格子的图片
	static IMAGE* imgs[7];//原始图片
	static int size;//方块大小
};

#endif // !BLOCK_H


