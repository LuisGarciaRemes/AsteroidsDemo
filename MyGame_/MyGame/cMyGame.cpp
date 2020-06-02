// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include "Engine/Graphics/GraphicsAssistant.h"
#include "Engine/Graphics/cCamera.h"
#include <Engine/UserInput/UserInput.h>
#include <Engine/3DAudio/3DAudio.h>

// Inherited Implementation
//=========================

// Run
//----
const float SCREENHALFWIDHT = 9.4f;
const float SCREENHALFHEIGHT = 9.4f;
const int MAXNUMASTEROIDS = 10;
const int MAXNUMPROJECTILES = 5;
const int MAXASTEROIDSPEED = 7;
const int MINASTEROIDSPEED = 2;
const int MAXPOWERUPSPEED = 3;
const int MINPOWERUPSPEED = 1;
const int PROJECTILESPEED = 10;
const float PLAYERSPEED = 2.0f;
int gameobjectIndex = 0;

eae6320::MyGame::cGameObject* gameObjectPool[NUMOBJECTS];
eae6320::Graphics::cEffect* white;
eae6320::Graphics::cEffect* shifting;
eae6320::Graphics::cEffect* yellow;
eae6320::Graphics::cEffect* brown;
eae6320::Graphics::cCamera* camera;
eae6320::Graphics::cGeometry::Handle squareGeom;
eae6320::Graphics::cGeometry::Handle halfCircleGeom;
eae6320::Graphics::cGeometry::Handle quarterCircleGeom;
eae6320::MyGame::cGameObject* player;
eae6320::MyGame::cGameObject* powerUp;
eae6320::MyGame::cGameObject* asteroidToRender = nullptr;
eae6320::Collision::Collider* playerCol;
eae6320::Collision::Collider* powerUpCol;
eae6320::Audio3D::AudioSource* bGSource;
std::vector<eae6320::Audio3D::AudioSource*> sourceList;


bool renderProjectile = false;
bool renderAsteroid = false;
bool renderPowerUp = false;
float fireRate = 0.10f;
float fireTimer = 0.0f;
bool canFire = true;
int score = 0;
bool gameOver = true;
float startTimer = 0.0f;
float startDelay = 1.0f;
bool restart = true;
float asteroidTimer = 1.0f;
float asteroidDelay = 1.0f;
bool exitCheck = false;
bool invincible = false;
float invincTimer = 0.0f;
float invincDelay = 5.0f;


void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	float x_object = 0.0f;
	float y_object = 0.0f;
	float z_object = 0.0f;

	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) && !exitCheck )
	{
		// Exit the application
		std::string temp = "Thank you for playing!!!";
		eae6320::Logging::OutputMessage(temp.c_str());
		exitCheck = true;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A) || UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		x_object += -PLAYERSPEED;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D) || UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		x_object += PLAYERSPEED;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W) || UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		y_object += PLAYERSPEED;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S) || UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		y_object += -PLAYERSPEED;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space) && !player->IsEnabled())
	{
		RestartGame();
	}

	//Shoot projectile
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space) && canFire && player->IsEnabled())
	{
		eae6320::MyGame::cGameObject* projectile = nullptr;
		float x_projectile = 0.0f;
		float y_projectile = 0.0f;
		float z_projectile = 0.0f;

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::A) || UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		{
			x_projectile = -PROJECTILESPEED;
		}

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::D) || UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		{
			x_projectile = PROJECTILESPEED;
		}

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::W) || UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		{
			y_projectile = PROJECTILESPEED;
		}

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::S) || UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		{
			y_projectile = -PROJECTILESPEED;
		}	

		projectile = ReturnProjectile();

		if (projectile != nullptr && (y_projectile != 0.0f || x_projectile != 0.0f))
		{
			projectile->GetRigidBody()->position.x = player->GetRigidBody()->position.x;
			projectile->GetRigidBody()->position.y = player->GetRigidBody()->position.y;
			projectile->SetVelocity(eae6320::Math::sVector(x_projectile, y_projectile, z_projectile));
			renderProjectile = true;
			PlayShot();
			canFire = false;
		}
	}

	player->SetAcceleration(eae6320::Math::sVector(x_object,y_object,z_object));
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	if (exitCheck)
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}

	std::vector<eae6320::Collision::Collider*> tempList;

	camera->Update(i_elapsedSecondCount_sinceLastUpdate);

	//Check if projectile was fired
	if (renderProjectile)
	{
		ReturnProjectile()->SetEnabled(true);
		renderProjectile = false;
	}

	//Check if projectile was fired
	if (renderAsteroid && !gameOver && asteroidToRender != nullptr)
	{
		asteroidToRender->SetEnabled(true);
		renderAsteroid = false;
		asteroidToRender = nullptr;
	}

	//Check if projectile was fired
	if (renderPowerUp && !gameOver)
	{
		powerUp->SetEnabled(true);
		renderPowerUp = false;
	}

	//Update gameobject things
	for (int i = 0; i < NUMOBJECTS; i++)
	{
		if (gameObjectPool[i] != nullptr)
		{
			//Update position of gameobjects
			gameObjectPool[i]->Update(i_elapsedSecondCount_sinceLastUpdate);

			//Check boundary and teleport gameobject to other side of screen
			CheckBoundaries(gameObjectPool[i]);

			if (gameObjectPool[i]->IsEnabled())
			{
				tempList.push_back(gameObjectPool[i]->GetCollider());
			}
		}
	}

	std::vector<eae6320::CollisionSystem::CollisionPair> pairList = eae6320::CollisionSystem::Run(tempList);

	for (eae6320::CollisionSystem::CollisionPair  pair : pairList)
	{
		RespondToCollisions(pair);
	}

	if (!canFire)
	{
		if (fireTimer >= fireRate)
		{
			fireTimer = 0.0f;
			canFire = true;
		}
		else
		{
			fireTimer += i_elapsedSecondCount_sinceLastUpdate;
		}
	}

	if (restart)
	{
		if (startTimer >= startDelay)
		{
			startTimer = 0.0f;
			restart = false;
			gameOver = false;
			PlayStart();
		}
		else
		{
			startTimer += i_elapsedSecondCount_sinceLastUpdate;
		}
	}

	if (!gameOver)
	{
		if (asteroidTimer >= asteroidDelay)
		{
			asteroidTimer = 0.0f;
			SpawnAsteroid();
		}
		else
		{
			asteroidTimer += i_elapsedSecondCount_sinceLastUpdate;
		}
	}

	if (invincible)
	{
		if (invincTimer >= invincDelay)
		{
			invincTimer = 0.0f;
			invincible = false;
			player->SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry::g_manager.Get(halfCircleGeom), white, playerCol);
			PlayPowerDown();
		}
		else
		{
			invincTimer += i_elapsedSecondCount_sinceLastUpdate;
		}
	}

	if (!powerUp->IsEnabled() && !gameOver && !invincible)
	{
		SpawnPowerUp();
	}
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SubmitElapsedTime(i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	eae6320::Graphics::SubmitBackBufferColor(0.0f, 0.0f, 0.0f, 1.0f);
	eae6320::Graphics::SubmitCamera(camera);
	eae6320::Graphics::SubmitGameObjectsToRender(gameObjectPool);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Audio3D::AudioSource::Load("data/audio/bgmusic.wav", bGSource);

	eae6320::Graphics::cCamera::CreateCamera(camera);
	camera->GetRigidBody()->position.z = 20;
	eae6320::Graphics::cEffect::CreateEffect("data/Effects/customeffect.bin", shifting);
	eae6320::Graphics::cEffect::CreateEffect("data/Effects/standardeffect.bin", white);
	eae6320::Graphics::cEffect::CreateEffect("data/Effects/yelloweffect.bin", yellow);
	eae6320::Graphics::cEffect::CreateEffect("data/Effects/browneffect.bin", brown);

	eae6320::Graphics::cGeometry::CreateGeometry("data/Geometries/HalfRCircle.bin", halfCircleGeom);
	eae6320::Graphics::cGeometry::CreateGeometry("data/Geometries/QuarterRCircle.bin", quarterCircleGeom);
	eae6320::Graphics::cGeometry::CreateGeometry("data/Geometries/2x2Square.bin", squareGeom);

	eae6320::MyGame::cGameObject::CreateGameObject(player);
	eae6320::Collision::Collider::CreateCollider("data/Colliders/HalfRCircle.bin",playerCol);
	player->SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry::g_manager.Get(halfCircleGeom),white, playerCol);
	player->tag = "Player";

	eae6320::MyGame::cGameObject::CreateGameObject(powerUp);
	eae6320::Collision::Collider::CreateCollider("data/Colliders/HalfRCircle.bin", powerUpCol);
	powerUp->SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry::g_manager.Get(halfCircleGeom), shifting, powerUpCol);
	powerUp->tag = "PowerUp";
	powerUp->SetEnabled(false);

	gameObjectPool[0] = player;
	gameObjectPool[1] = powerUp;

	gameobjectIndex = 2;

	for (int i = 0; i < MAXNUMPROJECTILES; i++)
	{
		eae6320::MyGame::cGameObject::CreateGameObject(gameObjectPool[gameobjectIndex + i]);
		eae6320::Collision::Collider* tempCol;
		eae6320::Collision::Collider::CreateCollider("data/Colliders/QuarterRCircle.bin", tempCol);
		gameObjectPool[gameobjectIndex + i]->SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry::g_manager.Get(quarterCircleGeom), yellow, tempCol);
		gameObjectPool[gameobjectIndex + i]->tag = "Projectile";
		gameObjectPool[gameobjectIndex + i]->SetEnabled(false);
	}

	gameobjectIndex += MAXNUMPROJECTILES;

	for (int i = 0; i < MAXNUMASTEROIDS; i++)
	{
		eae6320::MyGame::cGameObject::CreateGameObject(gameObjectPool[i + gameobjectIndex]);
		eae6320::Collision::Collider* tempCol;
		eae6320::Collision::Collider::CreateCollider("data/Colliders/2x2Square.bin", tempCol);
		gameObjectPool[i + gameobjectIndex]->SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry::g_manager.Get(squareGeom), brown, tempCol);
		gameObjectPool[i + gameobjectIndex]->tag = "Asteroid";
		gameObjectPool[i + gameobjectIndex]->SetEnabled(false);
	}

	gameobjectIndex += MAXNUMASTEROIDS;

	PlayBGMusic();

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	if (eae6320::Graphics::cGeometry::g_manager.Get(squareGeom))
	{
		eae6320::Graphics::cGeometry::g_manager.Get(squareGeom)->DecrementReferenceCount();
	}

	if (eae6320::Graphics::cGeometry::g_manager.Get(halfCircleGeom))
	{
		eae6320::Graphics::cGeometry::g_manager.Get(halfCircleGeom)->DecrementReferenceCount();
	}

	if (eae6320::Graphics::cGeometry::g_manager.Get(quarterCircleGeom))
	{
		eae6320::Graphics::cGeometry::g_manager.Get(quarterCircleGeom)->DecrementReferenceCount();
	}


	for (eae6320::Audio3D::AudioSource* source : sourceList)
	{
		source->CleanUp();
	}

	eae6320::Graphics::cGeometry::g_manager.Release(squareGeom);
	eae6320::Graphics::cGeometry::g_manager.Release(halfCircleGeom);
	eae6320::Graphics::cGeometry::g_manager.Release(quarterCircleGeom);

	white->DecrementReferenceCount();
	shifting->DecrementReferenceCount();
	yellow->DecrementReferenceCount();
	brown->DecrementReferenceCount();

	return Results::Success;
}

void eae6320::cMyGame::CheckBoundaries(eae6320::MyGame::cGameObject* object)
{
	bool destroy = false;

	float offset;

	if (object->tag._Equal("Player") || object->tag._Equal("PowerUp"))
	{
		offset = 0.5f;
	}
	else
	{
		offset = 0.0f;
	}

	if (object->GetRigidBody()->position.x < -SCREENHALFWIDHT + offset)
	{
		object->GetRigidBody()->position.x = SCREENHALFWIDHT - offset;
		destroy = true;
	}
	else if (object->GetRigidBody()->position.x > SCREENHALFWIDHT - offset)
	{
		object->GetRigidBody()->position.x = -SCREENHALFWIDHT + offset;
		destroy = true;
	}

	if (object->GetRigidBody()->position.y < -SCREENHALFHEIGHT + offset)
	{
		object->GetRigidBody()->position.y = SCREENHALFHEIGHT - offset;
		destroy = true;
	}
	else if (object->GetRigidBody()->position.y > SCREENHALFHEIGHT - offset)
	{
		object->GetRigidBody()->position.y = -SCREENHALFHEIGHT + offset;
		destroy = true;
	}

	if (object->tag._Equal("Projectile") && destroy)
	{
		DestroyGameObject(object);
	}
}

void eae6320::cMyGame::SpawnAsteroid()
{
for (eae6320::MyGame::cGameObject* object : gameObjectPool)
		{
			if (object != nullptr && object->tag._Equal("Asteroid") && !object->IsEnabled())
			{
				int direction = 0;

				switch (rand() % 1)
				{
				case 0:
					direction = -1;
					break;
				case 1:
					direction = 1;
					break;
				default:
					break;
				}

				switch (rand()%4)
				{
				case 0:
					object->GetRigidBody()->position.x = (float)(rand() % ((int)SCREENHALFWIDHT * 2) - (int)SCREENHALFWIDHT);
					object->GetRigidBody()->position.y = -SCREENHALFHEIGHT;
					object->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) + MINASTEROIDSPEED) * (direction), (float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) + MINASTEROIDSPEED), 0.0f));
					break;
				case 1:
					object->GetRigidBody()->position.x = (float)(rand() % ((int)SCREENHALFWIDHT * 2) - (int)SCREENHALFWIDHT);
					object->GetRigidBody()->position.y = SCREENHALFHEIGHT;
					object->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) + MINASTEROIDSPEED) * (direction), (float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) - MAXASTEROIDSPEED), 0.0f));
					break;
				case 2:
					object->GetRigidBody()->position.x = SCREENHALFWIDHT;
					object->GetRigidBody()->position.y = (float)(rand() % ((int)SCREENHALFHEIGHT * 2) - (int)SCREENHALFHEIGHT);
					object->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) + MINASTEROIDSPEED), (float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) + MINASTEROIDSPEED) * (direction), 0.0f));
					break;
				case 3:
					object->GetRigidBody()->position.x = -SCREENHALFWIDHT;
					object->GetRigidBody()->position.y = (float)(rand() % ((int)SCREENHALFHEIGHT * 2) - (int)SCREENHALFHEIGHT);
					object->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) - MAXASTEROIDSPEED), (float)(rand() % (MAXASTEROIDSPEED - MINASTEROIDSPEED) + MINASTEROIDSPEED) * (direction), 0.0f));
					break;
				default:
					break;
				}
				renderAsteroid = true;
				asteroidToRender = object;
				return;
			}
		}
}

void eae6320::cMyGame::SpawnPowerUp()
{
	if ((rand() % 150) == 100)
	{
		renderPowerUp = true;
		int direction = 0;

		switch (rand() % 1)
		{
		case 0:
			direction = -1;
			break;
		case 1:
			direction = 1;
			break;
		default:
			break;
		}

		switch (rand() % 4)
		{
		case 0:
			powerUp->GetRigidBody()->position.x = (float)(rand() % ((int)SCREENHALFWIDHT * 2) - (int)SCREENHALFWIDHT);
			powerUp->GetRigidBody()->position.y = -SCREENHALFHEIGHT;
			powerUp->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) + MINPOWERUPSPEED) * (direction), (float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) + MINPOWERUPSPEED), 0.0f));
			break;
		case 1:
			powerUp->GetRigidBody()->position.x = (float)(rand() % ((int)SCREENHALFWIDHT * 2) - (int)SCREENHALFWIDHT);
			powerUp->GetRigidBody()->position.y = SCREENHALFHEIGHT;
			powerUp->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) + MINPOWERUPSPEED) * (direction), (float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) - MAXPOWERUPSPEED), 0.0f));
			break;
		case 2:
			powerUp->GetRigidBody()->position.x = SCREENHALFWIDHT;
			powerUp->GetRigidBody()->position.y = (float)(rand() % ((int)SCREENHALFHEIGHT * 2) - (int)SCREENHALFHEIGHT);
			powerUp->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) + MINPOWERUPSPEED), (float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) + MINPOWERUPSPEED) * (direction), 0.0f));
			break;
		case 3:
			powerUp->GetRigidBody()->position.x = -SCREENHALFWIDHT;
			powerUp->GetRigidBody()->position.y = (float)(rand() % ((int)SCREENHALFHEIGHT * 2) - (int)SCREENHALFHEIGHT);
			powerUp->SetVelocity(eae6320::Math::sVector((float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) - MAXPOWERUPSPEED), (float)(rand() % (MAXPOWERUPSPEED - MINPOWERUPSPEED) + MINPOWERUPSPEED) * (direction), 0.0f));
			break;
		default:
			break;
		}
	}
}

void eae6320::cMyGame::RespondToCollisions(eae6320::CollisionSystem::CollisionPair i_pair)
{
	eae6320::MyGame::cGameObject* objA = FindObjectCollider(i_pair.idA);
	eae6320::MyGame::cGameObject* objB = FindObjectCollider(i_pair.idB);

	if (objA->tag._Equal("Projectile") && objB->tag._Equal("Asteroid"))
	{
		score += 1;
		DestroyGameObject(objA);
		DestroyGameObject(objB);
		PlayExplosion();
	}
	else if (objA->tag._Equal("Player") && objB->tag._Equal("Asteroid") && !invincible)
	{
		DestroyGameObject(objA);
		DestroyGameObject(objB);
		PlayGameOver();
		gameOver = true;
		std::string temp = "Your score was: " + std::to_string(score);
		eae6320::Logging::OutputMessage(temp.c_str());
	}
	else if (objA->tag._Equal("Player") && objB->tag._Equal("Asteroid") && invincible)
	{
		DestroyGameObject(objB);
		PlayExplosion();
		score += 1;
	}
	else if (objA->tag._Equal("Player") && objB->tag._Equal("PowerUp"))
	{
		DestroyGameObject(objB);
		PlayPowerUp();
		player->SetGeometryAndEffectAndCollider(eae6320::Graphics::cGeometry::g_manager.Get(halfCircleGeom), shifting, playerCol);
		invincible = true;
	}

}

eae6320::MyGame::cGameObject* eae6320::cMyGame::ReturnProjectile()
{
	for (eae6320::MyGame::cGameObject* object : gameObjectPool)
	{
		if (object != nullptr && object->tag._Equal("Projectile") && !object->IsEnabled())
		{
			return object;
		}
	}
	return nullptr;
}

void eae6320::cMyGame::DestroyGameObject(eae6320::MyGame::cGameObject* object)
{
	object->SetEnabled(false);
	object->ResetRigidBody();
}

void eae6320::cMyGame::PlayExplosion()
{
	eae6320::Audio3D::AudioSource* source;
	eae6320::Audio3D::AudioSource::Load("data/audio/explosion.wav", source);
	source->PlayOnce();
	sourceList.push_back(source);
}

void eae6320::cMyGame::PlayBGMusic()
{
	bGSource->PlayLooped();
	sourceList.push_back(bGSource);
}

void eae6320::cMyGame::PlayPowerUp()
{
	eae6320::Audio3D::AudioSource* source;
	eae6320::Audio3D::AudioSource::Load("data/audio/powerup.wav", source);
	source->PlayOnce();
	sourceList.push_back(source);
}

void eae6320::cMyGame::PlayPowerDown()
{
	eae6320::Audio3D::AudioSource* source;
	eae6320::Audio3D::AudioSource::Load("data/audio/powerdown.wav", source);
	source->PlayOnce();
	sourceList.push_back(source);
}

void eae6320::cMyGame::PlayGameOver()
{
	eae6320::Audio3D::AudioSource* source;
	eae6320::Audio3D::AudioSource::Load("data/audio/gameover.wav", source);
	source->PlayOnce();
	sourceList.push_back(source);
}

void eae6320::cMyGame::PlayStart()
{
	eae6320::Audio3D::AudioSource* source;
	eae6320::Audio3D::AudioSource::Load("data/audio/start.wav", source);
	source->PlayOnce();
	sourceList.push_back(source);
}

void eae6320::cMyGame::PlayShot()
{
	eae6320::Audio3D::AudioSource* source;
	eae6320::Audio3D::AudioSource::Load("data/audio/shoot.wav", source);
	source->PlayOnce();
	sourceList.push_back(source);
}

void eae6320::cMyGame::RestartGame()
{
	for (eae6320::MyGame::cGameObject* object : gameObjectPool)
	{
		if (object != nullptr)
		{
			DestroyGameObject(object);
		}
	}

	restart = true;
	score = 0;
	player->GetRigidBody()->position.x = 0.0f;
	player->GetRigidBody()->position.y = 0.0f;
	player->SetEnabled(true);
}

eae6320::MyGame::cGameObject* eae6320::cMyGame::FindObjectCollider(uint16_t i_ID)
{
	for (eae6320::MyGame::cGameObject* object : gameObjectPool)
	{
		if (object != nullptr && object->GetCollider()->GetID() == i_ID)
		{
			return object;
		}
	}
	return nullptr;
}
