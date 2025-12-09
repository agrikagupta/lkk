
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
//
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>

#include "ofApp.h"
#include "ofxAssimpModelLoader.h"
#include "Util.h"

#include <limits>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup()
{
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bTerrainSelected = true;

	bCollisionResolution = false;
	bInCollision = false;
	collisionBoxCount = 0;
	lastValidPosition = ofVec3f(0, 0, 0);
	collisionNormal = ofVec3f(0, 1, 0);
	backgroundImage.load("images/sky.jpg");
	bTimingInfo = false;
	lastTreeBuildTime = 0;
	lastRaySearchTime = 0;
	//	ofSetWindowShape(1024, 768);
	cam.setDistance(20);
	cam.setNearClip(.1);
	cam.setFov(75); // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);

	cowTarget.setDistance(10);

	onboardOffset.set(0.0f, 2.0f, 0.5f);
	cowpovcam.setNearClip(0.1f);
	cowpovcam.setFarClip(2000.0f);

	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting
	//
	initLightingAndMaterials();

	// === MODELS ===

	cow.setup();

	mars.load("geo/finalterrain.obj");
	//mars.load("geo/test.obj");
	money1.load("moneybag/moneybag.fbx");
	money2.load("moneybag/moneybag.fbx");
	money3.load("moneybag/moneybag.fbx");

	money1.setScaleNormalization(false);
	money2.setScaleNormalization(false);
	money3.setScaleNormalization(false);

	money1.setScale(0.01, 0.01, 0.01);
	money2.setScale(0.01, 0.01, 0.01);
	money3.setScale(0.01, 0.01, 0.01);

	money1.setPosition(10, 0.6, 0);
	money2.setPosition(3, -10, -25);
	money3.setPosition(-11, 1.5, 9.5);

	mars.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(bgmvol.setup("Background Music Volume", 0.1, 0.0, 1.0));
	gui.add(timingToggle.setup("Timing Info", false));
	gui.setPosition(ofGetWidth() - gui.getWidth() - 10, 10);
	bHide = false;
	gui.minimize();

	float startTime = ofGetElapsedTimeMillis();
	octree.create(mars.getMesh(0), 20);
	float endTime = ofGetElapsedTimeMillis();
	lastTreeBuildTime = endTime - startTime;

	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	if (bTimingInfo)
	{
		cout << "Octree Build Time: " << lastTreeBuildTime << " ms" << endl;
	}

	bgm.load("audio/invaders.wav");
	bgm.setLoop(true);
	bgm.setVolume(bgmvol);
	bgm.play();
}

//--------------------------------------------------------------
void ofApp::update() {
	bgm.setVolume(bgmvol);

	// === COW ===
	cow.update();

	// Camera
	cowTarget.setTarget(cow.getPosition());


	// COLLISION LOGIC - NEEDS TO BE IMPLEMENTED IN COW SPACE
	/* if (bLanderLoaded)
	{
		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();
		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		ofVec3f landerPos = lander.getPosition();
		ofVec3f onboardPos = landerPos + onboardOffset;
		cowpovcam.setPosition(onboardPos);
		cowpovcam.lookAt(onboardPos, ofVec3f(0, 1, 0));

		vector<Box> currentColBoxes;
		octree.intersect(bounds, octree.root, currentColBoxes);
		collisionBoxCount = currentColBoxes.size();

		if (collisionBoxCount >= 10)
		{
			if (!bInCollision)
			{
				bInCollision = true;
				collisionNormal = getAverageNormalFromCollision();
			}
		}
		else
		{
			if (!bCollisionResolution)
			{
				lastValidPosition = lander.getPosition();
			}
			bInCollision = false;
		}

		if (bCollisionResolution && bInCollision)
		{
			float stepSize = 0.05;
			ofVec3f newPos = lander.getPosition() + (collisionNormal * stepSize);
			lander.setPosition(newPos.x, newPos.y, newPos.z);

			ofVec3f newMin = lander.getSceneMin() + lander.getPosition();
			ofVec3f newMax = lander.getSceneMax() + lander.getPosition();
			Box newBounds = Box(Vector3(newMin.x, newMin.y, newMin.z), Vector3(newMax.x, newMax.y, newMax.z));

			vector<Box> checkBoxes;
			octree.intersect(newBounds, octree.root, checkBoxes);
			colBoxList = checkBoxes;
			if (checkBoxes.size() < 10)
			{
				bCollisionResolution = false;
				bInCollision = false;
				lastValidPosition = lander.getPosition();
			}
		}
	}*/
	landerAltitude = computeAltitude();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// === COW ===
	cow.draw();

	// clear screen color & depth
	ofBackground(ofColor::black);

	ofPushStyle();
	ofDisableDepthTest(); // ignore depth for background
	ofDisableLighting();  // don't let GL lights affect the image

	if (backgroundImage.isAllocated())
	{
		backgroundImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	}

	ofEnableDepthTest(); // turn depth back on for 3D
	ofPopStyle();

	glDepthMask(false);
	if (!bHide)
		gui.draw();
	glDepthMask(true);

	ofCamera *activeCam = nullptr;

	switch (currentCam)
	{
	case CAM_EASYCAM:
		activeCam = &cam;
		break;
	case CAM_COWPOV:
		activeCam = &cowpovcam;
		break;
	case CAM_COWTARGET:
		activeCam = &cowTarget;
	}

	if (activeCam)
		activeCam->begin();

	ofPushMatrix();

	ofEnableLighting(); // shaded mode
	mars.drawFaces();

	cow.draw();

	money1.drawFaces();
	money2.drawFaces();
	money3.drawFaces();
	ofMesh mesh;

	// NEEDS TO BE REWRITTEN IN COWSPACE
	/* if (bLanderLoaded)
	{

		int meshCount = lander.getMeshCount();

		ofPushMatrix();
		ofMultMatrix(lander.getModelMatrix());

		glDepthMask(GL_TRUE);
		ofEnableAlphaBlending();

		for (int i = 0; i < meshCount; ++i)
		{
			if (i == glassMeshIndex)
				continue; // skip dome in opaque pass
			ofMesh mesh = lander.getMesh(i);
			ofMaterial mat = lander.getMaterialForMesh(i);
			mat.begin();
			mesh.drawFaces();
			mat.end();
		}

		glDepthMask(GL_FALSE); // no write depth for glass

		ofMesh mesh = lander.getMesh(glassMeshIndex);
		ofMaterial mat = lander.getMaterialForMesh(glassMeshIndex);

		ofFloatColor c = mat.getDiffuseColor();
		c.a = 0.20f;
		mat.setDiffuseColor(c);

		mat.begin();
		mesh.drawFaces();
		mat.end();

		glDepthMask(GL_TRUE); // restore depth writes
		ofDisableAlphaBlending();

		ofPopMatrix();
		if (!bTerrainSelected)
			drawAxis(lander.getPosition());
		if (bDisplayBBoxes)
		{
			ofNoFill();
			ofSetColor(ofColor::white);
			for (int i = 0; i < lander.getNumMeshes(); i++)
			{
				ofPushMatrix();
				ofMultMatrix(lander.getModelMatrix());
				ofRotate(-90, 1, 0, 0);
				Octree::drawBox(bboxList[i]);
				ofPopMatrix();
			}
		}

		if (bLanderSelected)
		{

			ofVec3f min = lander.getSceneMin() + lander.getPosition();
			ofVec3f max = lander.getSceneMax() + lander.getPosition();

			Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
			ofSetColor(ofColor::white);
			Octree::drawBox(bounds);

			// draw colliding boxes
			for (int i = 0; i < colBoxList.size(); i++)
			{
				ofNoFill();
				Octree::drawBox(colBoxList[i]);
			}
		}}*/

		//ofDisableAlphaBlending();

	if (bTerrainSelected)
		drawAxis(ofVec3f(0, 0, 0));

	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes)
	{
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
	}
	else if (bDisplayOctree)
	{
		octree.draw(numLevels, 0, levelColors);
	}

	ofPopMatrix();

	if (activeCam)
		activeCam->end();

	ofSetColor(255);
	if (hasAltitudeHit)
	{
		ofDrawBitmapStringHighlight("Altitude: " + ofToString(landerAltitude, 2),
									20, 50);
	}
	else
	{
		ofDrawBitmapStringHighlight("Altitude: N/A",
									20, 50);
	}
}

float ofApp::computeAltitude() {
	// NEEDS TO BE REWRITTEN IN COWSPACE
	/* if (!bLanderLoaded)
	{
		hasAltitudeHit = false;
		return 0.0f;
	}*/

	ofVec3f landerPos = lander.getPosition();
	ofVec3f rayOrigin = landerPos + ofVec3f(0, 50, 0);
	ofVec3f rayDir = ofVec3f(0, -1, 0);

	Ray ray(
		Vector3(rayOrigin.x, rayOrigin.y, rayOrigin.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	TreeNode hitNode;
	bool hit = octree.intersect(ray, octree.root, hitNode);

	if (!hit || hitNode.points.size() == 0) {
		hasAltitudeHit = false;
		return 0.0f;
	}

	float maxY = -std::numeric_limits<float>::infinity();
	ofVec3f groundPoint;

	for (int idx : hitNode.points) {
		ofVec3f v = octree.mesh.getVertex(idx);
		if (v.y > maxY) {
			maxY = v.y;
			groundPoint = v;
		}
	}
	float landerBottomY = lander.getSceneMin().y + landerPos.y;
	float altitude = landerBottomY - groundPoint.y;

	hasAltitudeHit = true;
	return altitude;
}

//
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location)
{

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}

void ofApp::keyPressed(int key)
{

	switch (key) {
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;

	case OF_KEY_SPACE:
		cow.space = true;
		break;
	case 'W':
	case 'w':
		cow.w = true;
		break;
	case 'S':
	case 's':
		cow.s = true;
		break;
	case 'D':
	case 'd':
		cow.d = true;
		break;
	case 'A':
	case 'a':
		cow.a = true;
		break;

	case '1':
		currentCam = CAM_COWTARGET; // position towards cow
		cowTarget.enableMouseInput();
		break;

	case '2':
		currentCam = CAM_COWPOV; // onboard cam?
		cowpovcam.disableMouseInput();
		break;

	case '3':
		currentCam = CAM_EASYCAM;
		cam.enableMouseInput();
		break;

	default:
		break;
	}
}

void ofApp::toggleWireframeMode()
{
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain()
{
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay()
{
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key)
{

	switch (key)
	{

	case OF_KEY_SPACE:
		cow.space = false;
		break;
	case 'W':
	case 'w':
		cow.w = false;
		break;
	case 'S':
	case 's':
		cow.s = false;
		break;
	case 'D':
	case 'd':
		cow.d = false;
		break;
	case 'A':
	case 'a':
		cow.a = false;
		break;

	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled())
		return;

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled())
		return;

	// === TEST FOR COW SELECTION ===
	glm::vec3 origin = cam.getPosition();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

	glm::vec3 min = cow.model.getSceneMin() + cow.getPosition();
	glm::vec3 max = cow.model.getSceneMax() + cow.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);

	if (hit && cow.isSelectable) {
		cow.beingSelected = true;
		mouseDownPos = getMousePointOnPlane(cow.getPosition(), cam.getZAxis());
		mouseLastPos = mouseDownPos;
		cow.beingDragged = true;
	} else {
		cow.beingSelected = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled())
		return;

	if (cow.beingDragged) {
		glm::vec3 cowPos = cow.getPosition();
		glm::vec3 mousePos = getMousePointOnPlane(cowPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;

		cowPos += delta;
		cow.setPosition(cowPos);
		mouseLastPos = mousePos;

		glm::vec3 min = cow.model.getSceneMin() + cow.getPosition();
		glm::vec3 max = cow.model.getSceneMax() + cow.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	cow.beingDragged = false;
}

// Set the camera to use the selected point as it's new target
//
void ofApp::setCameraTarget()
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials()
{

	static float ambient[] = {.5f, .5f, .5, 1.0f};
	static float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

	static float position[] = {5.0, 5.0, 5.0, 0.0};

	static float lmodel_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};

	static float lmodel_twoside[] = {GL_TRUE};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture()
{
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point)
{
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

ofVec3f ofApp::getAverageNormalFromCollision()
{
	ofVec3f avgNormal(0, 0, 0);
	int normalCount = 0;

	if (colBoxList.size() == 0)
	{
		return ofVec3f(0, 1, 0);
	}

	ofMesh terrainMesh = mars.getMesh(0);
	vector<glm::vec<3, float>> normals = terrainMesh.getNormals();
	vector<glm::vec<3, float>> vertices = terrainMesh.getVertices();

	for (int i = 0; i < colBoxList.size(); i++)
	{
		for (int j = 0; j < vertices.size(); j++)
		{
			Vector3 v(vertices[j].x, vertices[j].y, vertices[j].z);
			if (colBoxList[i].inside(v))
			{
				if (j < normals.size())
				{
					avgNormal += normals[j];
					normalCount++;
				}
			}
		}
	}

	if (normalCount > 0)
	{
		avgNormal /= normalCount;
		avgNormal.normalize();
	}
	else
	{
		avgNormal = ofVec3f(0, 1, 0);
	}

	return avgNormal;
}

//  intersect the mouse ray with the plane normal to the camera
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm)
{
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit)
	{
		// find the point of intersection on the plane using the distance
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else
		return glm::vec3(0, 0, 0);
}
