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
    size_t index, numScenes, activeLoop;
    VideoLoop loop1, loop2;
    bool finished;
    
    void init(vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd){
        this->videoFiles.clear();
        this->timeStartEnd.clear();
        this->videoFiles = videoFiles;
        this->timeStartEnd = timeStartEnd;
        this->numScenes = videoFiles.size();
        this->index = 0;
        this->finished = false;
        loadNextVideo(this->loop1);
        loadNextVideo(this->loop2);
        this->loop1.start();
        activeLoop = 1;
    }
    
    void add(vector<string> & videoFiles, vector<ofVec2f> & timeStartEnd){
        this->videoFiles.insert(this->videoFiles.end(), videoFiles.begin(), videoFiles.end());
        this->timeStartEnd.insert(this->timeStartEnd.end(), timeStartEnd.begin(), timeStartEnd.end());
        this->numScenes = this->videoFiles.size();
        this->finished = false;
    }
    
    void update() {
        if ( this->loop1.fadeStarted() ) this->loop2.start();
        if ( this->loop1.isFinished() ) {
            loadNextVideo(this->loop1);
            activeLoop = 2;
        }
        if ( this->loop2.fadeStarted() ) this->loop1.start();
        if ( this->loop2.isFinished() ) {
            loadNextVideo(this->loop2);
            activeLoop = 1;
        }
        
        this->loop1.update();
        this->loop2.update();
    }
    
    void draw(int x, int y, int w, int h) {
        if (activeLoop == 1){
            this->loop2.draw(x, y, w, h);
            this->loop1.draw(x, y, w, h);
        } else {
            this->loop1.draw(x, y, w, h);
            this->loop2.draw(x, y, w, h);
        }
    }
    
    void loadNextVideo(VideoLoop & loop){
        loop.init( this->videoFiles[index], this->timeStartEnd[index][0], this->timeStartEnd[index][1], true, 2 );
        if ( this->index == this->numScenes-1 ) this->finished = true;
        this->index = ( this->index < this->numScenes-1 ) ? this->index+1 : 0 ;
        cout << "Index:\t\t\t" + ofToString(this->index) << endl;
        cout << "VideoFiles Size:\t" + ofToString(this->videoFiles.size()) << endl;
        cout << "NumScenes:\t\t" + ofToString(this->numScenes) << endl;
        cout << "Finished:\t\t\t" + ofToString(this->finished) << endl;
//        this->index ++;
    }
    
    bool isFinished() { return this->finished; }
    
};
