#ifndef __CURVE_H__
#define __CURVE_H__

#include "Spline.h"
#define CURVE_BEZIER true
#define CURVE_BSPLINE false
class Curve :
	public Spline
{
private:
public:
	// 保存控制点
	Vec3f* vertices;
	// 保存控制点数量
	int num_vertices = 0;
	// 存储控制点的数组空间，memory_length >= num_vertices
	int memory_length = 0;
	Curve();
	Curve(int);
	~Curve();
	Curve(Spline& s) :Spline(s) {}

	// 用于画图的FOR VISUALIZATION。绘制曲线对应的控制点并将控制点连线，具体曲线绘制在子类中实现
	void Paint(ArgParser *args);

	// 用于实现样条类型转换的FOR CONVERTING BETWEEN SPLINE TYPES
	void OutputBezier(FILE *file) {}
	void OutputBSpline(FILE *file) {}

	// 用于得到控制点的FOR CONTROL POINT PICKING
	int getNumVertices() { return num_vertices; }
	Vec3f getVertex(int i) { return vertices[i]; }

	// 用于编辑操作的FOR EDITING OPERATIONS
	void moveControlPoint(int selectedPoint, float x, float y);
	void addControlPoint(int selectedPoint, float x, float y);
	void deleteControlPoint(int selectedPoint);

	// 用于产生三角形的FOR GENERATING TRIANGLES。交由子类实现。
	TriangleMesh* OutputTriangles(ArgParser *args) { return NULL; }

	void set(int, Vec3f&);
};


class BezierCurve :public Curve{
private:
	BezierCurve();
public:
	BezierCurve(int n);
	~BezierCurve();
	void Paint(ArgParser * args);
	void OutputBezier(FILE *file);
	void OutputBSpline(FILE *file);
	void addControlPoint(int selectedPoint, float x, float y);
	void deleteControlPoint(int selectedPoint);
	TriangleMesh* OutputTriangles(ArgParser *args);
	BezierCurve(Spline& s) :Curve(s) {}
};

class BSplineCurve :public Curve {
private:
	BSplineCurve();
public:
	BSplineCurve(int n);
	~BSplineCurve();
	void Paint(ArgParser * args);
	void OutputBezier(FILE *file);
	void OutputBSpline(FILE *file);
	void deleteControlPoint(int selectedPoint);
	TriangleMesh* OutputTriangles(ArgParser *args);
	BSplineCurve(Spline& s) :Curve(s) {}
};

#endif