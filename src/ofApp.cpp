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
    videoSequence.init();
    vidSeqPathNumVideos = 15;  // KNN (not number of videos returned)
    vidSeqPathVideosDuration = 5;
    vidSeqPathfadeDuration = 1;
    vidSeqSimVidNumVideos = 6;  // Actual number of videos returned
    vidSeqSimVidVideosDuration = 5;
    vidSeqSimVidfadeDuration = 3;
    serverURL = "http://127.0.0.1:5002";
    loading = false;
    // Start idle Mode
    vidSeqOrder = "StartToEnd";
    vidSeqEndId = -1;
    vidSeqEndIdSaved = -1;
    // Load the first round of videos
    getSimilarVideos(vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidNumVideos, vidSeqSimVidVideosDuration, "Load");
    // Prepare the path for the idle mode
    getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
    // Save video for reload later
    getSimilarVideos(vidSeqEndIdSaved, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidNumVideos, vidSeqSimVidVideosDuration, "Save");
    
    // Misc
    // 1 = "No hands"
    // 2 = "Hovering";
    // 3 = "Tickling";
    // 4 = "Touching";
    videoMode = 1;
    soundMode = 1;
    elapsedTime = 0;
    startTime = 0;
    endTime = 0;
    totalTime = 0;
    ascendDuration1 = 10000;
    ascendDuration2 = 20000;
    tripDuration = 10000;
    dropDuration = 500;
    descentDuration = 5000;
    morphAmount1 = 0;
    lastmorphAmount1 = 0;
    morphAmount2 = 0;
    lastmorphAmount2 = 0;

    // TEMP
    esp.predicted_label = 1;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Smooth the variation from the ESP
    t = (t < inertia) ? t+1 : 0;
    smoothedVariation[t] = esp.data_point_var_norm_smoothed;
    float sum = accumulate(smoothedVariation.begin(), smoothedVariation.end(), 0.0);
//   float v = sum / inertia;
    float v = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
    if (ofGetElapsedTimeMillis() % 4 == 0) {
        esp.data_point_var_norm = ofMap(v, .95, 0, 0, 1);
    }
    
    // ESP
    esp.update(morphAmount1);
    
    // SOUND
    soundOut.update(morphAmount1, morphAmount2);

    // Video Sequence
    videoSequence.update();

    // 'No Hands'
    if (esp.predicted_label == 1) {
        if (videoMode != 1) {
            // Start the idle videoMode
            vidSeqOrder = "StartToEnd";
            vidSeqEndId = -1;
//            videoSequence.load(videoFilesSaved, videoPoemsSaved, videoDescSaved, timeStartEndSaved, vidSeqSimVidfadeDuration, true);
            videoSequence.add(videoFilesSaved, videoPoemsSaved, videoDescSaved, timeStartEndSaved, vidSeqSimVidfadeDuration, true, true, true);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
            videoMode = 1;
        }
    } else {
        if (videoMode != 4) {
            videoSequence.add(videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathfadeDuration, false, true, false);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
            // Save video for reload later
            getSimilarVideos(vidSeqEndIdSaved, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidNumVideos, vidSeqSimVidVideosDuration, "Save");
            videoMode = 4;
        }
        // Keep adding videos to the path
        if ( videoSequence.isFinished() ) {
            videoSequence.add(videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathfadeDuration, false, true, false);
            getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
        }
    }

    // SOUND
    if (esp.predicted_label == 4) {
        if (elapsedTime < ascendDuration2) {
            startSoundMode(4);
            updateSoundMode(4, morphAmount1, morphAmount2);
        } else {
            startSoundMode(5);
            updateSoundMode(5, morphAmount1, morphAmount2);
        }
    } else if (esp.predicted_label == 1) {
        startSoundMode(1);
        updateSoundMode(1, morphAmount1, morphAmount2);
    }
    
//    cout << ofToString(elapsedTime) + "\t\t" + ofToString(morphAmount1) + "\t\t" + ofToString(morphAmount2) << endl;

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    videoSequence.draw(0, 0, ofGetWidth(), ofGetHeight(), morphAmount1);
    esp.draw(morphAmount1);

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
    if(key == 'l') {
        soundOut.lowpass1.showUI();
        soundOut.lowpass2.showUI();
    }
    if(key == 'd') {
        soundOut.distortion1.showUI();
        soundOut.distortion2.showUI();
    }
    if(key == 'm') {
        soundOut.mixer1.showUI();
        soundOut.mixer2.showUI();
    }
    if(key == 'c') soundOut.compressor.showUI();

    // VideoSequence
    if ( key == ' ' ) {
        vidSeqEndId = -1;
        vidSeqOrder = "StartToEnd";
        getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "Load");
        getVideosPath(vidSeqOrder, vidSeqEndId, videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqPathNumVideos, vidSeqPathVideosDuration, "None");
    }

}

//--------------------------------------------------------------
void ofApp::getVideosPath(string & vidSeqOrder, int & vidSeqEndId, vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd, int num_videos, int duration, string mode){
    cout << "\n---------\nGet video path" << endl;
    string request = serverURL + "/getpath?num_neighbors="+ofToString(num_videos)+"&duration="+ofToString(duration)+"&order="+vidSeqOrder+"&start_id="+ofToString(vidSeqEndId);
    cout << request + "\n"<< endl;
    loading = true;
    // mode = Add, Load, Save, None
    int id = ofLoadURLAsync(request, "getVideosPath"+mode);
//    if (response.open(request)) {
//        vidSeqEndId = parseResponse(response, videoFiles, videoPoems, videoDesc, timeStartEnd);
//        vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
//    }
}

//--------------------------------------------------------------
void ofApp::getSimilarVideos(int & vidSeqEndId, vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd, int num_videos, int duration, string mode){
    cout << "---------\nGet similar videos" << endl;
    string request = serverURL + "/getsimilarvideos?num_neighbors="+ofToString(num_videos)+"&duration="+ofToString(duration)+"&id="+ofToString(vidSeqEndId);
    cout << request + "\n" << endl;
    loading = true;
    // mode = Add, Load, Save, None
    int id = ofLoadURLAsync(request, "getSimilarVideos"+mode);
//    if (response.open(request)) {
//        vidSeqEndId =  parseResponse(response, videoFiles, videoPoems, videoDesc, timeStartEnd);
//    }
}

//--------------------------------------------------------------
int ofApp::parseResponse(ofxJSONElement & response, vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd){
    int numScenes = response["scenes"].size();
    videoFiles.clear();
    videoPoems.clear();
    videoDesc.clear();
    timeStartEnd.clear();
    for (Json::ArrayIndex i = 0; i < numScenes; ++i) {
        string file = response["scenes"][i]["file"].asString();
        string poem = response["scenes"][i]["poem"].asString();
        string desc = response["scenes"][i]["description"].asString();
        float start_time = response["scenes"][i]["start_time"].asFloat();
        float end_time = response["scenes"][i]["end_time"].asFloat();
        videoFiles.push_back(file);
        videoPoems.push_back(poem);
        videoDesc.push_back(desc);
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
            vidSeqEndId =  parseResponse(r, videoFiles, videoPoems, videoDesc, timeStartEnd);
            videoSequence.add(videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidfadeDuration, false, true, true);
        } else if (response.request.name == "getSimilarVideosLoad")  {
            vidSeqEndId =  parseResponse(r, videoFiles, videoPoems, videoDesc, timeStartEnd);
            videoSequence.load(videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidfadeDuration, true);
        } else if (response.request.name == "getSimilarVideosSave")  {
            vidSeqEndId =  parseResponse(r, videoFilesSaved, videoPoemsSaved, videoDescSaved, timeStartEndSaved);
        } else if (response.request.name == "getSimilarVideosNone")  {
            vidSeqEndId =  parseResponse(r, videoFiles, videoPoems, videoDesc, timeStartEnd);
        //
        } else if (response.request.name == "getVideosPathAdd")  {
            vidSeqEndId =  parseResponse(r, videoFiles, videoPoems, videoDesc, timeStartEnd);
            videoSequence.add(videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidfadeDuration, false, true, false);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        } else if (response.request.name == "getVideosPathLoad")  {
            vidSeqEndId =  parseResponse(r, videoFiles, videoPoems, videoDesc, timeStartEnd);
            videoSequence.load(videoFiles, videoPoems, videoDesc, timeStartEnd, vidSeqSimVidfadeDuration, true);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        } else if (response.request.name == "getVideosPathSave")  {
            vidSeqEndId =  parseResponse(r, videoFilesSaved, videoPoemsSaved, videoDescSaved, timeStartEndSaved);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        } else if (response.request.name == "getVideosPathNone")  {
            vidSeqEndId =  parseResponse(r, videoFiles, videoPoems, videoDesc, timeStartEnd);
            vidSeqOrder = ( vidSeqOrder == "StartToEnd" ) ? "EndToStart" : "StartToEnd";
        }
    } else {
        cout << response.status << " " << response.error << endl;
        if (response.status != -1) loading = false;
    }
}

//--------------------------------------------------------------
void ofApp::startSoundMode(int mode){
    if (soundMode != mode) {
        totalTime = elapsedTime;
        startTime = ofGetElapsedTimeMillis();
        lastmorphAmount1 = morphAmount1;
        lastmorphAmount2 = morphAmount2;
        soundOut.startMorph();
        soundMode = mode;
        cout << soundMode << endl;
    }
}

//--------------------------------------------------------------
void ofApp::updateSoundMode(int mode, float & m1, float & m2){
    // Ascent :)
    if (mode == 4) {
        if (elapsedTime < ascendDuration2) elapsedTime = totalTime + (ofGetElapsedTimeMillis() - startTime);
        morphAmount1 = ofMap(elapsedTime, totalTime, ascendDuration2, lastmorphAmount1, 2);
        morphAmount2 = ofMap(elapsedTime, totalTime+ascendDuration1, ascendDuration2, lastmorphAmount2, 1);
        // Descent :(
    } else if (mode == 1) {
        if ( elapsedTime < totalTime - descentDuration) elapsedTime = 0;
        if (elapsedTime > 0) elapsedTime = totalTime - (ofGetElapsedTimeMillis() - startTime);
        morphAmount1 = ofMap(elapsedTime, totalTime, max(0, totalTime-descentDuration), lastmorphAmount1, 0);
        morphAmount2 = ofMap(elapsedTime, totalTime, max(0, totalTime-descentDuration), lastmorphAmount2, 0);
        // Drop :D
    } else if (mode == 5) {
        float t = (ofGetElapsedTimeMillis() - startTime);
        if ( t > tripDuration) {
            morphAmount2 = ofMap(t, tripDuration, tripDuration+dropDuration, lastmorphAmount2, 0);
        }
        morphAmount1 = 1;
    }
    // morphAmount 0 --> 2 (clamp it)
    // morphAmount1 = short, morphAmount2 = long
    morphAmount1 = ofClamp(morphAmount1, 0, 1);
    morphAmount2 = ofClamp(morphAmount2, 0, 1);
    //    morphAmount1 = easing.easeInQuad(morphAmount1);
    //    morphAmount2 = easing.easeInQuad(morphAmount2);
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
