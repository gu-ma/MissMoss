//
//  ofSoundOut.h
//  ESPOscReceiver
//
//  Created by Guillaume on 14.06.18.
//

#include "ofxAudioUnit.h"

class ofSoundOut : public ofBaseApp {
    
public:

    ofxAudioUnit compressor, delay, distortion1, distortion2, lowpass1, lowpass2;
    
    ofxAudioUnitFilePlayer source1, source2, source3, source4;
    ofxAudioUnitMixer mixer1, mixer2;
    ofxAudioUnitOutput outputMain, outputBass1;
    
    ofxAudioUnitTap tap1, tap2, tap3, tap4;
    
    float morphAmount1, morphAmount2;
    bool doMorph;
    
    void setup() {
        
        source1.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));
        source2.setFile(ofToDataPath("Test-Basss+Kick_Kick.aif"));
        source3.setFile(ofToDataPath("Trout Lake North.mp3"));
        source4.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));
        //
        distortion1.setup(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
        distortion2.setup(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
        delay.setup(kAudioUnitType_Effect, kAudioUnitSubType_Delay);
        lowpass1.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
        lowpass2.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
        compressor.setup(kAudioUnitType_Effect, kAudioUnitSubType_DynamicsProcessor);
        
        // Mixer 1
        mixer1.setInputBusCount(3);
//        outputMain.setDevice(47);
//        source1.connectTo(distortion).connectTo(tap1).connectTo(mixer1, 0);
//        source2.connectTo(delay).connectTo(tap2).connectTo(mixer1, 1);
//        source3.connectTo(lowpass1).connectTo(tap3).connectTo(mixer1, 2);
        source1.connectTo(distortion1).connectTo(lowpass1).connectTo(tap1).connectTo(mixer1, 0);
        source2.connectTo(distortion2).connectTo(tap2).connectTo(mixer1, 1);
        source3.connectTo(tap3).connectTo(mixer1, 2);
        mixer1.connectTo(compressor).connectTo(outputMain);
        
        // Mixer 2
        cout << "\n[ofxAudioUnit] Output devices\n";
        outputBass1.listOutputDevices();
        outputBass1.setDevice(62); // Bluetooth bass
        //
        mixer2.setInputBusCount(1);
        source4.connectTo(lowpass2).connectTo(tap4).connectTo(mixer2, 0);
        mixer2.connectTo(outputBass1);
        
        initSettings();
        
        outputMain.start();
        outputBass1.start();
        
        source1.loop();
        source2.loop();
        source3.loop();
        source4.loop();
        
        // Misc
        startMorph();
        morph(0, 0);

    }
    
    void update() {
        if (doMorph) morphSettings(morphAmount1, morphAmount2);
    }
    
    void startMorph() { doMorph = true; }
    void stopMorph() { doMorph = false; }
    void morph(float morphAmount1, float morphAmount2) {
        this->morphAmount1 = morphAmount1;
        this->morphAmount2 = morphAmount2;
    }
    
    void initSettings() {
        // Volume
        mixer1.setInputVolume(0, 0);
        mixer1.setInputVolume(0, 1);
        mixer1.setInputVolume(1, 2);
        mixer2.setInputVolume(0.1, 0);
        // ------- DISTO
        // DELAY
        AudioUnitSetParameter(distortion1, kDistortionParam_Delay, kAudioUnitScope_Global, 0, 160, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_Decay, kAudioUnitScope_Global, 0, 20, 0);
        // 0 --> 10
        AudioUnitSetParameter(distortion1, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, 0, 0);
        // DECIMATION
        AudioUnitSetParameter(distortion1, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, 2.5, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, 80, 0);
        // 0 --> 20
        AudioUnitSetParameter(distortion1, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, 0, 0);
        // RINGMOD
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModFreq1, kAudioUnitScope_Global, 0, 230, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModFreq2, kAudioUnitScope_Global, 0, 280, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, 0, 0);
        // GENERAL
        AudioUnitSetParameter(distortion1, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, 0, 0);
        // ------- DISTO 02
        // DELAY
        AudioUnitSetParameter(distortion2, kDistortionParam_Delay, kAudioUnitScope_Global, 0, 60, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_Decay, kAudioUnitScope_Global, 0, 10, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, 0, 0);
        // DECIMATION
        AudioUnitSetParameter(distortion2, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, .5, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, 10, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, 0, 0);
        // RINGMOD
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModFreq1, kAudioUnitScope_Global, 0, 308, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModFreq2, kAudioUnitScope_Global, 0, 305, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, 0, 0);
        // GENERAL
        AudioUnitSetParameter(distortion2, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, 0, 0);
        // ------- LOWPASS
        AudioUnitSetParameter(lowpass1, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);
        AudioUnitSetParameter(lowpass2, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);

        // ------- LOWPASS
        AudioUnitSetParameter(lowpass1, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);
        AudioUnitSetParameter(lowpass2, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);
    }
    
    void morphSettings(float m1, float m2) {
        // 2x faster
        float m3 = ofMap(m1, 0, 1, 0, 2);
        m3 = ofClamp(m3, 0, 1);
        //
        // Volume
        mixer1.setInputVolume(m3, 0);
        mixer1.setInputVolume(m3, 1);
        mixer1.setInputVolume(1-m3, 2);
        mixer2.setInputVolume(m3, 0);
        // ------- DISTO 01 (BASS)
        // DELAY
//        AudioUnitSetParameter(distortion1, kDistortionParam_Delay, kAudioUnitScope_Global, 0, ofMap(morphAmount, 0, 1, 60, 80), 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_Decay, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 20, 40), 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, ofMap(m1, 0, 1, 0, 20), 0);
        // DECIMATION
        AudioUnitSetParameter(distortion1, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 2.5, 5), 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 80, 60), 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
        // RINGMOD
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
        // GENERAL
        AudioUnitSetParameter(distortion1, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
        // ------- DISTO 02 (KICK)
        // DELAY
        AudioUnitSetParameter(distortion2, kDistortionParam_Decay, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 10, 20), 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
        // DECIMATION
        AudioUnitSetParameter(distortion2, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 10), 0);
        // RINGMOD
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
        // GENERAL
        AudioUnitSetParameter(distortion2, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
        // ------- LOWPASS 01 (BASS) + 02 (BASS SOUNDWAVER)
        AudioUnitSetParameter(lowpass1, kLowPassParam_Resonance, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 7), 0);
        AudioUnitSetParameter(lowpass2, kLowPassParam_Resonance, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 7), 0);
    }
    
};
