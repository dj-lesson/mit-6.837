#ifndef __SPLINE_H__
#define __SPLINE_H__
#include "arg_parser.h"
#include "matrix.h"
#include "triangle_mesh.h"
#include <GL/glut.h>

#define pi 3.1415926

class Spline
{
private:
	
public:
	Spline();
	Spline(int);
	~Spline();
	Spline(Spline*);
	// 用于画图的FOR VISUALIZATION
	virtual void Paint(ArgParser *args)=0;

	// 用于实现样条类型转换的FOR CONVERTING BETWEEN SPLINE TYPES
	virtual void OutputBezier(FILE *file)=0;
	virtual void OutputBSpline(FILE *file) = 0;

	// 用于得到控制点的FOR CONTROL POINT PICKING
	virtual int getNumVertices() = 0;
	virtual Vec3f getVertex(int i) = 0;

	// 用于编辑操作的FOR EDITING OPERATIONS
	virtual void moveControlPoint(int selectedPoint, float x, float y) = 0;
	virtual void addControlPoint(int selectedPoint, float x, float y) = 0;
	virtual void deleteControlPoint(int selectedPoint) = 0;

	// 用于产生三角形的FOR GENERATING TRIANGLES
	virtual TriangleMesh* OutputTriangles(ArgParser *args) = 0;

	// 用于设置存储点的函数
	virtual void set(int, Vec3f&) = 0;

	// 给定4个控制点对应的控制点矩阵，绘制对应的Bezier曲线
	void drawBezier(Matrix m,int tessellation);
	void drawBezierWithDeCasteljau(Matrix m, int tessellation);
	//void recursive(Vec3f *&fs, int i);
	// 给定4个控制点对应的控制点矩阵，绘制对应的B样条曲线
	void drawBSpline(Matrix m, int tessellation);
	// Bezier样条矩阵
	Matrix *BEZIER_SPLINE_BASIS;
	// BSpline样条矩阵
	Matrix *BSPLINE_BASIS;
};

#endif