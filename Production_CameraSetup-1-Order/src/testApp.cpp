#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->saved = false;


	gui.init();
	gui.addInstructions();
	
	auto deviceList = ofVideoGrabber().listDevices();
	int index = 0;
	for(auto deviceItem : deviceList) {
		auto rawDevice = new ofxMachineVision::Device::VideoInputDevice(1280/2, 720/2);
		auto device = ofxMachineVision::DevicePtr(rawDevice);
		auto grabber = new ofxMachineVision::Grabber::Simple(device);
		grabber->open(deviceItem.id);
		grabber->startCapture();
		grabbers.push_back(grabber);

		grabber->setFocus(0);

		auto panel = gui.add(*grabber, string(deviceItem.deviceName));

		panel->onDraw.addListener([this, grabber, index, panel] (const DrawArguments &) {
			AssetRegister.drawText(ofToString(grabber->getFps()) + "fps", 20, 60);

			if (this->saved) {
				AssetRegister.drawText("SAVED",0, panel->getHeight() / 2 - 20, "", true, 40, panel->getWidth());
			} else {
				for(int i=0; i<this->order.size(); i++) {
					if (this->order[i] == index) {
						//draw index if we have it
						AssetRegister.drawText(ofToString(i),0, panel->getHeight() / 2 - 20, "", true, 40, panel->getWidth());
					}
				}
			}
		}, this);

		panel->onKeyboard.addListener([rawDevice] (const KeyboardArguments & key) {
			if (key.key == 's') {
				rawDevice->showSettings();
			}
		}, this);

		panel->onMouse.addListener([this, index] (const MouseArguments & args) {
			if (!this->saved) {
				if (args.isLocalPressed() && args.button == 0) {
					this->addIndex(index);
				}
			}
		}, this);

		index++;
	}
}

//--------------------------------------------------------------
void testApp::update(){

	if (ofGetFrameNum() == 0 && ofGetScreenWidth() == 1080 && ofGetScreenHeight() == 1920) {
		//ofSetWindowPosition(0,0);
		//ofSetWindowShape(1080*2, 1920);
	}

	for(auto grabber : grabbers) {
		grabber->update();
	}

	if (order.size() == grabbers.size()) {

		//--
		//write out order
		//
		ofBuffer content;
		for(auto index : order) {
			content.append( (char*) &index, sizeof(int));
		}
		ofFile(ORDER_FILENAME, ofFile::WriteOnly, true).writeFromBuffer(content);
		//
		//--


		//--
		//rebuild interface
		//
		auto rootGroup = this->gui.getController().getRootGroup();
		auto & panels = rootGroup->getElements();
		auto instructions = panels[0];
		vector<PanelPtr> oldPanels;
		for(int i=1; i<panels.size(); i++) {
			oldPanels.push_back(panels[i]);
		}
		rootGroup->clear();
		rootGroup->add(instructions);

		for(auto index : order) {
			auto panel = oldPanels[index];
			rootGroup->add(panel);
			panel->onMouse.removeListeners(this);
		}
		//
		//--

		this->order.clear();
		this->saved = true;
	}
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	if (key == OF_KEY_BACKSPACE) {
		if (!this->saved && this->order.size() > 0) {
			this->order.resize(this->order.size() - 1);
		}
	}
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void testApp::addIndex(int index) {
	if (this->order.size() < grabbers.size())  {
		bool contains = false;
		for(auto find : order) {
			if (find == index) {
				contains = true;
				break;
			}
		}
		if (contains == false) {
			order.push_back(index);
		}
	}
}