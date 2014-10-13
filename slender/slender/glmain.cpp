/*
	Slender 2
	CS1566 Final Project - Fall 2013
	Joey Gelpi - jbg29
	David Lang - djl52
*/

#include "slender.h"
#include "glmain.h"

#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))

#define min(a,b) ((a) < (b)? a:b)
#define FALSE 0 
#define TRUE  1

using namespace std;

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

GLfloat vertices_axes[][4] = {
	{0.0, 0.0, 0.0, 1.0},  /* origin */ 
	{5.0, 0.0, 0.0, 1.0},  /* maxx */ 
	{0.0, 5.0, 0.0, 1.0}, /* maxy */ 
	{0.0, 0.0, 5.0, 1.0}  /* maxz */ 

};

vector<myObject *> level_objects;		//Trees/walls
vector<myObject *> boundingBoxes;		//Cubes that cover entire buildings so slender doesnt get stuck in them

vector<float> slender_t_vals;

snowball *snowballList[8];
myCamera* my_camera;

int crt_render_mode;
int crt_shape, crt_rs, crt_vs;
int crt_transform;

float timer_tick_interval = 20;
int num_snowballs = 0;

bool flashlight_on = false;

bool move_cam_forward = false;
bool move_cam_back = false;
bool move_cam_left = false;
bool move_cam_right = false;

float ray_length = 1000;
float ray_start[4] = {0,0,0,1};
float ray_dir[4] = {0,0,0,1};
bool show_ray = false;

int window_w = 700;
int window_h = 700;

bool show_normals = false;
bool lock_pointer = false;

int curr_mouse_x = 0;
int curr_mouse_y = 0;

float window_center_y = float(window_h)/2.0;
float window_center_x = float(window_w)/2.0;

float fogDensity = 0.15f;

//Kill animation vars
bool dead = false;
bool killAnimation = false;
float kill_deg_u = 0.0;
float kill_deg_v = 0.0;
float kill_time = DEFAULT_KILL_TIME;


GLuint filter;									    // Which Filter To Use
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter = 1;								// Which Fog To Use
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};      // Fog Color

slender *slenBro;

typedef struct												// Create A Structure
{
	GLubyte	*imageData;										// Image Data (Up To 32 Bits)
	GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
	GLuint	width;											// Image Width
	GLuint	height;											// Image Height
	GLuint	texID;											// Texture ID Used To Select A Texture
} TextureImage;				//Structure Name

TextureImage texturesList[3];						//Storage for textures

//Load texture and give it an id
bool LoadTGA(TextureImage *texture, char *filename)			// Loads A TGA File Into Memory
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)
	
	FILE *file = fopen(filename, "r");						// Open The TGA File

	if(	file==NULL ||										// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		
		if (file == NULL)									// Did The File Even Exist? *Added Jim Strong*
		{
			perror("Error");
			return false;									// Return False
		}
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			perror("Error");
			return false;									// Return False
		}
		
	}

	texture->width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture->width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	texture->bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||							// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)						// Was Image Data Loaded
			free(texture->imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
	{														// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture->imageData[i];							// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);											// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &texture->texID);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture->texID);			// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	if (texture[0].bpp==24)									// Was The TGA 24 Bits
	{
		type=GL_RGB;										// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);

	return true;											// Texture Building Went Ok, Return True
}

void deleteFunc()
{
	for (int i = 0; i < level_objects.size(); i++)
	{
		delete level_objects[i];
	}
	for (int i = 0; i < 8; i++)
	{
		delete snowballList[i];
	}
	for (int i = 0; i < boundingBoxes.size(); i++)
	{
		delete boundingBoxes[i];
	}
	delete slenBro;
}

void myabort(void)
{
	abort();
	exit(1); /* exit so g++ knows we don't return. */
}

int main(int argc, char** argv)
{ 
	setbuf(stdout, NULL);   /* for writing to stdout asap */
	glutInit(&argc, argv);

	my_setup(argc, argv);  
	glut_setup();
	gl_setup();

	glutMainLoop();
	return(0);
}

void glut_setup (){

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);

	glutInitWindowSize(window_w, window_h);
	glutInitWindowPosition(500,20);
	glutCreateWindow("Slender 2: Slender Roasting on an Open Fire");

	 glutSetCursor(GLUT_CURSOR_NONE); 

	/* set up callback functions */
	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape);
	glutPassiveMotionFunc(my_mouse_move);
	glutMouseFunc(my_mouse);
	glutMotionFunc(my_mouse_drag);
	glutKeyboardFunc(my_keyboard);
	glutKeyboardUpFunc( my_keyboard_up );	
	glutIdleFunc( my_idle );	
	glutTimerFunc( 1.0, my_TimeOut, 0 );/* schedule a my_TimeOut call with the ID 0 in 20 seconds from now */

	return;
}

void gl_setup(void)
{
	//enable texturing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	//Load textures
	LoadTGA(&texturesList[0], "snow.tga");
	LoadTGA(&texturesList[1], "wall.tga");
	LoadTGA(&texturesList[2], "tree.tga");

	// enable depth handling (z-buffer)
	glEnable(GL_DEPTH_TEST);

	// enable auto normalize
	glEnable(GL_NORMALIZE);

	// define the background color 
	glClearColor(0,0,0,1);

	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	//glOrtho(stuff);
	gluPerspective( 40, 1.0, 1, 200.0);
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity() ;  // init modelview to identity

	// toggle to smooth shading (instead of flat)
	glShadeModel(GL_SMOOTH); 
	lighting_setup();

	//Create fog
	glClearColor(0.5f,0.5f,0.5f,1.0f);          // We'll Clear To The Color Of The Fog ( Modified )
 
	glFogi(GL_FOG_MODE, fogMode[fogfilter]);    // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
	glFogf(GL_FOG_DENSITY, fogDensity);         // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
	glFogf(GL_FOG_START, 1.0f);				// Fog Start Depth
	glFogf(GL_FOG_END, 15.0f);					// Fog End Depth
	glEnable(GL_FOG);							// Enables GL_FOG

	return ;
}

void my_setup(int argc, char **argv)
{
	// initialize global shape defaults and mode for drawing
	crt_render_mode = GL_POLYGON; // GL_LINE_LOOP;
	crt_shape = 0;

	crt_rs = 20;
	crt_vs = 20;

	my_camera = new myCamera(0, .6, 10, 0, 0, -1, 0, 1, 0, PERSP);

	slenBro = new slender();

	setup_level();


}

//Create all objects
void setup_level()
{

	//ballsnows
	snowballList[0] = new snowball(-15.3, -15.1);
	snowballList[1] = new snowball(-27, -10.5);
	snowballList[2] = new snowball(5.5, -6);
	snowballList[3] = new snowball(4, -3.5);
	snowballList[4] = new snowball(-8, 8.5);
	snowballList[5] = new snowball(-11.1, 7);
	snowballList[6] = new snowball(9.8, 28.8);
	snowballList[7] = new snowball(9.8, -28.8);
	for (int i = 0; i < 8; i++)
	{
		snowballList[i]->ball->setColor(.8, .8, 1);
	}

	myObject *o;

	//bounding boxes	
	o = new myObject(WALL);
	o->setWall(-11, 0, 11.5, 6, 7.5, 1);
	boundingBoxes.push_back(o);

	o = new myObject(WALL);
	o->setWall(2, 0, -1, 6.5, 6, 1);
	boundingBoxes.push_back(o);

	o = new myObject(WALL);
	o->setWall(-17, 0, -5, 9, 10, 1);
	boundingBoxes.push_back(o);
	
	#pragma region Trees

	//trees
	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-5, 0, 0);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-4, 0, -5);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-11, 0, -1);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-16, 0, 2);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-15, 0, 6);
	level_objects.push_back(o);
	
	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-17, 0, 11);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-13, 0, 14);
	level_objects.push_back(o);
	
	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-5, 0, 15);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-2, 0, 3);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-1, 0, 7);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-5, 0, -10);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-1, 0, -8);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-3, 0, -15);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-10, 0, -17);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-2, 0, 12);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(9, 0, 28);
	level_objects.push_back(o);

	//far left
	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-22, 0, -20);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-26, 0, -10);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-24, 0, -4);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-21, 0, 1);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-27, 0, 3);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-20, 0, 7);
	level_objects.push_back(o);
	
	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-23, 0, 9);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-28, 0, 15);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-25, 0, 22);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-21, 0, 26);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-20, 0, 20);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-26, 0, 17);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-15, 0, 22);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-7, 0, 23);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(3, 0, 5);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(6, 0, 17);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(8, 0, 22);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(7, 0, 23);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(6, 0, 6);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(5, 0, 10);
	level_objects.push_back(o);
	
	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(1, 0, 22);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(4, 0, 23);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-5, 0, -18);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-15, 0, -24);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-25, 0, -20);
	level_objects.push_back(o);

	o = new myObject(TREE);
	o->ScaleObject(.6, 30, .6);
	o->TranslateObjectTo(-8, 0, -23);
	level_objects.push_back(o);

	#pragma endregion Trees

	//boundries
	//left
	o = new myObject(WALL);
	o->setWall(-30, 0, 30, 1, 60, 1);
	level_objects.push_back(o);
	o = new myObject(WALL);
	o->setWall(-30, 1, 30, 1, 60, 1);
	level_objects.push_back(o);

	//top
	o = new myObject(WALL);
	o->setWall(-30, 0, -30, 40, 1, 1);
	level_objects.push_back(o);
	o = new myObject(WALL);
	o->setWall(-30, 1, -30, 40, 1, 1);
	level_objects.push_back(o);

	//right
	o = new myObject(WALL);
	o->setWall(10, 0, 30, 1, 60, 1);
	level_objects.push_back(o);
	o = new myObject(WALL);
	o->setWall(10, 1, 30, 1, 60, 1);
	level_objects.push_back(o);

	//bottom
	o = new myObject(WALL);
	o->setWall(-30, 0, 30, 40, 1, 1);
	level_objects.push_back(o);
	o = new myObject(WALL);
	o->setWall(-30, 1, 30, 40, 1, 1);
	level_objects.push_back(o);



	// walls

	//1
	o = new myObject(WALL);
	o->setWall(2, 0, -1, 3, .5, 1);
	level_objects.push_back(o);
	
	//2
	o = new myObject(WALL);
	o->setWall(5, 0, -1, 3.5, .5, 1);
	level_objects.push_back(o);

	//3
	o = new myObject(WALL);
	o->setWall(2, 0, -2.5, 3, .5, 1);
	level_objects.push_back(o);

	//4
	o = new myObject(WALL);
	o->setWall(8, 0, -1.5, .5, 4, 1);
	level_objects.push_back(o);

	//5
	o = new myObject(WALL);
	o->setWall(4.5, 0, -3, .5, 1, 1);
	level_objects.push_back(o);

	//6
	o = new myObject(WALL);
	o->setWall(4.5, 0, -5, .5, 2, 1);
	level_objects.push_back(o);

	//7
	o = new myObject(WALL);
	o->setWall(5, 0, -6.5, 3.5, .5, 1);
	level_objects.push_back(o);

	//Builindg 2
	//8
	o = new myObject(WALL);
	o->setWall(-9, 0, 4.5, 4, .5, 1);
	level_objects.push_back(o);

	
	//9
	o = new myObject(WALL);
	o->setWall(-9, 0, 9, .5, 4.5, 1);
	level_objects.push_back(o);
	
	//10
	o = new myObject(WALL);
	o->setWall(-7.5, 0, 6, 2.5, .5, 1);
	level_objects.push_back(o);

	//11
	o = new myObject(WALL);
	o->setWall(-5.5, 0, 11, .5, 5, 1);
	level_objects.push_back(o);

	//12
	o = new myObject(WALL);
	o->setWall(-9, 0, 10, 2, 1, 1);
	level_objects.push_back(o);

	//13
	o = new myObject(WALL);
	o->setWall(-11, 0, 11.5, 6, .5, 1);
	level_objects.push_back(o);

	//14
	o = new myObject(WALL);
	o->setWall(-11, 0, 10, .5, 6, 1);
	level_objects.push_back(o);

	//top left
	//15
	o = new myObject(WALL);
	o->setWall(-17, 0, -5, 9, 10, 1);
	level_objects.push_back(o);

	//16
	o = new myObject(WALL);
	o->setWall(-17, 1, -5, 9, 10, 1);
	level_objects.push_back(o);

	//17
	o = new myObject(WALL);
	o->setWall(-17, 2, -5, 9, 10, 1);
	level_objects.push_back(o);

	//18
	o = new myObject(WALL);
	o->setWall(-17, 3, -5, 9, 10, 1);
	level_objects.push_back(o);

	//19
	o = new myObject(WALL);
	o->setWall(-17, 4, -5, 9, 10, 1);
	level_objects.push_back(o);

	//20
	o = new myObject(WALL);
	o->setWall(-17, 5, -5, 9, 10, 1);
	level_objects.push_back(o);

	//21
	o = new myObject(WALL);
	o->setWall(-17, 6, -5, 9, 10, 1);
	level_objects.push_back(o);

	//22
	o = new myObject(WALL);
	o->setWall(-17, 7, -5, 9, 10, 1);
	level_objects.push_back(o);

	//23
	o = new myObject(WALL);
	o->setWall(-17, 8, -5, 9, 10, 1);
	level_objects.push_back(o);

	//24
	o = new myObject(WALL);
	o->setWall(-17, 9, -5, 9, 10, 1);
	level_objects.push_back(o);

	//25
	o = new myObject(WALL);
	o->setWall(-17, 10, -5, 9, 10, 1);
	level_objects.push_back(o);

	//26
	o = new myObject(WALL);
	o->setWall(-16, 0, -15, .5, 2, 1);
	level_objects.push_back(o);

	//27
	o = new myObject(WALL);
	o->setWall(-14.5, 0, -15, .5, 2, 1);
	level_objects.push_back(o);
	
}

void lighting_setup()
{
	GLfloat globalAmb[]     = {.6, .6, .6, 1};

	// create flashlight
	GLfloat amb[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat dif[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat spec[] = {0.5, 0.5, 0.5, 1.0};

	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	// reflective propoerites -- global ambiant light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

	// this was for the flashlights

	glLightfv(GL_LIGHT0, GL_POSITION, my_camera->pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, my_camera->look);

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 20.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 20.0);
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);

	//glEnable(GL_LIGHT0);

}

void my_reshape(int w, int h) {
	// ensure a square view port
	glViewport(0,0,min(w,h),min(w,h)) ;
	return ;
}

void my_keyboard( unsigned char key, int x, int y )
{
	switch( key )
	{
	case '`':
		if (lock_pointer == true)
		{
			lock_pointer = false;
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
		else
		{
			lock_pointer = true;
			//glutSetCursor(GLUT_CURSOR_NONE);
		}
		break;
	case 'a':
	case 'A':
		move_cam_left = true;
		break;
	case 'd':
	case 'D':
		move_cam_right = true;
		break;
	case 'w':
	case 'W':
		move_cam_forward = true;
		break;
	case 's':
	case 'S':
		move_cam_back = true;
		break;

	case 'P':
	case 'p':
		{
			my_camera->printInfo();
			GLfloat light_pos[4];
			glGetLightfv(GL_LIGHT0, GL_POSITION, light_pos);
			printf("Light Info:\n");
			printf("\tpos: %f, %f, %f\n", light_pos[0], light_pos[1], light_pos[2]);
		} break;
	case 'v':
	case 'V':
		if (show_normals)
		{
			show_normals = false;
		}
		else
		{
			show_normals = true;
		}
		break;
	case 'q': 
	case 'Q':
		atexit(deleteFunc);
		exit(0) ;
		break ;	
	default: break;
	}

	return ;
}

void my_keyboard_up( unsigned char key, int x, int y ) {
	switch( key )
	{
	case 'w':
	case 'W':
		move_cam_forward = false;
		break;
	case 'a':
	case 'A':
		move_cam_left = false;
		break;
	case 's':
	case 'S':
		move_cam_back = false;
		break;
	case 'd':
	case 'D':
		move_cam_right = false;
		break;

	default: break;
	}
}

void my_mouse(int button, int state, int mousex, int mousey)
{
	//printf("curr_mouse: %d, %d\n", mousex, mousey);
	switch( button )
	{

	case GLUT_LEFT_BUTTON:
		if( state == GLUT_DOWN )
		{	
			my_camera->isSprinting = true;
		}
		else if (state == GLUT_UP)
		{
			my_camera->isSprinting = false;
		}
		break ;

		/*if( state == GLUT_UP ) {
		}*/
		break ;
	default: break;
	}


	return;
}

void my_mouse_drag(int x, int y)
{
	curr_mouse_x = x;
	curr_mouse_y = y;
}

void my_mouse_move(int x, int y)
{
	curr_mouse_x = x;
	curr_mouse_y = y;
	//mouse_adjust();
}

void mouse_adjust()
{
	float diff_x = (curr_mouse_x - window_center_x)/5.0;
	float diff_y = (curr_mouse_y - window_center_y)/5.0;

	my_camera->LookLeftRight(diff_x);
	my_camera->RotateU(diff_y);

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, my_camera->look);

	glutWarpPointer(window_center_x, window_center_y);

	glutPostRedisplay();

}

int my_raytrace_cube(myObject *o, float result[3], float normal1[3], float normal2[3])
{

	/*
	float* new_p = applyMatrixToVector(ray_start, o->ctmi);
	float* new_dir = applyMatrixToVector(ray_dir, o->diri);

	int num_hits = 0;
	float t;
	float r[3];
	if (new_dir[0] != 0)
	{
	// left
	t = (-0.5 - new_p[0])/new_dir[0];
	r[0] = new_p[0] + t * new_dir[0];
	r[1] = new_p[1] + t * new_dir[1];
	r[2] = new_p[2] + t * new_dir[2];

	if (r[1] <= 0.5 && r[1] >= -0.5 && r[2] <= 0.5 && r[2] >= -0.5)
	{
	num_hits += 1;
	result[num_hits] = t;

	}
	// right
	t = (-new_p[0] + 0.5)/new_dir[0];
	r[0] = new_p[0] + t * new_dir[0];
	r[1] = new_p[1] + t * new_dir[1];
	r[2] = new_p[2] + t * new_dir[2];

	if (r[1] <= 0.5 && r[1] >= -0.5 && r[2] <= 0.5 && r[2] >= -0.5)
	{
	num_hits += 1;
	result[num_hits] = t;
	}

	if (num_hits == 2)
	{
	delete[] new_p;
	delete[] new_dir;

	if (result[1] < 0 && result[2] >= 0)
	{
	result[0] = 2;
	}
	else if (result[1] >= 0 && result[2] <0)
	{
	result[0] = 2;
	}
	else if (result[1] == result[2])
	{
	result[0] = 0;
	}
	else if (result[1] != result[2])
	{
	result[0] = 1;
	}
	return num_hits;
	}
	}

	if (new_dir[1] != 0)
	{
	// bottom
	t = (-0.5 - new_p[1])/new_dir[1];
	r[0] = new_p[0] + t * new_dir[0];
	r[1] = new_p[1] + t * new_dir[1];
	r[2] = new_p[2] + t * new_dir[2];

	if (r[0] <= 0.5 && r[0] >= -0.5 && r[2] <= 0.5 && r[2] >= -0.5)
	{
	num_hits += 1;
	result[num_hits] = t;
	if (num_hits == 2)
	{
	delete[] new_p;
	delete[] new_dir;

	if (result[1] < 0 && result[2] >= 0)
	{
	result[0] = 2;
	}
	else if (result[1] >= 0 && result[2] <0)
	{
	result[0] = 2;
	}
	else if (result[1] == result[2])
	{
	result[0] = 0;
	}
	else if (result[1] != result[2])
	{
	result[0] = 1;
	}
	return num_hits;
	}
	}
	// top
	t = (-new_p[1] + 0.5)/new_dir[1];
	r[0] = new_p[0] + t * new_dir[0];
	r[1] = new_p[1] + t * new_dir[1];
	r[2] = new_p[2] + t * new_dir[2];

	if (r[0] <= 0.5 && r[0] >= -0.5 && r[2] <= 0.5 && r[2] >= -0.5)
	{
	num_hits += 1;
	result[num_hits] = t;
	if (num_hits == 2)
	{
	delete[] new_p;
	delete[] new_dir;

	if (result[1] < 0 && result[2] >= 0)
	{
	result[0] = 2;
	}
	else if (result[1] >= 0 && result[2] <0)
	{
	result[0] = 2;
	}
	else if (result[1] == result[2])
	{
	result[0] = 0;
	}
	else if (result[1] != result[2])
	{
	result[0] = 1;
	}
	return num_hits;
	}
	}

	}

	if (new_dir[2] != 0)
	{
	// back
	t = (-0.5 - new_p[2])/new_dir[2];
	r[0] = new_p[0] + t * new_dir[0];
	r[1] = new_p[1] + t * new_dir[1];
	r[2] = new_p[2] + t * new_dir[2];

	if (r[0] <= 0.5 && r[0] >= -0.5 && r[1] <= 0.5 && r[1] >= -0.5)
	{
	num_hits += 1;
	result[num_hits] = t;
	if (num_hits == 2)
	{
	delete[] new_p;
	delete[] new_dir;

	if (result[1] < 0 && result[2] >= 0)
	{
	result[0] = 2;
	}
	else if (result[1] >= 0 && result[2] <0)
	{
	result[0] = 2;
	}
	else if (result[1] == result[2])
	{
	result[0] = 0;
	}
	else if (result[1] != result[2])
	{
	result[0] = 1;
	}
	return num_hits;
	}
	}

	// front
	t = (-new_p[2] + 0.5)/new_dir[2];
	r[0] = new_p[0] + t * new_dir[0];
	r[1] = new_p[1] + t * new_dir[1];
	r[2] = new_p[2] + t * new_dir[2];

	if (r[0] <= 0.5 && r[0] >= -0.5 && r[1] <= 0.5 && r[1] >= -0.5)
	{
	num_hits += 1;
	result[num_hits] = t;
	if (num_hits == 2)
	{
	delete[] new_p;
	delete[] new_dir;

	if (result[1] < 0 && result[2] >= 0)
	{
	result[0] = 2;
	}
	else if (result[1] >= 0 && result[2] <0)
	{
	result[0] = 2;
	}
	else if (result[1] == result[2])
	{
	result[0] = 0;
	}
	else if (result[1] != result[2])
	{
	result[0] = 1;
	}
	return num_hits;
	}
	}

	}

	return num_hits;
	*/
	float* newP = applyMatrixToVector(ray_start, o->ctmi);
	float* newDir = applyMatrixToVector(ray_dir, o->diri);

	int hit = 0;

	//This is basically a counter, you can add up to 2 t-vals to result[]
	//result[0] = how many hits, result[1/2] = t-vals
	result[0] = 0;

	double t = 0;

	GLfloat inter[3];			//Intersect ray/line

	float boundingSize = .5;	//How big the cubes are in each direction

	if (newDir[0] != 0)						//Cant divide by 0
	{
		//Left Side
		t = (-boundingSize - newP[0]) / newDir[0];	//t-val
		inter[0] = newDir[0] * t;			//calulate ray
		inter[1] = newDir[1] * t;
		inter[2] = newDir[2] * t;
		inter[0] += newP[0];
		inter[1] += newP[1];
		inter[2] += newP[2];		
		if (t > 0 && inter[1] <= boundingSize && inter[1] >= -boundingSize && inter[2] <= boundingSize && inter[2] >= -boundingSize)		//if in bounds
		{		
			if (result[0] != 2)				//If there are not already 2 t-vals
			{
				if (hit == 0)
				{
					normal1[0] = -1;
					normal1[1] = 0;
					normal1[2] = 0;
				}
				else if (hit == 1)
				{
					normal2[0] = -1;
					normal2[1] = 0;
					normal2[2] = 0;
				}
				hit = 1;
				result[0] += 1;				//Incremenent how many there are
				int x = result[0];			//Get that number
				result[x] = t;				//Store t
			}
		}

		//Right Side
		t = (boundingSize - newP[0]) / newDir[0];
		inter[0] = newDir[0] * t;
		inter[1] = newDir[1] * t;
		inter[2] = newDir[2] * t;
		inter[0] += newP[0];
		inter[1] += newP[1];
		inter[2] += newP[2];
		if (t > 0 && inter[1] <= boundingSize && inter[1] >= -boundingSize && inter[2] <= boundingSize && inter[2] >= -boundingSize)
		{		
			if (result[0] != 2)
			{
				if (hit == 0)
				{
					normal1[0] = 1;
					normal1[1] = 0;
					normal1[2] = 0;
				}
				else if (hit == 1)
				{
					normal2[0] = 1;
					normal2[1] = 0;
					normal2[2] = 0;
				}
				hit = 1;
				result[0] += 1;
				int x = result[0];
				result[x] = t;
			}
		}
	}

	if (newDir[1] != 0)
	{
		//ToP
		t = (-boundingSize - newP[1]) / newDir[1];
		inter[0] = newDir[0] * t;
		inter[1] = newDir[1] * t;
		inter[2] = newDir[2] * t;
		inter[0] += newP[0];
		inter[1] += newP[1];
		inter[2] += newP[2];
		if (t > 0 && inter[0] <= boundingSize && inter[0] >= -boundingSize && inter[2] <= boundingSize && inter[2] >= -boundingSize)
		{		
			if (result[0] != 2)
			{
				if (hit == 0)
				{
					normal1[0] = 0;
					normal1[1] = 1;
					normal1[2] = 0;
				}
				else if (hit == 1)
				{
					normal2[0] = 0;
					normal2[1] = 1;
					normal2[2] = 0;
				}
				hit = 1;
				result[0] += 1;
				int x = result[0];
				result[x] = t;
			}
		}

		//Bottom
		t = (boundingSize - newP[1]) / newDir[1];
		inter[0] = newDir[0] * t;
		inter[1] = newDir[1] * t;
		inter[2] = newDir[2] * t;
		inter[0] += newP[0];
		inter[1] += newP[1];
		inter[2] += newP[2];
		if (t > 0 && inter[0] <= boundingSize && inter[0] >= -boundingSize && inter[2] <= boundingSize && inter[2] >= -boundingSize)
		{		
			if (result[0] != 2)
			{
				if (hit == 0)
				{
					normal1[0] = 0;
					normal1[1] = -1;
					normal1[2] = 0;
				}
				else if (hit == 1)
				{
					normal2[0] = 0;
					normal2[1] = -1;
					normal2[2] = 0;
				}
				hit = 1;
				result[0] += 1;
				int x = result[0];
				result[x] = t;
			}
		}
	}

	if (newDir[2] != 0)
	{
		//Front
		t = (-boundingSize - newP[2]) / newDir[2];
		inter[0] = newDir[0] * t;
		inter[1] = newDir[1] * t;
		inter[2] = newDir[2] * t;
		inter[0] += newP[0];
		inter[1] += newP[1];
		inter[2] += newP[2];
		if (t > 0 && inter[1] <= boundingSize && inter[1] >= -boundingSize && inter[0] <= boundingSize && inter[0] >= -boundingSize)
		{		
			if (result[0] != 2)
			{
				if (hit == 0)
				{
					normal1[0] = 0;
					normal1[1] = 0;
					normal1[2] = 1;
				}
				else if (hit == 1)
				{
					normal2[0] = 0;
					normal2[1] = 0;
					normal2[2] = 1;
				}
				hit = 1;
				result[0] += 1;
				int x = result[0];
				result[x] = t;
			}
		}

		//Back
		t = (boundingSize - newP[2]) / newDir[2];
		inter[0] = newDir[0] * t;
		inter[1] = newDir[1] * t;
		inter[2] = newDir[2] * t;
		inter[0] += newP[0];
		inter[1] += newP[1];
		inter[2] += newP[2];
		if (t > 0 && inter[1] <= boundingSize && inter[1] >= -boundingSize && inter[0] <= boundingSize && inter[0] >= -boundingSize)
		{		
			if (result[0] != 2)
			{
				if (hit == 0)
				{
					normal1[0] = 0;
					normal1[1] = 0;
					normal1[2] = -1;
				}
				else if (hit == 1)
				{
					normal2[0] = 0;
					normal2[1] = 0;
					normal2[2] = -1;
				}
				hit = 1;
				result[0] += 1;
				int x = result[0];
				result[x] = t;
			}
		}
	}
	delete[] newP;
	delete[] newDir;



	return hit;
}

int my_raytrace_sphere(myObject* o, float result[3])
{
	float* new_p = applyMatrixToVector(ray_start, o->ctmi);
	float* new_dir = applyMatrixToVector(ray_dir, o->diri);

	new_p[3] = 0;
	new_dir[3] = 0;

	float a = dotprod(new_dir, new_dir);
	float b = 2*dotprod(new_p, new_dir);
	float c = dotprod(new_p, new_p) - 1;

	/*float a = (pow(new_p[0],2) + pow(new_p[1],2) + pow(new_p[2],2) - 1);
	float b = 2*(new_p[0]*new_dir[0] + new_p[1]*new_dir[1] + new_p[2]*new_dir[2]);
	float c = (pow(new_dir[0], 2) + pow(new_dir[1], 2) + pow(new_dir[2], 2));
	*/
	float q = 0;

	delete[] new_p;
	delete[] new_dir;

	float t_test = b*b - 4*a*c;
	if (t_test < 0)
	{
		return 0;
	}
	else
	{
		float t1 = (-b + sqrt(b*b - 4*a*c))/(2*a);
		float t2 = (-b - sqrt(b*b - 4*a*c))/(2*a);
		float type = 0;
		if (t1 < 0 && t2 >=0)
		{
			type = 1;
			result[0] = type;
			result[1] = t2;
			result[2] = t2;
		}
		else if (t1 >= 0 && t2 <0)
		{
			type = 1;
			result[0] = type;
			result[1] = t1;
			result[2] = t1;
		}
		else if (t1 == t2)
		{
			type = 1;
			result[0] = type;
			result[1] = t1;
			result[2] = t2;
		}
		else if (t1 != t2)
		{
			type = 2;
			result[0] = type;
			result[1] = t1;
			result[2] = t2;
		}
		return 1;
	}

	return 0;
}

int my_raytrace_cylinder(myObject* o, float result[3])
{
	float* new_p = applyMatrixToVector(ray_start, o->ctmi);
	float* new_dir = applyMatrixToVector(ray_dir, o->diri);

	new_p[3] = 0;
	new_dir[3] = 0;

	float dx = new_dir[0];
	float dy = new_dir[1];
	float dz = new_dir[2];

	float px = new_p[0];
	float py = new_p[1];
	float pz = new_p[2];

	float a = pow(dx, 2) + pow(dz, 2);
	float b = 2*(px*dx + pz*dz);
	float c = pow(px,2) + pow(pz, 2) - pow(o->ray, 2);

	float t_test = b*b - 4*a*c;
	if (t_test < 0)
	{
		delete[] new_p;
		delete[] new_dir;
		return 0;
	}
	else
	{
		bool hit1 = false;
		bool hit2 = false;
		float t1 = (-b + sqrt(b*b - 4*a*c))/(2*a);
		float t2 = (-b - sqrt(b*b - 4*a*c))/(2*a);

		float y1 = new_p[1]+t1*new_dir[1];
		float y2 = new_p[1]+t2*new_dir[1];

		float type = 0;

		if ((y1 >= 0 && y1 <= 1) && t1 >= 0)
		{
			hit1 = true;
			result[0] = 1;
			result[1] = t1;
			result[2] = t1;
		}

		if ((y2 >= 0 && y2 <= 1) && t2 >= 0)
		{
			hit2 = true;
			if (hit1)
			{
				result[0] = 2;
				if (t2 < t1)
				{
					result[1] = t2;
					result[2] = t1;
				}
				else
				{
					result[2] = t2;
				}
			}
			else
			{
				result[0] = 1;
				result[1] = result[2] = t2;
			}
		}


		if (hit1 || hit2)
		{
			delete[] new_p;
			delete[] new_dir;
			return 1;
		}
	}

	delete[] new_p;
	delete[] new_dir;
	return 0;
}

void my_raytrace(int mousex, int mousey)
{
	int hit = 0;
	myObject *o;
	// first we need to get the modelview matrix, the projection matrix, and the viewport
	/*glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);*/

	// gluUnProject with a Z value of 1 will find the point on the far clipping plane
	// corresponding the the mouse click. This is not the same as the vector
	// representing the click.
	/*gluUnProject(mousex, mousey, 1.0, modelViewMatrix, projMatrix, viewport, &clickPoint[0], &clickPoint[1], &clickPoint[2]);*/

	// Now we need a vector representing the click. It should start at the camera
	// position. We can subtract the click point, we will get the vector

	/* code for finding direction vector, set rayStart and rayDirection */

	// now go through the shapes and see if there is a hit

	float result[3];
	float normal1[3];
	float normal2[3];
	for (int i = 0; i < level_objects.size(); i++)
	{
		o = level_objects[i];
		hit = 0;
		switch (o->type)
		{
		case WALL:
		case CUBE:
		case TREE:
			hit = my_raytrace_cube(o, result, normal1, normal2);
			break;
		case SPHERE:
			hit = my_raytrace_sphere(o, result);
			break;
		case CYLINDER:
			hit = my_raytrace_cylinder(o, result);
			break;
		default: break;
		}

		// found intersection, get closest intersection and its normal
		if (hit)
		{
			float normal[3];

			float minT = min(result[1], result[2]);
			if (minT == result[1])
			{
				normal[0] = normal1[0];
				normal[1] = normal1[1];
				normal[2] = normal1[2];
			}
			else if (minT == result[2])
			{
				normal[0] = normal2[0];
				normal[1] = normal2[1];
				normal[2] = normal2[2];
			}
			if (minT <= .3)	
			{				
				if ((ray_dir[0] < 0 && (ray_dir[0] + normal[0] > 0)) || (ray_dir[0] > 0 && (ray_dir[0] + normal[0] < 0)))
				{
					ray_dir[0] = 0;
				}
				//if ((ray_dir[1] < 0 && (ray_dir[1] + normal[1] > 0)) || (ray_dir[1] > 0 && (ray_dir[1] + normal[1] < 0)))
				//{
				//	ray_dir[1] = 0;
				//}
				if ((ray_dir[2] < 0 && (ray_dir[2] - normal[2] > 0)) || (ray_dir[2] > 0 && (ray_dir[2] - normal[2] < 0)))
				{
					ray_dir[2] = 0;
				}
			}
		}
	}
}

/*
	Figure out if slender is intersecting with trees/bounding boxes
	Return appropriate t val so we can place him
*/
bool slender_raytrace()
{
	slender_t_vals.clear();
	int hit = 0;
	bool hit_something = false;
	myObject *o;

	float result[3];
	float normal1[3];
	float normal2[3];

	for (int i = 0; i < level_objects.size(); i++)
	{
		o = level_objects[i];
		hit = 0;
		switch (o->type)
		{
		case TREE:
			hit = my_raytrace_cube(o, result, normal1, normal2);
			break;
		default: break;
		}

		// found intersection, if he is inside something, return that T
		if (hit)
		{

			float maxT = max(result[1], result[2]);
			float minT = min(result[1], result[2]);
			
			if (slenBro->distFromPlayer <= (maxT+.5) && slenBro->distFromPlayer >= (minT-.5))
			{
				slender_t_vals.push_back(maxT);
				hit_something = true;
			}
		}
	}

	for (int i = 0; i < boundingBoxes.size(); i++)
	{
		o = boundingBoxes[i];
		hit = 0;
		switch (o->type)
		{
		case WALL:
		case CUBE:
			hit = my_raytrace_cube(o, result, normal1, normal2);
			break;
		default: break;
		}

		// found intersection
		if (hit)
		{

			float maxT = max(result[1], result[2]);
			float minT = min(result[1], result[2]);

			if (slenBro->distFromPlayer <= (maxT+.5) && slenBro->distFromPlayer >= (minT-.5))
			{
				slender_t_vals.push_back(maxT);
				hit_something = true;
			}
		}
	}

	return hit_something;
}

float dotprod(float v1[], float v2[]) {
	float tot = 0;
	int i;
	for (i=0; i<4; i++)
		tot += v1[i]*v2[i];
	return tot;
}

void normalize(GLfloat *p) { 
	double d=0.0;
	int i;
	for(i=0; i<3; i++) d+=p[i]*p[i];
	d=sqrt(d);
	if(d > 0.0) for(i=0; i<3; i++) p[i]/=d;
}

void cross(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) { 
	d[0]=(b[1]-a[1])*(c[2]-a[2])-(b[2]-a[2])*(c[1]-a[1]);
	d[1]=(b[2]-a[2])*(c[0]-a[0])-(b[0]-a[0])*(c[2]-a[2]);
	d[2]=(b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0]);
	normalize(d);
}

void draw_axes( void ) {
	glLineWidth( 5.0 );

	glDisable(GL_LIGHTING);

	glBegin(GL_LINES); 
	{
		glColor3fv(colors[1]);
		glVertex4fv(vertices_axes[0]);
		glVertex4fv(vertices_axes[1]);

		glColor3fv(colors[4]);
		glVertex4fv(vertices_axes[0]);
		glVertex4fv(vertices_axes[2]);

		glColor3fv(colors[6]);
		glVertex4fv(vertices_axes[0]);
		glVertex4fv(vertices_axes[3]);
	}
	glEnd();
	glLineWidth( 1.0 );

	glEnable(GL_LIGHTING);

}

//Sets a ray to passed in direction
void set_ray(float start_x, float start_y, float start_z, float dir_x, float dir_y, float dir_z)
{
	ray_start[0] = start_x;
	ray_start[1] = start_y;
	ray_start[2] = start_z;

	ray_dir[0] = dir_x;
	ray_dir[1] = dir_y;
	ray_dir[2] = dir_z;

	normalize(ray_dir);
}

float* applyMatrixToVector(float vec[4], float mat[4][4])
{
	float *new_vec = new float[4];
	for (int i = 0; i < 4; i++)
	{
		float curr_sum = 0;
		for (int j = 0; j < 4; j++)
		{
			curr_sum += mat[i][j]*vec[j];
		}
		new_vec[i] = curr_sum;
	}
	return new_vec;
}

void draw_ray()
{
	float end_x = ray_start[0] + ray_length*ray_dir[0];
	float end_y = ray_start[1] + ray_length*ray_dir[1];
	float end_z = ray_start[2] + ray_length*ray_dir[2];
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth( 3.0 );
	glBegin(GL_LINES);
	glVertex4f(ray_start[0], ray_start[1], ray_start[2], 1);
	glVertex4f(end_x, end_y, end_z, 1);
	glEnd();

	glLineWidth( 1.0 );
}

void draw_objects() 
{
	//Enable texturing
	glEnable(GL_TEXTURE_2D);

	//floor texture
	glBindTexture(GL_TEXTURE_2D, texturesList[0].texID);

	//draw floor
	glColor4f(1, 1, 1, 1);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex4f(-50, 0, 50, 1);
		glTexCoord2f(30.0f, 0.0f);
		glVertex4f(-50, 0, -50, 1);
		glTexCoord2f(30.0f, 30.0f);
		glVertex4f(50, 0, -50, 1);
		glTexCoord2f(0.0f, 30.0f);
		glVertex4f(50, 0, 50, 1);
	glEnd();

	//If you arent in the process of being murdered, draw walls/trees
	if (!killAnimation)
	{
		//walls
		for(int i = 0; i < level_objects.size(); i++)
		{
			if (level_objects[i]->type == WALL)
			{
				glBindTexture(GL_TEXTURE_2D, texturesList[1].texID);
			}
			else if (level_objects[i]->type == TREE)
			{
				glBindTexture(GL_TEXTURE_2D, texturesList[2].texID);
			}
			level_objects[i]->draw(GL_POLYGON);
		}

		//snowballs
		glBindTexture(GL_TEXTURE_2D, texturesList[0].texID);
		for (int i = 0; i < 8; i++)
		{
			if (!snowballList[i]->isDead())
				snowballList[i]->draw();
		}


		if (slenBro->getShowSlender())
		{
			glBindTexture(GL_TEXTURE_2D, texturesList[0].texID);
			slenBro->draw();
		}
	}
	else
	{
		slenBro->draw();
	}

	//Disable texturing
	glDisable(GL_TEXTURE_2D);
}

void my_display() 
{

	// clear all pixels, reset depth 
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// using my camera matrices and loading them on the stack
	glMultMatrixd(my_camera->R);
	glMultMatrixd(my_camera->T);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// only use Mpp for perspective camera
	if (my_camera->type == PERSP)
	{
		glMultMatrixd(my_camera->M);
	}
	glMultMatrixd(my_camera->S);

	glMatrixMode(GL_MODELVIEW);

	draw_objects();

	// this buffer is ready
	glutSwapBuffers();
}

void my_idle(void) 
{
	return ;
}

void my_TimeOut(int id)
{
	printf("");	//This is important or timeout just doesnt happen

	//If we arent dead or dying
	if (!killAnimation && !dead)
	{
		bool moving = false;

		//Did you get a snowball?
		for (int i = 0; i < 8; i++)
		{
			if (!snowballList[i]->isDead())
			{
				if (snowballList[i]->distFromPlayer(my_camera->pos[0], my_camera->pos[2]) <= 0.4)
				{
					snowballList[i]->killEmElephantMan();
					num_snowballs++;
					slenBro->increaseMoveRate();

					//Increase redness of fog
					fogColor[0] += .01;
					fogColor[1] -= .01;
					fogColor[2] -= .01;
					glClearColor(fogColor[0], fogColor[1], fogColor[2], 1); 
					glFogfv(GL_FOG_COLOR, fogColor);
				}
			}
		}

		if (lock_pointer == true)
		{
			mouse_adjust();
		}
	
		my_camera->update();

		#pragma region movement

		// moving the camera
		if (move_cam_forward)
		{
			moving = true;
			//Create direction
			float curr_dir[3] = {my_camera->look[0], 0, my_camera->look[2]};
			normalize(curr_dir);

			//Set look ray(ray_dir)
			set_ray(my_camera->pos[0], my_camera->pos[1], my_camera->pos[2],
				curr_dir[0], curr_dir[1], curr_dir[2]);

			//Check for collisions, change ray_dir if needed
			my_raytrace(0, 0);

			//Move camera by ray direction. ray_dir = camera dir(curr_dir) if not about to collide
			my_camera->move(ray_dir[0], ray_dir[1], ray_dir[2]);

			glLightfv(GL_LIGHT0, GL_POSITION, my_camera->pos);
		}
		if (move_cam_back)
		{
			moving = true;
			float curr_dir[3] = {my_camera->w[0], 0, my_camera->w[2]};
			normalize(curr_dir);

			set_ray(my_camera->pos[0], my_camera->pos[1], my_camera->pos[2],
				curr_dir[0], curr_dir[1], curr_dir[2]);
			my_raytrace(0, 0);

			my_camera->move(ray_dir[0], ray_dir[1], ray_dir[2]);
			glLightfv(GL_LIGHT0, GL_POSITION, my_camera->pos);
		}
		if (move_cam_left)
		{
			moving = true;
			float curr_dir[3] = {-my_camera->u[0], 0, -my_camera->u[2]};
			normalize(curr_dir);

			set_ray(my_camera->pos[0], my_camera->pos[1], my_camera->pos[2],
				curr_dir[0], curr_dir[1], curr_dir[2]);
			my_raytrace(0, 0);

			my_camera->move(ray_dir[0], ray_dir[1], ray_dir[2]);
			glLightfv(GL_LIGHT0, GL_POSITION, my_camera->pos);
		}
		if (move_cam_right)
		{
			moving = true;
			float curr_dir[3] = {my_camera->u[0], 0, my_camera->u[2]};
			normalize(curr_dir);

			set_ray(my_camera->pos[0], my_camera->pos[1], my_camera->pos[2],
				curr_dir[0], curr_dir[1], curr_dir[2]);
			my_raytrace(0, 0);

			my_camera->move(ray_dir[0], ray_dir[1], ray_dir[2]);
			glLightfv(GL_LIGHT0, GL_POSITION, my_camera->pos);
		}
		#pragma endregion movement

		if (slenBro->getShowSlender()) // currently showing slender
		{
			slenBro->countDownShowSlenderTimer(timer_tick_interval);
			if (slenBro->getShowSlenderTimer() <= 0) // if the show timer has expired
			{
				printf("Show timer expired. Set the current distance from player.\n");
				slenBro->resetShowSlender();
				slenBro->resetShowSlenderTimer();

				float dist = sqrt(pow(slenBro->pos[0] - my_camera->pos[0], 2)
								+ pow(slenBro->pos[1] - my_camera->pos[1], 2)
								+ pow(slenBro->pos[2] - my_camera->pos[2], 2));

				slenBro->setDistFromPlayer(dist);
			}
		}
		else // not currently showing slender
		{
			slenBro->countDownHideSlenderTimer(timer_tick_interval);
			if (slenBro->getHideSlenderTimer() <= 0) // hide timer has expired
			{
				printf("Slender hide timer has expired.  Place him in the world.\n");
				// reset hide timer
				slenBro->resetHideSlenderTimer(num_snowballs);

				// check to see if slender is going to be placed inside of an object/building
				set_ray(my_camera->pos[0],  my_camera->pos[1], my_camera->pos[2], -my_camera->look[0], 0, -my_camera->look[2]);

				float curr_dist = slenBro->distFromPlayer;
				bool slender_hit = slender_raytrace();

				// if slender is going to be in a building, put him outside it
				if (slender_hit)
				{
					for (int i = 0; i < slender_t_vals.size(); i++)
					{
						curr_dist = slender_t_vals[i] + 1;
					}
					/*
					curr_dist = -1;
					for (int i = 0; i < slender_t_vals.size(); i++)
					{
						if (curr_dist < 0)
						{
							curr_dist = slender_t_vals[i];
						}
						else if (slender_t_vals[i] < curr_dist)
						{
							curr_dist = slender_t_vals[i];
						}
					}

					curr_dist += 2;
					*/
				}

				// put slender behind player
				float slen_pos[3] = {ray_start[0]+curr_dist*ray_dir[0], slenBro->pos[1], ray_start[2]+curr_dist*ray_dir[2]};
				slenBro->setPosition(slen_pos);
			
				slenBro->showSlender();

			}
			else // just need to update slenders distance
			{
				float move_dist = 0;
				if (moving)
				{
					if (my_camera->isSprinting)
					{
						move_dist = SPRINT_SPEED;
					}
					else
					{
						move_dist = WALK_SPEED;
					}
				}
				slenBro->decreaseDistFromPlayer(move_dist);
				
				set_ray(my_camera->pos[0],  my_camera->pos[1], my_camera->pos[2], my_camera->look[0], 0, my_camera->look[2]);

				float curr_dist = slenBro->distFromPlayer;
				float slen_pos[3] = {ray_start[0]+curr_dist*ray_dir[0], slenBro->pos[1], ray_start[2]+curr_dist*ray_dir[2]};
				slenBro->setPosition(slen_pos);
				
			}
		}

		//Check for him killing you
		float dist = sqrt(pow(slenBro->pos[0] - my_camera->pos[0], 2)
								+ pow(slenBro->pos[1] - my_camera->pos[1], 2)
								+ pow(slenBro->pos[2] - my_camera->pos[2], 2));
		if (dist < 0.4)
		{
			//Set up things for kill animation
			killAnimation = true;
			
			if (!slenBro->getShowSlender())
			{
				float slen_pos[3] = {my_camera->pos[0]-0.4*my_camera->look[0], slenBro->pos[1], my_camera->pos[2]-0.4*my_camera->look[2]};
				slenBro->setPosition(slen_pos);
			}


			// u rotation
			float curr_look[4] = {0, my_camera->look[1], my_camera->look[2], 0};
			//float curr_look[4] = {0, 0, -1, 0};
			normalize(curr_look);
			float mag1 = sqrt(pow(curr_look[0], 2) + pow(curr_look[1], 2) + pow(curr_look[2], 2));
			
			//float plyr_to_slen[4] = {0, slenBro->head->center[1] - my_camera->pos[1], slenBro->head->center[2] - my_camera->pos[2], 0};
			float plyr_to_slen[4] = {0, 0.5, -1, 0};
			normalize(plyr_to_slen);
			float mag2 = sqrt(pow(plyr_to_slen[0], 2) + pow(plyr_to_slen[1], 2) + pow(plyr_to_slen[2], 2));
			
			kill_deg_u = acos(dotprod(plyr_to_slen, curr_look)/(mag1*mag2))*(180/M_PI);

			kill_deg_u = 2*kill_deg_u/(kill_time/timer_tick_interval);
			if (curr_look[1] < plyr_to_slen[1])
			{
				kill_deg_u *= -1;
			}

			// v rotation
			float curr_look1[4] = {my_camera->look[0], 0, my_camera->look[2], 0};
			normalize(curr_look1);
			float mag11 = sqrt(pow(curr_look1[0], 2) + pow(curr_look1[1], 2) + pow(curr_look1[2], 2));
			
			float plyr_to_slen1[4] = {slenBro->head->center[0] - my_camera->pos[0], 0, slenBro->head->center[2] - my_camera->pos[2], 0};
			normalize(plyr_to_slen1);
			float mag21 = sqrt(pow(plyr_to_slen1[0], 2) + pow(plyr_to_slen1[1], 2) + pow(plyr_to_slen1[2], 2));
			
			kill_deg_v = 2*acos(dotprod(curr_look1, plyr_to_slen1)/(mag11*mag21))*(180/M_PI)/(kill_time/timer_tick_interval);

			if (curr_look1[0] > plyr_to_slen1[0])
			{
				kill_deg_v *= -1;
			}

		}
		else if (num_snowballs == 8)
		{
			printf("You win!");
			dead = true;
		}
	}
	else if (killAnimation)
	{
		kill_time -= timer_tick_interval;
		fogColor[0] += .003;
		fogColor[1] -= .003;
		fogColor[2] -= .003;
		glClearColor(fogColor[0], fogColor[1], fogColor[2], 1); 
		glFogfv(GL_FOG_COLOR, fogColor);
		if (kill_time >= DEFAULT_KILL_TIME/2)
		{
			my_camera->LookLeftRight(kill_deg_v);
		}
		else
		{
			my_camera->RotateU(kill_deg_u);
		}

		if (kill_time <= 0)
		{
			dead = true;
		}
	}

	glutPostRedisplay();

	if (!dead)
	{
		glutTimerFunc(timer_tick_interval, my_TimeOut, 0);
	}

	return;
}