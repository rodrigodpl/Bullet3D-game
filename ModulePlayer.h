#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 2000.0f
#define TURN_DEGREES 20.0f * DEGTORAD
#define BRAKE_POWER 150.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void FillLoad(int load, vec3 pos);
	void Reset();
	int CheckLoad();


public:

	PhysVehicle3D* vehicle;
	PhysVehicle3D* loadbox;
	bool control_enabled;
	float turn;
	float acceleration;
	float brake;
};