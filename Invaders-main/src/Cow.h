#pragma once
#include <ofMain.h>
#include "ofxAssimpModelLoader.h"

// ====================
// COW: The player UFO!
// ====================

class Cow {
public:

	ofxAssimpModelLoader model;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 gravity;

	float rotation;
	glm::vec3 heading;

	bool isSelectable;
	bool beingSelected;
	bool beingDragged;

	bool w, s, a, d, space;

	Cow();

	void setup();
	void draw();
	void update();

	void applyForce(glm::vec3 force);
	void integrate();
	void rotate(float rot);
	void move(float push);

	void setPosition(glm::vec3 pos);
	glm::vec3 getPosition();

};
