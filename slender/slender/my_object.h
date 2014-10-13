/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#include <stdlib.h>
#include <glut.h>
#include <vector>
#include <math.h>
#include <cstdio>

#ifndef _MY_OBJECT_H_
#define _MY_OBJECT_H_

#define MAX_RS 75
#define MAX_VS 75

class myObject
{
    public:
        myObject() {};
        myObject(int);
        ~myObject();
        int type;
        GLfloat verts[MAX_RS][MAX_VS][4];
        GLfloat faces[MAX_VS*MAX_RS][4][2];
        GLfloat normals[MAX_RS][MAX_VS][3];
        int num_faces;
		float color[3];
        float center[3];
        float central_axis[4]; // v-axis
		float u[4];
        float ctm[4][4];
		float ctmi[4][4];   // inverse CTM
		float diri[4][4]; // inverse dir. matrix
        int radial_slices;
        int vertical_slices;
        double ray, height, ray2;
        void resetCTM();
        void make_object();
        void make_sphere();
        void make_cone();
        void make_cylinder();
        void make_torus();
        void make_house();
        void make_cube();
        void make_wall();
        void make_tree();
        std::vector<float> RotateVertX(std::vector<float>, float);
        std::vector<float> RotateVertY(std::vector<float>, float);
        std::vector<float> RotateVertZ(std::vector<float>, float);
        std::vector<float> ScaleVert(std::vector<float>, float, float, float);
        std::vector<float> TranslateVert(std::vector<float>, float, float, float);
        void updateCTM(float [][4]);
        void TranslateObjectToOrigin();
        void TranslateObjectTo(double, double, double);
        void TranslateObject(double, double, double);
        void RotateObject(double, int, int, int);
        void RotateObjectX(double);
        void RotateObjectY(double);
        void RotateObjectZ(double);
        void RotateObjectAboutCentralAxis(double);
		void RotateObjectAboutArbitraryAxis(double, double, double, double);
        void ScaleObject(double, double, double);
        void draw_normals();
        void draw_sphere(int);
        void draw_cone(int);
        void draw_cylinder(int);
        void draw_torus(int);
        void draw_house(int);
        void draw_cube(int);
        void draw_wall(int);
        void draw_tree(int);
        void draw(int);

		void setColor(float, float, float);
        void setWall(float, float, float, float, float, float);

		void ApplyCTM();
		void ApplyCTMi();

        void draw_quad(GLfloat[][4], int, int, int, int, int, int);
        void draw_param_quad(GLfloat[][MAX_VS][4], int, int, int, int);
        void draw_triangle(GLfloat[][4], int, int, int, int, int);

	private:
		GLfloat width;
		GLfloat length;
	
};

#endif
