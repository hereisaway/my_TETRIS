#include"Game.h"
#include"Block.h"
int main(void) {
	Game Game(20, 10, 130, 75, 19);//设置行列数，边界，块大小
	Game.play();
	return 0;
}