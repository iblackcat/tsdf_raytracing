#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#define _USE_MATH_DEFINES
#include <math.h>


// A macro to disallow the copy constructor and operator= functions 
// This should be used in the priavte:declarations for a class
#define    DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);                \
    TypeName& operator=(const TypeName&)

namespace mf {

typedef unsigned int u32;
typedef unsigned char u8;

class float3 {
public:
	float x, y, z;
	float3(float a, float b, float c) {
		x = a; y = b; z = c;
	}
	float3(float f[3]) {
		x = f[0]; y = f[1]; z = f[2];
	}
	float3() {
		x = 0.0; y = 0.0; z = 0.0;
	}
	float module() {
		return (float)sqrt(x*x + y*y + z*z);
	}
	float3 norm() {
		float tmp = (float)sqrt(x*x + y*y + z*z);
		if (tmp == 0) return float3(0, 0, 0);
		else return float3(x / tmp, y / tmp, z / tmp);
	}
	void operator=(float3 f) {
		x = f.x; y = f.y; z = f.z;
	}
	float& operator[](int i) {
		switch (i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
	}
	float3 operator-() {
		return float3(-x, -y, -z);
	}
	float3 operator-(float3 f) {
		return float3(x - f.x, y - f.y, z - f.z);
	}
	float3 operator+(float3 f) {
		return float3(x + f.x, y + f.y, z + f.z);
	}
	float operator*(float3 f) {
		return x*f.x + y*f.y + z*f.z;
	}
	float3 cross(float3 f) {
		return float3(y*f.z - z*f.y, z*f.x - x*f.z, x*f.y - y*f.x);
	}
	void print() {
		printf("%f %f %f\n", x, y, z);
	}
	void print(const char *str) {
		printf("%s: %f %f %f\n", str, x, y, z);
	}
};

class Matrix33 {
public:
	float3 data[3];
	Matrix33(float f[3][3]) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = f[i][j];
	}
	Matrix33(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
		data[0] = float3(a, b, c);
		data[1] = float3(d, e, f);
		data[2] = float3(g, h, i);
	}
	Matrix33(float3 x, float3 y, float3 z) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}
	Matrix33() {
		data[0] = float3(0.f, 0.f, 0.f);
		data[1] = float3(0.f, 0.f, 0.f);
		data[2] = float3(0.f, 0.f, 0.f);
	}
	void operator=(Matrix33 m) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = m[i][j];
	}
	float3& operator[](int i) {
		switch (i)
		{
		case 0: return data[0];
		case 1: return data[1];
		case 2: return data[2];
		}
	}
	Matrix33 operator*(Matrix33 m) {
		Matrix33 tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][0] * m[0][j] + data[i][1] * m[1][j] + data[i][2] * m[2][j];
		return tmp;
	}
	float3 operator*(float3 f) {
		float3 tmp;
		for (int i = 0; i < 3; i++)
			tmp[i] = data[i][0] * f[0] + data[i][1] * f[1] + data[i][2] * f[2];
		return tmp;
	}
	Matrix33 operator*(float d) {
		Matrix33 tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][j] * d;
		return tmp;
	}
	Matrix33 operator/(float d) {
		Matrix33 tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][j] / d;
		return tmp;
	}
	Matrix33 operator-(Matrix33 m) {
		Matrix33 tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][j] - m[i][j];
		return tmp;
	}
	Matrix33 operator+(Matrix33 m) {
		Matrix33 tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = m[i][j] + data[i][j];
		return tmp;
	}
	void print() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				printf("%f ", data[i][j]);
			}
			printf("\n");
		}
	}
	void print(const char *str) {
		printf("%s:\n", str);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				printf("%f ", data[i][j]);
			}
			printf("\n");
		}
	}
	Matrix33 inv() {
		Matrix33 tmp;
		float a1 = data[0][0], a2 = data[1][0], a3 = data[2][0];
		float b1 = data[0][1], b2 = data[1][1], b3 = data[2][1];
		float c1 = data[0][2], c2 = data[1][2], c3 = data[2][2];

		float A = a1*(b2*c3 - c2*b3) - a2*(b1*c3 - c1*b3) + a3*(b1*c2 - c1*b2);
#ifdef _DEBUG
		assert(A != 0.0);
#endif
		tmp[0][0] = b2*c3 - c2*b3; tmp[0][1] = c1*b3 - b1*c3; tmp[0][2] = b1*c2 - c1*b2;
		tmp[1][0] = c2*a3 - a2*c3; tmp[1][1] = a1*c3 - c1*a3; tmp[1][2] = a2*c1 - a1*c2;
		tmp[2][0] = a2*b3 - b2*a3; tmp[2][1] = b1*a3 - a1*b3; tmp[2][2] = a1*b2 - a2*b1;
		tmp = tmp / A;
		return tmp;
	}
	float det() {
		float a1 = data[0][0], a2 = data[1][0], a3 = data[2][0];
		float b1 = data[0][1], b2 = data[1][1], b3 = data[2][1];
		float c1 = data[0][2], c2 = data[1][2], c3 = data[2][2];

		float A = a1*(b2*c3 - c2*b3) - a2*(b1*c3 - c1*b3) + a3*(b1*c2 - c1*b2);
		return A;
	}
};

class double3 {
public:
	double x, y, z;
	double3(double a, double b, double c) {
		x = a; y = b; z = c;
	}
	double3(double f[3]) {
		x = f[0]; y = f[1]; z = f[2];
	}
	double3() {
		x = 0.0; y = 0.0; z = 0.0;
	}
	double module() {
		return (double)sqrt(x*x + y*y + z*z);
	}
	double3 norm() {
		double tmp = (double)sqrt(x*x + y*y + z*z);
		if (tmp == 0) return double3(0, 0, 0);
		else return double3(x / tmp, y / tmp, z / tmp);
	}
	void operator=(double3 f) {
		x = f.x; y = f.y; z = f.z;
	}
	double& operator[](int i) {
		switch (i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
	}
	double3 operator-() {
		return double3(-x, -y, -z);
	}
	double3 operator-(double3 f) {
		return double3(x - f.x, y - f.y, z - f.z);
	}
	double3 operator+(double3 f) {
		return double3(x + f.x, y + f.y, z + f.z);
	}
	double operator*(double3 f) {
		return x*f.x + y*f.y + z*f.z;
	}
	double3 cross(double3 f) {
		return double3(y*f.z - z*f.y, z*f.x - x*f.z, x*f.y - y*f.x);
	}
	void print() {
		printf("%f %f %f\n", x, y, z);
	}
	void print(const char *str) {
		printf("%s: %f %f %f\n", str, x, y, z);
	}
};

class Matrix33d {
public:
	double3 data[3];
	Matrix33d(double f[3][3]) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = f[i][j];
	}
	Matrix33d(double a, double b, double c, double d, double e, double f, double g, double h, double i) {
		data[0] = double3(a, b, c);
		data[1] = double3(d, e, f);
		data[2] = double3(g, h, i);
	}
	Matrix33d(double3 x, double3 y, double3 z) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}
	Matrix33d() {
		data[0] = double3(0.f, 0.f, 0.f);
		data[1] = double3(0.f, 0.f, 0.f);
		data[2] = double3(0.f, 0.f, 0.f);
	}
	Matrix33d(Matrix33 f) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = (double)f[i][j];
	}
	void operator=(Matrix33d m) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = m[i][j];
	}
	double3& operator[](int i) {
		switch (i)
		{
		case 0: return data[0];
		case 1: return data[1];
		case 2: return data[2];
		}
	}
	Matrix33d operator*(Matrix33d m) {
		Matrix33d tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][0] * m[0][j] + data[i][1] * m[1][j] + data[i][2] * m[2][j];
		return tmp;
	}
	double3 operator*(double3 f) {
		double3 tmp;
		for (int i = 0; i < 3; i++)
			tmp[i] = data[i][0] * f[0] + data[i][1] * f[1] + data[i][2] * f[2];
		return tmp;
	}
	Matrix33d operator*(double d) {
		Matrix33d tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][j] * d;
		return tmp;
	}
	Matrix33d operator/(double d) {
		Matrix33d tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][j] / d;
		return tmp;
	}
	Matrix33d operator-(Matrix33d m) {
		Matrix33d tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = data[i][j] - m[i][j];
		return tmp;
	}
	Matrix33d operator+(Matrix33d m) {
		Matrix33d tmp;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				tmp[i][j] = m[i][j] + data[i][j];
		return tmp;
	}
	void print() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				printf("%f ", data[i][j]);
			}
			printf("\n");
		}
	}
	void print(const char *str) {
		printf("%s:\n", str);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				printf("%f ", data[i][j]);
			}
			printf("\n");
		}
	}
	Matrix33d inv() {
		Matrix33d tmp;
		double a1 = data[0][0], a2 = data[1][0], a3 = data[2][0];
		double b1 = data[0][1], b2 = data[1][1], b3 = data[2][1];
		double c1 = data[0][2], c2 = data[1][2], c3 = data[2][2];

		double A = a1*(b2*c3 - c2*b3) - a2*(b1*c3 - c1*b3) + a3*(b1*c2 - c1*b2);
#ifdef _DEBUG
		assert(A != 0.0);
#endif
		tmp[0][0] = b2*c3 - c2*b3; tmp[0][1] = c1*b3 - b1*c3; tmp[0][2] = b1*c2 - c1*b2;
		tmp[1][0] = c2*a3 - a2*c3; tmp[1][1] = a1*c3 - c1*a3; tmp[1][2] = a2*c1 - a1*c2;
		tmp[2][0] = a2*b3 - b2*a3; tmp[2][1] = b1*a3 - a1*b3; tmp[2][2] = a1*b2 - a2*b1;
		tmp = tmp / A;
		return tmp;
	}
	double det() {
		double a1 = data[0][0], a2 = data[1][0], a3 = data[2][0];
		double b1 = data[0][1], b2 = data[1][1], b3 = data[2][1];
		double c1 = data[0][2], c2 = data[1][2], c3 = data[2][2];

		double A = a1*(b2*c3 - c2*b3) - a2*(b1*c3 - c1*b3) + a3*(b1*c2 - c1*b2);
		return A;
	}
};

class CameraPose {
public:
	double3 rotation, translation, center, q;
	Matrix33d rot, Q, A;
	CameraPose(Matrix33d m, double a, double b, double r, double x, double y, double z) {
		rotation = double3(a, b, r);
		translation = double3(x, y, z);
		setMatrix();
		A = m;
		Q = A*rot;
		q = A*translation;
		center = -(inv(rot)*translation);
	}
	CameraPose() {}
	void refreshByARt() {
		Q = A*rot;
		q = A*translation;
		center = -(inv(rot)*translation);
	}
	double3 getRotation(Matrix33d m) {
		double3 tmp;
		//there are more than one solution for a rotation matrix, we choose one possible solution of them.
		if (m[0][2] != 1 && m[0][2] != -1) {
			tmp[1] = -(double)asin(m[0][2]);
			tmp[0] = (double)atan2(m[1][2] / cos(tmp[1]), m[2][2] / cos(tmp[1]));
			tmp[2] = (double)atan2(m[0][1] / cos(tmp[1]), m[0][0] / cos(tmp[1]));
		}
		else {
			tmp[2] = 0.0;
			if (m[0][2] == -1) {
				tmp[1] = double(M_PI) / 2;
				tmp[0] = tmp[2] + (double)atan2(m[1][0], m[2][0]);
			}
			else {
				tmp[1] = -double(M_PI) / 2;
				tmp[0] = tmp[2] + (double)atan2(-m[1][0], -m[2][0]);
			}
		}
		return tmp;
	}
	Matrix33d inv(Matrix33d m) {
		Matrix33d tmp;
		double3 m_r = getRotation(m);
		double a = -m_r[0];
		double b = -m_r[1];
		double r = -m_r[2];
		tmp[0][0] = double(cos(b)*cos(r));
		tmp[0][1] = double(sin(a)*sin(b)*cos(r) + cos(a)*sin(r));
		tmp[0][2] = double(-cos(a)*sin(b)*cos(r) + sin(a)*sin(r));
		tmp[1][0] = double(-cos(b)*sin(r));
		tmp[1][1] = double(-sin(a)*sin(b)*sin(r) + cos(a)*cos(r));
		tmp[1][2] = double(cos(a)*sin(b)*sin(r) + sin(a)*cos(r));
		tmp[2][0] = double(sin(b));
		tmp[2][1] = double(-sin(a)*cos(b));
		tmp[2][2] = double(cos(a)*cos(b));
		return tmp;
	}
	void setMatrix() {
		double a = rotation[0];
		double b = rotation[1];
		double r = rotation[2];
		rot[0][0] = double(cos(b)*cos(r));
		rot[0][1] = double(cos(b)*sin(r));
		rot[0][2] = double(-sin(b));
		rot[1][0] = double(-cos(a)*sin(r) + sin(a)*sin(b)*cos(r));
		rot[1][1] = double(cos(a)*cos(r) + sin(a)*sin(b)*sin(r));
		rot[1][2] = double(sin(a)*cos(b));
		rot[2][0] = double(sin(a)*sin(r) + cos(a)*sin(b)*cos(r));
		rot[2][1] = double(-sin(a)*cos(r) + cos(a)*sin(b)*sin(r));
		rot[2][2] = double(cos(a)*cos(b));
	}
};

template<typename T> 
T binterd(T* I_i, double vx, double vy, int size) {
	int z_x = int(vx + 0.00001);  //0~w
	int z_y = int(vy + 0.00001);  //0~h
	double x = vx - z_x;
	double y = vy - z_y;
	//if (z_x < 0 || z_x + 1 >= g_w || z_y < 0 || z_y + 1 >= g_h) return T(0);
	return T((1.0 - y)   * (1.0 - x)  * I_i[z_y	 *size + z_x] +
			  x			 * (1.0 - y)  * I_i[z_y	 *size + z_x + 1] +
			  x          *  y	      * I_i[(z_y + 1)*size + z_x + 1] +
			 (1.0 - x)   *  y	      * I_i[(z_y + 1)*size + z_x]);
}

} //namespace mf

#endif //UTILS_H
