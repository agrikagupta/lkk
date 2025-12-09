#include "Cow.h"

// ====================
// COW: The player UFO!
// ====================

Cow::Cow() {
	position = glm::vec3(0, 18, 65);
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);

	gravity = glm::vec3(0, -1, 0);

	rotation = 0;
	heading = glm::vec3(0, 0, 1);

	w = s = a = d = space = false;

	isSelectable = true;
	beingSelected = false;
	beingDragged = false;
}

void Cow::setup() {
	model.load("geo/finalspaceship.obj");
	model.setScaleNormalization(false);
}

void Cow::draw() {
	model.drawFaces();
}

void Cow::update() {

	applyForce(gravity);
	if (space) applyForce(glm::vec3(0, 5, 0));
	if (w) applyForce(heading * 3);
	if (s) applyForce(heading * 2);
	if (a) rotation += 2;
	if (d) rotation -= 2;

	float radians = glm::radians(rotation);
	heading = glm::vec3(sin(radians), 0, cos(radians));

	integrate();

	model.setPosition(position.x, position.y, position.z);
	model.setRotation(0, rotation, 0, 1, 0);
}



void Cow::applyForce(glm::vec3 force) {
	acceleration += force;
}

void Cow::integrate() {
	velocity += acceleration / 60.0f;
	position += velocity / 60.0f;
	acceleration = glm::vec3(0);
}

void Cow::setPosition(glm::vec3 pos) {
	position = pos;
}

glm::vec3 Cow::getPosition() {
	return model.getPosition();
}
