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

void arrowShoot() {
	Arrow *newArrow = new Arrow();
	float speed = 50;
	newArrow->lifeTime = 10; // in seconds
	newArrow->position = globals.avatar->position + 5.0f * globals.avatar->lookAt;
	newArrow->direction = speed * normalize(globals.avatar->lookAt);
	globals.arrows.push_back(newArrow);
}

void arrowDestroy(Arrow* arrow, int at) {
	globals.arrows.erase(globals.arrows.begin() + at);
	delete arrow;
}

void targetAdd() {
	Target* newTarget = new Target();
	newTarget->speed = 2;
	newTarget->radius = 10;
	newTarget->position = glm::vec3(0.0);
	newTarget->direction = glm::normalize(globals.avatar->lookAt);
	globals.targets.push_back(newTarget);
}

void targetDestroy(Target* target, int at) {
	globals.targets.erase(globals.targets.begin() + at);
	delete target;
}

void transparentAdd() {
	Transparent* newTransparent = new Transparent();
	newTransparent->position = glm::vec3(0.0);
	globals.transparents.push_back(newTransparent);
}