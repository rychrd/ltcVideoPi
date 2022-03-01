
#include "ofApp.h"
#define FR 30.0

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Setup an audio device with ofSoundStream
    snd.listDevices();
    snd.setDeviceID(2); // usb to analogue dongle ID
    snd.setup(this, 0, 1, 48000, 512, 4); // outputs,inputs,samplerate, buffer, num buffers 
    
    //Setup a ltc reader by passing a pointer to this ofSoundStream with expected framerate
    reader.setup(&snd, FR);
    
    //setup OMXPlayer

    string videoPath = ofToDataPath("video/ghost_front.mp4", true);
    settings.videoPath = videoPath;
    settings.useHDMIForAudio = false;
    settings.enableLooping = true;
    settings.enableTexture = false;
    settings.listener = this;
    
    player.setup(settings);
    
    length = player.getTotalNumFrames();
   // player.seekToFrame(0);
    
   bSpeedUp = false;
   bSlowDown = false;
   reloads = 0;
   prevLTC = 0;
   movFrame = 0;
   bCanSeek = true;
}

//--------------------------------------------------------------
void ofApp::onVideoEnd(ofxOMXPlayer* player)
{

    bCanSeek = false;

}
//--------------------------------------------------------------
void ofApp::onVideoLoop(ofxOMXPlayer* player)
{
    bCanSeek = false;
}
//--------------------------------------------------------------

void ofApp::update(){
    
ltcFrame = ( 3600 * reader.ltcHour()
             + 60 * reader.ltcMinute()
             + reader.ltcSecond() ) 
	     * FR
             + reader.ltcFrame();

if(!player.getTotalNumFrames())
{
 player.reopen();
 reloads++;
} 


//movFrame = player.getCurrentFrame();
modFrame = floor(fmod(ltcFrame, length));


drift = modFrame - player.getMediaTime() * FR;
absDrift = abs(drift);

if(absDrift < 1.0) 
 {
	if(player.engine.currentSpeed != player.engine.normalSpeedIndex)
	{
	 player.setNormalSpeed();
    	}
 }

if((drift > 5.0) && (player.getPlaybackSpeed() < 1125))
{
    player.engine.currentSpeed = 6;
    player.engine.SetSpeed();
    bSpeedUp = true;
    bSlowDown = false;
}
	
if((drift < -5.0) && (player.getPlaybackSpeed() > 62))
{
    player.engine.currentSpeed = 0;
    player.engine.SetSpeed();
    bSlowDown = true;
    bSpeedUp = false;
}

if(absDrift > 500 && (player.getMediaTime() * FR > 60.0))
 {
        player.seekToTimeInSeconds(floor(modFrame / 30.0));
        cout << "seeked!!" << endl;
 }

}
//--------------------------------------------------------------
void ofApp::draw(){
    

 if(player.isTextureEnabled())
{
    player.draw(0,0,ofGetWidth(), ofGetHeight());

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
//    ofDrawBitmapString("EOS submitted : " + ofToString(player.isSubmitEOS()), 20, 250);
    ofDrawBitmapString("seek allowed : " + ofToString(bCanSeek), 20, 270);


    ofPopStyle();
}
}
//--------------------------------------------------------------

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
 // Last parameter is the channel index - set to 1 for mono mic input
    reader.readLtc(input, nChannels, 1);
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}
//--------------------------------------------------------------
void ofApp::doRestart()
{
  bRestart = false;
}
//--------------------------------------------------------------

