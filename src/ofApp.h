#pragma once

#include "ofMain.h"
#include "ofxJSON.h"

#include "ofEsp.h"
#include "ofSoundOut.h"
#include "LogAudio.h"
#include "ofVideoSequence.h"
#include "easing.h"


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
    int vidSeqEndId, vidSeqEndIdSaved;
    int vidSeqPathNumVideos, vidSeqPathVideosDuration;
    int vidSeqSimVidNumVideos, vidSeqSimVidVideosDuration;
    float vidSeqSimVidfadeDuration, vidSeqPathfadeDuration;
    string vidSeqOrder;
    vector<string> videoFiles, videoFilesSaved;
    vector<ofVec2f> timeStartEnd, timeStartEndSaved;
    vector<string> videoPoems, videoPoemsSaved;
    vector<string> videoDesc, videoDescSaved;
    string serverURL;
    bool loading;
    void urlResponse(ofHttpResponse & response);
    void getVideosPath(string & vidSeqOrder, int & startId, vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd, int num_videos, int duration, string mode);
    void getSimilarVideos(int & vidSeqEndId, vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd, int num_videos, int duration, string mode);
    int parseResponse(ofxJSONElement & response, vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd);
    
    // GENERAL
    Easing easing;
    int videoMode, soundMode;
    int startTime, endTime, totalTime, elapsedTime;
    int ascendDuration, tripDuration, dropDuration, descentDuration;
    float morphAmount1, lastmorphAmount1;
    float morphAmount2, lastmorphAmount2;
    void startSoundMode(int mode);
    void updateSoundMode(int mode, float & m1, float & m2);
    
};
