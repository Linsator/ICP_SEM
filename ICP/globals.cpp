#include <glm/glm.hpp>
#include "globals.h"

// global variable - encapsulates all global variables
s_globals globals;

Avatar avatarMoveForward(Avatar& avatar)
{
	avatar.position = glm::vec3(avatar.position.x + (avatar.lookAt.x * avatar.movement_speed), avatar.position.y, avatar.position.z + (avatar.lookAt.z * avatar.movement_speed));
	return avatar;
}
Avatar avatarMoveBackward(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x - avatar.lookAt.x * avatar.movement_speed, avatar.position.y, avatar.position.z - avatar.lookAt.z * avatar.movement_speed);
	return avatar;
}
Avatar avatarMoveLeft(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x + avatar.lookAt.z * avatar.movement_speed, avatar.position.y, avatar.position.z - avatar.lookAt.x * avatar.movement_speed);
	return avatar;
}
Avatar avatarMoveRight(Avatar& avatar) {
	avatar.position = glm::vec3(avatar.position.x - avatar.lookAt.z * avatar.movement_speed, avatar.position.y, avatar.position.z + avatar.lookAt.x * avatar.movement_speed);
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
	newTarget->position = glm::vec3(0.0);
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
	newParticle->lifeTime = 1;
	newParticle->scale = glm::vec3(0.2f);
	newParticle->position = arrow->position + arrow->bBox_shift;
	newParticle->speed = glm::vec3(10.0f);
	globals.particles.push_back(newParticle);
}

uchar getHeightAt(float x, float z) {
	int offset = globals.heightMap.cols / 2;
	return globals.heightMap.at<uchar>(cv::Point(round(x) + offset, round(z) + offset));
}