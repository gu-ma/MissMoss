//
//  ofEsp.h
//  ESPOscReceiver
//
//  Created by Guillaume on 14.06.18.
//
#include "ofxOsc.h"

// listen on port 12345
#define OSC_PORT 12345
#define OSC_NUM_MSG_STRINGS 20

class ofEsp : public ofBaseApp {

public:
    
    ofTrueTypeFont font;
    
    //---- OSC
    ofxOscReceiver osc_receiver;
    
    int current_msg_string;
    string msg_strings[OSC_NUM_MSG_STRINGS];
    float timers[OSC_NUM_MSG_STRINGS];
    
    vector<double> data_point;
    int data_point_var;
    int data_point_avg;
    int data_point_max_smoothed;
    int data_point_min_smoothed;
    int predicted_label;
    float data_point_var_norm, data_point_var_norm_smoothed;
    
    ofPolyline data_point_line;

    //----- Sound
    ofSoundStream soundStream;
    
    float pan;
    int sampleRate;
    bool bNoise;
    float volume;
    
    vector <float> lAudio;
    vector <float> rAudio;
    
    //------------------- for the simple sine wave synthesis
    float targetFrequency;
    float phase;
    float phaseAdder;
    float phaseAdderTarget;

    //--------------------------------------------------------------
    void setup() {
        
        font.load("Botanika-5-Demi.otf", 198);
        
        //----- OSC
        // listen on the given port
        cout << "\n[ESP] listening for osc messages on port " << OSC_PORT << "\n";
        osc_receiver.setup(OSC_PORT);
        
        //----- SOUND
        // 2 output channels,
        // 0 input channels
        // 44100 samples per second
        // 512 samples per buffer
        // 4 num buffers (latency)
        
        int bufferSize = 512;
        sampleRate = 44100;
        phase = 0;
        phaseAdder = 0.0f;
        phaseAdderTarget = 0.0f;
        volume = 0.015f;
        bNoise = true;
        
        lAudio.assign(bufferSize, 0.0);
        rAudio.assign(bufferSize, 0.0);
        
        soundStream.printDeviceList();
        
        //if you want to set the device id to be different than the default
        //    soundStream.setDeviceID(1);     //note some devices are input only and some are output only
        
        soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);
        
        // on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
        // use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
        soundStream.getDeviceList();
    }
    
    //--------------------------------------------------------------
    void update(){
        
        //----- OSC
        // hide old messages
        for(int i = 0; i < OSC_NUM_MSG_STRINGS; i++){
            if(timers[i] < ofGetElapsedTimef()){
                msg_strings[i] = "";
            }
        }
        
        // check for waiting messages
        while(osc_receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            osc_receiver.getNextMessage(m);
            
            // data point average
            if(m.getAddress() == "/data_point/avg") data_point_avg = m.getArgAsInt32(0);
            
            // data point variation norm
            else if(m.getAddress() == "/data_point/var_norm") data_point_var_norm = m.getArgAsFloat(0);

            // data point variation norm smoothed
            else if(m.getAddress() == "/data_point/var_norm_smoothed") data_point_var_norm_smoothed = m.getArgAsFloat(0);

            // data point variation max smoothed
            else if(m.getAddress() == "/data_point/max_smoothed") data_point_max_smoothed = m.getArgAsInt32(0);
            
            // data point variation min smoothed
            else if(m.getAddress() == "/data_point/min_smoothed") data_point_min_smoothed = m.getArgAsInt32(0);
            
            // data point variation
            else if(m.getAddress() == "/data_point/var") data_point_var = m.getArgAsInt32(0);
            
            // predicted label
            else if(m.getAddress() == "/predicted_label") predicted_label = m.getArgAsInt32(0);
            
            else if(m.getAddress() == "/data_point/vector"){
                data_point_line.clear();
                int count = m.getNumArgs();
                int step = int( ofGetWidth()/count );
                for(int i = 0; i < count; i++){
                    if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                        data_point_line.addVertex(i*step, ofMap(m.getArgAsInt32(i), 0, 800, ofGetHeight(), 0));
                    }
                }
            }
            cout << data_point_max_smoothed << endl;

        }
        
        //----- SOUND
        
//        pan = (float)ofGetMouseX() / (float)ofGetWidth();
//        float height = (float)ofGetHeight();
//        float heightPct = ((height-ofGetMouseY()) / height);
        targetFrequency = 100 + 500.0f * data_point_var_norm;
        phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
    }
    
    //--------------------------------------------------------------
    void draw(float amount){
        
        float a = ofMap(amount, 0, 1, 255, -255*16);
        ofSetColor(255,255,255,ofClamp(a, 0, 255));
        string buf;
        buf = "listening for osc messages on port " + ofToString(OSC_PORT);
        ofDrawBitmapString(buf, 60, 70);
        
        a = ofMap(amount, 0, 1, 255, -255*8);
        ofSetColor(255,255,255,ofClamp(a, 0, 255));
        buf = "data point average: " + ofToString(data_point_avg);
        buf += "\ndata point Variation: " + ofToString(data_point_var);
        buf += "\ndata point Variation norm: " + ofToString(data_point_var_norm);
        buf += "\ndata point Max smoothed: " + ofToString(data_point_max_smoothed);
        buf += "\ndata point Min smoothed: " + ofToString(data_point_min_smoothed);
        buf += "\ndata point Variation norm smoothed: " + ofToString(data_point_var_norm_smoothed);
        buf += "\nPredicted Label: " + ofToString(predicted_label);
        buf += "\n\nphaseAdderTarget: " + ofToString(phaseAdderTarget);
        ofDrawBitmapString(buf, 60, 90);

        a = ofMap(amount, 0, 1, 255, -255*4);
        ofSetColor(255,255,255,ofClamp(a, 0, 255));
        string txt;
        if ( predicted_label == 1 ) txt = "No hands";
        else if ( predicted_label == 2 ) txt = "Hovering";
        else if ( predicted_label == 3 ) txt = "Tickling";
        else if ( predicted_label == 4 ) txt = "Touching";
        font.drawString(txt, 50, 420);
        
        data_point_line.getSmoothed(5).draw();
        ofSetColor(255,255,255,255);

    }
    
    //--------------------------------------------------------------
    void audioOut(float * output, int bufferSize, int nChannels){
        pan = 0.5f;
        float leftScale = 1 - pan;
        float rightScale = pan;
        
        // sin (n) seems to have trouble when n is very large, so we
        // keep phase in the range of 0-TWO_PI like this:
        while (phase > TWO_PI){
            phase -= TWO_PI;
        }
        
        phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
        
        if ( bNoise == true){
            // ---------------------- noise --------------
            for (int i = 0; i < bufferSize; i++){
                phase += phaseAdder;
                float sample = sin(phase);
                float r01 = ofRandom(0, 1 - sample);
                float r02 = ofRandom(0, 1 - sample);
                lAudio[i] = output[i*nChannels    ] = (sample + r01) * volume * leftScale;
                rAudio[i] = output[i*nChannels + 1] = (sample + r02) * volume * rightScale;
            }
        } else {
            for (int i = 0; i < bufferSize; i++){
                phase += phaseAdder;
                float sample = sin(phase);
                lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
                rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
            }
        }
        
    }

    
    
};
