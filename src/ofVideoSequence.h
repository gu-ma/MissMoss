//
//  ofVideoSequence.h
//  test_json_videplayer
//
//  Created by Guillaume on 19.06.18.
//


class ofVideoSequence : public ofBaseApp {
    
public:
    
    class VideoLoop {
    private:
        
        float start_pct, end_pct, fade_start_pct;
        bool playing, finished, fade, showPoem;
        ofVideoPlayer videoPlayer;
        ofFbo movieFbo;
        string poem;
        LogAudio tts;
        ofTrueTypeFont font;

    public:

        void init(){
            this->tts.start();
            font.load("Botanika-3-Lite.otf", 34);
        }
        
        void load(string videoFile, string videoPoem, int timeStart, int timeEnd, bool loop, float fadeDuration){
            this->playing = false;
            this->finished = false;
            this->fade = false;
            this->poem = videoPoem;
            this->showPoem = ofRandomf()>.5;
            this->videoPlayer.load(videoFile);
            this->videoPlayer.setVolume(0);
            if (loop) this->videoPlayer.setLoopState(OF_LOOP_NORMAL);
            this->start_pct = timeStart / this->videoPlayer.getDuration();
            this->end_pct = timeEnd / this->videoPlayer.getDuration();
            float fade_duration_pct = fadeDuration / this->videoPlayer.getDuration();
            this->fade_start_pct = ( this->end_pct-fade_duration_pct > this->start_pct ) ? this->end_pct-fade_duration_pct : this->start_pct;
            this->videoPlayer.setPosition(this->start_pct);
            this->movieFbo.allocate(this->videoPlayer.getWidth(), this->videoPlayer.getHeight());
            //            cout << videoFile << endl;
            //            cout << this->start_pct << endl;
            //            cout << this->end_pct << endl;
            //            cout << this->fade_start_pct << endl;
        }
        
        void update(){
            if ( this->videoPlayer.isLoaded() && this->videoPlayer.isInitialized() ) {
                if ( this->videoPlayer.getPosition() < this->start_pct ) this->videoPlayer.setPosition(this->start_pct);
                if ( this->videoPlayer.getPosition() >= this->fade_start_pct ) this->fade = true;
                if ( this->videoPlayer.getPosition() >= this->end_pct || this->videoPlayer.getPosition() >= 1 ) {
                    this->finished = true;
                    this->stop();
                }
            }
            this->videoPlayer.update();
        }
        
        void draw(int x, int y, int w, int h) {
            if ( this->videoPlayer.isPlaying() ) {
                this->movieFbo.begin();
                this->videoPlayer.draw(0,0);
                ofClearAlpha();
                this->movieFbo.end();
                ofRectangle previewWindow(x, y, w, h);
                ofRectangle videoGrabberRect(0, 0, videoPlayer.getWidth(), videoPlayer.getHeight());
                videoGrabberRect.scaleTo(previewWindow, OF_SCALEMODE_FILL);
                int a = ofMap(this->videoPlayer.getPosition(), this->fade_start_pct, this->end_pct, 300, 0);
                ofSetColor(255,255,255,a);
                this->movieFbo.draw(videoGrabberRect);
                if (this->showPoem) font.drawString(this->poem, 60, ofGetHeight()-80);
                ofSetColor(255,255,255,255);
            }
        }
        
        void start() {
            if ( !this->playing ) {
                this->videoPlayer.play();
                if ( !this->tts.startSpeaking && this->showPoem) {
                    //                  _voice, _pbas, _pmod, _rate, _volm, _rawMsg
                    this->tts.logAudio("Fred", "", "", "130", "0.5", this->poem);
                }
                this->playing = true;
            }
        }
        
        void stop() {
            if ( this->playing ) {
                this->videoPlayer.stop();
                this->playing = false;
            }
        }
        
        bool isPlaying() { return this->playing; }
        bool isFinished() { return this->finished; }
        bool fadeStarted() { return this->fade; }

    };
    
    vector<string> videoFiles;
    vector<string> videoPoems;
    vector<string> videoDesc;
    vector<ofVec2f> timeStartEnd;
    int index, numScenes, activeLoop;
    int indexLoopStart, indexLoopEnd;
    float fadeDuration;
    VideoLoop vidLoop1, vidLoop2;
    bool finished, loop, initialised;
    
    void init() {
        this->vidLoop1.init();
        this->vidLoop2.init();
    }
    
    void load(vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd, float fadeDuration, bool loop){
        this->videoFiles = videoFiles;
        this->videoPoems = videoPoems;
        this->videoDesc = videoDesc;
        this->timeStartEnd = timeStartEnd;
        this->numScenes = videoFiles.size();
        this->fadeDuration = fadeDuration;
        this->index = 0;
        this->finished = false;
        this->initialised = true;
        this->loop = loop;
        this->indexLoopStart = 0;
        this->indexLoopEnd = numScenes-1;
        loadNextVideo(this->vidLoop1, this->fadeDuration);
        loadNextVideo(this->vidLoop2, this->fadeDuration);
        this->vidLoop1.start();
        activeLoop = 1;
    }
    
    void add(vector<string> & videoFiles, vector<string> & videoPoems, vector<string> & videoDesc, vector<ofVec2f> & timeStartEnd, float fadeDuration, bool loop, bool continuous, bool clear){
        this->indexLoopStart = 0;
        int indexInsert = this->index;
//        if (clear && this->initialised) {
//            this->videoFiles.erase (this->videoFiles.begin(), this->videoFiles.begin()+this->index);
//            this->videoPoems.erase (this->videoPoems.begin(), this->videoPoems.begin()+this->index);
//            this->videoDesc.erase (this->videoDesc.begin(), this->videoDesc.begin()+this->index);
//            this->timeStartEnd.erase (this->timeStartEnd.begin(), this->timeStartEnd.begin()+this->index);
//            indexInsert = 0;
//        }
        if (continuous && this->initialised) {
            this->videoFiles.erase (this->videoFiles.begin()+indexInsert, this->videoFiles.end());
            this->videoPoems.erase (this->videoPoems.begin()+indexInsert, this->videoPoems.end());
            this->videoDesc.erase (this->videoDesc.begin()+indexInsert, this->videoDesc.end());
            this->timeStartEnd.erase (this->timeStartEnd.begin()+indexInsert, this->timeStartEnd.end());
            this->indexLoopStart = indexInsert;
        }
        this->videoFiles.insert(this->videoFiles.end(), videoFiles.begin(), videoFiles.end());
        this->videoPoems.insert(this->videoPoems.end(), videoPoems.begin(), videoPoems.end());
        this->videoDesc.insert(this->videoDesc.end(), videoDesc.begin(), videoDesc.end());
        this->timeStartEnd.insert(this->timeStartEnd.end(), timeStartEnd.begin(), timeStartEnd.end());
        this->numScenes = this->videoFiles.size();
        this->indexLoopEnd = numScenes-1;
        this->fadeDuration = fadeDuration;
        this->finished = false;
        this->initialised = true;
        this->loop = loop;
        this->indexLoopEnd = numScenes-1;

    }
    
    void update() {
        if ( this->initialised ) {
            if ( this->vidLoop1.fadeStarted() ) this->vidLoop2.start();
            if ( this->vidLoop1.isFinished() ) {
                loadNextVideo(this->vidLoop1, this->fadeDuration);
                activeLoop = 2;
            }
            if ( this->vidLoop2.fadeStarted() ) this->vidLoop1.start();
            if ( this->vidLoop2.isFinished() ) {
                loadNextVideo(this->vidLoop2, this->fadeDuration);
                activeLoop = 1;
            }
            this->vidLoop1.update();
            this->vidLoop2.update();
        }
    }
    
    void draw(int x, int y, int w, int h, float morphAmount1) {
        if (activeLoop == 1){
            this->vidLoop2.draw(x, y, w, h);
            this->vidLoop1.draw(x, y, w, h);
        } else {
            this->vidLoop1.draw(x, y, w, h);
            this->vidLoop2.draw(x, y, w, h);
        }
        int a = ofMap(morphAmount1*8, 0, 1, 150, 0);
        a = ofClamp(a, 0, 255);
        ofSetColor(0,0,0,a);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        ofSetColor(255,255,255,255);
    }
    
    void loadNextVideo(VideoLoop & loop, float fadeDuration){
        loop.load( this->videoFiles[this->index], this->videoPoems[this->index], this->timeStartEnd[this->index][0], this->timeStartEnd[this->index][1], true, fadeDuration);
        if ( this->index == this->numScenes-1 ) this->finished = true;
        // Set into loop mode ONLY when adding files repeatly
        if ( this->loop ) this->index = ( this->index < this->indexLoopEnd ) ? this->index+1 : this->indexLoopStart ;
        else this->index++;
        cout << "Index\t\tSize\t\tFinished" << endl;
        cout << ofToString(this->index) + "\t\t\t" + ofToString(this->numScenes) + "\t\t" + ofToString(this->finished) << endl;
    }
    
    bool isFinished() { return this->finished; }
    
};
