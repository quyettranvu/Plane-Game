#include<windows.h>
#include<glut.h>
#include<gl/GLAUX.H>
#pragma comment(lib, "glaux.lib") 
//#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include<string>
#include <stdlib.h>
#include<Windows.h>
#include<mmsystem.h>

//#define rad (3.1416/180)
#define EN_SIZE 20 
#include "RGBpixmap.h"
#include "Rain_Effect.h"


using namespace std;

//Việc sử dụng pixmap như 1 cách gán hình ảnh 2D,3D lên vật thể(hiện tại mình sẽ dùng glTexture
RGBpixmap pix[6];

//Task:gán mác ảnh(Bằng glTexture cho đẹp) vào nền

/*Giải thích ý nghĩa những tham số được sử dụng:
	-color1,color2,color3:tham số cho việc thiết lập đổi màu
	-r[],g[],b[]:mảng chứa 10 giá trị thiết lập tô màu cho nhà
	-EN_SIZE: qui định kích thước cho việc xác định ground(nếu tăng lên sẽ khiến hoạt cảnh chậm lại)
	-torusPosx, torusPosy là những mảng chứa tọa độ x,y của những vòng xuyến trong môi trường
	-tola:một mảng hai chiều chứa những giá trị set up tương ứng cho tham số trong việc xây nhà(hàm house,singletolaHouse)
	-TIME:giá trị được gán bằng t với t được gán khởi đầu mỗi khi init lại
	-rotX,rotY,rotZ:những giá trị quy định quay theo chiều x,y,z của máy bay
	-speed:giá trị thiết lập cho vận tốc của máy bay
	-tZ cho đến tZ6:giá trị thiết lập cho những biến làm việc với vận tốc tương ứng với mỗi lần môi trường thay đổi theo tZ
	-angleBackFrac:góc thay đổi tương ứng cho rotX,rotY,rotZ của máy bay
	-START:giá trị set up bắt đầu chương trình
	-rot:giá trị set up false ban đầu cho việc kiểm soát việc quay màn hình máy bay
	-zoom:giá trị set up mỗi lần mình phóng to gần vật thể
	-tX,tY:vị trí của máy bay
	-angle:giá trị set up thay đổi góc máy bay khi mình ấn các nút di chuyển


*/
float zoom = 4;
int tola[5000][5000];
float tX = 0, tY = 0, tZ = -8, rX = 0, rY = 0, rZ = 4;
float tZ1 = -20, tZ2 = -40, tZ3 = -60, tZ4 = -80, tZ5 = -100, tZ6 = -120;
float rotX = 0, rotY = 0, rotZ = 0;
//float cosX = 0, cosY = 1, cosZ = 0;
float angle = 0;
//float xEye = 0.0f, yEye = 5.0f, zEye = 30.0f;
//float cenX = 0, cenY = 0, cenZ = 0, roll = 0;
//float radius = 0;
//float theta = 0, slope = 0;
float speed = 0.3;
float angleBackFrac = 0.2;
bool saheedMinarVisible = false;
float r[] = { 0.1,0.4,0.0,0.9,0.2,0.5,0.0,0.7,0.5,0.0 };
float g[] = { 0.2,0.0,0.4,0.5,0.2,0.0,0.3,0.9,0.0,0.2 };
float b[] = { 0.4,0.5,0.0,0.7,0.9,0.0,0.1,0.2,0.5,0.0 };
int TIME = 0;
bool START = false;
float torusPosX[7] = { 1,-2,3,-4,-2,0,2 };
float torusPosY[7] = { 2,3,10,6,7,4,1 };
float color1 = 0.0f;
float color2 = 0.0f;
float color3 = 0.0f;

bool rot = false;

 
 
//GLuint g_box;                          // sử dụng để tạo display list cho đối tượng.
GLuint texture;                        // quản lý texture
char texture_name[100]={"brics.bmp"}; // lưu trữ tên file texture

//Hàm Load Bitmap
AUX_RGBImageRec *LoadBMP(char *Filename)
{
	FILE *File = NULL;

	if (!Filename)
		return NULL;
	fopen_s(&File, Filename, "r");
	if (File)
	{
		fclose(File);
		return auxDIBImageLoadA((LPCSTR)Filename);
	}
	return NULL;
}

//Hàm Load Texture
bool LoadGLTextures()
{
	int ret = false;
	AUX_RGBImageRec *texture_image = NULL;

	if (texture_image = LoadBMP(texture_name))
	{
		glGenTextures(1, &texture);  // Bắt đầu quá trình gen texture.
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//map dữ liệu bit map vào texture.
		glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_image->sizeX,
			texture_image->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, texture_image->data);
	}
	else
	{
		ret = false;
		if (texture_image)
		{
			if (texture_image->data)
				free(texture_image->data);
			free(texture_image);
		}
	}
	return ret;
}

static void resize(int width, int height)
{
	const float ar = (float)width / (float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//Thực hiện vẽ bộ cột điện năng lượng mặt trời(một lưu ý chung ở đây là mình đều chỉ tăng size cho đối tượng tương đối theo chiều tương ứng(nên bản thân giá trị của nó không lớn mà chỉ lớn so với các chiều  khác)
void drawShohidMinar() {

	//thực hiện vẽ bục(lần lượt từ bục dưới lên cao)
	glColor3d(0.4, 0.2, 0.2);//sử dụng màu nâu
	glPushMatrix();
	glTranslated(0, 1.55, 0);
	glScaled(2, 0.05, 1.5);
	glutSolidCube(1);
	glPopMatrix();

	glColor3d(0.4, 0.2, 0.2);
	glPushMatrix();
	glTranslated(0, 1.6, 0);
	glScaled(1.9, 0.05, 1.4);
	glutSolidCube(1);
	glPopMatrix();

	//glColor3d(0.4,0.2,0.2);
	//glColor3d(1,0.8,0.7);
	glColor3d(0.4, 0.2, 0.2);
	glPushMatrix();
	glTranslated(0, 1.65, 0);
	glScaled(1.8, 0.05, 1.3);
	glutSolidCube(1);
	glPopMatrix();

	/// bệ đỡ tấm mặt trời
	glColor3d(1, 1, 1);
	glPushMatrix();
	glTranslated(0, 1.68, -0.4);
	glScaled(0.5, 0.02, 0.08);
	glutSolidCube(1);
	glPopMatrix();

	/// Cột điện ở giữa(của tấm bảng mặt trời chính)
	glPushMatrix();
	glTranslated(0, 1.99, -0.4);
	glScaled(0.06, 0.7, 0.04);
	glutSolidCube(1);
	glPopMatrix();

	/// Dây điện đứng của cột điện này(thứ tự vẽ:3 dây điện trên xong 3 dây điện dưới)
	
	glColor3d(0, 0, 0);

	glPushMatrix();
	glTranslated(0.07, 1.99, -0.4);
	glScaled(0.003, 0.7, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.11, 1.99, -0.4);
	glScaled(0.003, 0.7, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.15, 1.99, -0.4);
	glScaled(0.003, 0.7, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	///
	glPushMatrix();
	glTranslated(-0.22, 0, 0);
	glPushMatrix();
	glTranslated(0.07, 1.99, -0.4);
	glScaled(0.003, 0.7, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.11, 1.99, -0.4);
	glScaled(0.003, 0.7, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.15, 1.99, -0.4);
	glScaled(0.003, 0.7, 0.003);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	///Dây điện ngang của cột điện này
	glPushMatrix();
		glTranslated(2.2, 0, -0.1); //thiết lập lại vị trí so với dây điện đứng
		glScaled(4.2, 1, 1);
		glColor3d(0, 0, 0);

		glPushMatrix();
		glTranslated(-0.528, 1.85, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 2.02, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 2.18, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();
		glColor3d(1, 1, 1);
	glPopMatrix();

	///Dây điện đứng ngoài cùng nhất của cột điện này
	glColor3d(1, 1, 1);
	glPushMatrix();
	glTranslated(-0.22, 1.99, -0.4);
	glScaled(0.06, 0.7, 0.04);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.22, 1.99, -0.4);
	glScaled(0.06, 0.7, 0.04);
	glutSolidCube(1);
	glPopMatrix();

	/// Xây dựng những cột điện nằm chéo phía trên của bệ pin mặt trời
	glPushMatrix();
		glTranslated(0, 0.743, -1.424); //thiết lập lại vị trí so với những cột điện đứng
		glRotated(45, 1, 0, 0);

	//glColor3d(1,0,1);
		glPushMatrix();
		glTranslated(0, 1.99, -0.4);
		glScaled(0.06, 0.3, 0.04);
		glutSolidCube(1);
		glPopMatrix();
		//glColor3d(1,1,1);

		glPushMatrix();
		glTranslated(-0.22, 1.99, -0.4);
		glScaled(0.06, 0.3, 0.04);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.22, 1.99, -0.4);
		glScaled(0.06, 0.3, 0.04);
		glutSolidCube(1);
		glPopMatrix();

		//Xây dựng thanh chắn nằm ngang của cột điện phía trên
		glPushMatrix();
		glTranslated(0, 2.15, -0.4);
		glScaled(0.5, 0.04, 0.04);
		glutSolidCube(1);
		glPopMatrix();

	///Xây dựng những dây điện đứng cho cột điện ở phía trên
	
	glColor3d(0, 0, 0);

		glPushMatrix();
		glTranslated(0.07, 1.99, -0.4);
		glScaled(0.003, 0.277, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.11, 1.99, -0.4);
		glScaled(0.003, 0.277, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.15, 1.99, -0.4);
		glScaled(0.003, 0.277, 0.003);
		glutSolidCube(1);
		glPopMatrix();

	glColor3d(0, 0, 0);
		glPushMatrix();
		glTranslated(-0.22, 0, 0);//thiết lập lại vị trí  của những dây điện đứng ở phía bên kia

		glPushMatrix();
		glTranslated(0.07, 1.99, -0.4);
		glScaled(0.003, 0.277, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.11, 1.99, -0.4);
		glScaled(0.003, 0.277, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.15, 1.99, -0.4);
		glScaled(0.003, 0.277, 0.003);
		glutSolidCube(1);
		glPopMatrix();
	glPopMatrix();

	///Xây dựng những dây điện nằm ngang cho những tấm bảng phía trên(gồm 2 dây điện nằm ngang chính giữa)
	glPushMatrix();
		glTranslated(2.2, 0, -0.1);
		glScaled(4.2, 1, 1);
		glColor3d(0, 0, 0);

		glPushMatrix();
		glTranslated(-0.528, 1.85, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 2, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		/*glPushMatrix();
		glTranslated(-0.528, 2.15, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();*/
		glColor3d(1, 1, 1);
		glPopMatrix(); //câu lệnh này liên kết với glPushMatrix() ở phía trên trong phần khung phía trên
	glPopMatrix();


	/*     BẮT ĐẤU XÂY DỰNG NHỮNG CỘT ĐIỆN PHỤ NẰM PHÍA BÊN CẠNH CỦA CỘT ĐIỆN CHÍNH     */
	///Xây dựng cột điện nằm bên trái thứ nhất (theo thứ tự build khối cạnh đứng trước xong đến khối cạnh nằm ngang phía dưới
		glColor3d(1, 1, 1);
	glPushMatrix();
		glTranslated(0.1, 0, -0.4); //Bắt đầu thực hiện đặt cột điện nghiêng góc 45 độ so với cột điện chính
		glRotated(45, 0, 1, 0);

		glPushMatrix();
		glTranslated(-0.605, 1.94, -0.3);
		glScaled(0.045, 0.65, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.45, 1.94, -0.3);
		glScaled(0.045, 0.65, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 2.258, -0.3);
		glScaled(0.199, 0.04, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 1.68, -0.3);
		glScaled(0.199, 0.02, 0.06);
		glutSolidCube(1);
		glPopMatrix();

	///Xây dựng dây điện đứng cho cột điện này
	glColor3d(0, 0, 0);
	glPushMatrix();
		glTranslated(-0.64, -0.05, 0.1);
		glScaled(1, 1.02, 1); 

		glPushMatrix();
		glTranslated(0.078, 1.99, -0.4);
		glScaled(0.003, 0.56, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.11, 1.99, -0.4);
		glScaled(0.003, 0.56, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.145, 1.99, -0.4);
		glScaled(0.003, 0.56, 0.003);
		glutSolidCube(1);
		glPopMatrix();
	glPopMatrix();

	///Dây điện nằm ngang của cột điện này
		glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(-0.528, 1.85, -0.3);
	glScaled(0.1, 0.003, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.528, 2, -0.3);
	glScaled(0.1, 0.003, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.528, 2.15, -0.3);
	glScaled(0.1, 0.003, 0.003);
	glutSolidCube(1);
	glPopMatrix();
	glColor3d(1, 1, 1);
	glPopMatrix();

	///Xây dựng cột điện nằm bên trái thứ hai
		glPushMatrix();
	glTranslated(0.65, 0, 0.3);
	glRotated(-45, 0, 1, 0);

	glPushMatrix();
	glTranslated(-0.605, 1.94, -0.3);
	glScaled(0.045, 0.65, 0.03);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.45, 1.94, -0.3);
	glScaled(0.045, 0.65, 0.03);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.528, 2.258, -0.3);
	glScaled(0.199, 0.04, 0.03);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.528, 1.68, -0.3);
	glScaled(0.199, 0.02, 0.06);
	glutSolidCube(1);
	glPopMatrix();

	///Xây dựng dây điện đứng cho cột điện này
		glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(-0.64, -0.05, 0.1);
	glScaled(1, 1.02, 1);
	glPushMatrix();
	glTranslated(0.078, 1.99, -0.4);
	glScaled(0.003, 0.56, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.11, 1.99, -0.4);
	glScaled(0.003, 0.56, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.145, 1.99, -0.4);
	glScaled(0.003, 0.56, 0.003);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();
	glColor3d(1, 1, 1);

	///Xây dựng dây điện ngang cho cột điện này
	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(-0.528, 1.85, -0.3);
	glScaled(0.1, 0.003, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.528, 2, -0.3);
	glScaled(0.1, 0.003, 0.003);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.528, 2.15, -0.3);
	glScaled(0.1, 0.003, 0.003);
	glutSolidCube(1);
	glPopMatrix();
	glColor3d(1, 1, 1);

	glPopMatrix();


	///Xây dựng cột điện nằm bên phải thứ nhất
	
	glPushMatrix();
	/// pasher piller left 1
	glTranslated(0.06, 0, 0.14);
		glPushMatrix();

		glTranslated(-0.2, 0, -0.31);
		glRotated(45, 0, 1, 0);

		glPushMatrix();
		glTranslated(-0.605, 1.88, -0.3);
		glScaled(0.045, 0.4, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.45, 1.88, -0.3);
		glScaled(0.045, 0.4, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 2.08, -0.3);
		glScaled(0.2, 0.04, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 1.68, -0.3);
		glScaled(0.199, 0.02, 0.06);
		glutSolidCube(1);
		glPopMatrix();

	///Xây dựng dây điện đứng cho cột điện này
	glColor3d(0, 0, 0);
	glPushMatrix();
		glTranslated(-0.641, 0.43, 0.1);
		glScaled(1, 0.73, 1);
			glPushMatrix();
			glTranslated(0.078, 1.99, -0.4);
			glScaled(0.003, 0.56, 0.003);
			glutSolidCube(1);
			glPopMatrix();

			glPushMatrix();
			glTranslated(0.11, 1.99, -0.4);
			glScaled(0.003, 0.56, 0.003);
			glutSolidCube(1);
			glPopMatrix();

			glPushMatrix();
			glTranslated(0.145, 1.99, -0.4);
			glScaled(0.003, 0.56, 0.003);
			glutSolidCube(1);
			glPopMatrix();
	glPopMatrix();

	///Xây dựng dây điện nằm ngang cho cột điện này
	glColor3d(0, 0, 0);
		glPushMatrix();
		glTranslated(-0.528, 1.8, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 1.96, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();
		glColor3d(1, 1, 1);
	///ROD

	glPopMatrix();

	///Xây dựng cột điện nằm bên phải thứ hai
	glPushMatrix();
		glTranslated(0.83, 0, 0.39);
		glRotated(-45, 0, 1, 0);

		glPushMatrix();
		glTranslated(-0.605, 1.88, -0.3);
		glScaled(0.045, 0.4, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.45, 1.88, -0.3);
		glScaled(0.045, 0.4, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 2.1, -0.3);
		glScaled(0.199, 0.04, 0.03);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 1.68, -0.3);
		glScaled(0.199, 0.02, 0.06);
		glutSolidCube(1);
		glPopMatrix();


	///Xây dựng dây điện nằm ngang cho cột điện này
		glColor3d(0, 0, 0);
		glPushMatrix();
		glTranslated(-0.528, 1.8, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-0.528, 1.96, -0.3);
		glScaled(0.1, 0.003, 0.003);
		glutSolidCube(1);
		glPopMatrix();
		glColor3d(1, 1, 1);

		///Xây dựng dây điện đứng cho cột điện này
		glColor3d(0, 0, 0);
		glPushMatrix();
		glTranslated(-0.641, 0.43, 0.1);
		glScaled(1, 0.73, 1);
		glPushMatrix();
		glTranslated(0.078, 1.99, -0.4);
		glScaled(0.003, 0.56, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.11, 1.99, -0.4);
		glScaled(0.003, 0.56, 0.003);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.145, 1.99, -0.4);
		glScaled(0.003, 0.56, 0.003);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();
		glColor3d(1, 1, 1);

	glPopMatrix();


	glPopMatrix();

	/// Circle
	
	glColor3d(1, 0, 0);
	glPushMatrix();
	glTranslated(0, 2.1, -0.44);
	glScaled(0.35, 0.35, 0.01);
	glutSolidSphere(1, 50, 50);
	glPopMatrix();

	//glColor3d(0, 0, 0);
	//glPushMatrix();
	//glTranslated(-0.18, 1.9, -0.45);
	//glScaled(0.01, 0.5, 0.01);
	//glutSolidCube(1);
	//glPopMatrix();

	//glColor3d(0, 0, 0);
	//glPushMatrix();
	//glTranslated(0.18, 1.9, -0.45);
	//glScaled(0.01, 0.5, 0.01);
	//glutSolidCube(1);
	//glPopMatrix();


}


//Hàm thực hiện vẽ quạt của máy bay:nếu chỉ sử dụng khối thứ nhất trong 2 khối->1 cánh; sử dụng khối 2->2 cánh
void fan() {
	//glColor3d(0.5, 1, 0); 
	//glPushMatrix();
	//glTranslated(0, 0, 0);
	//glScaled(1, 1, 0.7);
	//glutSolidSphere(0, 30, 30);
	//glPopMatrix();

	glColor3d(0.5, 1, 0); //màu vàng lục
	glPushMatrix();
	glTranslated(2.5, 0, 0);
	//glRotated(5, 0, 1, 0);
	glScaled(0.2, 1.5, 0.05);
	glutSolidSphere(1, 30, 30);
	glPopMatrix();
}
void fan2()
{
	glColor3d(0.5, 1, 0);
	glPushMatrix();
	//glTranslated(0, 0, 0);
	glTranslated(2.5, 0, 0);
	//glRotated(-5, 0, 1, 0);
	//glRotated(90, 0, 0, 1);
	glScaled(0.2, 1.5, 0.05);
	glutSolidSphere(1, 30, 30);
	glPopMatrix();
}

//Hàm thay đổi màu của thân máy bay 
void timer(int value)
{
	angle += 3.0f;

	//changeColor?
	color1 += 0.001f;
	color2 += 0.003f;
	color3 += color2;
	if (color1 > 1.0)
		color1 = 0;
	if (color2 > 1.0)
		color2 = 0;
	if (color3 > 1.0)
		color3 = 0;

	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

//Hàm vẽ người

//Hàm vẽ mưa


/*Variables used in this function*/
/*
	t:value for getting the initialisations when starting the program
	a:calculating the angle of rotating fan





*/
//Hàm thực hiện cho việc vẽ máy bay
void plane() {
	const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	double a = t * 90.0; 

	/// Phần thân chính của máy bay:
	//glColor3d(0.5, 1, 0);
	glColor3f(color1, color2, color3); //gọi tới giá trị thay đổi màu của thân máy bay
	glPushMatrix();
	glTranslated(0, 0, 0);
	glScaled(3, 0.4, 0.5);
	glutSolidSphere(1, 30, 30);
	glPopMatrix();

	//Phần buồng lái:tại đây mình có thể set up cho người ngồi
	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(1.7, 0.1, 0);
	glScaled(1.5, 0.7, 0.8);
	//glRotated(40, 0, 1, 0);
	glutSolidSphere(0.45, 30, 30);
	glPopMatrix();

	///Phần cánh phải:việc sử dụng glRotated ở đây có dụng ý khi máy bay được thực hiện xoay vòng
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0, 0, 1.2);
	glRotated(-50, 0, 1, 0);
	glScaled(0.7, 0.1, 3);
	glRotated(25, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();


	//Phần động cơ dưới cánh bên phải
		//Phần động cơ dưới cánh(bên trong)
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(-0.3, -0.15, 1.5);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	//Phần động cơ dưới cánh(bên ngoài)
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0.2, -0.15, 0.9);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	/// FAN
//        glPushMatrix();
//            glTranslated(0,0,0.5);
//            glRotated(10*a,0,0,1);
//            glScaled(0.1,0.1,0.1);
//            fan();
//        glPopMatrix();


	

	///Phần cánh trái
		glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0, 0, -1.2);
	glRotated(50, 0, 1, 0);
	glScaled(0.7, 0.1, 3);
	glRotated(-25, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();

	//Phần động cơ dưới cánh bên trái
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(-0.3, -0.15, -1.5);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0.2, -0.15, -0.9);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();


	///Thực hiện vẽ đuôi máy bay
	glPushMatrix();
		glTranslated(-2.8, 0, 0);
		glScaled(0.8, 0.5, 0.3);

		///Đuôi máy bay phải
		glColor3d(0.8, 1, 0);
		glPushMatrix();
		glTranslated(0.4, 0, 1.5);
		glRotated(-30, 0, 1, 0);
		glScaled(0.7, 0.1, 3);
		glRotated(10, 0, 1, 0);
		glutSolidCube(1);
		glPopMatrix();

		///Đuôi máy bay trái
		glColor3d(0.8, 1, 0);
		glPushMatrix();
		glTranslated(0.4, 0, -1.5);
		glRotated(30, 0, 1, 0);
		glScaled(0.7, 0.1, 3);
		glRotated(-10, 0, 1, 0);
		glutSolidCube(1);
		glPopMatrix();
	glPopMatrix();

	///Phần đuôi trên
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(-2.7, 0.5, 0);
	glRotated(45, 0, 0, 1);
	glScaled(0.8, 2, 0.1);
	glRotated(-20, 0, 0, 1);//sử dụng này để nó nghiêng đẹp hơn
	glutSolidCube(0.5);
	glPopMatrix();

	//    glColor3d(0.8,1,0);
	//    glPushMatrix();
	//        glTranslated(-2.95,0.85,0);
	//        glRotated(90,0,1,0);
	//        glScaled(0.05,0.05,0.6);
	//        glutSolidTorus(0.5,0.5,50,50);
	//    glPopMatrix();


	///Quạt máy bay
				//If you want 1 fan:
			glPushMatrix();
				//glTranslated(0, 0, 0);
				glRotated(10 * a, 1, 0, 0);
				//glRotated(90,1,0,0);
				fan();
			glPopMatrix();

			////If you want 2 fans
		 //   glPushMatrix();
		 //       glTranslated(0,0,0);
			//	glPushMatrix();
		 //       glRotated(10*a,1,0,0);
		 //       //glRotated(90,1,0,0);
		 //       fan();
			//	glPopMatrix();
			//	glPushMatrix();
			//	glRotated(-(10 * a), 1, 0, 0);
			//	fan2();
			//	glPopMatrix();
		 //   glPopMatrix();
}

/*Hàm thực hiện cho việc vẽ nhà:singleTolaHouse và house*/
void singleTolaHouse(int R, int G, int B) {
	glColor3d(r[R % 11], g[G % 11], b[B % 11]);

	//Lần lượt xây dựng các khối nhà
	//Khối nhà bé
	glPushMatrix();
	glTranslated(0, 0, 0);
	glutSolidCube(1);
	glPopMatrix();

	//Hai khối nhà cao vừa
	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(0.2, 0, 0);
	glScaled(0.3, 0.3, 1.001);
	glutSolidCube(1);
	glPopMatrix();

	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(-0.2, 0, 0);
	glScaled(0.3, 0.3, 1.001);
	glutSolidCube(1);
	glPopMatrix();

	//Hai khối nhà cao phía sau
	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(0, 0, 0.2);
	glScaled(1.001, 0.3, 0.3);
	glutSolidCube(1);
	glPopMatrix();

	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(0, 0, -0.2);
	glScaled(1.001, 0.3, 0.3);
	glutSolidCube(1);
	glPopMatrix();

}

/*Giải thích giá trị n của hàm house:*/
void house(int n, int R, int G)
{
	for (int i = 0; i < n; i++) 
	{
		glPushMatrix();
		glTranslated(0, 0.8+i, 0); //Giãn khoảng cách giữa các nhà(dịch theo chiều đứng-chiều y)
		singleTolaHouse(G, R, i); //Nếu như mình thay đổi thứ tự của G,R,i thì mình sẽ thu được những màu khác nhau
		glPopMatrix();
	}
}


//Thưc hiện vẽ 2 vùng đất đối nhau nơi đặt bệ cột điện
void soheedMinarEnv() {

	/// Vùng đất,bề mặt ở dưới
	glColor3d(0, 0.5, 0.1);
	glPushMatrix();
	glTranslated(0, 0, 0);
	glScaled(EN_SIZE * 2, 0.3, EN_SIZE * 2);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-8, -2.7, -5);
	glRotated(65, 0, 1, 0);
	//glRotated(15,0,1,0);
	glScaled(2, 2, 2);
	drawShohidMinar();
	glPopMatrix();

	glPushMatrix();
	glTranslated(8, -2.7, -5);
	glRotated(-65, 0, 1, 0);
	//glRotated(15,0,1,0);
	glScaled(2, 2, 2);
	drawShohidMinar();
	glPopMatrix();
}

//Thực hiện vẽ môi trường
void environment(int n) {

	//Gọi tới hàm vẽ mưa
	glPushMatrix();
	drawRain();
	glPopMatrix();

	///Gọi lại vùng đất, bề mặt ở dưới
	//glColor3d(0, 0.5, 0.1);
	LoadGLTextures();
	glPushMatrix();
	glTranslated(0, 0, 0);
	glScaled(EN_SIZE * 2, 0.3, EN_SIZE * 2);
	glTexCoord2f(EN_SIZE * 2, 0.3);
	glutSolidCube(1);
	glPopMatrix();


	//Thực hiện vẽ những vòng xuyến màu xanh lá cây
	glColor3d(0, 1, 0.1);
	glPushMatrix();
	glTranslated(torusPosX[n], torusPosY[n], 0);
	glScaled(0.3, 0.3, 0.3);
	glutSolidTorus(1, 3, 30, 30);
	glPopMatrix();

	//Thực hiện chạy khối lệnh để gọi tới những 
	for (int i = -(EN_SIZE / 2) + 1; i < (EN_SIZE / 2); i += 2) {
		for (int j = -(EN_SIZE / 2) + 1; j < (EN_SIZE / 2); j += 2) {
			if (tola[i + (EN_SIZE / 2) + 1][j + (EN_SIZE / 2) + 1] != 0) {
				glPushMatrix();
				glTranslated(i, 0, j);
				house(tola[i + (EN_SIZE / 2) + 1][j + (EN_SIZE / 2) + 1], i, j);
				glPopMatrix();

			}
			else if (i >= -5 && i <= 5) {
			}
			else {
				tola[i + (EN_SIZE / 2) + 1][j + (EN_SIZE / 2) + 1] = (rand() % 5) + 1;
				glPushMatrix();
				glTranslated(i, 0, j);
				house(tola[i + (EN_SIZE / 2) + 1][j + (EN_SIZE / 2) + 1], i, j);
				glPopMatrix();
			}
		}
	}



	// glColor3d(0,1,0.7);

 //    glPushMatrix();
 //        glRotated(angle,0,1,0);
 //        glPushMatrix();
 //            glTranslated(tX,tY,tZ);
 //            glScaled(1,1,2);
 //            //glRotated(90,1,0,0);
 //            glutSolidCube(1);
 //        glPopMatrix();
 //    glPopMatrix();
}


//Bắt đầu gọi tới hàm vẽ(kết hợp các thành phần lại với nhau)
void draw() {
	double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	double a = t * 90.0;

	TIME = t;

	///Gọi tới máy bay
		if (rotX > 11)rotX = 11;
	if (rotX < -11)rotX = -11;
	if (rotZ > 10)rotZ = 10;
	if (rotZ < -15)rotZ = -15;

	glPushMatrix();
	glTranslated(0, 1, 0);
	glRotated(90, 0, 1, 0);
	glRotated(5, 0, 0, 1);
	glRotated(rotX, 1, 0, 0);
	glRotated(rotY, 0, 1, 0);
	glRotated(rotZ, 0, 0, 1);

	glScaled(0.4, 0.4, 0.4);
	plane();
	glPopMatrix();


	///Gọi tới môi trường
		if (tX >= 4.1)tX = 4.1;
	if (tX <= -4.1)tX = -4.1;
	if (tY > 0.1)tY = 0.1;
	if (tY < -15)tY = -15;


	glPushMatrix();
	glTranslated(tX, tY, tZ);
	environment(2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(tX, tY, tZ1);
	soheedMinarEnv();
	glPopMatrix();

	glPushMatrix();
	glTranslated(tX, tY, tZ2);
	environment(3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(tX, tY, tZ3);
	environment(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(tX, tY, tZ4);
	environment(5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(tX, tY, tZ5);
	environment(4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(tX, tY, tZ6);
	environment(2);
	glPopMatrix();

	//Dĩ nhiên máy bay di chuyển với tốc độ bao nhiêu môi trường sẽ dịch theo như vậy
	tZ += speed;
	tZ1 += speed;
	tZ2 += speed;
	tZ3 += speed;
	tZ4 += speed;
	tZ5 += speed;
	tZ6 += speed;

	if (tZ >= 20)tZ = -110;
	if (tZ1 >= 20)tZ1 = -110;
	if (tZ2 >= 20)tZ2 = -110;
	if (tZ3 >= 20)tZ3 = -110;
	if (tZ4 >= 20)tZ4 = -110;
	if (tZ5 >= 20)tZ5 = -110;
	if (tZ6 >= 20)tZ6 = -110;

	if (rotX > 0)rotX -= angleBackFrac;
	if (rotX < 0)rotX += angleBackFrac;
	if (rotY > 0)rotY -= angleBackFrac;
	if (rotY < 0)rotY += angleBackFrac;
	if (rotZ > 0)rotZ -= angleBackFrac;
	if (rotZ < 0)rotZ += angleBackFrac;

	//cout<<tX<<" "<<tY<<" "<<tZ<<endl;
	//cout<<rotX<<" "<<rotY<<" "<<rotZ<<endl;

	speed += 0.0002;
	if (speed >= 0.7)speed = 0.7;
}

//Hàm vẽ đoạn text nằm chính giữa đoạn đường bay của máy bay(sử dụng cặp lệnh glRasterPos và glutBitmapCharacter)-theo kiểu bitmap
/*
  glRasterPos — Vị trí raster là vị trí trong tọa độ cửa sổ mà tại đó thao tác raster tiếp theo sẽ bắt đầu(giúp thực hiện viết texts trong openGL)
  void glutBitmapCharacter(void *font, int character);-hiển thị 1 kí tự bitmap trong openGL)
*/
void drawBitmapText(const char *str, float x, float y, float z)
{
	const char *c;
	glRasterPos3f(x, y + 8, z);

	for (c = str; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}

//Hàm viết đoạn text bằng những đường stroke(gạch nhẹ)
/*
void glutStrokeCharacter(void *font, int character);
*/
void drawStrokeText(const char* str, int x, int y, int z)
{
	const char *c;
	glPushMatrix();
		glTranslatef(x, y + 8, z);
		glScalef(0.002f, 0.002f, z);

		drawRain(); //đặt đây để thêm hiệu ứng mưa rơi cho dòng chữ
		for (c = str; *c != '\0'; c++)
		{
			glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
		}
	glPopMatrix();
}

//Tương tự như hàm drawStrokeText
void drawStrokeText2(const char* str, int x, int y, int z)
{
	const char *c;
	glPushMatrix();
	glTranslatef(x, y + 8, z);
	glScalef(0.005f, 0.005f, z);

	for (c = str; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();
}

//Hàm viết kí tự lẻ
void drawStrokeChar(char c, float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y + 8, z);
	glScalef(0.002f, 0.002f, z);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	glPopMatrix();
}

static void display(void)
{
	const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	double a = t * 90.0;


	double aa = a;
	
	//Nếu bỏ khối này đi là nhìn được xung quanh
	if (!rot) {
		a = 0;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(0.0, 4.5, 10.0,
		0, 4, 0,
		0, 1.0f, 0.0f);

	if (START) {
		glPushMatrix();
		glTranslated(0, 0, 0);
		glScaled(zoom, zoom, zoom);
		glRotated(a, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, texture);
		draw();
		glPopMatrix();


		drawStrokeText("UP: W, DOWN: S, LEFT: A, RIGHT: D, MAIN MENU: M", -8, 0.9, 0);
		drawBitmapText("Quyet Tran Vu's OpenGL Planing Game Code", 0, 0, 0);
		drawStrokeText("TIME : ", 3, 0, 0);

		int mod, number = 0;
		while (TIME) {
			mod = TIME % 10;
			number = number * 10 + mod;
			TIME /= 10;
		}
		//tmp là 1 giá trị để set up cho thời gian tăng lên đúng(tạo khoảng cách giữa các số)
		float tmp = 0;
		while (number) {
			mod = number % 10;
			//gọi hàm vẽ kí tự số(=số thời gian trôi qua)
			drawStrokeChar(mod + 48, 4 + tmp, 0, 0);
			number /= 10;
			tmp += 0.2;
		}
	}
	else {
		glPushMatrix();
		glTranslated(0, 3, 0);
		glRotated(aa, 0, 1, 0);
		glScaled(1.5, 1.5, 1.5);
		plane();
		glPopMatrix();


		drawStrokeText("Press G to Start", -1, -1, 0);
		drawStrokeText2("Plane Game", -2, 0, 0);
	}

	//glColor3d(1,1,0);
	//drawStrokeText("Osama Hosam's OpenGL Tutorials",200,200,0);

	//	glRasterPos2i(100, 120);
	//    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	//    glutBitmapString(GLUT_BITMAP_HELVETICA_18, "text to render");
	//drawStrokeChar(49,2,0,0);

	glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
	float frac = 0.3;
	float rotFrac = 1;
	switch (key)
	{
	case 27:
	case 'q':
		exit(0);
		break;
	//hai trường hợp hỗ trợ đổi rot để thay đổi góc nhìn
	case 'r':
		rot = true;
		break;
	case 't':
		rot = false;
		break;

	//hai trường hợp thay đổi hướng nhìn gần-xa
	case 'z':
		zoom += 0.05;
		break;
	case 'Z':
		zoom -= 0.05;

	//trường hợp di chuyển cho máy bay
	case 'w':
		tY -= frac;
		rotZ += rotFrac;
		break;
	case 's':
		tY += frac;
		rotZ -= rotFrac;
		break;
	case 'a':
		tX += frac;
		rotX -= rotFrac * 3;
		rotY += rotFrac / 2;
		break;
	case 'd':
		tX -= frac;
		rotX += rotFrac * 3;
		rotY -= rotFrac / 2;
		break;
		//        case 'y':
		//            rotX-=rotFrac;
		//            break;
		//        case 'h':
		//            rotX+=rotFrac;
		//            break;
		//        case 'g':
		//            rotY+=rotFrac;
		//            break;
		//        case 'j':
		//            rotY-=rotFrac;
		//            break;

	//hai trường hợp bắt đầu,kết thúc game
	case 'g':
		START = true;
		break;
	case 'm':
		START = false;
		break;
		//        case 'o':
		//            cosX-=frac*cos(rotX*rad);
		//            cosY+=frac*cos(rotY*rad);
		//            cosZ-=frac*cos(rotZ*rad);
		//            //cout<<"Front : "<<cosX<<" "<<cosY<<" "<<cosZ<<endl;
		//            break;
		//        case 'l':
		//            cosX+=frac*cos(rotX*rad);
		//            cosY-=frac*cos(rotY*rad);
		//            cosZ+=frac*cos(rotZ*rad);
		//            //cout<<"Back : "<<cosX<<" "<<cosY<<" "<<cosZ<<endl;
		//            break;
	}


	glutPostRedisplay();
}

static void idle(void)
{
	glutPostRedisplay();
}

//Sử dụng ánh sáng
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int main(int argc, char *argv[])
{
	initRain();
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1366, 720);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

	glutCreateWindow("GLUT Shapes-Fixed Project by Quyet");

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);
	glutKeyboardFunc(key);
	//glutIdleFunc(idle);

	//PlaySound(TEXT("[MP3DOWNLOAD.TO] Yiruma - River Flows in You-HQ.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
	sndPlaySound("[MP3DOWNLOAD.TO] Yiruma - River Flows in You-HQ.wav", SND_ASYNC);

	glClearColor(1, 1, 1, 1);
	glEnable(GL_CULL_FACE);//cho phép vật thể quay mặt trước,sau (lưu ý nếu mode là GL_FRONT_AND_BACK:không có đa giác được vẽ, chỉ có điểm và đường)
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	//Bằng việc sử dụng GL_LESS thì mình sẽ ko bị nhìn vật thể "RỖNG"
	glDepthFunc(GL_LESS);//Theo mặc định, hàm độ sâu GL_LESS được sử dụng để loại bỏ tất cả các đoạn có giá trị độ sâu cao hơn hoặc bằng giá trị của bộ đệm độ sâu hiện tại

	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE); //giúp cho các vecto được chuẩn hóa sau khi biến đổi và trước khi được chiếu sáng.
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glutMainLoop();

	return EXIT_SUCCESS;
}
