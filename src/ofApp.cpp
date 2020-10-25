
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
//    settings.listener = this;
    
    player.setup(settings);
    
    length = player.getTotalNumFrames();
    player.seekToFrame(0);
    
   bSpeedUp = false;
   bSlowDown = false;
   reloads = 0;
   prevLTC = 0;
   movFrame = 0;
   bIsJumping = false;
}

//--------------------------------------------------------------
//void ofApp::onVideoEnd(ofxOMXPlayer* player)
//{/
//}
//--------------------------------------------------------------
//void ofApp::onVideoLoop(ofxOMXPlayer* player)
//{
//player->reopen();

//}
//--------------------------------------------------------------

void ofApp::update(){
    
ltcFrame = (3600 * reader.ltcHour()
               + 60 * reader.ltcMinute()
               + reader.ltcSecond()) * 24
               + reader.ltcFrame();

if(!player.getTotalNumFrames())
{
 player.reopen();
 reloads++;
} 

movFrame = player.getCurrentFrame();
modFrame = ltcFrame % length;

drift	= modFrame - movFrame;
absDrift = abs(drift);

if(absDrift < 1) 
 {
	if(player.engine.currentSpeed != player.engine.normalSpeedIndex)
	{
	 player.setNormalSpeed();
    	}
	bSpeedUp = false;
    	bSlowDown = false;
 }

else if(absDrift > 24 && !bIsJumping)
	{
	  bIsJumping = true;
  	  player.setPaused(true);
  	  player.seekToFrame(abs(modFrame));
	  player.setPaused(false);
	  ofLog(OF_LOG_NOTICE, "did pause - jump to frame");
 	}

else 
	{
  	 bIsJumping = false;
	}
	
//if(player.isFrameNew())
//{	
if((drift > 5) && (player.getPlaybackSpeed() != 1125))
  	  {
  	   player.engine.currentSpeed = 6;
  	   player.engine.SetSpeed();
  	   bSpeedUp = true;
  	   bSlowDown = false;
    	  }
	
if((drift < -5) && (player.getPlaybackSpeed() != 62))
  	  {
  	   player.engine.currentSpeed = 0;
  	   player.engine.SetSpeed();
 	   bSlowDown = true;
 	   bSpeedUp = false;
    	  }
//}	
if((prevLTC == ltcFrame) && (absDrift == 0))
  {
//   player.setPaused(true);
//   bSpeedUp = bSlowDown = true;
  } 
else 
  {
//   player.setPaused(false);
//   bSpeedUp = bSlowDown = false;
  }


prevLTC = ltcFrame;
//bIsJumping = false;
}
//--------------------------------------------------------------
void ofApp::draw(){
    

 if(player.isTextureEnabled())
{
    player.draw(0,0,ofGetWidth()*0.5, ofGetHeight()*0.5);

    ofPushStyle();

    ofSetColor(255);

    //Print ltc values

    int h = reader.ltcHour();
    int m = reader.ltcMinute();
    int s = reader.ltcSecond();
    int f = reader.ltcFrame();

    string ltc = ofToString(h,2,'0') + ":" + ofToString(m,2,'0') + ":" + ofToString(s,2,'0') + ":" + ofToString(f,2,'0');
    ofDrawBitmapString("LTC in : " + ltc, 20, 50);
    ofDrawBitmapString("ltc frame : " + ofToString(ltcFrame), 20, 70);
    ofDrawBitmapString("player frame : " + ofToString(player.getCurrentFrame()), 20, 90);

    ofDrawBitmapString("drift(-= player ahead) : " + ofToString(drift), 20, 110);
    ofDrawBitmapString("ABS drift : " + ofToString(absDrift), 20, 130);
    ofDrawBitmapString("modulo frame : " + ofToString(modFrame), 20, 150);

    ofDrawBitmapString("total length : " + ofToString(player.getTotalNumFrames()), 20, 170);
    ofDrawBitmapString("reloads : " + ofToString(reloads), 20, 190);
    ofDrawBitmapString("player speed : " + ofToString(player.getPlaybackSpeed()), 20, 210);
    ofDrawBitmapString("engine speed : " + ofToString(player.engine.currentSpeed), 20, 230);


    ofPopStyle();
}
}
//--------------------------------------------------------------

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    //Each time audioIn is called, call readLtc to update ltc values
    reader.readLtc(input, nChannels, 2);
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

//    if(key == 32) { player.togglePause(); }
//    if(key == 48) { player.increaseSpeed(); } 
//   else
//    {
//    if(key == 57) { player.decreaseSpeed(); } 
//}
}
//--------------------------------------------------------------
void ofApp::doRestart()
{
  bRestart = false;
}
//--------------------------------------------------------------

