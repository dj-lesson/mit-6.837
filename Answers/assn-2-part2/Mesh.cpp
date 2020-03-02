#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
	fstream fs(filename, ios::in);
	int a, b, c;
	float af, bf, cf;
	char kind = ' ';
	while (fs>>kind)
	{
		if (kind == 'v') {
			fs >> af >> bf >> cf;
			Vector3f v(af, bf, cf);
			bindVertices.push_back(v);
		}
		else {
			fs >> a >> b >> c;
			Tuple3u t(a, b, c);
			faces.push_back(t);
		}
	}
	fs.close();

	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".
	for (int i = 0; i < faces.size(); i++) {
		Vector3f v1 = currentVertices[faces[i][0] - 1]- currentVertices[faces[i][1] - 1];
		Vector3f v2 = currentVertices[faces[i][1] - 1] - currentVertices[faces[i][2] - 1];
		Vector3f v = Vector3f::cross(v1,v2).normalized();
		glNormal3f(v.x(), v.y(), v.z());
		glBegin(GL_TRIANGLES);
		glVertex3f(currentVertices[faces[i][0]-1].x(), currentVertices[faces[i][0]-1].y(), currentVertices[faces[i][0]-1].z());
		glVertex3f(currentVertices[faces[i][1]-1].x(), currentVertices[faces[i][1]-1].y(), currentVertices[faces[i][1]-1].z());
		glVertex3f(currentVertices[faces[i][2]-1].x(), currentVertices[faces[i][2]-1].y(), currentVertices[faces[i][2]-1].z());
		glEnd();
	}
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
	fstream fs(filename);
	for (int i = 0; i < currentVertices.size(); i++) {
		vector<float> v(numJoints,0);
		for (int j = 1; j < numJoints; j++) {
			fs >> v[j];
		}
		attachments.push_back(v);
	}
	fs.close();
}
