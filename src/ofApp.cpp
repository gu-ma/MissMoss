#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
//    ofSetFullscreen(true);
//    ofSetFrameRate(60);
    ofBackground(0);
    
    // ESP
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
    retrieveVideos();

}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Smooth the variation from the ESP
    t = (t < inertia) ? t+1 : 0;
    smoothedVariation[t] = esp.data_point_var_norm_smoothed;
    float sum = accumulate(smoothedVariation.begin(), smoothedVariation.end(), 0.0);
    float v = sum / inertia;
    if (ofGetElapsedTimeMillis() % 4 == 0) {
        soundOut.variation1 = ofMap(v, .95, 0, 0, 1);
    }
    
    // ESP
    esp.update();
    soundOut.update();
    
    // Video Sequence
    videoSequence.update();
    if ( videoSequence.isFinished() ) retrieveVideos();

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    videoSequence.draw(0, 0, ofGetWidth(), ofGetHeight());
    esp.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // ESP - Sound
    if (key == '-' || key == '_' ){
        esp.volume -= 0.001;
        esp.volume = MAX(esp.volume, 0);
    } else if (key == '+' || key == '=' ){
        esp.volume += 0.001;
        esp.volume = MIN(esp.volume, 1);
    }
    
    if( key == 's' ){
        esp.soundStream.start();
    }
    
    if( key == 'e' ){
        esp.soundStream.stop();
    }
    
    // SoundOut
    if(key == 'l') soundOut.lowpass2.showUI();
    if(key == 'd') soundOut.distortion.showUI();
    
    // VideoSequence
    if ( key == ' ' ) {
        vidSeqEndId = -1;
        vidSeqOrder = "StartToEnd";
        retrieveVideos();
    }

}

//--------------------------------------------------------------
void ofApp::retrieveVideos(){
    
    if (response.open( serverURL + "/getpath?num_neighbors=10&duration=5&order="+vidSeqOrder+"&start_id="+ofToString(vidSeqEndId))) {
        
        vector<string> videoFiles;
        vector<ofVec2f> timeStartEnd;
        
        int numScenes = response["scenes"].size();
        
        for (Json::ArrayIndex i = 0; i < numScenes; ++i) {
            string file = response["scenes"][i]["file"].asString();
            float start_time = response["scenes"][i]["start_time"].asFloat();
            float end_time = response["scenes"][i]["end_time"].asFloat();
            videoFiles.push_back(file);
            timeStartEnd.push_back(ofVec2f(start_time, end_time));
//            cout << response["scenes"][i] << endl;
        }
        
        vidSeqStartId = response["scenes"][0]["start_id"].asInt();
        vidSeqEndId = response["scenes"][numScenes-1]["end_id"].asInt();
        cout << vidSeqStartId << endl;
        cout << vidSeqEndId << endl;
        
        videoSequence.init(videoFiles, timeStartEnd);
        
        vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        
    }
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
