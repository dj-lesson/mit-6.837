#include "Spline.h"



Spline::Spline()
{
	float bezier_data[16] = {-1,3,-3,1,3,-6,3,0,-3,3,0,0,1,0,0,0};
	BEZIER_SPLINE_BASIS = new Matrix(bezier_data);
	float bspline_data[16] = { -1,3,-3,1,3,-6,0,4,-3,3,3,1,1,0,0,0 };
	BSPLINE_BASIS = new Matrix(bspline_data);
	*BSPLINE_BASIS=(*BSPLINE_BASIS)*((float)1 / 6);
}


Spline::Spline(int)
{
}

Spline::~Spline()
{
	delete BEZIER_SPLINE_BASIS;
	delete BSPLINE_BASIS;
}

Spline::Spline(Spline *)
{
}

void Spline::drawBezier(Matrix m,int tesselation)
{
	m = m * (*BEZIER_SPLINE_BASIS);
	Matrix T,temp;
	float t = 0;
	Vec3f v;
	glLineWidth(3);
	glColor4f(0, 1, 0, 1);
	glBegin(GL_LINES);
	for (int i =0;i<=tesselation;i++)
	{
		t = (float)i / tesselation;
		T.Set(0, 0, t*t*t);
		T.Set(0,1, t*t);
		T.Set(0,2, t);
		T.Set(0,3, 1);
		temp = m * T;
		if (i == 0) {
			v.Set(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
			continue;
		}
		glVertex3d(v[0], v[1], v[2]);
		glVertex3d(temp.Get(0, 0), temp.Get(0,1), temp.Get(0, 2));
		v.Set(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
	}
	glEnd();
	glLineWidth(1);
}

void Spline::drawBezierWithDeCasteljau(Matrix m, int tessellation)
{
	float t = (float)1 / tessellation;
	Vec3f vs[4];
	Vec3f temp_vs[4];
	Vec3f vector_t;

	glLineWidth(3);
	glColor4f(0, 1, 0, 0);
	glBegin(GL_LINES);

	for (int j = 0; j <= tessellation; j++) {
		for (int i = 0; i < 4; i++) {
			temp_vs[i].Set(m.Get(i, 0), m.Get(i, 1), m.Get(i, 2));
		}

		// 循环三次求一个点
		for (int m = 3; m >=0; m--) {

			for (int i = 0; i < 4; i++) {
				vs[i] = temp_vs[i];
			}

			for (int i = 0; i < m; i++) {
				temp_vs[i] = j*t * (vs[i+1]-vs[i])+vs[i];
			}

		}
		if (j != 0) {
			glVertex3d(vector_t[0], vector_t[1], vector_t[2]);
			glVertex3d(temp_vs[0][0], temp_vs[0][1], temp_vs[0][2]);
		}
		vector_t = temp_vs[0];
	}
	glEnd();
	glLineWidth(1);
}

//void Spline::recursive(Vec3f *& fs, int i)
//{
//	for(int i=0;i<)
//}

void Spline::drawBSpline(Matrix m, int tessellation)
{
	m = m * (*BSPLINE_BASIS);
	Matrix T, temp;
	float t = 0;
	Vec3f v;
	glLineWidth(3);
	glColor4f(0, 1, 0, 1);
	glBegin(GL_LINES);
	for (int i = 0; i <= tessellation; i++)
	{
		t = (float)i / tessellation;
		T.Set(0, 0, t*t*t);
		T.Set(0, 1, t*t);
		T.Set(0, 2, t);
		T.Set(0, 3, 1);
		temp = m * T;
		if (i == 0) {
			v.Set(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
			continue;
		}
		glVertex3d(v[0], v[1], v[2]);
		glVertex3d(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
		v.Set(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
	}
	glEnd();
	glLineWidth(1);
}
