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
    
    ofxAudioUnitFilePlayer source1, source2, source3, source4, source5, source6;
    ofxAudioUnitMixer mixer1, mixer2;
    ofxAudioUnitOutput outputMain, outputBass1;
    
    ofxAudioUnitTap tap1, tap2, tap3, tap4, tap5, tap6;
    
    float morphAmount1, morphAmount2;
    bool doMorph;
    
    void setup() {
        
        source1.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));
        source2.setFile(ofToDataPath("Test-Basss+Kick_Kick.aif"));
        source3.setFile(ofToDataPath("Trout Lake North.mp3"));
        source4.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));
        source5.setFile(ofToDataPath("Test-Basss+Kick_Kick.aif"));
        source6.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));
        //
        distortion1.setup(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
        distortion2.setup(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
        delay.setup(kAudioUnitType_Effect, kAudioUnitSubType_Delay);
        lowpass1.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
        lowpass2.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
        compressor.setup(kAudioUnitType_Effect, kAudioUnitSubType_DynamicsProcessor);

        // Mixer 1
        mixer1.setInputBusCount(5);
        source1.connectTo(distortion1).connectTo(tap1).connectTo(mixer1, 0); // Bass disto
        source2.connectTo(distortion2).connectTo(tap2).connectTo(mixer1, 1); // Kick disto
        source3.connectTo(tap3).connectTo(mixer1, 2); // Birds
        source4.connectTo(tap4).connectTo(mixer1, 3); // Bass Clean
        source5.connectTo(tap5).connectTo(mixer1, 4); // Kick clean
        mixer1.connectTo(compressor).connectTo(outputMain);
        
        // Mixer 2
        mixer2.setInputBusCount(1);
        source6.connectTo(lowpass2).connectTo(tap6).connectTo(mixer2, 0);
        mixer2.connectTo(outputBass1);
        
        cout << "\n[ofxAudioUnit] Output devices\n";
        outputBass1.listOutputDevices();
        outputMain.setDevice(217);   // MBP
        //        outputMain.setDevice(51);
        outputBass1.setDevice(203);  // MBP
        //        outputBass1.setDevice(217); // Bluetooth bass (iMAC)
        //        outputBass1.setDevice(66); // Bluetooth bass (iMAC)

        initSettings();
        
        outputMain.start();
        outputBass1.start();
        
        source1.loop();
        source2.loop();
        source3.loop();
        source4.loop();
        source5.loop();
        source6.loop();

    }
    
    void update(float morphAmount1, float morphAmount2, float maxVolume) {
        this->morphAmount1 = morphAmount1;
        this->morphAmount2 = morphAmount2;
        if (doMorph) morphSettings(morphAmount1, morphAmount2, maxVolume);
    }
    
    void startMorph() { doMorph = true; }
    void stopMorph() { doMorph = false; }
    
    void initSettings() {
        // Volume
        mixer1.setInputVolume(0, 0);
        mixer1.setInputVolume(0, 1);
        mixer1.setInputVolume(1, 2);
        mixer1.setInputVolume(0, 3);
        mixer1.setInputVolume(0, 4);
        //
        mixer2.setInputVolume(0.1, 0);
        //
        // ------- DISTO 01 (BASS)
        // DELAY
        AudioUnitSetParameter(distortion1, kDistortionParam_Delay, kAudioUnitScope_Global, 0, 118.706, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_Decay, kAudioUnitScope_Global, 0, 10, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, 22, 0);
        // RINGMOD
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModFreq1, kAudioUnitScope_Global, 0, 44, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModFreq2, kAudioUnitScope_Global, 0, 56, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, 32, 0);
        // DECIMATION
        AudioUnitSetParameter(distortion1, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, 2.5, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, 20, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, 12, 0);
        // GENERAL
        AudioUnitSetParameter(distortion1, kDistortionParam_LinearTerm, kAudioUnitScope_Global, 0, 0.4, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_SquaredTerm, kAudioUnitScope_Global, 0, 12.31, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_CubicTerm, kAudioUnitScope_Global, 0, 14, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_PolynomialMix, kAudioUnitScope_Global, 0, 100, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_SoftClipGain, kAudioUnitScope_Global, 0, -0.9, 0);
        AudioUnitSetParameter(distortion1, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, 20, 0);
        // ------- DISTO 02 (KICK)
        // DELAY
        AudioUnitSetParameter(distortion2, kDistortionParam_Delay, kAudioUnitScope_Global, 0, 59.353, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_Decay, kAudioUnitScope_Global, 0, 10, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, 20, 0);
        // RINGMOD
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModFreq1, kAudioUnitScope_Global, 0, 308.437, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModFreq2, kAudioUnitScope_Global, 0, 305.725, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, 20, 0);
        // DECIMATION
        AudioUnitSetParameter(distortion2, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, .271, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, 10, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, 4, 0);
        // GENERAL
        AudioUnitSetParameter(distortion2, kDistortionParam_LinearTerm, kAudioUnitScope_Global, 0, 0.385, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_SquaredTerm, kAudioUnitScope_Global, 0, 18.11, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_CubicTerm, kAudioUnitScope_Global, 0, 5, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_PolynomialMix, kAudioUnitScope_Global, 0, 100, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_SoftClipGain, kAudioUnitScope_Global, 0, -0.9, 0);
        AudioUnitSetParameter(distortion2, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, 20, 0);
        // ------- LOWPASS
        AudioUnitSetParameter(lowpass1, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);
        AudioUnitSetParameter(lowpass2, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);
        // ------- COMPRESSOR MIXER 1
        AudioUnitSetParameter(compressor, kDynamicsProcessorParam_Threshold, kAudioUnitScope_Global, 0, -18, 0);
        AudioUnitSetParameter(compressor, kDynamicsProcessorParam_HeadRoom, kAudioUnitScope_Global, 0, 18, 0);
    }
    
    void morphSettings(float m1, float m2, float maxVolume) {
        // 2x faster
        float m3 = ofMap(m1, 0, 1, 0, 2);
        m3 = ofClamp(m3, 0, 1);
        //
        // Volume
        mixer1.setInputVolume(ofMap(m2, 0, 1, 0, maxVolume), 0);
        mixer1.setInputVolume(ofMap(m2, 0, 1, 0, maxVolume/4), 1);
        mixer1.setInputVolume(1-m3, 2);
        mixer1.setInputVolume(ofMap(m3, 0, 1, 0, maxVolume), 3);
        mixer1.setInputVolume(ofMap(m3, 0, 1, 0, maxVolume), 4);
        //
        mixer2.setInputVolume(m3, 0);
        //
//        cout << ofMap(m2, 0, 1, 0, maxVolume) << endl;
//        cout << ofMap(m3, 0, 1, 0, maxVolume) << endl;
//        // ------- DISTO 01 (BASS)
//        // DELAY
////        AudioUnitSetParameter(distortion1, kDistortionParam_Decay, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 10), 0);
//        AudioUnitSetParameter(distortion1, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, ofMap(m1, 0, 1, 0, 25), 0);
//        // DECIMATION
//        AudioUnitSetParameter(distortion1, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 2.5, 5), 0);
//        AudioUnitSetParameter(distortion1, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
//        AudioUnitSetParameter(distortion1, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, .5), 0);
//        // RINGMOD
//        AudioUnitSetParameter(distortion1, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
//        // GENERAL
//        AudioUnitSetParameter(distortion1, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 10), 0);
//        // ------- DISTO 02 (KICK)
//        // DELAY
////        AudioUnitSetParameter(distortion2, kDistortionParam_Decay, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 10), 0);
//        AudioUnitSetParameter(distortion2, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
//        // DECIMATION
//        AudioUnitSetParameter(distortion2, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 5), 0);
//        // RINGMOD
//        AudioUnitSetParameter(distortion2, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 20), 0);
//        // GENERAL
//        AudioUnitSetParameter(distortion2, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 10), 0);
        // ------- LOWPASS 01 (BASS) + 02 (BASS SOUNDWAVER)
        AudioUnitSetParameter(lowpass1, kLowPassParam_Resonance, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 7), 0);
        AudioUnitSetParameter(lowpass2, kLowPassParam_Resonance, kAudioUnitScope_Global, 0, ofMap(m2, 0, 1, 0, 7), 0);
    }
    
};
