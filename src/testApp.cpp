#include "testApp.h"
#include "ofxBox2d.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);
	grayDiffSmall.allocate(320,240);

	bLearnBakground = true;
	threshold = 100;
	blur = 5;

	printf("Windowsize is %d x %d\n",ofGetWidth(),ofGetHeight());

    scaleX = ofGetWidth() / (float)320;
    scaleY = ofGetHeight() / (float)240;

    printf("scaleX: %f\nscaleY: %f\n",scaleX,scaleY);

	//PONG STUFF
	bgColor = 0;
	bGamePaused = true;

	dir = 1;
	counter = 0;
	paddleW = 20;
	paddleH = 100;
	ballRadius = 20;
	ballX = 300;
	ballY = 300;

	box2d.init();
	box2d.setGravity(0, 0);
	box2d.createFloor();
	box2d.checkBounds(true);
	box2d.setFPS(30.0);

	ball.setPhysics(1, 1.1, 0.0);
	ball.setup(box2d.getWorld(), ballX, ballY, ballRadius);

	ofxBox2dRect ceiling;
	ceiling.setPhysics(3.0, 0.53, 0.1);
	ceiling.setup(box2d.getWorld(),  0, 3, ofGetWidth(), 10,true);
	resetBall();
	int yDir = 1;
	if (ofRandom(0,10) > 5)
		yDir = -1;
	ball.setVelocity(ofRandom(5*dir, 10*dir),ofRandom(2*yDir, 5*yDir));

	paddle1 = new ofxBox2dRect();
	paddle1->setPhysics(3.0, 0.53, 0.1);
	paddle1->setup(box2d.getWorld(), 200, ofGetHeight()/2, 20, 100, true);
    paddles.push_back(paddle1);

	paddle2 = new ofxBox2dRect();
	paddle2->setPhysics(3.0, 0.53, 0.1);
	paddle2->setup(box2d.getWorld(),  ofGetWidth()/2-200, ofGetHeight()/2, 20, 100, true);
	paddles.push_back(paddle2);

	//WARP

    warpPoints[0].x = 0;
    warpPoints[0].y = 0;
    warpPoints[1].x = 320;
    warpPoints[1].y = 0;
    warpPoints[2].x = 320;
    warpPoints[2].y = 240;
    warpPoints[3].x = 0;
    warpPoints[3].y = 240;

	// 'gui' is a global variable declared in ofxSimpleGuiToo.h

	gui.addTitle("Options");
    gui.config->gridSize.set(340,0,0);
	gui.addFPSCounter();
	gui.addSlider("Threshold",threshold,0,255);
	gui.addSlider("Background color",bgColor,0,255);
    gui.addSlider("Blur",blur,0,50);
	gui.addToggle("Make background", bLearnBakground);
	gui.addToggle("Draw Blobs on Game",bDrawBlobs);
	gui.addToggle("Pause",bGamePaused);

	gui.addContent("Color Image", colorImg,320).newColumn = true;;
	gui.addContent("Gray Image", grayImage,320);
	gui.addQuadWarper("Warp", colorImg, warpPoints);
    gui.addContent("Background Image", grayBg,320);
	gui.addContent("Difference", grayDiff,320);
    //GUI
    gui.loadFromXML();
	gui.show();
}

//--------------------------------------------------------------
void testApp::update(){


	bool bNewFrame = false;

    vidGrabber.grabFrame();
    bNewFrame = vidGrabber.isFrameNew();

	if (bNewFrame){

        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);

        grayImage = colorImg;
        grayImage.warpPerspective(warpPoints[0],warpPoints[1],warpPoints[2],warpPoints[3]);

		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
		grayDiff.blur(blur);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
	}

if(!bGamePaused) {
        ofxCvBlob blob1;
		ofxCvBlob blob2;

        if(contourFinder.nBlobs > 1){
            blob1 = contourFinder.blobs[0];
            blob2 = contourFinder.blobs[1];

        if (blob2.boundingRect.x < blob1.boundingRect.x){
            ofxCvBlob blobTemp = blob1;
            blob1 = blob2;
            blob2 = blobTemp;
        }

        //blob1.draw(0,0);

        paddle1->setPosition(200, (blob1.boundingRect.y + blob1.boundingRect.height/2) * scaleY);
        //blob2.draw(0,0);
        paddle2->setPosition(ofGetWidth()/2-200, (blob2.boundingRect.y + blob2.boundingRect.height/2) * scaleY);
        //printf("pos: %f\n",blob2.boundingRect.y);
        }

        updateGame();
    }

}

//--------------------------------------------------------------
void testApp::updateGame() {
	counter++;
	if (counter < 60){
	}
	else if (counter == 60){
		printf("Game started!\n");
		int yDir = 1;
		if (ofRandom(0,10) > 5)
			yDir = -1;
		resetBall();
		//ball.setVelocity(3,2);
		ball.setVelocity(ofRandom(5*dir, 10*dir),ofRandom(2*yDir, 5*yDir));
	}
	else {
		if ((ball.getVelocity().x*ball.getVelocity().x) < 6 && (ball.getPosition().y <= 50 || ball.getPosition().y >= ofGetHeight()-50)){
			ofPoint vel = ball.getVelocity();
			ball.setVelocity(vel.x*3, vel.y);
		}
		ballX = ball.getPosition().x;
		ballY = ball.getPosition().y;
		if (ballX > ofGetWidth()){
			cout << "Player 1 wins\n";
			dir = -1;
			resetBall();
			counter = 0;
		}
		else if (ballX < 0){
			dir = 1;
			resetBall();
			counter = 0;
			cout << "Player 2 wins\n";
		}
	}

	if (paddle1->getPosition().y > ofGetHeight())
		paddle1->setPosition(paddle1->getPosition().x, ofGetHeight()-paddleH);
	if (paddle1->getPosition().y < 0)
		paddle1->setPosition(paddle1->getPosition().x, 0);

	if (paddle2->getPosition().y + paddleH/2 > ofGetHeight())
		paddle2->setPosition(paddle2->getPosition().x, ofGetHeight() - paddleH/2);

	if (paddle2->getPosition().y - paddleH/2 < 0)
		paddle2->setPosition(paddle2->getPosition().x, paddleH/2);

	box2d.update();
}

//--------------------------------------------------------------
void testApp::resetBall(){
	printf("Resetting Ball!\n");
	ballX = ofGetWidth()/4;
	ballY = ofGetHeight()/2;
	ball.setPosition(ballX, ballY);
	ball.setVelocity(0,0);
}

//--------------------------------------------------------------
void testApp::draw(){

    ofBackground(100,100,100);

    // DRAW THE GAME
    ofPushMatrix();
        ofTranslate(1280,0,0);
        ofFill();
        ofSetColor(bgColor,bgColor,bgColor);
        ofRect(0,0,1280,1024);
        ball.draw();

        for(int i=0; i<paddles.size(); i++) {
            paddles[i]->draw();
        }
        ofFill();
        ofSetColor(0xffffff);
        //Left
        ofRect(0,0,3,ofGetHeight());
        //Right
        ofRect(1280-3,0,3, ofGetHeight());
        //Bottom
        ofRect(0,ofGetHeight()-3,1280, 3);
        //Top
        ofRect(0,0,1280, 3);
        //Center
        ofRect(640-5,0,5,ofGetHeight());
    ofPopMatrix();

    //DRAW THE BLOBS
    if(bDrawBlobs) {
        ofPushMatrix();
            ofTranslate(1280,0,0);
            ofScale(4,4.266666,0);
            for (int i = 0; i < contourFinder.nBlobs; i++){
                contourFinder.blobs[i].draw(0,0);
                ofSetColor(0xffffff);
                ofFill();
                ofRect(contourFinder.blobs[i].boundingRect.x,contourFinder.blobs[i].boundingRect.y,1,1);
            }
        ofPopMatrix();
    }

	//GUI
	gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key>='0' && key<='9') {
		gui.setPage(key - '0');
		gui.show();
	} else {
		switch(key) {
			case 'k': gui.toggleDraw(); break;
			case '[': gui.prevPage(); break;
			case ']': gui.nextPage(); break;
			case 'p': gui.nextPageWithBlank(); break;
		}
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

