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
        bool finished, fade;
        ofVideoPlayer videoPlayer;
        ofFbo movieFbo;
        
    public:
        
        void init(string videoFile, int timeStart, int timeEnd, bool loop, float fadeDuration){
            this->finished = false;
            this->fade = false;
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
                ofSetColor(255,255,255,255);
            }
        }
        
        void start() { this->videoPlayer.play(); }
        void stop() { this->videoPlayer.stop(); }
        bool isPlaying() { return this->videoPlayer.isPlaying(); }
        bool isFinished() { return this->finished; }
        bool fadeStarted() { return this->fade; }

    };
    
    vector<string> videoFiles;
    vector<ofVec2f> timeStartEnd;
    int index, numScenes, activeLoop;
    float fadeDuration;
    VideoLoop vidLoop1, vidLoop2;
    bool finished, loop;
    
    void init(vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd, float fadeDuration, bool loop){
        this->videoFiles.clear();
        this->timeStartEnd.clear();
        this->videoFiles = videoFiles;
        this->timeStartEnd = timeStartEnd;
        this->numScenes = videoFiles.size();
        this->fadeDuration = fadeDuration;
        this->index = 0;
        this->finished = false;
        this->loop = loop;
        loadNextVideo(this->vidLoop1, this->fadeDuration);
        loadNextVideo(this->vidLoop2, this->fadeDuration);
        this->vidLoop1.start();
        activeLoop = 1;
    }
    
    void add(vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd, float fadeDuration, bool loop, bool continuous){
        if (continuous) {
            this->videoFiles.erase (this->videoFiles.begin()+index, this->videoFiles.end());
            this->timeStartEnd.erase (this->timeStartEnd.begin()+index, this->timeStartEnd.end());
        }
        this->videoFiles.insert(this->videoFiles.end(), videoFiles.begin(), videoFiles.end());
        this->timeStartEnd.insert(this->timeStartEnd.end(), timeStartEnd.begin(), timeStartEnd.end());
        this->numScenes = this->videoFiles.size();
        this->fadeDuration = fadeDuration;
        this->finished = false;
        this->loop = loop;
    }
    
    void update() {
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
    
    void draw(int x, int y, int w, int h) {
        if (activeLoop == 1){
            this->vidLoop2.draw(x, y, w, h);
            this->vidLoop1.draw(x, y, w, h);
        } else {
            this->vidLoop1.draw(x, y, w, h);
            this->vidLoop2.draw(x, y, w, h);
        }
    }
    
    void loadNextVideo(VideoLoop & loop, float fadeDuration){
        loop.init( this->videoFiles[index], this->timeStartEnd[index][0], this->timeStartEnd[index][1], true, fadeDuration);
        if ( this->index == this->numScenes-1 ) this->finished = true;
        // Set into loop mode ONLY when adding files repeatly
        if ( this->loop ) this->index = ( this->index < this->numScenes-1 ) ? this->index+1 : 0 ;
        else this->index++;
        cout << "Index:\t\t\t" + ofToString(this->index) << endl;
        cout << "VideoFiles Size:\t" + ofToString(this->videoFiles.size()) << endl;
        cout << "NumScenes:\t\t" + ofToString(this->numScenes) << endl;
        cout << "Finished:\t\t\t" + ofToString(this->finished) << endl;
//        this->index ++;
    }
    
    bool isFinished() { return this->finished; }
    
};
