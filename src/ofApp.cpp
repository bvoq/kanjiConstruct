#include "ofApp.h"


bool mouseFirstPressed = false;
bool mouseHasntReleased = false;
bool mouseIsReleased = false;
int mouseX = -1000;
int mouseY = -1000;

#include <string>

#include "utilities.h"
#include "kanjistructure.h"
#include "drawkanji.h"
#include "kanjigame.h"




//--------------------------------------------------------------
void ofApp::setup(){
    // init kanjis
    initKanjis("kanjiList");
    // init font
    initFont();
    string path = "kanjiList";
    
    initKanjiGame();
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------


void ofApp::draw(){
    ofBackground(255);
    ofFill();
    ofSetColor(0,0,0);
    //printDictionary();
    drawKanjiGame();
    
    if(mouseIsReleased) {
        mouseIsReleased = false;
        mouseHasntReleased = false;
    }
    mouseFirstPressed = false;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP) {
        offsetY += 100;
    }
    if(key == OF_KEY_DOWN) {
        offsetY -= 100;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(!mouseHasntReleased) {
        mouseFirstPressed = true;
        mouseHasntReleased = true;
        mouseX = x;
        mouseY = y;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseIsReleased = true;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
