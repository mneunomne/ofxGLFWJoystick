//
//  ofxGLFWJoystick.cpp
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 07/02/15.
//
//

#include "ofxGLFWJoystick.h"

ofxGLFWJoystick::ofxGLFWJoystick(){

	numJoysticks = 0;
	lookForJoysticks();

	for(int i = 0; i < GLFW_JOYSTICK_LAST; i++){
		joyData[i].available = glfwJoystickPresent(i);
		joyData[i].name = string(glfwGetJoystickName(i));
		joyData[i].buttonData = NULL;
		joyData[i].axisData = NULL;
	}

	//this triggers a deadlock on OSX... sad! we'll need to manually update this...
	//ofAddListener(ofEvents().update, &one(), &ofxGLFWJoystick::update, OF_EVENT_ORDER_BEFORE_APP);
}


void ofxGLFWJoystick::lookForJoysticks(){

	int n = 0;
	for(int i = 0; i < GLFW_JOYSTICK_LAST; i++){
		if(glfwJoystickPresent(i)){
			string name = string(glfwGetJoystickName(i));
			if(name != joyData[i].name){
				joyData[i].name = name;
				ofLogNotice("ofxGLFWJoystick") << "Joystick Found at index " << i << ": '" << name << "'";
			}
			n++;
		}else{
			if (joyData[i].name.size()){
				ofLogNotice("ofxGLFWJoystick") << "Joystick Lost at index " << i << ": '" << joyData[i].name << "'";
			}
			joyData[i].available = false;
			joyData[i].name = "";
		}
	}
	numJoysticks = n;
}


void ofxGLFWJoystick::update(){

	if(ofGetFrameNum()%600 == 1){ //update joystick info every now and then
		lookForJoysticks();
	}

	vector<string> joys;
	for(int j = 0; j < numJoysticks; j++){
		bool joyOK = glfwJoystickPresent(j);
		if(joyOK){
			joyData[j].available = true;
			joyData[j].axisData = glfwGetJoystickAxes(j, &joyData[j].numAxis);
			joyData[j].buttonData = glfwGetJoystickButtons(j, &joyData[j].numButtons);
		}else{
			joyData[j].available = false;
			joyData[j].axisData = NULL;
			joyData[j].buttonData = NULL;
			joyData[j].numAxis = 0;
			joyData[j].numButtons = 0;
		}
	}
}


float ofxGLFWJoystick::getAxisValue(int axisID, int joyID){

	if(isJoystickAvailable(joyID)){
		if(axisID < joyData[joyID].numAxis && axisID >= 0){
			return joyData[joyID].axisData[axisID];
		}
	}
	return 0.0f;
}


unsigned char ofxGLFWJoystick::getButtonValue(int buttonID, int joyID){

	if(isJoystickAvailable(joyID)){
		if(buttonID < joyData[joyID].numButtons && buttonID >= 0){
			return joyData[joyID].buttonData[buttonID];
		}
	}
	return 0;
}


void ofxGLFWJoystick::drawDebug(int x, int y){

	ofPushMatrix();
	ofPushStyle();
	ofTranslate(x, y);
	int joyRad = 25;
	int buttonSize = 20;
	int padding = 10;
	float yOffset = 0;

	for(int j = 0; j < numJoysticks; j++){


		ofTranslate(0 , yOffset);

		int xOffset = joyRad * 0.5f;
		bool joyOK = glfwJoystickPresent(j);

		if(joyOK){
			string msg = joyData[j].name + " numAxis: " + ofToString(joyData[j].numAxis) +
			"  numButtons: " + ofToString(joyData[j].numButtons);

			ofSetColor(255);
			ofDrawBitmapString(msg, 0, - 22);

			for(int i = 0; i < joyData[j].numAxis; i+=2){
				ofSetColor(255, 64);
				float xx = xOffset + joyRad * 0.5f;
				float yy = joyRad * 0.5f;
				ofCircle(xx, yy, joyRad);
				float x = ofMap(joyData[j].axisData[i], -1, 1, -joyRad, joyRad);
				float y = 0;
				if(i + 1 < joyData[j].numAxis ){
					y = ofMap(joyData[j].axisData[i + 1], -1, 1, -joyRad, joyRad);
				}
				ofColor c; c.setHsb((i * 16)%255, 255, 255);
				ofSetColor(c);
				ofDrawArrow(ofVec2f(xx, yy), ofVec2f(xx + x, yy + y), 4);
				xOffset += joyRad * 2 + padding;
			}

			ofSetRectMode(OF_RECTMODE_CENTER);
			for(int i = 0; i < joyData[j].numButtons; i ++ ){
				bool pressed = joyData[j].buttonData[i] > 0; //mmm not sure why its a char if its only 0/1
				ofSetColor( pressed ? 0:255, pressed ? 255:0, 0 );
				ofRect(xOffset, joyRad *  0.5, joyRad * 0.7, joyRad * 0.7);
				ofSetColor(255);
				ofDrawBitmapString(ofToString(i), xOffset - 7, joyRad * 0.5 + 22);
				xOffset += joyRad + padding * 0.5;
			}
			ofSetRectMode(OF_RECTMODE_CORNER);
			yOffset = joyRad * 2 + 4 * padding;
		}
	}
	ofPopStyle();
	ofPopMatrix();
}


bool ofxGLFWJoystick::isJoystickAvailable(int joyID){
	if(joyID < GLFW_JOYSTICK_LAST){
		if(joyData[joyID].available){
			return true;
		}
	}
	return false;
}


string ofxGLFWJoystick::getJoystickName(int joyID){
	if(isJoystickAvailable(joyID)){
		return joyData[joyID].name;
	}
	return "";
}


int ofxGLFWJoystick::getNumButtons(int joyID){
	if(isJoystickAvailable(joyID)){
		return joyData[joyID].numButtons;
	}
	return 0;
}


int ofxGLFWJoystick::getNumAxis(int joyID){
	if(isJoystickAvailable(joyID)){
		return joyData[joyID].numAxis;
	}
	return 0;
}


const unsigned char * ofxGLFWJoystick::getButtonsForJoystick(int joyID){
	if(isJoystickAvailable(joyID)){
		return joyData[joyID].buttonData;
	}
	return NULL;
}


const float * ofxGLFWJoystick::getButtonsForAxis(int joyID){
	if(isJoystickAvailable(joyID)){
		return joyData[joyID].axisData;
	}
	return NULL;
}







