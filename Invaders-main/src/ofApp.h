#pragma once

#include "Octree.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include <glm/gtx/intersect.hpp>
#include "Cow.h"

// Camera choices
enum CameraType
{
    CAM_EASYCAM = 0,
    CAM_COWPOV,
    CAM_COWTARGET
};

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
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // Helpers
    void toggleWireframeMode();
    void toggleSelectTerrain();
    void togglePointsDisplay();
    void initLightingAndMaterials();
    void savePicture();
    bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
    void drawAxis(ofVec3f location);
    float computeAltitude();
    glm::vec3 getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm);
    void setCameraTarget();

    // Collision helpers
    void checkCowCollision();                   // main collision+response
    ofVec3f getCowAverageNormalFromCollision(); // normals for bounce direction
    ofVec3f getAverageNormalFromCollision();    // lander version (if you still use it)

    // Scene / flags
    bool bWireframe;
    bool bDisplayPoints;
    bool bAltKeyDown;
    bool bCtrlKeyDown;
    bool bTerrainSelected;

    bool bDisplayBBoxes;
    bool bDisplayLeafNodes;
    bool bDisplayOctree;

    // Old lander collision stuff (optional)
    bool bCollisionResolution;
    bool bInCollision;
    int collisionBoxCount;
    ofVec3f lastValidPosition;
    ofVec3f collisionNormal;

    // Cow collision
    bool bCowCollision;
    ofVec3f cowCollisionNormal;

    // Altitude sensing
    float landerAltitude; // actually cow altitude now
    bool hasAltitudeHit;

    // Cameras
    ofEasyCam cam;
    ofCamera cowpovcam;
    ofCamera cowTarget;
    CameraType currentCam;
    glm::vec3 onboardOffset;

    // Assets
    ofImage backgroundImage;
    ofSoundPlayer bgm;

    // Models
    ofxAssimpModelLoader mars;
    ofxAssimpModelLoader money1, money2, money3;

    // Physics objects
    Cow cow;
    ParticleSystem particles;

    // Octree
    Octree octree;
    vector<Box> colBoxList;
    vector<Box> cowColBoxList;
    vector<Box> bboxList;

    // GUI
    ofxPanel gui;
    ofxIntSlider numLevels;
    ofxFloatSlider bgmvol;
    ofxToggle timingToggle;
    bool bHide;
    bool bTimingInfo;
    float lastTreeBuildTime;
    float lastRaySearchTime;

    // Mouse/cow dragging
    glm::vec3 mouseDownPos;
    glm::vec3 mouseLastPos;

    // For debugging crash speed etc.
    float lastImpactSpeed;
};
