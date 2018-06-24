#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
//    ofSetFullscreen(true);
//    ofSetFrameRate(60);
    ofBackground(0);
    ofRegisterURLNotification(this);
    
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
    vidSeqPathNumVideos = 10;
    vidSeqPathVideosDuration = 5;
    vidSeqPathfadeDuration = 1;
    vidSeqSimVidNumVideos = 2;
    vidSeqSimVidVideosDuration = 5;
    vidSeqSimVidfadeDuration = 3;
    serverURL = "http://127.0.0.1:5002";
    loading = false;
    // Start idle Mode
    vidSeqOrder = "StartToEnd";
    vidSeqEndId = -1;
    // Load the first round of videos
    getSimilarVideos(vidSeqEndId, videoFiles, timeStartEnd, vidSeqSimVidNumVideos, vidSeqSimVidVideosDuration, "Init");
//    videoSequence.init(videoFiles, timeStartEnd, vidSeqSimVidfadeDuration, true);
    // Prepare the path for the idle mode
    getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
    
    // Misc
    // 1 = "No hands"
    // 2 = "Hovering";
    // 3 = "Tickling";
    // 4 = "Touching";
    mode = 1;
    elapsedTime = 0;
    startTime = 0;
    endTime = 0;
    totalTime = 0;
    tripDuration = 20000; // in ms
    descentDuration = 1000; // in ms
    morphAmount = 0;
    lastmorphAmount = 0;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Smooth the variation from the ESP
    t = (t < inertia) ? t+1 : 0;
    smoothedVariation[t] = esp.data_point_var_norm_smoothed;
    float sum = accumulate(smoothedVariation.begin(), smoothedVariation.end(), 0.0);
//   float v = sum / inertia;
//    float v = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
//    if (ofGetElapsedTimeMillis() % 4 == 0) {
//        soundOut.variation1 = ofMap(v, .95, 0, 0, 1);
//    }
    
    // ESP
    esp.update();
    
    // SOUND
    soundOut.update();

    // Video Sequence
    if (!loading) videoSequence.update();

    // If 'Touching'
    if (esp.predicted_label == 4) {
        if (mode != 4) {
            // Start the trip mode
            videoSequence.add(videoFiles, timeStartEnd, vidSeqPathfadeDuration, false, true);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
            // Save video for reload later
            getSimilarVideos(vidSeqEndId, videoFilesSaved, timeStartEndSaved, vidSeqSimVidNumVideos, vidSeqSimVidVideosDuration, "Save");
            // Time stuff
            startTime = ofGetElapsedTimeMillis();
            soundOut.startMorph();
            mode = 4;
        }
        
        // Counter + Morph
        elapsedTime = ofGetElapsedTimeMillis() - startTime;
        endTime = ofGetElapsedTimeMillis();
        totalTime = elapsedTime;
        lastmorphAmount = morphAmount;
        morphAmount = ofMap(elapsedTime, 0, tripDuration, 0, 1);
        
        // Keep adding videos to the path
        if ( videoSequence.isFinished() ) {
            videoSequence.add(videoFiles, timeStartEnd, vidSeqPathfadeDuration, false, false);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
        }
        
    // Any other cases
    } else {
        
        if (mode != 1) {
            // Start the idle mode
            vidSeqOrder = "StartToEnd";
            vidSeqEndId = -1;
            videoSequence.init(videoFilesSaved, timeStartEndSaved, vidSeqSimVidfadeDuration, true);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
            mode = 1;
        }
        
        if (elapsedTime <= totalTime - descentDuration) {
            soundOut.stopMorph();
        } else {
            elapsedTime = totalTime - (ofGetElapsedTimeMillis() - endTime);
            morphAmount = ofMap(elapsedTime, totalTime, totalTime - descentDuration, lastmorphAmount, 0);
        }

    }
    soundOut.morphAmount(morphAmount);

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    videoSequence.draw(0, 0, ofGetWidth(), ofGetHeight());
    esp.draw(morphAmount);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // TEMP TEST
    if( key == '1' ) esp.predicted_label = 1;
    if( key == '4' ) esp.predicted_label = 4;
    
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
    if(key == 'd') {
        soundOut.distortion1.showUI();
        soundOut.distortion2.showUI();
    }
    if(key == 'c') soundOut.lowpass2.showUI();

    // VideoSequence
    if ( key == ' ' ) {
        vidSeqEndId = -1;
        vidSeqOrder = "StartToEnd";
        getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "Init");
        getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
    }

}

//--------------------------------------------------------------
void ofApp::getVideosPath(string & vidSeqOrder, int & vidSeqEndId, vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd, int num_videos, int duration, string mode){
    cout << "\n---------\nGet video path" << endl;
    string request = serverURL + "/getpath?num_neighbors="+ofToString(num_videos)+"&duration="+ofToString(duration)+"&order="+vidSeqOrder+"&start_id="+ofToString(vidSeqEndId);
    cout << request + "\n"<< endl;
    loading = true;
    // mode = Add, Init, Save, None
    int id = ofLoadURLAsync(request, "getVideosPath"+mode);
//    if (response.open(request)) {
//        vidSeqEndId = parseResponse(response, videoFiles, timeStartEnd);
//        vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
//    }
}

//--------------------------------------------------------------
void ofApp::getSimilarVideos(int & vidSeqEndId, vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd, int num_videos, int duration, string mode){
    cout << "---------\nGet similar videos" << endl;
    string request = serverURL + "/getsimilarvideos?num_neighbors="+ofToString(num_videos)+"&duration="+ofToString(duration)+"&id="+ofToString(vidSeqEndId);
    cout << request + "\n" << endl;
    loading = true;
    // mode = Add, Init, Save, None
    int id = ofLoadURLAsync(request, "getSimilarVideos"+mode);
//    if (response.open(request)) {
//        vidSeqEndId =  parseResponse(response, videoFiles, timeStartEnd);
//    }
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
//        cout << response["scenes"][i] << endl;
    }
    return response["scenes"][numScenes-1]["end_id"].asInt();
}

//--------------------------------------------------------------
void ofApp::urlResponse(ofHttpResponse & response) {
    if (response.status==200) {
        ofxJSONElement r;
        r.parse(response.data);
        loading = false;
        cout << response.request.name << endl;
        //
        if (response.request.name == "getSimilarVideosAdd")  {
            vidSeqEndId =  parseResponse(r, videoFiles, timeStartEnd);
            videoSequence.add(videoFiles, timeStartEnd, vidSeqSimVidfadeDuration, false, true);
        } else if (response.request.name == "getSimilarVideosInit")  {
            vidSeqEndId =  parseResponse(r, videoFiles, timeStartEnd);
            videoSequence.init(videoFiles, timeStartEnd, vidSeqSimVidfadeDuration, true);
        } else if (response.request.name == "getSimilarVideosSave")  {
            vidSeqEndId =  parseResponse(r, videoFilesSaved, timeStartEndSaved);
        } else if (response.request.name == "getSimilarVideosNone")  {
            vidSeqEndId =  parseResponse(r, videoFiles, timeStartEnd);
        //
        } else if (response.request.name == "getVideosPathAdd")  {
            vidSeqEndId =  parseResponse(r, videoFiles, timeStartEnd);
            videoSequence.add(videoFiles, timeStartEnd, vidSeqSimVidfadeDuration, false, true);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        } else if (response.request.name == "getVideosPathInit")  {
            vidSeqEndId =  parseResponse(r, videoFilesSaved, timeStartEndSaved);
            videoSequence.init(videoFiles, timeStartEnd, vidSeqSimVidfadeDuration, true);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        } else if (response.request.name == "getVideosPathSave")  {
            vidSeqEndId =  parseResponse(r, videoFilesSaved, timeStartEndSaved);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        } else if (response.request.name == "getVideosPathNone")  {
            vidSeqEndId =  parseResponse(r, videoFiles, timeStartEnd);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        }
    } else {
        cout << response.status << " " << response.error << endl;
        if (response.status != -1) loading = false;
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
