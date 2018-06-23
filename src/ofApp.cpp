#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
//    ofSetFullscreen(true);
//    ofSetFrameRate(60);
    ofBackground(0);
    
    // ESP
    // 1 = "No hands"
    // 2 = "Hovering";
    // 3 = "Tickling";
    // 4 = "Touching";
    t = 0;
    inertia = 500;
    smoothedVariation.assign(inertia, 0);
    esp.setup();
    
    // Sound
    soundOut.setup();
    
    // Video Sequence
    serverURL = "http://127.0.0.1:5002";
    vidSeqOrder = "StartToEnd";
    vidSeqEndId = -1;
    // Load the first round of videos
    getSimilarVideos(vidSeqEndId, videoFiles, timeStartEnd);
    videoSequence.init(videoFiles, timeStartEnd);
    getSimilarVideos(vidSeqEndId, videoFiles, timeStartEnd);
    
    // Misc
    // 1 = "No hands"
    // 2 = "Hovering";
    // 3 = "Tickling";
    // 4 = "Touching";
    mode = 1;
    elapsedTime = 0;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Smooth the variation from the ESP
    t = (t < inertia) ? t+1 : 0;
    smoothedVariation[t] = esp.data_point_var_norm_smoothed;
    float sum = accumulate(smoothedVariation.begin(), smoothedVariation.end(), 0.0);
//    float v = sum / inertia;
    float v = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
    if (ofGetElapsedTimeMillis() % 4 == 0) {
        soundOut.variation1 = ofMap(v, .95, 0, 0, 1);
    }
    
    // ESP
    esp.update();
    soundOut.update();
    mode = esp.predicted_label == 1;

    // Video Sequence
    videoSequence.update();
    // If mode is 'Idle'
    if (mode == 1) {
//        if ( videoSequence.isFinished() ) {
//            videoSequence.add(videoFiles, timeStartEnd);
//            getSimilarVideos(vidSeqEndId, videoFiles, timeStartEnd);
//        }
    // Else if 'Touching'
    } else if (mode == 4) {
        if ( videoSequence.isFinished() ) {
            videoSequence.add(videoFiles, timeStartEnd);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd);
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    videoSequence.draw(0, 0, ofGetWidth(), ofGetHeight());
    esp.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // To test
    if( key == 1 ) mode = 1;
    if( key == 4 ) mode = 4;
    
    // ESP - Sound
    if (key == '-' || key == '_' ){
        esp.volume -= 0.001;
        esp.volume = MAX(esp.volume, 0);
    } else if (key == '+' || key == '=' ){
        esp.volume += 0.001;
        esp.volume = MIN(esp.volume, 1);
    }
    if( key == 's' ) esp.soundStream.start();
    if( key == 'e' ) esp.soundStream.stop();
    
    // SoundOut
    if(key == 'l') soundOut.lowpass2.showUI();
    if(key == 'd') soundOut.distortion.showUI();
    
    // VideoSequence
    if ( key == ' ' ) {
        vidSeqEndId = -1;
        vidSeqOrder = "StartToEnd";
        getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd);
        videoSequence.init(videoFiles, timeStartEnd);
        getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd);
    }

}

//--------------------------------------------------------------
void ofApp::getVideosPath(string & vidSeqOrder, int & vidSeqEndId, vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd){
    if (response.open( serverURL + "/getpath?num_neighbors=10&duration=5&order="+vidSeqOrder+"&start_id="+ofToString(vidSeqEndId))) {
        vidSeqEndId = parseResponse(response, videoFiles, timeStartEnd);
        vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
    }
}

//--------------------------------------------------------------
void ofApp::getSimilarVideos(int & vidSeqEndId, vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd){
    if (response.open( serverURL + "/getsimilarvideos?num_neighbors=10&duration=5&id="+ofToString(vidSeqEndId))) {
        vidSeqEndId =  parseResponse(response, videoFiles, timeStartEnd);
    }
}

//--------------------------------------------------------------
int ofApp::parseResponse(ofxJSONElement & response, vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd){
    int numScenes = response["scenes"].size();
    videoFiles.clear();
    timeStartEnd.clear();
    for (Json::ArrayIndex i = 0; i < numScenes; ++i) {
        string file = response["scenes"][i]["file"].asString();
        float start_time = response["scenes"][i]["start_time"].asFloat();
        float end_time = response["scenes"][i]["end_time"].asFloat();
        videoFiles.push_back(file);
        timeStartEnd.push_back(ofVec2f(start_time, end_time));
    }
    return response["scenes"][numScenes-1]["end_id"].asInt();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
