#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 100.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	bool Reset();
	void ResetVehicle(float x, float y, float z, float angle);
	float GetSpeed();
	btVector3 GetForwardVec3();
	vec3 GetPosition();

public:

	PhysBody3D* elevators;
	Cube c_elevators;

	bool elev;

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

	uint stopSFX, slowSFX, mediumSFX, fastSFX, backSFX;
	uint stopESFX, slowESFX, mediumESFX, fastESFX, backESFX;

	vec3 lastPos;
};