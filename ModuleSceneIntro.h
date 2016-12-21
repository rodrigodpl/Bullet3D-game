#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "Timer.h"

#define MAX_SNAKE 2
#define SECTIONS 50

struct PhysVehicle3D;
struct PhysMotor3D;

enum scene_st {

	STARTING,
	PLAYING,
	LOSE,
	WIN
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void BuildTrack(float* turns);

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	/*
	PhysBody3D* pb_snake[MAX_SNAKE];
	Sphere s_snake[MAX_SNAKE];

	PhysBody3D* pb_snake2[MAX_SNAKE];
	Sphere s_snake2[MAX_SNAKE];
	*/

	btRigidBody* vehicle_chassis;
	btRigidBody* loadbox_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	p2List<Trackpiece*> track;
	p2List<PhysBody3D*> columns;
	p2List<PhysBody3D*> logs;
	p2List<PhysBody3D*> marbles;
	p2List<PhysBody3D*> doors;
	p2List<PhysBody3D*> ramps;
	p2List<PhysBody3D*> load;
	PhysVehicle3D* player;

	int progression;
	int kept_load;

	Timer timer;
	Uint32 cronometer;

	scene_st state;

};
