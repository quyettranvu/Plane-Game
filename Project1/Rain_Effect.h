#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include<glut.h>

#define RAINSIZE 1000 //biến quy định mật độ hạt mưa(nếu càng lớn->lượng mưa rơi càng rõ)
int winWidth = 1000, winHeight = 1000;
//int counter = 0;
//time_t t;
float rotationAngle = 0; //thuộc tính định nghĩa góc rơi của mưa

//tạo 1 cấu trúc chứa những thuộc tính liên quan đến độ rơi của mưa(tọa độ;chuyển động;bán kính; độ dài; góc rơi;góc chuyển động)
struct drop {
	float x = 400;
	float y = 400;
	float inc = 0.01;
	float radius = 10;
	float scale = 1.0;
	float rotationAngle = 0;
	float rotationInc = 1;
};

drop rain[RAINSIZE];

//Bằng việc sử dụng hàm rand() mình sẽ tạo những giá trị bất kì
void initRain() {
	//srand((unsigned)time(&t));
	for (int i = 0; i < RAINSIZE; i++) {
		rain[i].x = rand() % winWidth;
		rain[i].y = rand() % winHeight;
		rain[i].inc = 1.5 + (float)(rand() % 100) / 1000.0;
		rain[i].radius = (float)(rand() % 8);
		rain[i].scale = (float)(rand() % 20000) / 1000.0;
		rain[i].rotationAngle = (float)(rand() % 3000) / 1000.0;
		rain[i].rotationInc = (float)(rand() % 100) / 1000.0;
		if ((rand() % 100) > 1000) {
			rain[i].rotationInc = -rain[i].rotationInc;
		}
	}
}

//Thực hiện vẽ mưa(nối 2 điểm lại bằng đường thẳng)(lưu ý là mình sẽ nối giữa 2 tọa độ điểm (x,y) và điểm (x,y+bán kính đến điểm thứ hai))
void drawParticleShape(int i) {
	glBegin(GL_POINTS);
	glVertex2d(rain[i].x, rain[i].y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2d(rain[i].x, rain[i].y);
	glVertex2d(rain[i].x, rain[i].y + rain[i].radius * 2);
	glEnd();
}

//Thực hiện tạo rơi
void drawDrop(int i) {
	glColor3f(0.0, 0.0, 1.0); //màu xanh lơ
	glLineWidth(2);
	drawParticleShape(i);
	rain[i].y -= rain[i].inc; //mỗi khi mưa rơi hết đoạn chiều dài thì mình cho nó trừ đi lại giá trị chuyển động và gán lại giá trị height ban đầu cho hạt mưa
	if (rain[i].y < 0) {
		rain[i].y = winHeight; 
	}
}

//Gọi hàm mưa bằng việc sử dụng vòng lặp liên quan đến mật độ mưa
void drawRain() {
	for (int i = 0; i < RAINSIZE; i++) {
		drawDrop(i);
	}
}
