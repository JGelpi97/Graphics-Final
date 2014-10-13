/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/


#define _USE_MATH_DEFINES

#include "my_object.h"


#define CUBE     1
#define HOUSE    2
#define SPHERE   3
#define CYLINDER 4
#define CONE     5
#define TORUS    6
#define WALL     7
#define TREE     8

#define BLACK   0
#define RED     1
#define YELLOW  2
#define MAGENTA 3
#define GREEN   4
#define CYAN    5
#define BLUE    6
#define GREY    7
#define WHITE   8

const long double PI = 3.141592653589793238L;
float normal_scale = 0.3;




myObject::myObject(int t)
{
    type = t;
    num_faces = 0;
    make_object();
}

myObject::~myObject()
{}

void myObject::make_object()
{
	u[0] = 1;
	u[1] = 0;
	u[2] = 0;
	u[3] = 1;
    switch (type)
    {
    case CUBE:
        {
            make_cube();
        } break;
    case HOUSE:
        {
            make_house();
        } break;
    case SPHERE:
        {
            make_sphere();
        } break;
    case CYLINDER:
        {
            make_cylinder();
        } break;
    case CONE:
        {
            make_cone();
        } break;
    case TORUS:
        {
            make_torus();
        } break;
    case WALL
        :
        {
            make_wall();
        } break;
    case TREE:
        {
            make_cylinder();
        } break;
    }
}

void myObject::make_sphere()
{
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

    resetCTM();
    
	color[0] = 1.0;
	color[1] = 0.0;
	color[2] = 1.0;

    this->ray = 1.0;
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;

	radial_slices = 20;
	vertical_slices = 20;
    int rs = radial_slices - 1;
    int vs = vertical_slices;
	
	std::vector<float> vert_start;
    vert_start.resize(4,0);
    vert_start[0]=0;
    vert_start[1]=ray;
    vert_start[2]=0;
    vert_start[3]=1;

	std::vector<float> curr_vert;
    std::vector<float> curr_vert2;

	//verts[0][*] are at the top
	float theta_v;
    float theta_r;

	// rotate about z
    for (int r_i = 0; r_i <= rs; r_i++)
    {
        theta_v = -r_i*PI/rs;
        curr_vert = RotateVertZ(vert_start, theta_v);
		
		// rotate about y
        for (int v_i = 0; v_i < vs; v_i++)
        {
            theta_r = -v_i*2*PI/vs;
            
			curr_vert2 = RotateVertY(curr_vert, theta_r);
            if (r_i == rs)
            {
                curr_vert2[0] = 0;
                curr_vert2[1] = -ray;
                curr_vert2[0] = 0;
            }
			verts[r_i][v_i][0] = curr_vert2[0];
            verts[r_i][v_i][1] = curr_vert2[1];
            verts[r_i][v_i][2] = curr_vert2[2];
            verts[r_i][v_i][3] = curr_vert2[3];
		}

	}
	
	int face_count[50][50];
	
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			face_count[i][j] = 0;

			normals[i][j][0] = 0;
            normals[i][j][1] = 0;
            normals[i][j][2] = 0;
            normals[i][j][3] = 1;
		}
	}
	
	
	// CALCULATING VERTEX NORMALS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// STORING THE FACES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// radial slices
    num_faces = 0;
	for (int i = 0; i < radial_slices-1; i++)
	{
		// vertical slices
		for (int j = 0; j < vertical_slices; j++)
		{
            int sample_array[4][2] = {{i, j}, {i, (j+1)%vs},
							{i+1, (j+1)%vs}, {i+1, j}};
			// storing the indices of each vertex for the face

			for (int k = 0; k < 4; k++)
            {
                for (int l = 0; l < 2; l++)
                {
                    faces[num_faces][k][l] = sample_array[k][l];
                }
            }

			num_faces++;
			
			// calculating normal for this particular face
            float p[3] = {verts[i][j][0], verts[i][j][1], verts[i][j][2]};
            float q[3] = {verts[i+1][j][0], verts[i+1][j][1], verts[i+1][j][2]};
            float r[3] = {verts[i+1][(j+1)%vs][0], verts[i+1][(j+1)%vs][1], verts[i+1][(j+1)%vs][2]};

			float pq[3] = {q[0] - p[0],
				            q[1] - p[1],
				            q[2] - p[2]};
				
			float pr[3] = {r[0] - p[0],
				            r[1] - p[1],
				            r[2] - p[2]};

			// pq X pr
            float norm[3] = {pq[1]*pr[2] - pr[1]*pq[2],
                             pq[2]*pr[0] - pr[2]*pq[0],
                             pq[0]*pr[1] - pr[0]*pq[1]};

            float magnitude = sqrt(pow(norm[0], 2) + pow(norm[1], 2) + pow(norm[2], 2));
			
			// storing the sum in normals array for now
            norm[0] = norm[0]/magnitude;
            norm[1] = norm[1]/magnitude;
            norm[2] = norm[2]/magnitude;

			normals[i][j][0] += norm[0];
			normals[i][j][1] += norm[1];
			normals[i][j][2] += norm[2];
			
			normals[i][(j+1)%vs][0] += norm[0];
			normals[i][(j+1)%vs][1] += norm[1];
			normals[i][(j+1)%vs][2] += norm[2];
			
			normals[i+1][(j+1)%vs][0] += norm[0];
			normals[i+1][(j+1)%vs][1] += norm[1];
			normals[i+1][(j+1)%vs][2] += norm[2];
			
			normals[i+1][j][0] += norm[0];
			normals[i+1][j][1] += norm[1];
			normals[i+1][j][2] += norm[2];
			
			// will use the face count at each point to determine
			// the average
			face_count[i][j] += 1;
			face_count[i][(j+1)%vs] += 1;
			face_count[i+1][(j+1)%vs] += 1;
			face_count[i+1][j] += 1;
			
		}
        
	}

	for (int i = 0; i < radial_slices; i++)
	{
		for (int j = 0; j < vs; j++)
		{
            // handling the special cases for top & bottom vertices:
            // Because of how rotation gets done, there will be vs number of vertices
            // all at (0, ray, 0) and (0, -ray, 0).  The sphere should technically only have
            // a single vertex at each pole, but because of the way I have the sphere done,
            // the vertices at the top and bottom will only be used for two faces, causing the
            // normals to be odd.
            if (i == 0)
            {
                normals[i][j][0] = 0;
                normals[i][j][1] = 1;
                normals[i][j][2] = 0;
            }
            else if (i == radial_slices - 1)
            {
                normals[i][j][0] = 0;
                normals[i][j][1] = -1;
                normals[i][j][2] = 0;
            }
            else
            {
			    normals[i][j][0] = normals[i][j][0]/float(face_count[i][j]);
			    normals[i][j][1] = normals[i][j][1]/float(face_count[i][j]);
			    normals[i][j][2] = normals[i][j][2]/float(face_count[i][j]);
            }
		}
	}

}

void myObject::draw_sphere(int render_mode)
{

    float x, n_x;
    float y, n_y;
    float z, n_z;
    int v_index;
    int r_index;
    // i = face identifier
	// j = vertex identifier
	// then we need to use the x, y, and z locations
    glColor3f(color[0], color[1], color[2]);
	for (int i = 0; i < num_faces; i++)
	{
		glBegin(render_mode);
		for (int j = 0; j < 4; j++)
		{
			r_index = faces[i][j][0];
			v_index = faces[i][j][1];
			x = verts[r_index][v_index][0];
            y = verts[r_index][v_index][1];
            z = verts[r_index][v_index][2];
            n_x = normals[r_index][v_index][0];
            n_y = normals[r_index][v_index][1];
            n_z = normals[r_index][v_index][2];

			if (j == 0)
			{
				glTexCoord2f(0, 0);
			}
			else if (j == 1)
			{
				glTexCoord2f(0, 1);
			}
			else if (j == 2)
			{
				glTexCoord2f(1, 1);
			}
			else if (j == 3)
			{
				glTexCoord2f(1, 0);
			}
            glNormal3f(n_x, n_y, n_z);

			glVertex4f(x, y, z, 1);
		}
		glEnd();
		
	}
}

void myObject::make_cone()
{
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

    this->ray = 1;
    this->height = 1;
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;
	
    resetCTM();

	color[0] = 0.0;
	color[1] = 1.0;
	color[2] = 0.0;

	radial_slices = 20;
	vertical_slices = 20;
    int rs = radial_slices;
    int vs = vertical_slices;

	num_faces = 0;
	
	int v_i = 0;
	int r_i = 0;
	//using a tip radius that is slightly > 0 so that
	//scaling will work correctly.
	float tip_radius = 0.01;
	float scaling_factor = ray/float(rs);

    std::vector<float> vert_start;
    vert_start.resize(4,0);
    vert_start[0]=0;
    vert_start[1]= float(-height)/float(2);
    vert_start[2]=0;
    vert_start[3]=1;
	

    std::vector<float> vert1;
    std::vector<float> vert2;
    std::vector<float> vert3;
	//verts[0][*] are the vertices around the tip
	//verts[rs][*] are the vertices around opening at the top 
	
	for (int r_i = 0; r_i < rs; r_i++)
	{	
		// scale along x and translate up to h
		vert1 = TranslateVert(vert_start, scaling_factor*r_i, 0, 0);
		vert2 = TranslateVert(vert1, 0, (height/rs)*r_i, 0);
		
		// rotate around y
		for (float theta_v = 0; theta_v < 2*PI; theta_v += 2*PI/vs)
		{
			vert3 = RotateVertY(vert2, theta_v);
			verts[r_i][v_i][0] = vert3[0];
            verts[r_i][v_i][1] = vert3[1];
            verts[r_i][v_i][2] = vert3[2];
            verts[r_i][v_i][3] = vert3[3];
			v_i++;
		}
		
		//r_i++;
		v_i = 0;
	}
	
    int face_count[50][50];
	
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			face_count[i][j] = 0;

			normals[i][j][0] = 0;
            normals[i][j][1] = 0;
            normals[i][j][2] = 0;
            normals[i][j][3] = 1;
		}
	}

	// storing the faces
    num_faces = 0;
	for (int i = 0; i < rs-1; i++)
	{
		for (int j = 0; j < vs; j++)
		{
			// storing the indices of each vertex for the face
            int sample_array[4][2] = {{i+1, j}, {i+1, (j+1)%vs},
							{i, (j+1)%vs}, {i, j}};
			// storing the indices of each vertex for the face

			for (int k = 0; k < 4; k++)
            {
                for (int l = 0; l < 2; l++)
                {
                    faces[num_faces][k][l] = sample_array[k][l];
                }
            }

            num_faces++;

			// calculating normal for this particular face
            float p[3] = {verts[i][j][0], verts[i][j][1], verts[i][j][2]};
            float q[3] = {verts[i+1][j][0], verts[i+1][j][1], verts[i+1][j][2]};
            float r[3] = {verts[i+1][(j+1)%vs][0], verts[i+1][(j+1)%vs][1], verts[i+1][(j+1)%vs][2]};

			float pq[3] = {q[0] - p[0],
				            q[1] - p[1],
				            q[2] - p[2]};
				
			float pr[3] = {r[0] - p[0],
				            r[1] - p[1],
				            r[2] - p[2]};

            //pq X pr
            float norm[3] = {pq[1]*pr[2] - pr[1]*pq[2],
                             pq[2]*pr[0] - pr[2]*pq[0],
                             pq[0]*pr[1] - pr[0]*pq[1]};
            
            float magnitude = sqrt(pow(norm[0], 2) + pow(norm[1], 2) + pow(norm[2], 2));
			
            norm[0] = norm[0]/magnitude;
            norm[1] = norm[1]/magnitude;
            norm[2] = norm[2]/magnitude;
			// storing the sum in normals array for now
			
			normals[i][j][0] += norm[0];
			normals[i][j][1] += norm[1];
			normals[i][j][2] += norm[2];
			
			normals[i][(j+1)%vs][0] += norm[0];
			normals[i][(j+1)%vs][1] += norm[1];
			normals[i][(j+1)%vs][2] += norm[2];
			
			normals[i+1][(j+1)%vs][0] += norm[0];
			normals[i+1][(j+1)%vs][1] += norm[1];
			normals[i+1][(j+1)%vs][2] += norm[2];
			
			normals[i+1][j][0] += norm[0];
			normals[i+1][j][1] += norm[1];
			normals[i+1][j][2] += norm[2];
			
			// will use the face count at each point to determine
			// the average
			face_count[i][j] += 1;
			face_count[i][(j+1)%vs] += 1;
			face_count[i+1][(j+1)%vs] += 1;
			face_count[i+1][j] += 1;
			
		}
	}
	
	for (int i = 0; i < radial_slices; i++)
	{
		for (int j = 0; j < vs; j++)
		{
            // handling the case for the normals at the tip of the cone
            if (i == 0)
            {
                normals[i][j][0] = 0;
                normals[i][j][1] = -1;
                normals[i][j][2] = 0;
            }
            else
            {
			    normals[i][j][0] = normals[i][j][0]/float(face_count[i][j]);
			    normals[i][j][1] = normals[i][j][1]/float(face_count[i][j]);
			    normals[i][j][2] = normals[i][j][2]/float(face_count[i][j]);
            }
		}
	}

}

void myObject::draw_cone(int render_mode)
{
    float x, n_x;
    float y, n_y;
    float z, n_z;
    int v_index;
    int r_index;
    glColor3f(0.0, 1.0, 0.0);
	for (int i = 0; i < num_faces; i++)
	{
		glBegin(render_mode);
		for (int j = 0; j < 4; j++)
		{
			r_index = faces[i][j][0];
			v_index = faces[i][j][1];
			x = verts[r_index][v_index][0];
            y = verts[r_index][v_index][1];
            z = verts[r_index][v_index][2];
            n_x = normals[r_index][v_index][0];
            n_y = normals[r_index][v_index][1];
            n_z = normals[r_index][v_index][2];

            glNormal3f(n_x, n_y, n_z);
			glVertex4f(x,y,z,1);
		}
		glEnd();
		
	}
}

void myObject::make_cylinder()
{
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

    this->ray = 0.5;
    this->height = 1.0;
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;
	
    resetCTM();

	color[0] = 0.9;
	color[1] = 0.25;
	color[2] = 0;

	radial_slices = 2;
	vertical_slices = 15;
    int rs = radial_slices - 1;
    int vs = vertical_slices;

	int v_i = 0;
	int r_i = 0;

    std::vector<float> vert_start;
    vert_start.resize(4,0);
    vert_start[0]=ray;
    vert_start[1]=float(-height)/float(2);
    vert_start[2]=0;
    vert_start[3]=1;
	

    std::vector<float> vert1;
    std::vector<float> vert2;

	//verts[0][*] are the vertices around the tip
	//verts[rs][*] are the vertices around opening at the top 
	
	for (int i = 0; i <= rs; i++)
	{	
		// scale along x and translate up to h
		vert1 = TranslateVert(vert_start, 0, (height/float(rs))*r_i, 0);
		
		// rotate around y
		for (float theta_v = 0; theta_v < 2*PI; theta_v = theta_v + 2*PI/float(vs))
		{
			vert2 = RotateVertY(vert1, theta_v);
			verts[r_i][v_i][0] = vert2[0];
            verts[r_i][v_i][1] = vert2[1];
            verts[r_i][v_i][2] = vert2[2];
            verts[r_i][v_i][3] = vert2[3];
			
			v_i++;
		}
		
		r_i++;
		v_i = 0;
	}
	

	int face_count[50][50];
	
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			face_count[i][j] = 0;

			normals[i][j][0] = 0;
            normals[i][j][1] = 0;
            normals[i][j][2] = 0;
            normals[i][j][3] = 1;
		}
	}

	// storing the faces
    num_faces = 0;
	for (int i = 0; i < rs; i++)
	{
		for (int j = 0; j < vs; j++)
		{
			// storing the indices of each vertex for the face
            int sample_array[4][2] = {{i+1, j}, {i+1, (j+1)%vs},
							{i, (j+1)%vs}, {i, j}};
			// storing the indices of each vertex for the face

			for (int k = 0; k < 4; k++)
            {
                for (int l = 0; l < 2; l++)
                {
                    faces[num_faces][k][l] = sample_array[k][l];
                }
            }

            num_faces++;
			float p[3] = {verts[i+1][j][0], verts[i+1][j][1], verts[i+1][j][2]};
            float q[3] = {verts[i+1][(j+1)%vs][0], verts[i+1][(j+1)%vs][1], verts[i+1][(j+1)%vs][2]};
			float r[3] = {verts[i][j][0], verts[i][j][1], verts[i][j][2]};

			float pq[3] = {q[0] - p[0],
				            q[1] - p[1],
				            q[2] - p[2]};
				
			float pr[3] = {r[0] - p[0],
				            r[1] - p[1],
				            r[2] - p[2]};

            //pq X pr
            float norm[3] = {pq[1]*pr[2] - pr[1]*pq[2],
                             pq[2]*pr[0] - pr[2]*pq[0],
                             pq[0]*pr[1] - pr[0]*pq[1]};
            
            float magnitude = sqrt(pow(norm[0], 2) + pow(norm[1], 2) + pow(norm[2], 2));
			// storing the sum in normals array for now
			
            norm[0] = norm[0]/magnitude;
            norm[1] = norm[1]/magnitude;
            norm[2] = norm[2]/magnitude;
			// storing the sum in normals array for now
			
			normals[i][j][0] += norm[0];
			normals[i][j][1] += norm[1];
			normals[i][j][2] += norm[2];
			
			normals[i][(j+1)%vs][0] += norm[0];
			normals[i][(j+1)%vs][1] += norm[1];
			normals[i][(j+1)%vs][2] += norm[2];
			
			normals[i+1][(j+1)%vs][0] += norm[0];
			normals[i+1][(j+1)%vs][1] += norm[1];
			normals[i+1][(j+1)%vs][2] += norm[2];
			
			normals[i+1][j][0] += norm[0];
			normals[i+1][j][1] += norm[1];
			normals[i+1][j][2] += norm[2];
			
			// will use the face count at each point to determine
			// the average
			face_count[i][j] += 1;
			face_count[i][(j+1)%vs] += 1;
			face_count[i+1][(j+1)%vs] += 1;
			face_count[i+1][j] += 1;
			
		}
	}
	
	for (int i = 0; i < radial_slices; i++)
	{
		for (int j = 0; j < vs; j++)
		{
			normals[i][j][0] = normals[i][j][0]/float(face_count[i][j]);
			normals[i][j][1] = normals[i][j][1]/float(face_count[i][j]);
			normals[i][j][2] = normals[i][j][2]/float(face_count[i][j]);
		}
	}
}

void myObject::draw_cylinder(int render_mode)
{
    float x, n_x;
    float y, n_y;
    float z, n_z;
    int v_index;
    int r_index;
    glColor3f(1, 1, 1);
	for (int i = 0; i < num_faces; i++)
	{
		glBegin(render_mode);
		for (int j = 0; j < 4; j++)
		{			
			r_index = faces[i][j][0];
			v_index = faces[i][j][1];
			x = verts[r_index][v_index][0];
            y = verts[r_index][v_index][1];
            z = verts[r_index][v_index][2];
            n_x = normals[r_index][v_index][0];
            n_y = normals[r_index][v_index][1];
            n_z = normals[r_index][v_index][2];

            glNormal3f(n_x, n_y, n_z);
			if (j == 0)
			{
				glTexCoord2f(0, 0);
			}
			else if (j == 1)
			{
				glTexCoord2f(0, .2);
			}
			else if (j == 2)
			{
				glTexCoord2f(30, .2);
			}
			else if (j == 3)
			{
				glTexCoord2f(30, 0);
			}

			glVertex4f(x,y,z,1);
		}
		glEnd();
		
	}
}

void myObject::make_torus()
{
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

    this->ray = 1.2;
    this->ray2 = 0.2;
	center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    resetCTM();

	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 0;

	radial_slices = 20;
	vertical_slices = 20;
	int rs = radial_slices;
    int vs = vertical_slices;

	int v_i = 0;
	int r_i = 0;
	std::vector<float> vert_start;
    vert_start.resize(4,0);
    vert_start[0] = ray2;
    vert_start[1] = 0;
    vert_start[2] = 0;
    vert_start[3] = 1;

    std::vector<float> vert1;
    std::vector<float> vert2;
    std::vector<float> vert3;
	
	for (float theta_v = 0; theta_v < 2*PI; theta_v += 2*PI/rs)
	{
		// rotate about z, then translate along x to r1
		vert1 = RotateVertZ(vert_start, theta_v);
		vert2 = TranslateVert(vert1, ray, 0, 0);
		
		// rotate around y
		for (float theta_r = 0; theta_r < 2*PI; theta_r += 2*PI/vs)
		{
			vert3 = RotateVertY(vert2, theta_r);
			verts[r_i][v_i][0] = vert3[0];
            verts[r_i][v_i][1] = vert3[1];
            verts[r_i][v_i][2] = vert3[2];
            verts[r_i][v_i][3] = vert3[3];
			v_i++;
		}
		
		r_i++;
		v_i = 0;
	}
	
	int face_count[50][50];
	
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			face_count[i][j] = 0;

			normals[i][j][0] = 0;
            normals[i][j][1] = 0;
            normals[i][j][2] = 0;
            normals[i][j][3] = 1;
		}
	}

	// storing faces
    num_faces = 0;
	for (int i = 0; i < rs; i++)
	{
		for (int j = 0; j < vs; j++)
		{
            int sample_array[4][2] = {{i, j}, {i, (j+1)%vs},
							{(i+1)%rs, (j+1)%vs}, {(i+1)%rs, j}};
			// storing the indices of each vertex for the face
			
            for (int k = 0; k < 4; k++)
            {
                for (int l = 0; l < 2; l++)
                {
                    faces[num_faces][k][l] = sample_array[k][l];
                }
            }
			num_faces++;
			float p[3] = {verts[i+1][j][0], verts[i+1][j][1], verts[i+1][j][2]};
            float q[3] = {verts[i+1][(j+1)%vs][0], verts[i+1][(j+1)%vs][1], verts[i+1][(j+1)%vs][2]};
			float r[3] = {verts[i][j][0], verts[i][j][1], verts[i][j][2]};

			float pq[3] = {q[0] - p[0],
				            q[1] - p[1],
				            q[2] - p[2]};
				
			float pr[3] = {r[0] - p[0],
				            r[1] - p[1],
				            r[2] - p[2]};

            //pq X pr
            float norm[3] = {pq[1]*pr[2] - pr[1]*pq[2],
                             pq[2]*pr[0] - pr[2]*pq[0],
                             pq[0]*pr[1] - pr[0]*pq[1]};
            
            float magnitude = sqrt(pow(norm[0], 2) + pow(norm[1], 2) + pow(norm[2], 2));
			// storing the sum in normals array for now
			
            norm[0] = norm[0]/magnitude;
            norm[1] = norm[1]/magnitude;
            norm[2] = norm[2]/magnitude;
			// storing the sum in normals array for now
			
			normals[i][j][0] += norm[0];
			normals[i][j][1] += norm[1];
			normals[i][j][2] += norm[2];
			
			normals[i][(j+1)%vs][0] += norm[0];
			normals[i][(j+1)%vs][1] += norm[1];
			normals[i][(j+1)%vs][2] += norm[2];
			
			normals[i+1][(j+1)%vs][0] += norm[0];
			normals[i+1][(j+1)%vs][1] += norm[1];
			normals[i+1][(j+1)%vs][2] += norm[2];
			
			normals[i+1][j][0] += norm[0];
			normals[i+1][j][1] += norm[1];
			normals[i+1][j][2] += norm[2];
			
			// will use the face count at each point to determine
			// the average
			face_count[i][j] += 1;
			face_count[i][(j+1)%vs] += 1;
			face_count[i+1][(j+1)%vs] += 1;
			face_count[i+1][j] += 1;
			
		}
	}
	
	for (int i = 0; i < radial_slices; i++)
	{
		for (int j = 0; j < vs; j++)
		{
			normals[i][j][0] = normals[i][j][0]/float(face_count[i][j]);
			normals[i][j][1] = normals[i][j][1]/float(face_count[i][j]);
			normals[i][j][2] = normals[i][j][2]/float(face_count[i][j]);
		}
	}
}

void myObject::draw_torus(int render_mode)
{
    float x, n_x;
    float y, n_y;
    float z, n_z;
    int v_index;
    int r_index;
    glColor3f(1.0, 1.0, 0.0);
	for (int i = 0; i < num_faces; i++)
	{
		glBegin(render_mode);
		for (int j = 0; j < 4; j++)
		{
			r_index = faces[i][j][0];
			v_index = faces[i][j][1];
			x = verts[r_index][v_index][0];
            y = verts[r_index][v_index][1];
            z = verts[r_index][v_index][2];
            n_x = normals[r_index][v_index][0];
            n_y = normals[r_index][v_index][1];
            n_z = normals[r_index][v_index][2];

            glNormal3f(n_x, n_y, n_z);
			glVertex4f(x,y,z,1);
		}
		glEnd();
		
	}
}

void myObject::make_house()
{
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

	center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    resetCTM();

    GLfloat vertices_house[][4] = {{0,2,0,1}, 
      {-1,1,1,1}, {1,1,1,1}, {1,1,-1,1}, {-1,1,-1,1},
      {-1,-1,1,1}, {1,-1,1,1}, {1,-1,-1,1}, {-1,-1,-1,1}
    };

    GLfloat house_normals[][4] = {{0,1,0,1}, 
    {(-1 - 1/sqrt(float(2)))/float(3), (2/sqrt(float(2)))/float(3), (1 + 1/sqrt(float(2)))/float(3),1}, //front top left
    {(1 + 1/sqrt(float(2)))/float(3), (2/sqrt(float(2)))/float(3), (1 + 1/sqrt(float(2)))/float(3),1}, // front top right
    {(1 + 1/sqrt(float(2)))/float(3), (2/sqrt(float(2)))/float(3), (-1 - 1/sqrt(float(2)))/float(3),1}, // back top right
    {(-1 - 1/sqrt(float(2)))/float(3), (2/sqrt(float(2)))/float(3), (-1 - 1/sqrt(float(2)))/float(3),1}, // back top left

    {(-1)/float(3), (-1)/float(3), (1)/float(3),1}, // front bottom left
    {(1)/float(3), (-1)/float(3), (1)/float(3),1}, // front bottom right
    {(1)/float(3), (-1)/float(3), (-1)/float(3),1}, // back bottom right
    {(-1)/float(3), (-1)/float(3), (-1)/float(3),1}}; // back bottom left

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            verts[0][i][j] = vertices_house[i][j];
            normals[0][i][j] = house_normals[i][j];
        }
    }

}

void myObject::draw_house(int crt_render_mode)
{
    draw_triangle(verts[0],0,1,2,RED, crt_render_mode);
    draw_triangle(verts[0],0,2,3,GREEN, crt_render_mode);
    draw_triangle(verts[0],0,3,4,WHITE, crt_render_mode);
    draw_triangle(verts[0],0,4,1,GREY, crt_render_mode);

    draw_quad(verts[0],2,1,5,6, BLUE, crt_render_mode);
    draw_triangle(verts[0],2,6,3, CYAN, crt_render_mode);
    draw_triangle(verts[0],3,6,7, CYAN, crt_render_mode);
    draw_triangle(verts[0],3,7,8, YELLOW, crt_render_mode);
    draw_triangle(verts[0],8,3,4, YELLOW, crt_render_mode);
    draw_triangle(verts[0],4,8,1, MAGENTA, crt_render_mode);
    draw_triangle(verts[0],1,8,5, MAGENTA, crt_render_mode);

}

void myObject::make_cube()
{
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

	center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    resetCTM();

    GLfloat vertices_cube[][4] = {
      {-0.5,0.5,0.5,1}, {0.5,0.5,0.5,1}, {0.5,0.5,-0.5,1}, {-0.5,0.5,-0.5,1},
      {-0.5,-0.5,0.5,1}, {0.5,-0.5,0.5,1}, {0.5,-0.5,-0.5,1}, {-0.5,-0.5,-0.5,1}
      };

    GLfloat cube_normals[][4] = {
        {(-1)/float(3), (1)/float(3), (1)/float(3),1}, // front top left
        {(1)/float(3), (1)/float(3), (1)/float(3),1}, // front top right
        {(1)/float(3), (1)/float(3), (-1)/float(3),1}, // back top right
        {(-1)/float(3), (1)/float(3), (-1)/float(3),1}, // back top left

        {(-1)/float(3), (-1)/float(3), (1)/float(3),1}, // front bottom left
        {(1)/float(3), (-1)/float(3), (1)/float(3),1}, // front bottom right
        {(1)/float(3), (-1)/float(3), (-1)/float(3),1}, // back bottom right
        {(-1)/float(3), (-1)/float(3), (-1)/float(3),1}}; // back bottom left

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            verts[0][i][j] = vertices_cube[i][j];
            normals[0][i][j] = cube_normals[i][j];
        }
    }
}

void myObject::draw_cube(int crt_render_mode)
{
    draw_triangle(verts[0], 4,5,1,BLUE, crt_render_mode);
    draw_triangle(verts[0], 0,4,1,BLUE, crt_render_mode);
    draw_triangle(verts[0], 5,6,2,CYAN, crt_render_mode);
    draw_triangle(verts[0], 1,5,2,CYAN, crt_render_mode);
    draw_triangle(verts[0], 3,2,6,YELLOW, crt_render_mode);
    draw_triangle(verts[0], 7,3,6,YELLOW, crt_render_mode);
    draw_triangle(verts[0], 0,3,7,MAGENTA, crt_render_mode);
    draw_triangle(verts[0], 4,0,7,MAGENTA, crt_render_mode);
    // top
    draw_triangle(verts[0], 1,2,3,GREEN, crt_render_mode);
    draw_triangle(verts[0], 3,0,1,GREEN, crt_render_mode);
    // bottom
    draw_triangle(verts[0], 5,6,7,RED, crt_render_mode);
    draw_triangle(verts[0], 7,4,5,RED, crt_render_mode);
}

void myObject::make_wall()
{
    // x,y,z = coord. of bottom front left
    central_axis[0] = 0;
    central_axis[1] = 1;
    central_axis[2] = 0;
    central_axis[3] = 1;

	center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    resetCTM();

    GLfloat vertices_wall[][4] = {
      {-0.5,0.5,0.5,1}, {0.5,0.5,0.5,1}, {0.5,0.5,-0.5,1}, {-0.5,0.5,-0.5,1},
      {-0.5,-0.5,0.5,1}, {0.5,-0.5,0.5,1}, {0.5,-0.5,-0.5,1}, {-0.5,-0.5,-0.5,1}
      };

    GLfloat wall_normals[][4] = {
        {(-1)/float(3), (1)/float(3), (1)/float(3),1}, // front top left
        {(1)/float(3), (1)/float(3), (1)/float(3),1}, // front top right
        {(1)/float(3), (1)/float(3), (-1)/float(3),1}, // back top right
        {(-1)/float(3), (1)/float(3), (-1)/float(3),1}, // back top left

        {(-1)/float(3), (-1)/float(3), (1)/float(3),1}, // front bottom left
        {(1)/float(3), (-1)/float(3), (1)/float(3),1}, // front bottom right
        {(1)/float(3), (-1)/float(3), (-1)/float(3),1}, // back bottom right
        {(-1)/float(3), (-1)/float(3), (-1)/float(3),1}}; // back bottom left

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            verts[0][i][j] = vertices_wall[i][j];
            normals[0][i][j] = wall_normals[i][j];
        }
    }
}

void myObject::draw_wall(int crt_render_mode)
{
	glColor4f(1, 1, 1, 1);
	//front
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex4f(verts[0][4][0], verts[0][4][1], verts[0][4][2], 1);	//front bottom left
		glTexCoord2f(0.0f, 1);
		glVertex4f(verts[0][0][0], verts[0][0][1], verts[0][0][2], 1);	//front top left
		glTexCoord2f(length, 1);
		glVertex4f(verts[0][1][0], verts[0][1][1], verts[0][1][2], 1);	//front top right
		glTexCoord2f(length, 0.0f);	
		glVertex4f(verts[0][5][0], verts[0][5][1], verts[0][5][2], 1);	//front bottom right
	glEnd();

	//back
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex4f(verts[0][6][0], verts[0][6][1], verts[0][6][2], 1);	//back bottom right
		glTexCoord2f(0.0f, 1);
		glVertex4f(verts[0][2][0], verts[0][2][1], verts[0][2][2], 1);	//back top right
		glTexCoord2f(length, 1);
		glVertex4f(verts[0][3][0], verts[0][3][1], verts[0][3][2], 1);	//back top left
		glTexCoord2f(length, 0.0f);	
		glVertex4f(verts[0][7][0], verts[0][7][1], verts[0][7][2], 1);	//back bottom left
	glEnd();

	//right
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex4f(verts[0][5][0], verts[0][5][1], verts[0][5][2], 1);	//front bottom right
		glTexCoord2f(0.0f, 1);
		glVertex4f(verts[0][1][0], verts[0][1][1], verts[0][1][2], 1);	//front top right
		glTexCoord2f(width, 1);
		glVertex4f(verts[0][2][0], verts[0][2][1], verts[0][2][2], 1);	//back top right
		glTexCoord2f(width, 0);
		glVertex4f(verts[0][6][0], verts[0][6][1], verts[0][6][2], 1);	//back bottom right
	glEnd();

	//left
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex4f(verts[0][7][0], verts[0][7][1], verts[0][7][2], 1);	//back bottom left
		glTexCoord2f(0.0f, 1);
		glVertex4f(verts[0][3][0], verts[0][3][1], verts[0][3][2], 1);	//back top left
		glTexCoord2f(width, 1);
		glVertex4f(verts[0][0][0], verts[0][0][1], verts[0][0][2], 1);	//front top left
		glTexCoord2f(width, 0);
		glVertex4f(verts[0][4][0], verts[0][4][1], verts[0][4][2], 1);	//front bottom left
	glEnd();

	/*
    draw_triangle(verts[0], 4,5,1,RED, crt_render_mode);
    draw_triangle(verts[0], 0,4,1,RED, crt_render_mode);
    draw_triangle(verts[0], 5,6,2,RED, crt_render_mode);
    draw_triangle(verts[0], 1,5,2,RED, crt_render_mode);
    draw_triangle(verts[0], 3,2,6,RED, crt_render_mode);
    draw_triangle(verts[0], 7,3,6,RED, crt_render_mode);
    draw_triangle(verts[0], 0,3,7,RED, crt_render_mode);
    draw_triangle(verts[0], 4,0,7,RED, crt_render_mode);
    // top
    draw_triangle(verts[0], 1,2,3,RED, crt_render_mode);
    draw_triangle(verts[0], 3,0,1,RED, crt_render_mode);
    // bottom
    draw_triangle(verts[0], 5,6,7,RED, crt_render_mode);
    draw_triangle(verts[0], 7,4,5,RED, crt_render_mode);
	*/
}

void myObject::setWall(float x, float y, float z, float l, float w, float h)
{
    // x,y,z = coord. of bottom front left
	ScaleObject(l, h, w);
	TranslateObject(x + l/2, y + h/2, z - w/2);
	width = w;
	length = l;

}

void myObject::draw_normals()
{
	glColor3f(0.0, 1.0, 1.0);
	float x, x1, y, y1, z, z1;

	if (type == CUBE || type == HOUSE || type == WALL)
	{
		float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }

		for (int i = 0; i < num_verts; i++)
		{
			x = verts[0][i][0];
			y = verts[0][i][1];
			z = verts[0][i][2];

			x1 = x + normals[0][i][0]*normal_scale;
			y1 = y + normals[0][i][1]*normal_scale;
			z1 = z + normals[0][i][2]*normal_scale;

			glBegin(GL_LINES);
			glVertex4f(x,y,z,1);
			glVertex4f(x1,y1,z1,1);

			glEnd();


		}

	}
	else
	{
    
		for (int i = 0; i < radial_slices; i++)
		{
			for (int j = 0; j < vertical_slices; j++)
			{   
				x = verts[i][j][0];
				y = verts[i][j][1];
				z = verts[i][j][2];

				x1 = x + normals[i][j][0]*normal_scale;
				y1 = y + normals[i][j][1]*normal_scale;
				z1 = z + normals[i][j][2]*normal_scale;

				glBegin(GL_LINES);
				glVertex4f(x,y,z,1);
				glVertex4f(x1,y1,z1,1);

				glEnd();
			}
		}
	}
}

void myObject::draw(int render_mode)
{
    switch (type)
    {
        case CUBE:
        {
            draw_cube(render_mode);
        }; break;
        case HOUSE:
        {
            draw_house(render_mode);
        }; break;
        case SPHERE:
        {
            draw_sphere(render_mode);
        }; break;
        case CYLINDER:
        {
            draw_cylinder(render_mode);
        }; break;
        case TORUS:
        {
            draw_torus(render_mode);
        }; break;
        case CONE:
        {
            draw_cone(render_mode);
        }; break;
        case WALL:
        {
            draw_wall(render_mode);
        }; break;
        case TREE:
        {
            draw_cylinder(render_mode);
        }; break;

        default: break;
    }
}

std::vector<float> myObject::RotateVertX(std::vector<float> vert, float theta)
{
	std::vector<float> new_vert;
    new_vert.assign(4, 0);
    float r_matrix[4][4] = {{1,0,0,0},
					{0, cos(theta), -sin(theta), 0},
					{0, sin(theta), cos(theta), 0},
					{0, 0, 0, 1}};

	for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += vert[j]*r_matrix[i][j];
		}
		
		new_vert[i] = curr_sum;
	
	}
	
    return new_vert;
}

std::vector<float> myObject::RotateVertY(std::vector<float> vert, float theta)
{
    std::vector<float> new_vert;
    new_vert.assign(4, 0);
    float r_matrix[4][4] = {{cos(theta), 0, -sin(theta), 0},
					{0, 1, 0, 0},
					{sin(theta), 0, cos(theta), 0},
					{0, 0, 0, 1}};

    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += vert[j]*r_matrix[i][j];
		}
		
		new_vert[i] = curr_sum;
	
	}
	
    return new_vert;
}

std::vector<float> myObject::RotateVertZ(std::vector<float> vert, float theta)
{
    std::vector<float> new_vert;
    new_vert.assign(4, 0);
    float r_matrix[4][4] = {{cos(theta), -sin(theta), 0, 0},
					{sin(theta), cos(theta), 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1}};

    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += vert[j]*r_matrix[i][j];
		}
		
		new_vert[i] = curr_sum;
	
	}
	
    return new_vert;
}

std::vector<float> myObject::ScaleVert(std::vector<float> vert, float s_x, float s_y, float s_z)
{
    std::vector<float> new_vert;
    new_vert.assign(4, 0);

	float r_matrix[4][4] = {{s_x, 0, 0, 0},
				            {0, s_y, 0, 0},
				            {0, 0, s_z, 0},
				            {0, 0, 0, 1}};
	
	for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += vert[j]*r_matrix[i][j];
		}
		
		new_vert[i] = curr_sum;
	
	}
	
	return new_vert;
}

std::vector<float> myObject::TranslateVert(std::vector<float> vert, float dx, float dy, float dz)
{
	std::vector<float> new_vert;
    new_vert.assign(4, 0);
	float r_matrix[4][4] = {{1, 0, 0, dx},
				            {0, 1, 0, dy},
				            {0, 0, 1, dz},
				            {0, 0, 0, 1}};
	
	for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += vert[j]*r_matrix[i][j];
		}
		
		new_vert[i] = curr_sum;
	
	}
	
	return new_vert;	
}

void myObject::TranslateObjectToOrigin()
{
    // translation matrix
	float r_matrix[4][4] = {{1, 0, 0, -center[0]},
				            {0, 1, 0, -center[1]},
				            {0, 0, 1, -center[2]},
				            {0, 0, 0, 1}};

	float r_matrixi[4][4] = {{1, 0, 0, center[0]},
				             {0, 1, 0, center[1]},
				             {0, 0, 1, center[2]},
				             {0, 0, 0, 1}};

    center[0] = 0;
	center[1] = 0;
	center[2] = 0;

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += r_matrix[i][k]*ctm[k][j];
				curr_sumi += r_matrixi[i][k]*ctmi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
		}
	}

    // update each of the vertices
	float new_vert[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*r_matrix[i][j];
		            }
				    new_vert[i] = curr_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*r_matrix[i][j];
		        }
				new_vert[i] = curr_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];
        }
    }
}

void myObject::TranslateObjectTo(double x, double y, double z)
{
	double tx = x - center[0];
    double ty = y - center[1];
    double tz = z - center[2];
    if (x != 0 || y != 0 || z != 0)
    {
        this->TranslateObject(tx, ty, tz);
    }
}

void myObject::TranslateObject(double x, double y, double z)
{
    // translation matrix
	float r_matrix[4][4] = {{1, 0, 0, x},
				            {0, 1, 0, y},
				            {0, 0, 1, z},
				            {0, 0, 0, 1}};

	float r_matrixi[4][4] = {{1, 0, 0, -x},
				             {0, 1, 0, -y},
				             {0, 0, 1, -z},
				             {0, 0, 0, 1}};
	
    // updating center position of object
    center[0] += x;
	center[1] += y;
	center[2] += z;

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += r_matrix[i][k]*ctm[k][j];
				//curr_sumi += r_matrixi[i][k]*ctmi[k][j];
				curr_sumi += ctmi[i][k]*r_matrixi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
        }
    }

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
		}
	}

    // updating vertices
	float new_vert[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*r_matrix[i][j];
		            }
				    new_vert[i] = curr_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*r_matrix[i][j];
		        }
				new_vert[i] = curr_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];
        }
    }
}

void myObject::RotateObject(double deg, int x, int y, int z)
{
    double theta = PI*double(deg)/double(180);
    if (x == 1)
    {
        RotateObjectX(theta);
    }
    else if (y == 1)
    {
        RotateObjectY(theta);
    }
    else if (z == 1)
    {
        RotateObjectZ(theta);
    }
}

void myObject::RotateObjectX(double theta)
{
    // x rotation matrix about world axis
	float r_matrix[4][4] = {{1,0,0,0},
					        {0, cos(theta), -sin(theta), 0},
					        {0, sin(theta), cos(theta), 0},
					        {0, 0, 0, 1}};

	float r_matrixi[4][4] = {{1,0,0,0},
					        {0, cos(-theta), -sin(-theta), 0},
					        {0, sin(-theta), cos(-theta), 0},
					        {0, 0, 0, 1}};
	
    // calculating new center location
    float new_center[3];
	for (int i = 0; i < 3; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 3; j++)
        {
			curr_sum += r_matrix[i][j]*center[j];
		}
		new_center[i] = curr_sum;
	}
	center[0] = new_center[0];
	center[1] = new_center[1];
	center[2] = new_center[2];

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
	float new_diri[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
			float diri_sum = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += r_matrix[i][k]*ctm[k][j];
				//curr_sumi += r_matrixi[i][k]*ctmi[k][j];
				curr_sumi += ctmi[i][k]*r_matrixi[k][j];
				diri_sum += diri[i][k]*r_matrixi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
			new_diri[i][j] = diri_sum;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
			diri[i][j] = new_diri[i][j];
		}
	}

    //update vertices and normals
	float new_vert[4];
	float new_norm[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*r_matrix[i][j];
                        curr_norm_sum += normals[r][v][j]*r_matrix[i][j];
		            }
				    new_vert[i] = curr_sum;
				    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*r_matrix[i][j];
                    curr_norm_sum += normals[0][v][j]*r_matrix[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }

    //update the direction of the central axis
    float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*r_matrix[i][j];
			curr_sum1 += u[j]*r_matrix[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	float mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;
}

void myObject::RotateObjectY(double theta)
{
    // y rotation matrix of world axis
	float r_matrix[4][4] = {{cos(theta), 0, sin(theta), 0},
					        {0, 1, 0, 0},
					        {-sin(theta), 0, cos(theta), 0},
					        {0, 0, 0, 1}};
	
	float r_matrixi[4][4] = {{cos(-theta), 0, sin(-theta), 0},
					        {0, 1, 0, 0},
					        {-sin(-theta), 0, cos(-theta), 0},
					        {0, 0, 0, 1}};

    // calculate new center position
    float new_center[3];
	for (int i = 0; i < 3; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 3; j++)
        {
			curr_sum += r_matrix[i][j]*center[j];
		}
		new_center[i] = curr_sum;
	}
	center[0] = new_center[0];
	center[1] = new_center[1];
	center[2] = new_center[2];

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
	float new_diri[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
			float diri_sum = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += r_matrix[i][k]*ctm[k][j];
				//curr_sumi += r_matrixi[i][k]*ctmi[k][j];
				curr_sumi += ctmi[i][k]*r_matrixi[k][j];
				diri_sum += diri[i][k]*r_matrixi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
			new_diri[i][j] = diri_sum;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
			diri[i][j] = new_diri[i][j];
		}
	}

    // update vertices and normals
	float new_vert[4];
	float new_norm[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*r_matrix[i][j];
                        curr_norm_sum += normals[r][v][j]*r_matrix[i][j];
		            }
				    new_vert[i] = curr_sum;
				    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*r_matrix[i][j];
                    curr_norm_sum += normals[0][v][j]*r_matrix[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }

    // calculate new direction of objects central axis
    float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*r_matrix[i][j];
			curr_sum1 += u[j]*r_matrix[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	float mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;
}

void myObject::RotateObjectZ(double theta)
{
    // z rotation about world axis
	float r_matrix[4][4] = {{cos(theta), -sin(theta), 0, 0},
					        {sin(theta), cos(theta), 0, 0},
					        {0, 0, 1, 0},
					        {0, 0, 0, 1}};

	float r_matrixi[4][4] = {{cos(-theta), -sin(-theta), 0, 0},
					        {sin(-theta), cos(-theta), 0, 0},
					        {0, 0, 1, 0},
					        {0, 0, 0, 1}};
	
    // calculate new center position
    float new_center[3];
	for (int i = 0; i < 3; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 3; j++)
        {
			curr_sum += r_matrix[i][j]*center[j];
		}
		new_center[i] = curr_sum;
	}
	center[0] = new_center[0];
	center[1] = new_center[1];
	center[2] = new_center[2];

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
	float new_diri[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
			float diri_sum = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += r_matrix[i][k]*ctm[k][j];
				//curr_sumi += r_matrixi[i][k]*ctmi[k][j];
				curr_sumi += ctmi[i][k]*r_matrixi[k][j];
				diri_sum += diri[i][k]*r_matrixi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
			new_diri[i][j] = diri_sum;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
			diri[i][j] = new_diri[i][j];
		}
	}

    // update vertices and normals
	float new_vert[4];
	float new_norm[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*r_matrix[i][j];
                        curr_norm_sum += normals[r][v][j]*r_matrix[i][j];
		            }
				    new_vert[i] = curr_sum;
				    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*r_matrix[i][j];
                    curr_norm_sum += normals[0][v][j]*r_matrix[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }

    // calculate new direction of objects central axis
    float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*r_matrix[i][j];
			curr_sum1 += u[j]*r_matrix[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	float mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;
}

void myObject::ScaleObject(double s_x, double s_y, double s_z)
{
    // scaling matrix
	float r_matrix[4][4] = {{s_x, 0, 0, 0},
				            {0, s_y, 0, 0},
				            {0, 0, s_z, 0},
				            {0, 0, 0, 1}};

	float r_matrixi[4][4] = {{1/s_x, 0, 0, 0},
				            {0, 1/s_y, 0, 0},
				            {0, 0, 1/s_z, 0},
				            {0, 0, 0, 1}};
	
    // calculating center position
    float new_center[3];
	for (int i = 0; i < 3; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 3; j++)
        {
			curr_sum += r_matrix[i][j]*center[j];
		}
		new_center[i] = curr_sum;
	}
	center[0] = new_center[0];
	center[1] = new_center[1];
	center[2] = new_center[2];

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
	float new_diri[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
			float diri_sum = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += r_matrix[i][k]*ctm[k][j];
				/*curr_sumi += r_matrixi[i][k]*ctmi[k][j];*/
				curr_sumi += ctmi[i][k]*r_matrixi[k][j];
				diri_sum += diri[i][k]*r_matrixi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
			new_diri[i][j] = diri_sum;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
			diri[i][j] = new_diri[i][j];
		}
	}
    //update vertices
	float new_vert[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*r_matrix[i][j];
		            }
				    new_vert[i] = curr_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*r_matrix[i][j];
		        }
				new_vert[i] = curr_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];
        }
    }
}

void myObject::RotateObjectAboutCentralAxis(double deg)
{
    double theta = PI*double(deg)/double(180);
    float ux = central_axis[0];
    float uy = central_axis[1];
    float uz = central_axis[2];
    float ux_2 = pow(ux, 2);
    float uy_2 = pow(uy, 2);
    float uz_2 = pow(uz, 2);

    // rodriguez's matrix
    float M[4][4] = {{ux_2 + cos(theta)*(1 - ux_2), (ux*uy*(1-cos(theta))) - uz*sin(theta), (uz*ux*(1-cos(theta))) + uy*sin(theta), 0},
                     {(ux*uy*(1-cos(theta))) + uz*sin(theta), uy_2 + cos(theta)*(1-uy_2), (uy*uz*(1-cos(theta))) - ux*sin(theta), 0},
                     {(uz*ux*(1-cos(theta))) - uy*sin(theta), (uy*uz*(1-cos(theta))) + ux*sin(theta), uz_2 + cos(theta)*(1-uz_2), 0},
                     {0, 0, 0, 1}};

	float Mi[4][4] = {{ux_2 + cos(-theta)*(1 - ux_2), (ux*uy*(1-cos(-theta))) - uz*sin(-theta), (uz*ux*(1-cos(-theta))) + uy*sin(-theta), 0},
                     {(ux*uy*(1-cos(-theta))) + uz*sin(-theta), uy_2 + cos(-theta)*(1-uy_2), (uy*uz*(1-cos(-theta))) - ux*sin(-theta), 0},
                     {(uz*ux*(1-cos(-theta))) - uy*sin(-theta), (uy*uz*(1-cos(-theta))) + ux*sin(-theta), uz_2 + cos(-theta)*(1-uz_2), 0},
                     {0, 0, 0, 1}};

    // updating the vertices and normals
	float new_vert[4];
	float new_norm[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*M[i][j];
                        curr_norm_sum += normals[r][v][j]*M[i][j];
		            }
		
		            new_vert[i] = curr_sum;
                    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*M[i][j];
                    curr_norm_sum += normals[0][v][j]*M[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }
	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
	float new_diri[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
			float diri_sum = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += M[i][k]*ctm[k][j];
				//curr_sumi += r_matrixi[i][k]*ctmi[k][j];
				curr_sumi += ctmi[i][k]*Mi[k][j];
				diri_sum += diri[i][k]*Mi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
			new_diri[i][j] = diri_sum;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
			diri[i][j] = new_diri[i][j];
		}
	}

	// updating the central axis
	float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*M[i][j];
			curr_sum1 += u[j]*M[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	float mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;
}

void myObject::RotateObjectAboutArbitraryAxis(double deg, double ax, double ay, double az)
{
    double theta = PI*double(deg)/double(180);
	float mag = sqrt(pow(ax,2) + pow(ay,2) + pow(az,2));
    float ux = ax/mag;
    float uy = ay/mag;
    float uz = az/mag;
    float ux_2 = pow(ux, 2);
    float uy_2 = pow(uy, 2);
    float uz_2 = pow(uz, 2);

    // rodriguez's matrix
    float M[4][4] = {{ux_2 + cos(theta)*(1 - ux_2), (ux*uy*(1-cos(theta))) - uz*sin(theta), (uz*ux*(1-cos(theta))) + uy*sin(theta), 0},
                     {(ux*uy*(1-cos(theta))) + uz*sin(theta), uy_2 + cos(theta)*(1-uy_2), (uy*uz*(1-cos(theta))) - ux*sin(theta), 0},
                     {(uz*ux*(1-cos(theta))) - uy*sin(theta), (uy*uz*(1-cos(theta))) + ux*sin(theta), uz_2 + cos(theta)*(1-uz_2), 0},
                     {0, 0, 0, 1}};

	float Mi[4][4] = {{ux_2 + cos(-theta)*(1 - ux_2), (ux*uy*(1-cos(-theta))) - uz*sin(-theta), (uz*ux*(1-cos(-theta))) + uy*sin(-theta), 0},
                     {(ux*uy*(1-cos(-theta))) + uz*sin(-theta), uy_2 + cos(-theta)*(1-uy_2), (uy*uz*(1-cos(-theta))) - ux*sin(-theta), 0},
                     {(uz*ux*(1-cos(-theta))) - uy*sin(-theta), (uy*uz*(1-cos(-theta))) + ux*sin(-theta), uz_2 + cos(-theta)*(1-uz_2), 0},
                     {0, 0, 0, 1}};

    //updating the vertices
	float new_vert[4];
	float new_norm[4];

    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*M[i][j];
                        curr_norm_sum += normals[r][v][j]*M[i][j];
		            }
		
		            new_vert[i] = curr_sum;
                    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*M[i][j];
                    curr_norm_sum += normals[0][v][j]*M[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }

    // calculating new center position
    float new_center[3];
	for (int i = 0; i < 3; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 3; j++)
        {
			curr_sum += M[i][j]*center[j];
		}
		new_center[i] = curr_sum;
	}
	center[0] = new_center[0];
	center[1] = new_center[1];
	center[2] = new_center[2];

	//update CTM
	float new_ctm[4][4];
	float new_ctmi[4][4];
	float new_diri[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float curr_sum = 0;
			float curr_sumi = 0;
			float diri_sum = 0;
            for (int k = 0; k < 4; k++)
            {
                curr_sum += M[i][k]*ctm[k][j];
				//curr_sumi += r_matrixi[i][k]*ctmi[k][j];
				curr_sumi += ctmi[i][k]*Mi[k][j];
				diri_sum += diri[i][k]*Mi[k][j];
            }
            new_ctm[i][j] = curr_sum;
			new_ctmi[i][j] = curr_sumi;
			new_diri[i][j] = diri_sum;
        }
    }
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ctm[i][j] = new_ctm[i][j];
			ctmi[i][j] = new_ctmi[i][j];
			diri[i][j] = new_diri[i][j];
		}
	}


    // calculating new direction of objects central axis
    float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*M[i][j];
			curr_sum1 += u[j]*M[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;

}

void myObject::resetCTM()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ctm[i][j] = 0;
			ctmi[i][j] = 0;
			diri[i][j] = 0;
            if (i == j)
            {
                ctm[i][j] = 1;
				ctmi[i][j] = 1;
				diri[i][j] = 1;
            }
        }
    }
}

void myObject::ApplyCTM()
{
	//update vertices and normals
	float new_vert[4];
	float new_norm[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*ctm[i][j];
                        curr_norm_sum += normals[r][v][j]*ctm[i][j];
		            }
				    new_vert[i] = curr_sum;
				    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*ctm[i][j];
                    curr_norm_sum += normals[0][v][j]*ctm[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }

    //update the direction of the central axis
    float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*ctm[i][j];
			curr_sum1 += u[j]*ctm[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	float mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;


}

void myObject::ApplyCTMi()
{
	//update vertices and normals
	float new_vert[4];
	float new_norm[4];
    if (type != CUBE && type != HOUSE && type != WALL)
    {
        for (int r = 0; r < radial_slices; r++)
        {
            for (int v = 0; v < vertical_slices+1; v++)
            {
                for (int i = 0; i < 4; i++)
	            {
		            float curr_sum = 0;
                    float curr_norm_sum = 0;
		            for (int j = 0; j < 4; j++)
		            {
			            curr_sum += verts[r][v][j]*ctmi[i][j];
                        curr_norm_sum += normals[r][v][j]*ctmi[i][j];
		            }
				    new_vert[i] = curr_sum;
				    new_norm[i] = curr_norm_sum;
	            }

			    verts[r][v][0] = new_vert[0];
			    verts[r][v][1] = new_vert[1];
			    verts[r][v][2] = new_vert[2];
			    verts[r][v][3] = new_vert[3];

			    normals[r][v][0] = new_norm[0];
			    normals[r][v][1] = new_norm[1];
			    normals[r][v][2] = new_norm[2];
			    normals[r][v][3] = new_norm[3];
            }
        }
    }
    else
    {
        float num_verts = 8;
        if (type == HOUSE)
        {
            num_verts = 9;
        }
        for (int v = 0; v < num_verts; v++)
        {
            for (int i = 0; i < 4; i++)
	        {
		        float curr_sum = 0;
                float curr_norm_sum = 0;
		        for (int j = 0; j < 4; j++)
		        {
			        curr_sum += verts[0][v][j]*ctmi[i][j];
                    curr_norm_sum += normals[0][v][j]*ctmi[i][j];
		        }
				new_vert[i] = curr_sum;
                new_norm[i] = curr_norm_sum;
	        }

			verts[0][v][0] = new_vert[0];
			verts[0][v][1] = new_vert[1];
			verts[0][v][2] = new_vert[2];
			verts[0][v][3] = new_vert[3];

            normals[0][v][0] = new_norm[0];
			normals[0][v][1] = new_norm[1];
			normals[0][v][2] = new_norm[2];
			normals[0][v][3] = new_norm[3];
        }
    }

    //update the direction of the central axis
    float new_axis[4];
	float new_u[4];
    for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		float curr_sum1 = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += central_axis[j]*ctmi[i][j];
			curr_sum1 += u[j]*ctmi[i][j];
		}
		new_axis[i] = curr_sum;
		new_u[i] = curr_sum1;
	}
	float mag = sqrt(pow(new_axis[0], 2) + pow(new_axis[1], 2) + pow(new_axis[2], 2));
    central_axis[0] = new_axis[0]/mag;
    central_axis[1] = new_axis[1]/mag;
    central_axis[2] = new_axis[2]/mag;
	mag = sqrt(pow(new_u[0], 2) + pow(new_u[1], 2) + pow(new_u[2], 2));
	u[0] = new_u[0]/mag;
	u[1] = new_u[1]/mag;
	u[2] = new_u[2]/mag;

}

void myObject::draw_quad(GLfloat vertices[][4], int iv1, int iv2, int iv3, int iv4, int ic, int crt_render_mode)
{
    GLfloat colors [][3] = {
            {0.0, 0.0, 0.0},  /* black   */
            {1.0, 0.0, 0.0},  /* red     */
            {1.0, 1.0, 0.0},  /* yellow  */
            {1.0, 0.0, 1.0},  /* magenta */
            {0.0, 1.0, 0.0},  /* green   */
            {0.0, 1.0, 1.0},  /* cyan    */
            {0.0, 0.0, 1.0},  /* blue    */
            {0.5, 0.5, 0.5},  /* 50%grey */
            {1.0, 1.0, 1.0}   /* white   */
            };
    glBegin(crt_render_mode); 
    {
        glColor3fv(colors[ic]);
        /*note the explicit use of homogeneous coords below: glVertex4f*/
        glNormal3f(normals[0][iv1][0], normals[0][iv1][1], normals[0][iv1][2]);
        glVertex4fv(vertices[iv1]);
        glNormal3f(normals[0][iv2][0], normals[0][iv2][1], normals[0][iv2][2]);
        glVertex4fv(vertices[iv2]);
        glNormal3f(normals[0][iv2][0], normals[0][iv3][1], normals[0][iv3][2]);
        glVertex4fv(vertices[iv3]);
        glNormal3f(normals[0][iv4][0], normals[0][iv4][1], normals[0][iv4][2]);
        glVertex4fv(vertices[iv4]);
    }
    glEnd();
}

void myObject::draw_param_quad(GLfloat vertices[][MAX_VS][4], int line, int col, int ic, int crt_render_mode)
{
    GLfloat colors [][3] = {
            {0.0, 0.0, 0.0},  /* black   */
            {1.0, 0.0, 0.0},  /* red     */
            {1.0, 1.0, 0.0},  /* yellow  */
            {1.0, 0.0, 1.0},  /* magenta */
            {0.0, 1.0, 0.0},  /* green   */
            {0.0, 1.0, 1.0},  /* cyan    */
            {0.0, 0.0, 1.0},  /* blue    */
            {0.5, 0.5, 0.5},  /* 50%grey */
            {1.0, 1.0, 1.0}   /* white   */
            };
    glBegin(crt_render_mode); 
    {
        glColor3fv(colors[ic]);
        /*note the explicit use of homogeneous coords below: glVertex4f*/
        glVertex4fv(vertices[line][col]);
        glVertex4fv(vertices[line+1][col]);
        glVertex4fv(vertices[line+1][col+1]);
        glVertex4fv(vertices[line][col+1]);
    }
    glEnd();
}

void myObject::draw_triangle(GLfloat vertices[][4], int iv1, int iv2, int iv3, int ic, int crt_render_mode)
{
    GLfloat colors [][3] = {
            {0.0, 0.0, 0.0},  /* black   */
            {1.0, 0.0, 0.0},  /* red     */
            {1.0, 1.0, 0.0},  /* yellow  */
            {1.0, 0.0, 1.0},  /* magenta */
            {0.0, 1.0, 0.0},  /* green   */
            {0.0, 1.0, 1.0},  /* cyan    */
            {0.0, 0.0, 1.0},  /* blue    */
            {0.5, 0.5, 0.5},  /* 50%grey */
            {1.0, 1.0, 1.0}   /* white   */
            };
    glBegin(crt_render_mode); 
    {
        glColor3fv(colors[ic]);
        /*note the explicit use of homogeneous coords below: glVertex4f*/
        glNormal3f(normals[0][iv1][0], normals[0][iv1][1], normals[0][iv1][2]);
        glVertex4fv(vertices[iv1]);
        glNormal3f(normals[0][iv2][0], normals[0][iv2][1], normals[0][iv2][2]);
        glVertex4fv(vertices[iv2]);
        glNormal3f(normals[0][iv3][0], normals[0][iv3][1], normals[0][iv3][2]);
        glVertex4fv(vertices[iv3]);
    }
    glEnd();
}

void myObject::setColor(float r, float g, float b)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
}