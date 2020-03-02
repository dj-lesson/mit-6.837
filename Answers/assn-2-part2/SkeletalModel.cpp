#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
	fstream file(filename, ios::in);
	if (file.is_open()) {
		float x, y, z;
		int num;
		while (file >> x >> y >> z >> num)
		{
			Joint *joint = new Joint();
			joint->transform = Matrix4f::translation(x, y, z);
			if (num == -1) {
				m_rootJoint = joint;
			}
			else {
				m_joints[num]->children.push_back(joint);
			}
			m_joints.push_back(joint);
		}
	}
	file.close();
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
	drawJointsHelper(m_rootJoint);
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	drawSkeletonHelper(m_rootJoint,Vector4f(0,0,0,1));
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
	Vector4f v = m_joints[jointIndex]->transform.getCol(3);
	m_joints[jointIndex]->transform = Matrix4f().identity();
	m_joints[jointIndex]->transform.setCol(3, v);
	m_joints[jointIndex]->transform = m_joints[jointIndex]->transform*Matrix4f::rotateX(rX);
	m_joints[jointIndex]->transform = m_joints[jointIndex]->transform*Matrix4f::rotateY(rY);
	m_joints[jointIndex]->transform = m_joints[jointIndex]->transform*Matrix4f::rotateZ(rZ);
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	computeBindWorldToJointTransformsHelper(m_rootJoint,Matrix4f::identity());
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	updateCurrentJointToWorldTransformsHelper(m_rootJoint, Matrix4f::identity());
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
	for (int i = 0; i < m_mesh.bindVertices.size(); i++) {
		Vector4f p = Vector4f(m_mesh.bindVertices[i],1);
		Vector4f v(0, 0, 0, 0);
		for (int j = 0; j < m_joints.size(); j++) {
			if(m_mesh.attachments[i][j]==0) continue;
			v = v + (m_joints[j]->currentJointToWorldTransform)*(m_joints[j]->bindWorldToJointTransform)*p*m_mesh.attachments[i][j] ;
		}
		m_mesh.currentVertices[i]=v.xyz();
	}
}

void SkeletalModel::drawJointsHelper(Joint * joint)
{
	if (joint == NULL) { return; }
	m_matrixStack.push(joint->transform);
	glLoadMatrixf(m_matrixStack.top());
	glutSolidSphere(0.025, 12, 12);
	for (unsigned int i = 0; i < joint->children.size(); i++) {
		drawJointsHelper(joint->children[i]);
	}
	m_matrixStack.pop();
	glLoadMatrixf(m_matrixStack.top());
}

void SkeletalModel::drawSkeletonHelper(Joint * joint,Vector4f parentJ)
{

	if (joint == NULL) { return; }
	
	Matrix4f m = Matrix4f::identity();
	m.setCol(3, joint->transform.getCol(3));
	m_matrixStack.push(m);
	glLoadMatrixf(m_matrixStack.top());

	if (joint != m_rootJoint) {
		Vector3f z = (m*parentJ-parentJ).xyz().normalized();
		z.negate();
		Vector3f y = Vector3f::cross(z, Vector3f(0, 0, 1)).normalized();
		Vector3f x = Vector3f::cross(y, z).normalized();
		
		Matrix4f c_matrix = Matrix4f::identity();
		Quat4f q;
		q.setAxisAngle(-acosf(Vector3f::dot(z, Vector3f(0, 0, 1))), y);
		c_matrix = c_matrix * (Matrix4f::rotation(q));

		q.setAxisAngle(- acosf(Vector3f::dot(y, Vector3f(0, 1, 0))), Vector3f::cross(y, Vector3f(0, 1, 0)));
		c_matrix = c_matrix * (Matrix4f::rotation(q));

		c_matrix = c_matrix * Matrix4f::scaling(0.05, 0.05, joint->transform.getCol(3).xyz().abs());

		c_matrix = c_matrix * Matrix4f::translation(0.0, 0.0, 0.5);

		m_matrixStack.push(c_matrix);
		glLoadMatrixf(m_matrixStack.top());

		glutSolidCube(1.0f);
		m_matrixStack.pop();
		glLoadMatrixf(m_matrixStack.top());
	}
	m_matrixStack.pop();
	glLoadMatrixf(m_matrixStack.top());
	m_matrixStack.push(joint->transform);
	glLoadMatrixf(m_matrixStack.top());

	for (unsigned int i = 0; i < joint->children.size(); i++) {
		drawSkeletonHelper(joint->children[i],joint->transform*parentJ);
	}
	m_matrixStack.pop();
	glLoadMatrixf(m_matrixStack.top());
}
void SkeletalModel::computeBindWorldToJointTransformsHelper(Joint *joint, Matrix4f before_m)
{
	Matrix4f m = joint->transform.inverse();
	joint->bindWorldToJointTransform = m*before_m;
	for (int i = 0; i < joint->children.size(); i++) {
		computeBindWorldToJointTransformsHelper(joint->children[i],joint->bindWorldToJointTransform);
	}
}
void SkeletalModel::updateCurrentJointToWorldTransformsHelper(Joint *joint,Matrix4f before_m)
{
	Matrix4f m = joint->transform;
	joint->currentJointToWorldTransform =  before_m*m;
	for (int i = 0; i < joint->children.size(); i++) {
		updateCurrentJointToWorldTransformsHelper(joint->children[i], joint->currentJointToWorldTransform);
	}
}
/*
void SkeletalModel::drawSkeletonHelper(Joint * joint, Vector4f parentJ)
{

	if (joint == NULL) { return; }

	Matrix4f m = Matrix4f::identity();
	m.setCol(3, joint->transform.getCol(3));
	glPushMatrix();
	glMultMatrixf(m);
	//m_matrixStack.push(m);
	//glLoadMatrixf(m_matrixStack.top());

	if (joint != m_rootJoint) {
		glPushMatrix();
		Vector3f z = (m*parentJ - parentJ).xyz().normalized();
		z.negate();
		Vector3f y = Vector3f::cross(z, Vector3f(0, 0, 1)).normalized();
		Vector3f x = Vector3f::cross(y, z).normalized();

		glRotatef(-180 * acosf(Vector3f::dot(z, Vector3f(0, 0, 1))) / M_PI, y.x(), y.y(), y.z());
		glRotatef(-180 * acosf(Vector3f::dot(y, Vector3f(0, 1, 0))) / M_PI, Vector3f::cross(y, Vector3f(0, 1, 0)).x(), Vector3f::cross(y, Vector3f(0, 1, 0)).y(), Vector3f::cross(y, Vector3f(0, 1, 0)).z());
		////glRotatef(-180 * acosf(Vector3f::dot(x, Vector3f(1, 0, 0))) / M_PI, Vector3f::cross(x, Vector3f(1, 0, 0)).x(), Vector3f::cross(x, Vector3f(1, 0, 0)).y(), Vector3f::cross(x, Vector3f(1, 0, 0)).z());
		glScalef(0.05, 0.05, joint->transform.getCol(3).xyz().abs());
		glTranslatef(0.0, 0.0, 0.5);

		glutSolidCube(1.0f);
		glPopMatrix();
	}
	glPopMatrix();
	glPushMatrix();
	glMultMatrixf(joint->transform);

	for (unsigned int i = 0; i < joint->children.size(); i++) {
		drawSkeletonHelper(joint->children[i], joint->transform*parentJ);
	}
	glPopMatrix();
}*/