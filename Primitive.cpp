
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "glut/glut.h"
#include "Bullet/include/btBulletDynamicsCommon.h"


#pragma comment (lib, "glut/glut32.lib")

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);
	glLineWidth(2.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	InnerRender();

	if (type != Primitive_Sphere) {
		glColor3f(0, 0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		InnerRender();
	}


	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
Cube::Cube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void Cube::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

// SPHERE ============================================
Sphere::Sphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

Sphere::Sphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void Sphere::InnerRender() const
{
	glutSolidSphere(radius, 25, 25);
}


// CYLINDER ============================================
Cylinder::Cylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

Cylinder::Cylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void Cylinder::InnerRender() const
{
	int n = 10;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
Line::Line() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

Line::Line(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
Plane::Plane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

Plane::Plane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void Plane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();

}

#define CONST_TRACK_ANGLE 0.4
// TRACKPIECE =================================
Trackpiece::Trackpiece(float turn, int W, int H) : Primitive(), turn(turn), w(W), h(H)
{
	type = PrimitiveTypes::Primitive_Trackpiece;

	btVector3 p0(0, 0, 0);
	btVector3 p1(-cos(CONST_TRACK_ANGLE) * w, sin(CONST_TRACK_ANGLE) * w, 0);
	btVector3 p2((-cos(CONST_TRACK_ANGLE) * w) + turn, sin(CONST_TRACK_ANGLE) * w, h);
	btVector3 p3(turn, 0, h);
	btVector3 p4(turn + w, 0, h);
	btVector3 p5(turn + w + (cos(CONST_TRACK_ANGLE) * w), sin(CONST_TRACK_ANGLE) * w, h);
	btVector3 p6(w + (cos(CONST_TRACK_ANGLE) * w), sin(CONST_TRACK_ANGLE) * w, 0);
	btVector3 p7(w, 0, 0);
	btVector3 p8(-cos(CONST_TRACK_ANGLE) * w, sin(CONST_TRACK_ANGLE) * w + w, 0);
	btVector3 p9(-cos(CONST_TRACK_ANGLE) * w + turn, sin(CONST_TRACK_ANGLE) * w + w, h);
	btVector3 p10(cos(CONST_TRACK_ANGLE) * w + w, sin(CONST_TRACK_ANGLE) * w + w, 0);
	btVector3 p11(cos(CONST_TRACK_ANGLE) * w + w + turn, sin(CONST_TRACK_ANGLE) * w + w, h);

	points.PushBack(p0);
	points.PushBack(p1);
	points.PushBack(p2);
	points.PushBack(p3);
	points.PushBack(p4);
	points.PushBack(p5); 
	points.PushBack(p6);
	points.PushBack(p7);
	points.PushBack(p8);
	points.PushBack(p9);
	points.PushBack(p10);
	points.PushBack(p11);

}

void Trackpiece::InnerRender() const
{
	glLineWidth(3.0f);

	vec3 origin(transform.translation());
	float x = 0, y = 0, z = 0;

	glBegin(GL_QUADS);

	glVertex3f(points[0].x(), points[0].y(), points[0].z());
	glVertex3f(points[1].x(), points[1].y(), points[1].z());
	glVertex3f(points[2].x(), points[2].y(), points[2].z());
	glVertex3f(points[3].x(), points[3].y(), points[3].z());

	glVertex3f(points[4].x(), points[4].y(), points[4].z());
	glVertex3f(points[5].x(), points[5].y(), points[5].z());
	glVertex3f(points[6].x(), points[6].y(), points[6].z());
	glVertex3f(points[7].x(), points[7].y(), points[7].z());

	glVertex3f(points[5].x(), points[5].y(), points[5].z());
	glVertex3f(points[11].x(), points[11].y(), points[11].z());
	glVertex3f(points[10].x(), points[10].y(), points[10].z());
	glVertex3f(points[6].x(), points[6].y(), points[6].z());
	
	glVertex3f(points[1].x(), points[1].y(), points[1].z());
	glVertex3f(points[8].x(), points[8].y(), points[8].z());
	glVertex3f(points[9].x(), points[9].y(), points[9].z());
	glVertex3f(points[2].x(), points[2].y(), points[2].z());
			

	glEnd();

}

btCollisionShape* Trackpiece::CreateCollider() {

	btConvexHullShape* shp1 = new btConvexHullShape();
	btConvexHullShape* shp2 = new btConvexHullShape();
	btConvexHullShape* shp3 = new btConvexHullShape();
	btConvexHullShape* shp4 = new btConvexHullShape();
	btConvexHullShape* shp5 = new btConvexHullShape();

	btCompoundShape* comp_shape = new btCompoundShape();
	btTransform t;
	t.setIdentity();

	shp1->addPoint(points[0]);
	shp1->addPoint(points[1]);
	shp1->addPoint(points[2]);
	shp1->addPoint(points[3]);

	comp_shape->addChildShape(t, shp1);

	shp2->addPoint(points[0]);
	shp2->addPoint(points[3]);
	shp2->addPoint(points[4]);
	shp2->addPoint(points[7]);

	comp_shape->addChildShape(t, shp2);

	shp3->addPoint(points[4]);
	shp3->addPoint(points[5]);
	shp3->addPoint(points[6]);
	shp3->addPoint(points[7]);

	comp_shape->addChildShape(t, shp3);

	shp4->addPoint(points[5]);
	shp4->addPoint(points[11]);
	shp4->addPoint(points[10]);
	shp4->addPoint(points[6]);

	comp_shape->addChildShape(t, shp4);

	shp5->addPoint(points[1]);
	shp5->addPoint(points[8]);
	shp5->addPoint(points[9]);
	shp5->addPoint(points[2]);

	comp_shape->addChildShape(t, shp5);


	btCollisionShape* col_shape = comp_shape;

	return col_shape;

}
