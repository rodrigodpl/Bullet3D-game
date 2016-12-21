#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"

#include "glut\glut.h"

#include <time.h>
#include <cstdio>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	progression = 0, kept_load = 0, cronometer = 0;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	float turns[SECTIONS];

	int prev_pos = 0;

	srand(time(NULL));

	for (int i = 0; i < SECTIONS; i++) {

		int pos = (rand() % 10);
		
		if(rand() % 2)
			pos = -pos;

		if ((prev_pos > 60 && pos > 0) || (prev_pos < -60 && pos < 0))
			pos = -pos;
		
		prev_pos += pos;

		turns[i] = pos;

	}

	BuildTrack(turns);

	timer.Start();

	state = STARTING;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	switch (state) {

	case STARTING:
		
		if (timer.Read() > 5000) {

			App->player->control_enabled = true;
			state = PLAYING;
			timer.Start();
		}
		break;

	case PLAYING:

		if (timer.Read() > cronometer + 500) {

			cronometer = timer.Read();

			kept_load = App->player->CheckLoad();

			if (kept_load == 0)
				state = LOSE;
			else if (progression > 100) 
				state = WIN;
			

		}

		break;

	case LOSE:

		if (App->player->CheckLoad() > 0 && timer.Read() <  cronometer + 2000)
			state = PLAYING;
		else if (timer.Read() > cronometer + 5000) 
			App->player->Reset();
		
		break;

	case WIN: 
		
		break;

	}
	


	Cylinder C(1, 20);
	Cylinder L(0.8, 6);
	Sphere S(0.6);
	Sphere M(0.6);
	Cube D(10, 5, 0.4);
	Cube R(8, 15, 0.2);

	C.color = Gray;
	L.color = Brown;
	S.color = Green;
	M.color = Blue;
	D.color = White;
	R.color = Yellow;


	for (p2List_item<Trackpiece*>* item = track.getFirst(); item; item = item->next) 
		item->data->Render();

	for (p2List_item<PhysBody3D*>* item = load.getFirst(); item; item = item->next) {
		item->data->GetTransform(S.transform.M);															 
		S.Render();
	}

	for (p2List_item<PhysBody3D*>* item = columns.getFirst(); item; item = item->next) {
		(item->data->GetTransform(C.transform.M));
		C.Render();
	}

	for (p2List_item<PhysBody3D*>* item = logs.getFirst(); item; item = item->next) {
		(item->data->GetTransform(L.transform.M));
		L.Render();
	}

	for (p2List_item<PhysBody3D*>* item = marbles.getFirst(); item; item = item->next) {
		item->data->GetTransform(M.transform.M);
		M.Render();
	}

	for (p2List_item<PhysBody3D*>* item = doors.getFirst(); item; item = item->next) {
		item->data->GetTransform(D.transform.M);
		D.Render();
	}

	for (p2List_item<PhysBody3D*>* item = ramps.getFirst(); item; item = item->next) {
		item->data->GetTransform(R.transform.M);
		R.Render();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.5f, 0.5f, 0.5f);

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-2000.0f, 0.0f, -2000.0f);
	glVertex3f(-2000.0f, 0.0f, 2000.0f);
	glVertex3f(2000.0f, 0.0f, 2000.0f);
	glVertex3f(2000.0f, 0.0f, -2000.0f);

	glEnd();

	if (player != NULL)
		player = App->player->vehicle;

	if(!(App->physics->debug))
		App->camera->FollowPlayer(player);



	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

void ModuleSceneIntro::BuildTrack(float* turns) {

	float x = 0, y = 0, z = 0, prev_turn = 0;

	for (int i = 0; i < SECTIONS; i++) {

		Trackpiece* tp = NULL;

		tp = new Trackpiece(turns[i]);
		tp->color = White;
		tp->SetPos(x, y, z);
		track.add(tp);
		App->physics->AddTrackpiece(*tp);

		if (i == 0 || i == SECTIONS - 1) {
			z += tp->h;
			x += turns[i];
			prev_turn = turns[i];
			continue;
		}

		int obstacle = rand() % 10;

		int max_x = 0, min_x = 0;
		PhysBody3D* new_body = NULL;
		PhysBody3D* aux_body = NULL;

		max_x = ((turns[i] - prev_turn) >= 0) ? tp->points[4].x() - 1 : tp->points[7].x() - 1;
		min_x = ((turns[i] - prev_turn) >= 0) ? x + tp->points[0].x() + 1 : x + tp->points[3].x() + 1;

		if (max_x == 0)
			max_x++;

		Cylinder C(1, 20);
		Cylinder L(0.8, 6);
		Sphere M(0.6);
		Cube D(10, 5, 0.4);
		Cylinder D_holder(0.3, 8);
		Cube R(8, 15, 0.2);

		switch (obstacle) {

		case 1: 

			C.SetPos(min_x + (rand() % abs(max_x) + 1), 10, z + (rand() % tp->h));
			C.SetRotation(90, { 0, 0, 1 });
			new_body = App->physics->AddBody(C, 0);
			columns.add(new_body);
			break;

		case 2:

			for (int i = 0; i < 3; i++) {
				L.SetPos(min_x + (rand() % abs(max_x) + 1), 5 + i * 3, z + (rand() % tp->h));
				new_body = App->physics->AddBody(L, 10);
				logs.add(new_body);
			}
			break;

		case 3:

			for (int i = 0; i < 30; i++) {
				M.SetPos(min_x + (rand() % abs(max_x) + 1), 5 + i * M.radius, z + (rand() % tp->h));
				new_body = App->physics->AddBody(M, 1);
				marbles.add(new_body);
			}
			break;

		case 4:

			if ((max_x - min_x) > D.size.x) {
				D.SetPos(min_x + (max_x / 2 - (D.size.x / 2)), 4.5, z + (tp->h / 2));
				D_holder.SetPos(min_x + (max_x / 2), 4.5, z + (tp->h / 2));
				new_body = App->physics->AddBody(D, 10);
				aux_body = App->physics->AddBody(D_holder, 0);


				btHingeConstraint* constraint = App->physics->AddConstraintHinge(*new_body, *aux_body,
					vec3{ 0, 0, 0 }, vec3{ 0, 0, 0 }, vec3{ 0, 1, 0 }, vec3{ 0, 1, 0 }, true);

				if(rand() % 2)
					constraint->enableAngularMotor(true, 3, 20);
				else
					constraint->enableAngularMotor(true, -3, 20);

				constraint->setLimit(1, 0);

				doors.add(new_body);
			}

			break;

		case 5:

			R.SetPos(min_x + (max_x / 2), 2, z + (tp->h / 2));
			R.SetRotation(70, { 1, 0, 0 });
			new_body = App->physics->AddBody(R, 0);
			ramps.add(new_body);
			break;

		}
		
		z += tp->h;
		x += turns[i];
		prev_turn = turns[i];
	}

}

