#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading AI");

	control_enabled = false;

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(4, 5, 3);
	car.chassis_offset.Set(0, 3, 0);
	car.mass = 300.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.7f;
	float wheel_width = 0.7f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;


	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(7, 2, 15);

	car.chassis_size.x *= 1.5; car.chassis_size.y *= 0.5; car.chassis_size.z *= 3;
	car.chassis_offset.x *= 1.5; car.chassis_offset.y *= 0.5; car.chassis_offset.z *= 3;

	car.suspensionDamping = 0.2f;
	car.suspensionStiffness = 8.0f;
	car.frictionSlip = 6.0f;

	car.wheels[0].connection.Set(car.chassis_size.x * 0.5 - 1.0f * wheel_width, connection_height,   car.chassis_size.z * 0.5 - wheel_radius - 1.0f);
	car.wheels[1].connection.Set(-car.chassis_size.x * 0.5 + 1.0f * wheel_width, connection_height, car.chassis_size.z  * 0.5 - wheel_radius - 1.0f);
	car.wheels[2].connection.Set(car.chassis_size.x * 0.5 - 1.0f * wheel_width, connection_height, -car.chassis_size.z * 0.5 + wheel_radius + 1.0f);
	car.wheels[3].connection.Set(-car.chassis_size.x * 0.5 + 1.0f * wheel_width, connection_height, -car.chassis_size.z  * 0.5 + wheel_radius + 1.0f);


	loadbox = App->physics->AddLoadBox(car, 0.5);
	loadbox->SetPos(7, 2, 5);

	btHingeConstraint* constraint = App->physics->AddConstraintHinge(*vehicle, *loadbox, vec3(0, 0, -3.5), vec3(0, 0, 3), vec3(0, 1, 0), vec3(0, 1, 0));

	constraint->setLimit(0.0f, 0.001);

	FillLoad(50, vec3(7, 2, 5));

	App->audio->PlayFx(App->audio->engine, -1);
	App->audio->SetVolume(0.1f, App->audio->engine);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{

	turn = acceleration = brake = 0.0f;

	if (control_enabled) {

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			acceleration = MAX_ACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;

		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		{
			brake = BRAKE_POWER;
			if (vehicle->GetKmh() > 10)
				App->audio->PlayFx(App->audio->braking);
			else
				App->audio->HaltChannel(App->audio->braking);
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			acceleration = -MAX_ACCELERATION;
			if (vehicle->GetKmh() < 0)
				App->audio->PlayFx(App->audio->reverse);
			else
				App->audio->HaltChannel(App->audio->reverse);
		}

		if (vehicle->GetKmh() > 100 && vehicle->GetKmh() < 300)
			App->audio->SetVolume((vehicle->GetKmh() - 100 / 300), App->audio->engine);

		vehicle->ApplyEngineForce(acceleration);
		vehicle->Turn(turn);
		vehicle->Brake(brake);

	}

	vehicle->Render();
	loadbox->Render();

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) 
		Reset();
	
	

	char title[80];
	sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModulePlayer::FillLoad(int load, vec3 pos) {


	for (int i = 0; i < load; i++) {

		Sphere* S = new Sphere(0.6);

		S->SetPos(pos.x , pos.y + (S->radius * 2 * i), pos.z);
		PhysBody3D* S_p = App->physics->AddBody(*S, 1);
		App->scene_intro->load.add(S_p);
		
	}

}

void ModulePlayer::Reset() {


	btTransform t;
	t.setIdentity();
	
	App->scene_intro->vehicle_chassis->setLinearVelocity(btVector3(0, 0, 0));
	App->scene_intro->vehicle_chassis->setAngularVelocity(btVector3(0, 0, 0));
	App->scene_intro->vehicle_chassis->setWorldTransform(t);
	vehicle->SetPos(7, 2, 15);

	App->scene_intro->loadbox_chassis->setLinearVelocity(btVector3(0, 0, 0));
	App->scene_intro->loadbox_chassis->setAngularVelocity(btVector3(0, 0, 0));
	App->scene_intro->loadbox_chassis->setWorldTransform(t);
	loadbox->SetPos(7, 2, 5);

	int i = 0;
	for (p2List_item<PhysBody3D*>* item = App->scene_intro->load.getFirst(); item; item = item->next) {

		item->data->SetPos(7, 2.5 + (1.2 * i), 5);
		item->data->Stop();
		i++;
	}

	vehicle->Brake(5000.f);

	App->scene_intro->state = STARTING;
	App->scene_intro->cronometer = App->scene_intro->progression = 0;
	App->scene_intro->timer.Start();

	control_enabled = false;
	

}


int ModulePlayer::CheckLoad() {

	mat4x4 player_pos;
	vehicle->GetTransform(player_pos.M);
	App->scene_intro->progression = player_pos[14] / App->scene_intro->track.getLast()->data->transform.translation().z * 100;

	mat4x4 loadbox_pos;
	loadbox->GetTransform(loadbox_pos.M);
	int safe_load = 0;

	int loadbox_left_limit = loadbox_pos[12] - (loadbox->info.chassis_size.x / 2) - 1;
	int loadbox_right_limit = loadbox_pos[12] + (loadbox->info.chassis_size.x / 2) + 1;

	int loadbox_upper_limit = loadbox_pos[13] + loadbox->info.chassis_size.y * 2;
	int loadbox_down_limit = loadbox_pos[13];

	int loadbox_front_limit = loadbox_pos[14] + (loadbox->info.chassis_size.z / 2) + 1;
	int loadbox_back_limit = loadbox_pos[14] - (loadbox->info.chassis_size.z / 2) - 1;


	for (p2List_item<PhysBody3D*>* item = App->scene_intro->load.getFirst(); item; item = item->next) {

		Sphere S(0.6);
		item->data->GetTransform(S.transform.M);

		if (S.transform.translation().x > loadbox_left_limit && S.transform.translation().x < loadbox_right_limit &&
			S.transform.translation().y > loadbox_down_limit && S.transform.translation().y < loadbox_upper_limit &&
			S.transform.translation().z > loadbox_back_limit && S.transform.translation().z < loadbox_front_limit)
			safe_load++;

	}

	return safe_load;

}




