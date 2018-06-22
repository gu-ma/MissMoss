#pragma once

#include "ofMain.h"
#include "ofxJSON.h"

#include "ofEsp.h"
#include "ofSoundOut.h"
#include "ofVideoSequence.h"


class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // ESP
    vector<float> smoothedVariation;
    int t, inertia;
    ofEsp esp;
    
    // SOUND
    ofSoundOut soundOut;
    
    // VIDEO SEQUENCE
    ofxJSONElement response;
    ofVideoSequence videoSequence;
    int vidSeqStartId, vidSeqEndId;
    string vidSeqOrder;
    vector<string> videoFiles;
    vector<ofVec2f> timeStartEnd;
    void getVideosPath(string & vidSeqOrder, int & startId, vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd);
    string serverURL;

    
    // GENERAL
    bool tripStarted;
    int tripTime;

};
