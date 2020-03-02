#include "Curve.h"



Curve::Curve()
{
}
// 仅绘制控制点和控制点连线，曲线绘制由子类实现
void Curve::Paint(ArgParser *args) {
	glColor4f(1, 0, 0, 1);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3d(vertices[0][0], vertices[0][1], vertices[0][2]);
	glEnd();
	for (int i=1;i<num_vertices;i++)
	{
		glColor4f(0, 0, 1, 0);
		glBegin(GL_LINES);
		glVertex3d(vertices[i-1][0], vertices[i-1][1], vertices[i-1][2]);
		glVertex3d(vertices[i][0], vertices[i][1], vertices[i][2]);
		glEnd();

		glColor4f(1, 0, 0, 1);
		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3d(vertices[i][0], vertices[i][1], vertices[i][2]);
		glEnd();
	}
	glPointSize(1);
}
Curve::Curve(int num_vervices)
{
	this->num_vertices = num_vervices;
	vertices = new Vec3f[num_vervices*2];
	memory_length = num_vertices * 2;
}
// 移动控制点
void Curve::moveControlPoint(int selectedPoint, float x, float y)
{
	vertices[selectedPoint].Set(x, y, vertices[selectedPoint].z());
}

// 添加一个控制点
void Curve::addControlPoint(int selectedPoint, float x, float y)
{
	// 如果存储控制点的空间不足，则两倍扩容
	if (num_vertices >= memory_length) {
		Vec3f * temp = vertices;
		vertices = new Vec3f[num_vertices*2];
		for (int i=0;i<num_vertices;i++)
		{
			vertices[i] = temp[i];
		}
		memory_length = memory_length * 2;
		delete []temp;
	}
	for (int i = num_vertices - 1; i >= selectedPoint; i--)
		vertices[i + 1] = vertices[i];
	num_vertices++;
	
	vertices[selectedPoint] = Vec3f(x, y, 0);
}
// TODO
// 删除一个控制点，这里只对应了B样条的控制点删除
void Curve::deleteControlPoint(int selectedPoint)
{
	if (num_vertices <= 4) return;
	for (int i = selectedPoint; i < num_vertices-1; i++) {
		vertices[i] = vertices[i + 1];
	}
	num_vertices--;
}

void Curve::set(int i, Vec3f& v) {
	assert(i < num_vertices);
	vertices[i] = v;
}
Curve::~Curve()
{
	delete[] vertices;
}

BezierCurve::BezierCurve()
{
}

BezierCurve::BezierCurve(int n):Curve(n)
{
}

BezierCurve::~BezierCurve()
{
}

// 绘制Bezier曲线
void BezierCurve::Paint(ArgParser *args) {
	// 绘制控制点
	Curve::Paint(args);

	Matrix m;
	for (int i = 0; i <= num_vertices - 4; i += 3) {
		for (int j = i; j < i + 4; j++) {
			m.Set(j - i, 0, vertices[j][0]);
			m.Set(j - i, 1, vertices[j][1]);
			m.Set(j - i, 2, vertices[j][2]);
		}
		//drawBezier(m, args->curve_tessellation);
		drawBezierWithDeCasteljau(m, args->curve_tessellation);
	}
}
void BezierCurve::OutputBezier(FILE * file)
{
	fprintf(file, "\nbezier\n");
	fprintf(file, "num_vertices %d\n", num_vertices);
	for (int i = 0; i < num_vertices; i++) {
		fprintf(file, "%f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
	}
}
//void BezierCurve::OutputBSpline(FILE * file)
//{
//	fprintf(file, "\nbspline\n");
//	fprintf(file, "num_vertices %d\n", (num_vertices - 1) / 3 *5+3);
//	Matrix m;
//	for (int i = 0; i <= num_vertices - 4; i+=3) {
//		for (int j = i; j < i + 4; j++) {
//			m.Set(j - i, 0, vertices[j][0]);
//			m.Set(j - i, 1, vertices[j][1]);
//			m.Set(j - i, 2, vertices[j][2]);
//		}
//		m = m * (*BEZIER_SPLINE_BASIS);
//		Matrix temp = *BSPLINE_BASIS;
//		temp.Inverse();
//		m = m * (temp);
//		if (i == 0) {
//			fprintf(file, "%f %f %f\n", m.Get(0, 0), m.Get(0, 1), m.Get(0, 2));
//			fprintf(file, "%f %f %f\n", m.Get(0, 0), m.Get(0, 1), m.Get(0, 2));
//			fprintf(file, "%f %f %f\n", m.Get(0, 0), m.Get(0, 1), m.Get(0, 2));
//		}
//		fprintf(file, "%f %f %f\n", m.Get(1, 0), m.Get(1, 1), m.Get(1, 2));
//		fprintf(file, "%f %f %f\n", m.Get(2, 0), m.Get(2, 1), m.Get(2, 2));
//
//		fprintf(file, "%f %f %f\n", m.Get(3, 0), m.Get(3, 1), m.Get(3, 2));
//		fprintf(file, "%f %f %f\n", m.Get(3, 0), m.Get(3, 1), m.Get(3, 2));
//		fprintf(file, "%f %f %f\n", m.Get(3, 0), m.Get(3, 1), m.Get(3, 2));
//	}
//}
void BezierCurve::OutputBSpline(FILE * file)
{
	fprintf(file, "\nbspline\n");
	fprintf(file, "num_vertices %d\n", (num_vertices - 1) / 3 +3);
	Matrix m;
	for (int i = 0; i <= num_vertices - 4; i += 3) {
		for (int j = i; j < i + 4; j++) {
			m.Set(j - i, 0, vertices[j][0]);
			m.Set(j - i, 1, vertices[j][1]);
			m.Set(j - i, 2, vertices[j][2]);
		}
		m = m * (*BEZIER_SPLINE_BASIS);
		Matrix temp = *BSPLINE_BASIS;
		temp.Inverse();
		m = m * (temp);
		fprintf(file, "%f %f %f\n", m.Get(0, 0), m.Get(0, 1), m.Get(0, 2));
		fprintf(file, "%f %f %f\n", m.Get(1, 0), m.Get(1, 1), m.Get(1, 2));
		fprintf(file, "%f %f %f\n", m.Get(2, 0), m.Get(2, 1), m.Get(2, 2));
		fprintf(file, "%f %f %f\n", m.Get(3, 0), m.Get(3, 1), m.Get(3, 2));
	}
}
// Bezier曲线添加控制点，在同一位置同时添加三个控制点，添加后保证曲线整体形状不变
void BezierCurve::addControlPoint(int selectedPoint, float x, float y)
{
	Curve::addControlPoint(selectedPoint, x, y);
	Curve::addControlPoint(selectedPoint, x, y);
	Curve::addControlPoint(selectedPoint, x, y);
}
// TODO Bezier曲线删除控制点
void BezierCurve::deleteControlPoint(int selectedPoint)
{
}
TriangleMesh * BezierCurve::OutputTriangles(ArgParser * args)
{
	TriangleNet* net=NULL;
	float theta = (float)2*pi / args->revolution_tessellation;
	for (int i = 0; i < args->revolution_tessellation; i++) {
		// 构建三角形网络，网格布局横向分辨率为1，纵向分辨率为 小段Bezier曲线数量*每小段曲线的拟合线段数量
		TriangleNet* net2 = new TriangleNet(1, (args->curve_tessellation)*(num_vertices - 1) / 3);
		Matrix m;
		for (int i2 = 0; i2 <= num_vertices - 4; i2 += 3) {
			// 生成控制点矩阵
			for (int j = i2; j < i2 + 4; j++) {
				m.Set(j - i2, 0, vertices[j][0]);
				m.Set(j - i2, 1, vertices[j][1]);
				m.Set(j - i2, 2, vertices[j][2]);
			}
			// 控制点矩阵和样条矩阵相乘得变换矩阵
			m = m * (*BEZIER_SPLINE_BASIS);
			Matrix T, temp;
			float t = 0;
			Vec3f v;
			for (int i3 = 0; i3 <= args->curve_tessellation; i3++)
			{
				// 构造T矩阵
				t = (float)i3 / args->curve_tessellation;
				T.Set(0, 0, t*t*t);
				T.Set(0, 1, t*t);
				T.Set(0, 2, t);
				T.Set(0, 3, 1);
				// 变换矩阵与T矩阵相乘
				temp = m * T;
				v.Set(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
				// 添加原曲线上点绕Y轴旋转theta*i角度的点
				Matrix::MakeYRotation(theta*i).Transform(v);
				net2->SetVertex(0, (i2 / 3)*(args->curve_tessellation) + i3, v);
				// 添加原曲线上点绕Y轴旋转theta*(i+1)角度的点
				Matrix::MakeYRotation(theta).Transform(v);
				net2->SetVertex(1, (i2 / 3)*(args->curve_tessellation) + i3, v);
			}
		}
		if (net == NULL) net = net2;
		else {
			net->Merge(*net2);
			delete net2;
		}
	}
	return net;
}
BSplineCurve::BSplineCurve()
{
}

BSplineCurve::BSplineCurve(int n):Curve(n)
{
}

BSplineCurve::~BSplineCurve()
{
}
void BSplineCurve::Paint(ArgParser *args) {
	Curve::Paint(args);
	Matrix m;
	for (int i = 0; i <= num_vertices - 4; i ++) {
		for (int j = i; j < i + 4; j++) {
			m.Set(j - i, 0, vertices[j][0]);
			m.Set(j - i, 1, vertices[j][1]);
			m.Set(j - i, 2, vertices[j][2]);
		}
		glEnd();
		drawBSpline(m, args->curve_tessellation);
	}
}

void BSplineCurve::OutputBezier(FILE * file)
{
	fprintf(file, "\nbezier\n");
	fprintf(file, "num_vertices %d\n", (num_vertices -3)*3+1);
	Matrix m;
	for (int i = 0; i <= num_vertices - 4; i++) {
		for (int j = i; j < i + 4; j++) {
			m.Set(j - i, 0, vertices[j][0]);
			m.Set(j - i, 1, vertices[j][1]);
			m.Set(j - i, 2, vertices[j][2]);
		}
		// Q2 = Q1*S1*(s2)^(-1)
		m = m * (*BSPLINE_BASIS);
		Matrix temp = *BEZIER_SPLINE_BASIS;
		temp.Inverse();
		m = m * (temp);
		if (i == 0) {
			fprintf(file, "%f %f %f\n", m.Get(0, 0), m.Get(0, 1), m.Get(0, 2));
		}
		fprintf(file, "%f %f %f\n", m.Get(1, 0), m.Get(1, 1), m.Get(1, 2));
		fprintf(file, "%f %f %f\n", m.Get(2, 0), m.Get(2, 1), m.Get(2, 2));
		fprintf(file, "%f %f %f\n", m.Get(3, 0), m.Get(3, 1), m.Get(3, 2));
	}
}

void BSplineCurve::OutputBSpline(FILE * file)
{
	fprintf(file, "\nbspline\n");
	fprintf(file, "num_vertices %d\n", num_vertices);
	for (int i = 0; i < num_vertices; i++) {
		fprintf(file, "%f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
	}
}

void BSplineCurve::deleteControlPoint(int selectedPoint)
{
	Curve::deleteControlPoint(selectedPoint);
}

TriangleMesh * BSplineCurve::OutputTriangles(ArgParser * args)
{
	TriangleNet* net = NULL;
	float theta = (float)2 * pi / args->revolution_tessellation;
	for (int i = 0; i < args->revolution_tessellation; i++) {
		// 构建三角形网络，网格布局横向分辨率为1，纵向分辨率为 小段BSpline曲线数量*每小段曲线的拟合线段数量
		TriangleNet* net2 = new TriangleNet(1, (args->curve_tessellation)*(num_vertices - 3));
		Matrix m;
		for (int i2 = 0; i2 <= num_vertices - 4; i2 ++) {
			for (int j = i2; j < i2 + 4; j++) {
				m.Set(j - i2, 0, vertices[j][0]);
				m.Set(j - i2, 1, vertices[j][1]);
				m.Set(j - i2, 2, vertices[j][2]);
			}
			m = m * (*BSPLINE_BASIS);
			Matrix T, temp;
			float t = 0;
			Vec3f v;
			for (int i3 = 0; i3 <= args->curve_tessellation; i3++)
			{
				t = (float)i3 / args->curve_tessellation;
				T.Set(0, 0, t*t*t);
				T.Set(0, 1, t*t);
				T.Set(0, 2, t);
				T.Set(0, 3, 1);
				temp = m * T;
				v.Set(temp.Get(0, 0), temp.Get(0, 1), temp.Get(0, 2));
				Matrix::MakeYRotation(theta*i).Transform(v);
				net2->SetVertex(0, i2*(args->curve_tessellation) + i3, v);
				Matrix::MakeYRotation(theta).Transform(v);
				net2->SetVertex(1, i2*(args->curve_tessellation) + i3, v);
			}
		}
		if (net == NULL) net = net2;
		else net->Merge(*net2);
	}
	return net;
}
