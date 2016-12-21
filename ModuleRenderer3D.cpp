#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "SDL\include\SDL_opengl.h"

#include "glut\glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.35f, 0.35f, 0.35f, 1.0f);
		lights[0].diffuse.Set(0.4f, 0.2f, 0.0f, 1.0f);
		lights[0].SetPos(0.0f, 150.0f, 150.0f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		//lights[1].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

	glClearColor(Dark_Orange.r, Dark_Orange.g, Dark_Orange.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	//lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	char str[30];
	str[0] = '\0';
	float time_left;

	switch (App->scene_intro->state) {

	case STARTING: 
		

		if (App->scene_intro->timer.Read() < 3000) {
			strcat(str, "Steady\0");
			DrawUI(str, 600, 600, Red);
		}
		else if (App->scene_intro->timer.Read() < 4000) {
			strcat(str, "Ready\0");
			DrawUI(str, 600, 600, Orange);
		}
		else {
			strcat(str, "Go!\0");
			DrawUI(str, 600, 600, Green);
		}
		
		break;

	case PLAYING:

		strcat(str, "Progress: ");

		char prog_str[10];
		sprintf(prog_str, "%d / 100 \0", App->scene_intro->progression);
		strcat(str, prog_str);

		DrawUI(str, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 100, White);

		str[0] = '\0';
		strcat(str, "Time left: ");

		char time_str[10];
		time_left = (float)(180000 - App->scene_intro->timer.Read()) / 1000;
		sprintf(time_str, "%f\0", time_left);
		strcat(str, time_str);


		if (time_left > 60)
			DrawUI(str, 200, SCREEN_HEIGHT - 100, White);
		else if (time_left > 30)
			DrawUI(str, 200, SCREEN_HEIGHT - 100, Orange);
		else
			DrawUI(str, 200, SCREEN_HEIGHT - 100, Red);


		str[0] = '\0';
		strcat(str, "Load: ");

		char load_str[10];
		sprintf(load_str, "%d/50\0", App->scene_intro->kept_load);
		strcat(str, load_str);

		if (App->scene_intro->kept_load > 30)
			DrawUI(str, 200, 100, Green);
		else if (App->scene_intro->kept_load > 10)
			DrawUI(str, 200, 100, Orange);
		else
			DrawUI(str, 200, 100, Red);

		break;

	case LOSE:
		
		strcat(str, "You Lost!\0"); 
		DrawUI(str, 600, 600, White);

		break;

	case WIN:

		strcat(str, "You Won!\0");
		DrawUI(str, 600, 900, White);
		
		if (App->scene_intro->timer.Read() > 4000) {

			str[0] = '\0';
			strcat(str, "Kept load: ");

			char load_score_str[30];
			sprintf(load_score_str, "  %d * 100 = %d\0", App->scene_intro->kept_load, App->scene_intro->kept_load * 100);
			strcat(str, load_score_str);

			DrawUI(str, 600, 800, White);

		}
		if (App->scene_intro->timer.Read() > 6000) {

			str[0] = '\0';
			strcat(str, "Time left: ");

			char time_score_str[30];
			sprintf(time_score_str, " %d * 50 = %d\0", (180000 - App->scene_intro->cronometer) / 1000, (180000 - App->scene_intro->cronometer) * 50 / 1000);
			strcat(str, time_score_str);

			DrawUI(str, 600, 750, White);

		}
		if (App->scene_intro->timer.Read() > 8000) {

			str[0] = '\0';
			strcat(str, "Score: ");

			char score_str[30];
			sprintf(score_str, " %d + %d = %d\0", App->scene_intro->kept_load * 100, (180000 - App->scene_intro->cronometer) * 50 / 1000, (App->scene_intro->kept_load * 100) + ((180000 - App->scene_intro->cronometer) * 50 / 1000));
			strcat(str, score_str);

			DrawUI(str, 600, 700, White);

		}

		break;
	}


	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::DrawUI(char* str, int x, int y, Color color) {


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	glColor3f(color.r, color.g, color.b); 

	glRasterPos2i(x, y);
	int len = (int)strlen(str);

	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);


	glPopMatrix();
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);


}
