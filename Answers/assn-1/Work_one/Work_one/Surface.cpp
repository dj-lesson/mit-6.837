#include "Surface.h"



Surface::Surface()
{
}


Surface::Surface(int)
{
}

Surface::~Surface()
{
}

SurfaceOfRevolution::SurfaceOfRevolution()
{
}

SurfaceOfRevolution::SurfaceOfRevolution(int)
{
}

SurfaceOfRevolution::~SurfaceOfRevolution()
{
	if (curve != NULL) delete curve;
}
SurfaceOfRevolution::SurfaceOfRevolution(Curve* s) {
	curve = s;
}
void SurfaceOfRevolution::Paint(ArgParser * args)
{
	curve->Paint(args);
}
void SurfaceOfRevolution::OutputBSpline(FILE * file)
{
	fprintf(file, "surface_of_revolution\n");
	curve->OutputBSpline(file);
}
TriangleMesh * SurfaceOfRevolution::OutputTriangles(ArgParser * args)
{
	return curve->OutputTriangles(args);
}
BezierPatch::BezierPatch()
{
}

BezierPatch::BezierPatch(int)
{
}

BezierPatch::~BezierPatch()
{
}

void BezierPatch::set(int i, Vec3f& v) {
	vertices[i] = v;
}

void BezierPatch::get(int u, int v, Vec3f &vertex)
{
	vertex = vertices[u];
}

void BezierPatch::Paint(ArgParser * args)
{
	glPointSize(5);
	glBegin(GL_LINES);
	
	for (int j = 1; j < 16; j += 4) {
		for (int i = j; i < j+3; i++) {
			glVertex3d(vertices[i - 1][0], vertices[i - 1][1], vertices[i - 1][2]);
			glVertex3d(vertices[i][0], vertices[i][1], vertices[i][2]);
		}
	}

	for (int j = 4; j < 8; j ++) {
		for (int i = j; i < 16; i+=4) {
			glVertex3d(vertices[i - 4][0], vertices[i - 4][1], vertices[i - 4][2]);
			glVertex3d(vertices[i][0], vertices[i][1], vertices[i][2]);
		}
	}

	glEnd();
	glPointSize(1);
}

TriangleMesh * BezierPatch::OutputTriangles(ArgParser * args)
{
	TriangleNet *net = new TriangleNet(args->patch_tessellation, args->patch_tessellation);
	float unit = (float)1 / args->patch_tessellation;
	float bx[16] = { vertices[0][0],vertices[1][0],vertices[2][0] ,vertices[3][0] ,
	vertices[4][0] ,vertices[5][0] ,vertices[6][0] ,vertices[7][0] ,
	vertices[8][0] ,vertices[9][0] ,vertices[10][0] ,vertices[11][0] ,
	vertices[12][0] ,vertices[13][0] ,vertices[14][0] ,vertices[15][0] };
	float by[16] = { vertices[0][1],vertices[1][1],vertices[2][1] ,vertices[3][1] ,
	vertices[4][1] ,vertices[5][1] ,vertices[6][1] ,vertices[7][1] ,
	vertices[8][1] ,vertices[9][1] ,vertices[10][1] ,vertices[11][1] ,
	vertices[12][1] ,vertices[13][1] ,vertices[14][1] ,vertices[15][1] };
	float bz[16] = { vertices[0][2],vertices[1][2],vertices[2][2] ,vertices[3][2] ,
	vertices[4][2] ,vertices[5][2] ,vertices[6][2] ,vertices[7][2] ,
	vertices[8][2] ,vertices[9][2] ,vertices[10][2] ,vertices[11][2] ,
	vertices[12][2] ,vertices[13][2] ,vertices[14][2] ,vertices[15][2] };
	
	Matrix mx(bx), my(by), mz(bz);

	Matrix M_u;
	Vec4f V_v;
	Vec3f result_vertex;
	Matrix t1;
	Vec4f v1;
	for (int i=0;i<=args->patch_tessellation;i++)
	{
		M_u.Set(0, 0, i*i*i*unit*unit*unit);
		M_u.Set(1, 0, i*i*unit*unit);
		M_u.Set(2, 0, i*unit);
		M_u.Set(3, 0, 1);

		for (int j=0;j<=args->patch_tessellation;j++)
		{
			V_v.Set(j*j*j*unit*unit*unit, j*j*unit*unit, j*unit, 1);
			t1 = M_u * (*BEZIER_SPLINE_BASIS)*mx*(*BEZIER_SPLINE_BASIS);
			v1 = V_v;
			t1.Transform(v1);
			result_vertex.Set(v1[0], 0, 0);

			t1 = M_u * (*BEZIER_SPLINE_BASIS)*my*(*BEZIER_SPLINE_BASIS);
			v1 = V_v;
			t1.Transform(v1);
			result_vertex.Set(result_vertex[0], v1[0], 0);

			t1 = M_u * (*BEZIER_SPLINE_BASIS)*mz*(*BEZIER_SPLINE_BASIS);
			v1 = V_v;
			t1.Transform(v1);
			result_vertex.Set(result_vertex[0],result_vertex[1],v1[0]);

			net->SetVertex(j, i, result_vertex);
		}
	}
	return net;
}
