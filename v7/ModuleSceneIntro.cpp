#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

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

	timer.Start();

	int pos = 100;
	int pos2 = 50;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));


	CreateWorld();
	
	//MUSIC
	alabamaBGM = Mix_LoadMUS("Sounds/BGM/alabama.ogg");
	bloodpressureBGM = Mix_LoadMUS("Sounds/BGM/bloodpressure.ogg");
	correctSFX = App->audio->LoadFx("Sounds/SFXs/correct.ogg");

	Mix_PlayMusic(alabamaBGM, -1);
	song = true;

	timer.Chronometer(false, NULL);

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
	Cube p2(360, 0.1, 260);
	p2.SetPos(0, 0.1, 0);
	p2.color = Grass;

	c_obj[0].color = Red;
	c_obj[0].Render();

	c_obj[1].color = Blue;
	c_obj[1].Render();

	c_obj[2].color = Yellow;
	c_obj[2].Render();

	c_dest[0].color = Red;
	c_dest[0].Render();

	c_dest[1].color = Blue;
	c_dest[1].Render();

	c_dest[2].color = Yellow;
	c_dest[2].Render();

	for (int i = 0; i < 4; i++)
	{
	c_wall[i].color = Wall;
	wall[i]->GetTransform(&(c_wall[i].transform));
	c_wall[i].Render();
	}

	for (int i = 0; i < 152; i++)
	{
		if (exist[i] == true)
		{
			c_pallet[i].color = ColorBox;
			pallet[i]->GetTransform(&(c_pallet[i].transform));
			c_pallet[i].Render();
		}
	}

	p2.Render();

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		LOG("Reset Starts:");
		Reset();
		App->player->Reset();
	}

	// Title Stuff

	char title[500];

	int minutes = MAX_MINUTES - timer.Chronometer(true, 3);
	int seconds = MAX_SECONDS - timer.Chronometer(true, 2);

	if (minutes < 0)
		lostMatch = true;

	if (lostMatch && (!objectiveA || !objectiveB || !objectiveC)) // Defeat
	{
		LOG("Defeat Reset Starts:");

		sprintf_s(title, "Run out of Time!        >> D E F E A T <<                                                                                                                                                                                                       <R> to Restart");

		finishedMatch = true;

		if (countdown >= 200)
		{
			LOG("Reset Starts:");
			Reset();
			App->player->Reset();
			finishedMatch = lostMatch = false;
			countdown = 0;
		}
		countdown++;
	}

	else if (objectiveA && objectiveB && objectiveC) // Victory
	{
		finishedMatch = true;
		if (timeBeforeWin[1] > 9)
			sprintf_s(title, "Remaining Time > %i:%i         >> V I C T O R Y <<                                                                                                                                                                                               <R> to Restart",
			timeBeforeWin[2], timeBeforeWin[1]);
		else if (timeBeforeWin[1] < 9)
			sprintf_s(title, "Remaining Time > %i:0%i         >> V I C T O R Y <<                                                                                                                                                                                               <R> to Restart",
			timeBeforeWin[2], timeBeforeWin[1]);

		if (countdown >= 200)
		{
			LOG("Reset Starts:");
			Reset();
			App->player->Reset();
			finishedMatch = false;
			countdown = 0;
		}
		countdown++;
	}

	else
	{
		if (seconds > 9)
		{
			sprintf_s(title, "Objectives Completed > %i    Speed > %.1f Km/h    Time Left > %i:%i                                                                                                                           <ARROWS> to Move  <SPACE> to control Elevator  <R> to Restart",
				objetivesDone, App->player->GetSpeed() / 3, minutes, seconds);
		}
		else if (seconds <= 9)
		{
			sprintf_s(title, "Objectives Completed > %i    Speed > %.1f Km/h    Time Left > %i:0%i                                                                                                                           <ARROWS> to Move  <SPACE> to control Elevator  <R> to Restart",
				objetivesDone, App->player->GetSpeed() / 3, minutes, seconds);
		}
		timeBeforeWin[1] = seconds;
		timeBeforeWin[2] = minutes;
	}

	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == dest[0] && body2 == obj[0])
	{
		objectiveA = true;
		if (!addObjetiveA)
		{
			objetivesDone++;
			addObjetiveA = true;
			App->audio->PlayFx(correctSFX, NULL);
		}

		LOG("Objective A achieved.");
	}
	if (body1 == dest[1] && body2 == obj[1])
	{
		objectiveB = true;
		if (!addObjetiveB)
		{
			objetivesDone++;
			addObjetiveB = true;
			App->audio->PlayFx(correctSFX, NULL);
		}

		LOG("Objective B achieved.");
	}
	if (body1 == dest[2] && body2 == obj[2])
	{
		objectiveC = true;
		if (!addObjetiveC)
		{
			objetivesDone++;
			addObjetiveC = true;
			App->audio->PlayFx(correctSFX, NULL, true);
		}

		LOG("Objective C achieved.");
	}
}

bool ModuleSceneIntro::Reset()
{
	objectiveA = objectiveB = objectiveC = false;
	objetivesDone = 0;

	obj[1]->SetPos(pos_objective[1].x, pos_objective[1].y-10, pos_objective[1].z);
	obj[2]->SetPos(pos_objective[2].x, pos_objective[2].y-10, pos_objective[2].z);	
	obj[0]->SetPos(pos_objective[0].x, pos_objective[0].y-10, pos_objective[0].z);

	timer.Chronometer(false, NULL);

	if (song)
	{
		Mix_PlayMusic(bloodpressureBGM, -1);
		song = false;
	}
	else
	{
		Mix_PlayMusic(alabamaBGM, -1);
		song = true;
	}

	LOG("SceneIntro Reset Successfully!");
	return true;
}


void ModuleSceneIntro::CreateWorld()
{
	srand(time(NULL));


	c_wall[0].size.Set(1, 40, 260);
	wall[0] = App->physics->AddBody(c_wall[0], 0);
	wall[0]->SetPos(180, 20, 0);

	c_wall[1].size.Set(1, 40, 260);
	wall[1] = App->physics->AddBody(c_wall[1], 0);
	wall[1]->SetPos(-180, 20, 0);

	c_wall[2].size.Set(360, 40, 1);
	wall[2] = App->physics->AddBody(c_wall[2], 0);
	wall[2]->SetPos(0, 20, 130);

	c_wall[3].size.Set(360, 40, 1);
	wall[3] = App->physics->AddBody(c_wall[3], 0);
	wall[3]->SetPos(0, 20, -130);
	

	for (int i = 0; i < 3; i++)
	{
		c_dest[i].size.Set(9, 1, 9);
		c_obj[i].size.Set(4, 4, 4);
		goal[i] = rand() % 152;
		objective[i] = rand() % 152;
	}

	for (int i = 0; i < 152; i++)
	{
		c_pallet[i].size.Set(9, 18, 9);
		exist[i] = false;
	}

	//STAGE

	//PASILLO1
	for (int i = 0; i < 10; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-143.0f + (i * 13.0f), 12, 93.0f);
			c_obj[1].SetPos(-143.0f + (i * 13.0f), 12, 93.0f);
			pos_objective[1].Set(-143.0f + (i * 13.0f), 12, 93.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-143.0f + (i * 13.0f), 12, 93.0f);
			c_obj[2].SetPos(-143.0f + (i * 13.0f), 12, 93.0f);
			pos_objective[2].Set(-143.0f + (i * 13.0f), 12, 93.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-143.0f + (i * 13.0f), 12, 93.0f);
			c_obj[0].SetPos(-143.0f + (i * 13.0f), 12, 93.0f);
			pos_objective[0].Set(-143.0f + (i * 13.0f), 12, 93.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
			c_dest[1].SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
			c_dest[2].SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
			c_dest[0].SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-143.0f + (i * 13.0f), 1, 93.0f);
			exist[i] = true;
		}
	}

	for (int i = 10; i < 20; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
			c_obj[1].SetPos(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
			pos_objective[1].Set(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
			c_obj[2].SetPos(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
			pos_objective[2].Set(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
			c_obj[0].SetPos(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
			pos_objective[0].Set(-143.0f + ((i - 10) * 13.0f), 12, 80.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
			c_dest[1].SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
			c_dest[2].SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
			c_dest[0].SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-143.0f + ((i - 10) * 13.0f), 1, 80.0f);
			exist[i] = true;
		}
	}


	for (int i = 20; i < 30; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
			c_obj[1].SetPos(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
			pos_objective[1].Set(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
			c_obj[2].SetPos(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
			pos_objective[2].Set(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
			c_obj[0].SetPos(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
			pos_objective[0].Set(-143.0f + ((i - 20) * 13.0f), 12, -93.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
			c_dest[1].SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
			c_dest[2].SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
			c_dest[0].SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-143.0f + ((i - 20) * 13.0f), 1, -93.0f);
			exist[i] = true;
		}
	}

	for (int i = 30; i < 40; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
			c_obj[1].SetPos(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
			pos_objective[1].Set(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
			c_obj[2].SetPos(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
			pos_objective[2].Set(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
			c_obj[0].SetPos(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
			pos_objective[0].Set(-143.0f + ((i - 30) * 13.0f), 12, -80.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
			c_dest[1].SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
			c_dest[2].SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
			c_dest[0].SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-143.0f + ((i - 30) * 13.0f), 1, -80.0f);
			exist[i] = true;
		}
	}

	//PASILLO4
	for (int i = 40; i < 50; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
			c_obj[1].SetPos(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
			pos_objective[1].Set(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
			c_obj[2].SetPos(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
			pos_objective[2].Set(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
			c_obj[0].SetPos(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
			pos_objective[0].Set(143.0f - ((i - 40) * 13.0f), 12, 93.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
			c_dest[1].SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
			c_dest[2].SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
			c_dest[0].SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(143.0f - ((i - 40) * 13.0f), 1, 93.0f);
			exist[i] = true;
		}
	}

	for (int i = 50; i < 60; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
			c_obj[1].SetPos(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
			pos_objective[1].Set(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
			c_obj[2].SetPos(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
			pos_objective[2].Set(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
			c_obj[0].SetPos(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
			pos_objective[0].Set(143.0f - ((i - 50) * 13.0f), 12, 80.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
			c_dest[1].SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
			c_dest[2].SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
			c_dest[0].SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(143.0f - ((i - 50) * 13.0f), 1, 80.0f);
			exist[i] = true;
		}
	}

	for (int i = 60; i < 70; i++)	
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
			c_obj[1].SetPos(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
			pos_objective[1].Set(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
			c_obj[2].SetPos(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
			pos_objective[2].Set(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
			c_obj[0].SetPos(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
			pos_objective[0].Set(143.0f - ((i - 60) * 13.0f), 12, -93.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
			c_dest[1].SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
			c_dest[2].SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
			c_dest[0].SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(143.0f - ((i - 60) * 13.0f), 1, -93.0f);
			exist[i] = true;
		}
	}

	for (int i = 70; i < 80.0f; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
			c_obj[1].SetPos(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
			pos_objective[1].Set(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
			c_obj[2].SetPos(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
			pos_objective[2].Set(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
			c_obj[0].SetPos(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
			pos_objective[0].Set(143.0f - ((i - 70) * 13.0f), 12, -80.0f);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
			c_dest[1].SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
			c_dest[2].SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
			c_dest[0].SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(143.0f - ((i - 70) * 13.0f), 1, -80.0f);
			exist[i] = true;
		}
	}

	//PASILLO2

	for (int i = 80.0f; i < 93.0f; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
			c_obj[1].SetPos(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
			pos_objective[1].Set(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
			c_obj[2].SetPos(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
			pos_objective[2].Set(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
			c_obj[0].SetPos(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
			pos_objective[0].Set(-143.0f + ((i - 80.0f) * 13.0f), 12, -39);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
			c_dest[1].SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
			c_dest[2].SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
			c_dest[0].SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-143.0f + ((i - 80.0f) * 13.0f), 1, -39);
			exist[i] = true;
		}
	}

	for (int i = 93.0f; i < 106; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
			c_obj[1].SetPos(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
			pos_objective[1].Set(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
			c_obj[2].SetPos(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
			pos_objective[2].Set(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
			c_obj[0].SetPos(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
			pos_objective[0].Set(-143.0f + ((i - 93.0f) * 13.0f), 12, -26);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
			c_dest[1].SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
			c_dest[2].SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
			c_dest[0].SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-143.0f + ((i - 93.0f) * 13.0f), 1, -26);
			exist[i] = true;
		}
	}

	for (int i = 106; i < 111; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(91 + ((i - 106) * 13.0f), 12, -39);
			c_obj[1].SetPos(91 + ((i - 106) * 13.0f), 12, -39);
			pos_objective[1].Set(91 + ((i - 106) * 13.0f), 12, -39);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(91 + ((i - 106) * 13.0f), 12, -39);
			c_obj[0].SetPos(91 + ((i - 106) * 13.0f), 12, -39);
			pos_objective[2].Set(91 + ((i - 106) * 13.0f), 12, -39);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(91 + ((i - 106) * 13.0f), 12, -39);
			c_obj[0].SetPos(91 + ((i - 106) * 13.0f), 12, -39);
			pos_objective[0].Set(91 + ((i - 106) * 13.0f), 12, -39);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(91 + ((i - 106) * 13.0f), 1, -39);
			c_dest[1].SetPos(91 + ((i - 106) * 13.0f), 1, -39);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(91 + ((i - 106) * 13.0f), 1, -39);
			c_dest[2].SetPos(91 + ((i - 106) * 13.0f), 1, -39);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(91 + ((i - 106) * 13.0f), 1, -39);
			c_dest[0].SetPos(91 + ((i - 106) * 13.0f), 1, -39);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(91 + ((i - 106) * 13.0f), 1, -39);
			exist[i] = true;
		}
	}

	for (int i = 111; i < 116; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(91 + ((i - 111) * 13.0f), 12, -26);
			c_obj[1].SetPos(91 + ((i - 111) * 13.0f), 12, -26);
			pos_objective[1].Set(91 + ((i - 111) * 13.0f), 12, -26);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(91 + ((i - 111) * 13.0f), 12, -26);
			c_obj[2].SetPos(91 + ((i - 111) * 13.0f), 12, -26);
			pos_objective[2].Set(91 + ((i - 111) * 13.0f), 12, -26);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(91 + ((i - 111) * 13.0f), 12, -26);
			c_obj[0].SetPos(91 + ((i - 111) * 13.0f), 12, -26);
			pos_objective[0].Set(91 + ((i - 111) * 13.0f), 12, -26);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(91 + ((i - 111) * 13.0f), 1, -26);
			c_dest[1].SetPos(91 + ((i - 111) * 13.0f), 1, -26);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(91 + ((i - 111) * 13.0f), 1, -26);
			c_dest[2].SetPos(91 + ((i - 111) * 13.0f), 1, -26);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(91 + ((i - 111) * 13.0f), 1, -26);
			c_dest[0].SetPos(91 + ((i - 111) * 13.0f), 1, -26);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(91 + ((i - 111) * 13.0f), 1, -26);
			exist[i] = true;
		}
	}

	//PASILLO 3

	for (int i = 116; i < 129; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(143.0f - ((i - 116) * 13.0f), 12, 39);
			c_obj[1].SetPos(143.0f - ((i - 116) * 13.0f), 12, 39);
			pos_objective[1].Set(143.0f - ((i - 116) * 13.0f), 12, 39);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(143.0f - ((i - 116) * 13.0f), 12, 39);
			c_obj[2].SetPos(143.0f - ((i - 116) * 13.0f), 12, 39);
			pos_objective[2].Set(143.0f - ((i - 116) * 13.0f), 12, 39);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(143.0f - ((i - 116) * 13.0f), 12, 39);
			c_obj[0].SetPos(143.0f - ((i - 116) * 13.0f), 12, 39);
			pos_objective[0].Set(143.0f - ((i - 116) * 13.0f), 12, 39);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
			c_dest[1].SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
			c_dest[2].SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
			c_dest[0].SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(143.0f - ((i - 116) * 13.0f), 1, 39);
			exist[i] = true;
		}
	}

	for (int i = 129; i < 142; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(143.0f - ((i - 129) * 13.0f), 12, 26);
			c_obj[1].SetPos(143.0f - ((i - 129) * 13.0f), 12, 26);
			pos_objective[1].Set(143.0f - ((i - 129) * 13.0f), 12, 26);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(143.0f - ((i - 129) * 13.0f), 12, 26);
			c_obj[2].SetPos(143.0f - ((i - 129) * 13.0f), 12, 26);
			pos_objective[2].Set(143.0f - ((i - 129) * 13.0f), 12, 26);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(143.0f - ((i - 129) * 13.0f), 12, 26);
			c_obj[0].SetPos(143.0f - ((i - 129) * 13.0f), 12, 26);
			pos_objective[0].Set(143.0f - ((i - 129) * 13.0f), 12, 26);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
			c_dest[1].SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
			c_dest[2].SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
			c_dest[0].SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(143.0f - ((i - 129) * 13.0f), 1, 26);
			exist[i] = true;
		}
	}

	for (int i = 142; i < 147; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-91 - ((i - 142) * 13.0f), 12, 39);
			c_obj[1].SetPos(-91 - ((i - 142) * 13.0f), 12, 39);
			pos_objective[1].Set(-91 - ((i - 142) * 13.0f), 12, 39);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-91 - ((i - 142) * 13.0f), 12, 39);
			c_obj[2].SetPos(-91 - ((i - 142) * 13.0f), 12, 39);
			pos_objective[2].Set(-91 - ((i - 142) * 13.0f), 12, 39);
		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-91 - ((i - 142) * 13.0f), 12, 39);
			c_obj[0].SetPos(-91 - ((i - 142) * 13.0f), 12, 39);
			pos_objective[0].Set(-91 - ((i - 142) * 13.0f), 12, 39);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-91 - ((i - 142) * 13.0f), 1, 39);
			c_dest[1].SetPos(-91 - ((i - 142) * 13.0f), 1, 39);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-91 - ((i - 142) * 13.0f), 1, 39);
			c_dest[2].SetPos(-91 - ((i - 142) * 13.0f), 1, 39);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-91 - ((i - 142) * 13.0f), 1, 39);
			c_dest[0].SetPos(-91 - ((i - 142) * 13.0f), 1, 39);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-91 - ((i - 142) * 13.0f), 1, 39);

			exist[i] = true;
		}
	}

	for (int i = 147; i < 152; i++)
	{
		if (i == objective[1])
		{
			obj[1] = App->physics->AddPallet(c_obj[1]);
			obj[1]->SetPos(-91 - ((i - 147) * 13.0f), 12, 26);
			c_obj[1].SetPos(-91 - ((i - 147) * 13.0f), 12, 26);
			pos_objective[1].Set(-91 - ((i - 147) * 13.0f), 12, 26);
		}
		else if (i == objective[2])
		{
			obj[2] = App->physics->AddPallet(c_obj[2]);
			obj[2]->SetPos(-91 - ((i - 147) * 13.0f), 12, 26);
			c_obj[2].SetPos(-91 - ((i - 147) * 13.0f), 12, 26);
			pos_objective[2].Set(-91 - ((i - 147) * 13.0f), 12, 26);

		}
		else if (i == objective[0])
		{
			obj[0] = App->physics->AddPallet(c_obj[0]);
			obj[0]->SetPos(-91 - ((i - 147) * 13.0f), 12, 26);
			c_obj[0].SetPos(-91 - ((i - 147) * 13.0f), 12, 26);
			pos_objective[0].Set(-91 - ((i - 147) * 13.0f), 12, 26);
		}
		else if (i == goal[1])
		{
			dest[1] = App->physics->AddBody(c_dest[1], 0);
			dest[1]->SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
			c_dest[1].SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
		}
		else if (i == goal[2])
		{
			dest[2] = App->physics->AddBody(c_dest[2], 0);
			dest[2]->SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
			c_dest[2].SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
		}
		else if (i == goal[0])
		{
			dest[0] = App->physics->AddBody(c_dest[0], 0);
			dest[0]->SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
			c_dest[0].SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
		}
		else
		{
			pallet[i] = App->physics->AddBody(c_pallet[i], 100);
			pallet[i]->SetPos(-91 - ((i - 147) * 13.0f), 1, 26);
			exist[i] = true;
		}
	}

	obj[0]->body->setFriction(10);
	dest[0]->SetAsSensor(true);
	dest[0]->collision_listeners.add(this);

	obj[1]->body->setFriction(10);
	dest[1]->SetAsSensor(true);
	dest[1]->collision_listeners.add(this);

	obj[2]->body->setFriction(10);
	dest[2]->SetAsSensor(true);
	dest[2]->collision_listeners.add(this);

}