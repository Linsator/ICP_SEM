#include <glm/glm.hpp>
#include "globals.h"

// global variable - encapsulates all global variables
s_globals globals;

Avatar avatarMoveForward(Avatar& avatar)
{
	avatar.position = glm::vec3(avatar.position.x + (avatar.lookAt.x * avatar.movement_speed), avatar.position.y, avatar.position.z + (avatar.lookAt.z * avatar.movement_speed));
	avatar.position.y = getHeightAt(avatar.position.x, avatar.position.z) + avatar.height;
	return avatar;
}
Avatar avatarMoveBackward(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x - avatar.lookAt.x * avatar.movement_speed, avatar.position.y, avatar.position.z - avatar.lookAt.z * avatar.movement_speed);
	avatar.position.y = getHeightAt(avatar.position.x, avatar.position.z) + avatar.height;
	return avatar;
}
Avatar avatarMoveLeft(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x + avatar.lookAt.z * avatar.movement_speed, avatar.position.y, avatar.position.z - avatar.lookAt.x * avatar.movement_speed);
	avatar.position.y = getHeightAt(avatar.position.x, avatar.position.z) + avatar.height;
	return avatar;
}
Avatar avatarMoveRight(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x - avatar.lookAt.z * avatar.movement_speed, avatar.position.y, avatar.position.z + avatar.lookAt.x * avatar.movement_speed);
	avatar.position.y = getHeightAt(avatar.position.x, avatar.position.z) + avatar.height;
	return avatar;
}
Avatar avatarMoveUp(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x, avatar.position.y + avatar.movement_speed, avatar.position.z);
	return avatar;
}
Avatar avatarMoveDown(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x, avatar.position.y - avatar.movement_speed, avatar.position.z);
	return avatar;
}


void targetAdd() {
	Target* newTarget = new Target();
	newTarget->speed = 2;
	newTarget->radius = 10;
	newTarget->scale = glm::vec3(10.0f);
	newTarget->position = RandomPos(20,400, -150, 150);
	
	if (newTarget->position.y == -1.0f) {
		delete newTarget;
		return;
	}
	
	newTarget->position.y = getHeightAt(newTarget->position.x, newTarget->position.z);
	newTarget->direction = glm::normalize(globals.avatar->lookAt);
	globals.targets.push_back(newTarget);
}


void targetDestroy(Target* target, int at) {
	globals.targets.erase(globals.targets.begin() + at);
	delete target;
}

void arrowShoot() {
	Arrow *newArrow = new Arrow();
	float speed = 50;
	newArrow->canMove = true;
	newArrow->lifeTime = 10; // in seconds
	newArrow->position = globals.avatar->position + 5.0f * globals.avatar->lookAt;
	newArrow->direction = speed * normalize(globals.avatar->lookAt);
	newArrow->scale = glm::vec3(2.0f, 2.0f, 1.0f);
	newArrow->bBox_shift = glm::vec3(0.0, -0.25, 2.75);
	newArrow->bBox_scale = glm::vec3(0.5f);
	globals.arrows.push_back(newArrow);
}

void arrowDestroy(Arrow* arrow, int at) {
	globals.arrows.erase(globals.arrows.begin() + at);
	delete arrow;
}

void transparentAdd() {
	Transparent* newTransparent = new Transparent();
	newTransparent->life = 3;
	newTransparent->scale = glm::vec3(10.0f);
	newTransparent->position = glm::vec3(8.0f, 2.0f, 0);
	globals.transparents.push_back(newTransparent);
}

void transparentDestroy(Transparent* transparent, int at) {
	globals.transparents.erase(globals.transparents.begin() + at);
	delete transparent;
}

void particleAdd(Arrow* arrow) {
	Particle* newParticle = new Particle();
	newParticle->lifeTime = 0.5;
	newParticle->scale = glm::vec3(0.25f);
	newParticle->position = arrow->position + arrow->bBox_shift;
	newParticle->speed = glm::vec3(0.0f);
	globals.particles.push_back(newParticle);
}

void particleDestroy(Particle* particle, int at) {
	globals.particles.erase(globals.particles.begin() + at);
	delete particle;
}

uchar getHeightAt(float x, float z) {
	int offset = globals.heightMap.cols / 2;
	return globals.heightMap.at<uchar>(cv::Point(round(x) + offset, round(z) + offset));
}


glm::vec3 RandomPos(int xLow, int xHigh, int zLow, int zHigh, int depth) {
	if (depth > 100)
	{
		return  glm::vec3(0.0f, -1.0f, 0.0f);
	}
	std::srand(std::time(nullptr)); // use current time as seed for random generator
	int xrand = std::rand();
	int zrand = std::rand();
	glm::vec3 pos = glm::vec3((float)(xLow + xrand % (xHigh - xLow)), 0.0f, (float)(zLow + zrand % (zHigh - zLow)));

	//check if threre is already target
	for (int i = 0; i < globals.targets.size(); i++)
	{
		Target* tar = globals.targets[i];
		if (glm::length(tar->position - pos) < tar->scale.x)
		{
			//try again
			pos = RandomPos(xLow, xHigh, zLow, zHigh, depth + 1);
		}
	}
	return pos;
}