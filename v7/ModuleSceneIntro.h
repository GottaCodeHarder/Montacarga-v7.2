#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_MINUTES 3 // 3 + 1
#define MAX_SECONDS 59 // by definition

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void CreateWorld();

	bool Reset();

	bool objectiveA = false;
	bool objectiveB = false;
	bool objectiveC = false;

	bool addObjetiveA = false;
	bool addObjetiveB = false;
	bool addObjetiveC = false;
	
	uint objetivesDone = 0;
	uint timeBeforeWin[3];

public:

	Timer timer;

	PhysBody3D* dest[3];
	Cube c_dest[3];

	Cube c_obj[3];
	PhysBody3D* obj[3];

	PhysBody3D* stage;

	Cube c_wall[4];
	PhysBody3D* wall[4];

	Plane p;

	PhysBody3D* pallet[152];
	Cube c_pallet[152];

	bool exist[152];

	//VARIABLES RANDOM PARA POSICION DE objective

	int objective[3];
	int goal[3];

	vec3 pos_objective[3];

	float bestTime;
	int countdown = 0;
	
	bool song;
	bool finishedMatch = false;
	bool lostMatch = false;

private:
	// Background Music
	Mix_Music *alabamaBGM, *bloodpressureBGM;
	uint correctSFX;
};
