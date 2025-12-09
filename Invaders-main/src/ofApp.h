#pragma once

#include "Octree.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include <glm/gtx/intersect.hpp>
#include "Cow.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void gotMessage(ofMessage msg);
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();
	void toggleSelectTerrain();
	void setCameraTarget();
	bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
	bool raySelectWithOctree(ofVec3f &pointRet);
	glm::vec3 getMousePointOnPlane(glm::vec3 p, glm::vec3 n);

	ofEasyCam cam;
	enum CameraMode
	{
		CAM_COWTARGET = 0,
		CAM_COWPOV,
		CAM_EASYCAM
	};
	ofVec3f onboardOffset;
	CameraMode currentCam;
	ofEasyCam cowpovcam;
	ofEasyCam cowTarget;

	ofxAssimpModelLoader mars, lander;
	ofxAssimpModelLoader money1, money2, money3;
	Cow cow;

	ofLight light;
	Box boundingBox, landerBounds;
	Box testBox;
	vector<Box> colBoxList;
	Octree octree;
	TreeNode selectedNode;
	glm::vec3 mouseDownPos, mouseLastPos;

	ofxIntSlider numLevels;
	ofxFloatSlider bgmvol;
	ofxPanel gui;

	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;
	bool bPointSelected;
	bool bHide;
	bool pointSelected = false;
	bool bDisplayLeafNodes = false;
	bool bDisplayOctree = false;
	bool bDisplayBBoxes = false;
	ofImage backgroundImage;
	bool bTerrainSelected;

	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	vector<Box> bboxList;
	vector<ofColor> levelColors;

	const float selectionRange = 4.0;

	// Feature 5: Collision Resolution
	bool bCollisionResolution;
	ofVec3f lastValidPosition;
	ofVec3f collisionNormal;
	bool bInCollision;
	int collisionBoxCount;
	ofVec3f getAverageNormalFromCollision();

	// Feature 6: Timing Information
	bool bTimingInfo;
	ofxToggle timingToggle;
	float lastTreeBuildTime;
	float lastRaySearchTime;

	int glassMeshIndex = 0;

	float landerAltitude;
	bool hasAltitudeHit;
	float computeAltitude();

	// Cow collision detection
	vector<Box> cowColBoxList;
	bool bCowCollision;
	ofVec3f cowCollisionNormal;
	void checkCowCollision();
	ofVec3f getCowAverageNormalFromCollision();

	ofSoundPlayer bgm;
};
