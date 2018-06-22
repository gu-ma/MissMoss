//
//  ofSoundOut.h
//  ESPOscReceiver
//
//  Created by Guillaume on 14.06.18.
//

#include "ofxAudioUnit.h"

class ofSoundOut : public ofBaseApp {
    
public:

    ofxAudioUnit compressor, delay, distortion, lowpass1, lowpass2;
    
    ofxAudioUnitFilePlayer source1, source2, source3, source4;
    ofxAudioUnitMixer mixer1, mixer2;
    ofxAudioUnitOutput outputMain, outputBass1;
    
    ofxAudioUnitTap tap1, tap2, tap3, tap4;
    
    float variation1;
    
    void setup() {
        
        source1.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));
        source2.setFile(ofToDataPath("Test-Basss+Kick_Kick.aif"));
        source3.setFile(ofToDataPath("Trout Lake North.mp3"));
        source4.setFile(ofToDataPath("Test-Basss+Kick_Bass.aif"));

        distortion.setup(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
        // DELAY
        AudioUnitSetParameter(distortion, kDistortionParam_Delay, kAudioUnitScope_Global, 0, 60, 0);
        AudioUnitSetParameter(distortion, kDistortionParam_Decay, kAudioUnitScope_Global, 0, 20, 0);
        // 0 --> 10
        AudioUnitSetParameter(distortion, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, 0, 0);
        // DECIMATION
        AudioUnitSetParameter(distortion, kDistortionParam_Decimation, kAudioUnitScope_Global, 0, 2.5, 0);
        AudioUnitSetParameter(distortion, kDistortionParam_Rounding, kAudioUnitScope_Global, 0, 80, 0);
        // 0 --> 20
        AudioUnitSetParameter(distortion, kDistortionParam_DecimationMix, kAudioUnitScope_Global, 0, 0, 0);
        // RINGMOD
        AudioUnitSetParameter(distortion, kDistortionParam_RingModFreq1, kAudioUnitScope_Global, 0, 230, 0);
        AudioUnitSetParameter(distortion, kDistortionParam_RingModFreq2, kAudioUnitScope_Global, 0, 280, 0);
        AudioUnitSetParameter(distortion, kDistortionParam_RingModMix, kAudioUnitScope_Global, 0, 0, 0);
        // GENERAL
        AudioUnitSetParameter(distortion, kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, 0 , 0);
        delay.setup(kAudioUnitType_Effect, kAudioUnitSubType_Delay);
        lowpass1.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
        lowpass2.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
        AudioUnitSetParameter(lowpass2, kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 70, 0);
        compressor.setup(kAudioUnitType_Effect, kAudioUnitSubType_DynamicsProcessor);

        // Mixer 1
        mixer1.setInputBusCount(3);
//        outputMain.setDevice(47);
        source1.connectTo(distortion).connectTo(tap1).connectTo(mixer1, 0);
        source2.connectTo(delay).connectTo(tap2).connectTo(mixer1, 1);
        source3.connectTo(lowpass1).connectTo(tap3).connectTo(mixer1, 2);
        //
        mixer1.connectTo(compressor).connectTo(outputMain);
        //
        mixer1.setInputVolume(0, 0);
        mixer1.setInputVolume(0, 1);
        mixer1.setInputVolume(1, 2);
        
        // Mixer 2
        cout << "\n[ofxAudioUnit] Output devices\n";
        outputBass1.listOutputDevices();
        outputBass1.setDevice(62); // Bluetooth bass
        //
        mixer2.setInputBusCount(1);
        source4.connectTo(lowpass2).connectTo(tap4).connectTo(mixer2, 0);
        mixer2.connectTo(outputBass1);
        mixer1.setInputVolume(0.1, 0);
        
        outputMain.start();
        outputBass1.start();
        
        source1.loop();
        source2.loop();
        source3.loop();
        source4.loop();
        

    }
    
    void update() {
        
        float volume = ofClamp(variation1, 0, 1);
//        cout << volume << endl;

        mixer1.setInputVolume(volume, 0);
        mixer1.setInputVolume(volume, 1);
        mixer1.setInputVolume(1-volume, 2);
        mixer2.setInputVolume(volume, 0);

//        float distoRingModMix = ofClamp(variation1*10, 0, 100);
//
//        AudioUnitSetParameter(distortion,
//                              kDistortionParam_RingModMix,
//                              kAudioUnitScope_Global,
//                              0,
//                              distoRingModMix,
//                              0);
//
//        float distoFinalMix = ofClamp(variation1*30, 0, 100);
//
//        AudioUnitSetParameter(distortion,
//                              kDistortionParam_FinalMix,
//                              kAudioUnitScope_Global,
//                              0,
//                              distoFinalMix,
//                              0);
//
//        float lowpassResonance = ofClamp(variation1*6, 0, 6);
//
//        AudioUnitSetParameter(lowpass2,
//                              kLowPassParam_Resonance,
//                              kAudioUnitScope_Global,
//                              0,
//                              lowpassResonance,
//                              0);

    }
    
};
