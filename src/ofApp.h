#pragma once

#include "ofMain.h"
#include "ofxLtcReader.h"
#include "ofxOMXPlayer.h"

class ofApp : public ofBaseApp , public ofxOMXPlayerListener{

	public:
		void setup();
		void update();
		void draw();

	        void audioIn(float * input, int bufferSize, int nChannels);
		void keyPressed (int key);
		void onVideoEnd(ofxOMXPlayer *player);
		void onVideoLoop(ofxOMXPlayer *player);

		void doRestart();

		ofSoundStream snd;
    		ofxLtcReader reader;
		ofxOMXPlayer player;
		ofxOMXPlayerSettings settings;		

		float prevLTC, movFrame;
		float drift, absDrift;
		int frameTarget;
		float modFrame;
		float ltcFrame;
		int length, reloads;
		bool bRestart, bSpeedUp, bSlowDown, bCanSeek;

};
