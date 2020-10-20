
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Setup an audio device with ofSoundStream
    snd.listDevices();
    snd.setDeviceID(3); // 3 is usb to analogue dongle
    snd.setup(this, 0, 2, 48000, 256, 4); // outputs,inputs,samplerate, buffer, num buffers 
    
    //Setup a ltc reader by passing a pointer to this ofSoundStream with expected framerate
    reader.setup(&snd,24);
    
    //setup OMXPlayer

    string videoPath = ofToDataPath("video/Timecoded_Big_bunny_1.mov", true);
    settings.videoPath = videoPath;
    settings.useHDMIForAudio = true;
    settings.enableLooping = true;
    settings.enableTexture = true;
    settings.listener = this;
    
    player.setup(settings);
    
    length = player.getTotalNumFrames();
    player.seekToFrame(0);
    
   bSpeedUp = false;
   bSlowDown = false;
   reloads = 0;
}

//--------------------------------------------------------------
void ofApp::onVideoEnd(ofxOMXPlayer *player)
{
}
//--------------------------------------------------------------
void ofApp::onVideoLoop(ofxOMXPlayer* player)
{
}
//--------------------------------------------------------------

void ofApp::update(){
    
ltcFrame = (3600 * reader.ltcHour()
               + 60 * reader.ltcMinute()
               + reader.ltcSecond()) * 24
               + reader.ltcFrame();

if(prevLTC == ltcFrame)
  {
   player.setPaused(true);
  } 
else 
  {
  player.setPaused(false);
  }

if(!player.getTotalNumFrames())
{
 player.reopen();
 reloads++;
} 

else 
{

if(player.isOpen())
  {
	modFrame = (ltcFrame % length);
	drift = ( modFrame - (player.getCurrentFrame()) );

	int absDrift = abs(drift);

	if (absDrift < 1 && (player.engine.currentSpeed != player.engine.normalSpeedIndex)) 
 	{
    	player.setNormalSpeed();
    	bSpeedUp = false;
    	bSlowDown = false;
 	}

	else

	{
	if(absDrift > 24)
	 {
  	  player.setPaused(true);
  	  player.seekToFrame(modFrame);
  	  player.setPaused(false);
 	 }
	}

	if(drift > 1 && (!bSpeedUp))
  	{
   	 frameTarget = (ltcFrame % length);
  	 player.engine.currentSpeed = 6;
  	 player.engine.SetSpeed();
  	 bSpeedUp = true;
  	 bSlowDown = false;
    	}

	if(drift < -1 && (!bSlowDown))
  	{
   	 frameTarget = (ltcFrame % length);
  	 player.engine.currentSpeed = 0;
  	 player.engine.SetSpeed();
 	 bSlowDown = true;
 	 bSpeedUp = false;
    
	}
  }
}
prevLTC = ltcFrame;

}
//--------------------------------------------------------------
void ofApp::draw(){
    


    player.draw(0,0,ofGetWidth(), ofGetHeight());

    ofPushStyle();

    ofSetColor(255,0,0);

    //Print ltc values

    int h = reader.ltcHour();
    int m = reader.ltcMinute();
    int s = reader.ltcSecond();
    int f = reader.ltcFrame();

    string ltc = ofToString(h,2,'0') + ":" + ofToString(m,2,'0') + ":" + ofToString(s,2,'0') + ":" + ofToString(f,2,'0');
    ofDrawBitmapString("LTC in : " + ltc, 20, 50);
    ofDrawBitmapString("ltc frame : " + ofToString(ltcFrame), 20, 70);
    ofDrawBitmapString("player frame : " + ofToString(player.getCurrentFrame()), 20, 90);

    ofDrawBitmapString("drift : " + ofToString(drift), 20, 110);
    ofDrawBitmapString("modulo frame : " + ofToString(modFrame), 20, 130);

    ofDrawBitmapString("total length : " + ofToString(player.getTotalNumFrames()), 20, 150);
    ofDrawBitmapString("reloads : " + ofToString(reloads), 20, 170);

    ofPopStyle();
}
//--------------------------------------------------------------

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    //Each time audioIn is called, call readLtc to update ltc values
    reader.readLtc(input, nChannels, 2);
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

//    if(key == 32) { player.togglePause(); }
    if(key == 48) { player.increaseSpeed(); } 
else
{
    if(key == 57) { player.decreaseSpeed(); } 
}
}
//--------------------------------------------------------------
void ofApp::doRestart()
{
  player.seekToFrame(modFrame);
  player.start();
  bRestart = false;
}
//--------------------------------------------------------------

