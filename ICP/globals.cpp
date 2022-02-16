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

Arrow arrowShoot(Arrow& arrow, Avatar& avatar) {
	arrow.exists = true;
	arrow.previous_time = glfwGetTime();
	arrow.speed = 1;
	arrow.position = avatar.position + avatar.lookAt;
	arrow.direction = avatar.lookAt;
	return arrow;
}