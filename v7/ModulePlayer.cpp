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
	LOG("Loading player");
	float PI = 3.141592;

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(0, 0, 0));
	trans.setRotation(btQuaternion(btVector3(0, 0, 1), PI / 2));

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2.0f, 0.2f, 3.0f);
	car.chassis_offset.Set(0.0f, 0.1f, 0.0f);
	car.mass = 500.0f;
	car.suspensionStiffness = 200.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 1.0f;
	car.maxSuspensionTravelCm = 500.0f;
	car.frictionSlip = 100.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 0.2f;
	float wheel_radius = 1.0f;
	float wheel_width = 0.8f;
	float suspensionRestLength = 0.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0.0f, -1.0f, 0.0f);
	vec3 axis(-1.0f, 0.0f, 0.0f);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(car.chassis_size.x, connection_height, car.chassis_size.z);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = false;
	car.wheels[0].drive = true;
	car.wheels[0].brake = true;
	car.wheels[0].steering = false;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-car.chassis_size.x, connection_height, car.chassis_size.z);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = false;
	car.wheels[1].drive = true;
	car.wheels[1].brake = true;
	car.wheels[1].steering = false;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(0.0f, connection_height, -car.chassis_size.z);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width / 3.0f;
	car.wheels[2].front = true;
	car.wheels[2].drive = false;
	car.wheels[2].brake = false;
	car.wheels[2].steering = true;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(0.0f, connection_height, -car.chassis_size.z);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width / 3.0f;
	car.wheels[3].front = true;
	car.wheels[3].drive = false;
	car.wheels[3].brake = false;
	car.wheels[3].steering = true;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 1, 0);

	btTransform trans2;
	trans2.setIdentity();
	trans2.setOrigin(btVector3(0.0f, 0.0f, -8.8));
	c_elevators.size.Set(0.1f, 0.8f, 9.0f);
	elevators = App->physics->Addelevators(c_elevators);
	elevators->SetPos(0.0f, 3.0f, 10.0f);
	elevators->body->setFriction(500);
	elevators->body->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	elevators->body->setRestitution(0);

	App->physics->AddConstraintSlider(*vehicle, *elevators, trans, trans2, false);

	elev = false;
	
	// SFXs without Elevator
	stopSFX = App->audio->LoadFx("Sounds/SFXs/0kmph.ogg");
	slowSFX = App->audio->LoadFx("Sounds/SFXs/1kmph.ogg");
	mediumSFX = App->audio->LoadFx("Sounds/SFXs/10kmph.ogg");
	fastSFX = App->audio->LoadFx("Sounds/SFXs/25kmph.ogg");
	backSFX = App->audio->LoadFx("Sounds/SFXs/back.ogg");

	// SFXs with Elevator
	stopESFX = App->audio->LoadFx("Sounds/SFXs/0kmphElevator.ogg");
	slowESFX = App->audio->LoadFx("Sounds/SFXs/1kmphElevator.ogg");
	mediumESFX = App->audio->LoadFx("Sounds/SFXs/10kmphElevator.ogg");
	fastESFX = App->audio->LoadFx("Sounds/SFXs/25kmphElevator.ogg");
	backESFX = App->audio->LoadFx("Sounds/SFXs/backElevator.ogg");

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
	vec3 p = GetPosition();
	if (App->scene_intro->finishedMatch == true)
		p = lastPos;
	else 
		lastPos = GetPosition();
	btVector3 vehicle_vector = GetForwardVec3();
	vec3 f(vehicle_vector.getX(), vehicle_vector.getY(), vehicle_vector.getZ());
	vec3 cameraPos = { p.x + (f.x * App->camera->cameraDistance.x), p.y + f.y + App->camera->cameraDistance.y, p.z + (f.z *  App->camera->cameraDistance.z) };
	vec3 cameraSpeed = cameraPos - App->camera->Position;
	App->camera->Look(App->camera->Position + cameraSpeed, GetPosition());

	turn = acceleration = brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (vehicle->GetKmh() <= 0)
			brake = BRAKE_POWER;
		else if (vehicle->GetKmh() <= 75.0f)
			acceleration = MAX_ACCELERATION;
	}

	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (vehicle->GetKmh() >= 0)
			brake = BRAKE_POWER;
		else if (vehicle->GetKmh() >= -25.0f)
			acceleration = -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		elev = !elev;

	if (elev)
		elevators->body->applyCentralForce(btVector3(0, 30, 0));
	else
		elevators->body->applyCentralImpulse(btVector3(0, -0.01, 0));
	
	
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		ResetVehicle(0,0,0,0);
	}

	// AUDIO SETTINGS

		if (vehicle->GetKmh() / 3 >= 20.0f)
		{
			if (elev)
				App->audio->PlayFx(fastESFX, NULL);
			else
				App->audio->PlayFx(fastSFX, NULL);
		}
		else if (vehicle->GetKmh() / 3 >= 10.0f && vehicle->GetKmh() / 3 < 20.0f)
		{
			if (elev)
				App->audio->PlayFx(mediumESFX, NULL);
			else
				App->audio->PlayFx(mediumSFX, NULL);
		}
		else if (vehicle->GetKmh() / 3 >= 2.0f && vehicle->GetKmh() / 3 < 10.0f)
		{
			if (elev)
				App->audio->PlayFx(slowESFX, NULL);
			else
				App->audio->PlayFx(slowSFX, NULL);
		}
		else if (vehicle->GetKmh() / 3 >= -1.0f && vehicle->GetKmh() / 3 < 2.0f)
		{
			if (elev)
				App->audio->PlayFx(stopESFX, NULL);
			else
				App->audio->PlayFx(stopSFX, NULL);
		}
		else
		{
			if (elev)
				App->audio->PlayFx(backESFX, NULL);
			else
				App->audio->PlayFx(backSFX, NULL);
		}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	return UPDATE_CONTINUE;
}

bool ModulePlayer::Reset()
{
	LOG("Player Reset Successfully!");
	ResetVehicle(0, 0, 0, 0);
	return true;
}

float ModulePlayer::GetSpeed()
{
	return vehicle->GetKmh();
}

btVector3 ModulePlayer::GetForwardVec3()
{
		return vehicle->vehicle->getForwardVector();;
}

vec3 ModulePlayer::GetPosition()
{
	vec3 position;
	btTransform a = vehicle->body->getWorldTransform();
	position.Set(a.getOrigin().getX(), a.getOrigin().getY(), a.getOrigin().getZ());
	return position;
}

void ModulePlayer::ResetVehicle(float x, float y, float z, float angle)
{
	vehicle->SetPos(x, y, z);
	vehicle->StopMotion();
	vehicle->Orient(angle);
	elevators->SetPos(x + 0.0f, y + 3.0f, z + 10.0f);
}