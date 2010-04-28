#include "testApp.h"
#include "ofxBox2d.h"

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
	threshold = 50;
	bDrawDiagnostic = true;

	printf("Windowsize is %d x %d\n",ofGetWidth(),ofGetHeight());

	//PONG STUFF
	dir = 1;
	counter = 0;
	paddleW = 20;
	paddleH = 100;
	ballRadius = 20;
	ballX = 300;
	ballY = 300;

	box2d.init();
	box2d.setGravity(0, 0);
	//box2d.createFloor();
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
	paddle1->setup(box2d.getWorld(),  100, 400, 20, 100, true);
    paddles.push_back(paddle1);

	paddle2 = new ofxBox2dRect();
	paddle2->setPhysics(3.0, 0.53, 0.1);
	paddle2->setup(box2d.getWorld(),  900, 400, 20, 100, true);
	paddles.push_back(paddle2);
}

//--------------------------------------------------------------
void testApp::update(){


	bool bNewFrame = false;

    vidGrabber.grabFrame();
    bNewFrame = vidGrabber.isFrameNew();

	if (bNewFrame){

        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);

        grayImage = colorImg;

		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
		grayDiff.blur(5);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
	}


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
        paddle1->setPosition(100, blob1.boundingRect.y);
        //blob2.draw(0,0);
        paddle2->setPosition(ofGetWidth()-100, blob2.boundingRect.y);
        printf("pos: %f\n",blob2.boundingRect.y);
        }

	updateGame();

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

	if (paddle1->getPosition().y > ofGetHeight()-paddleH)
		paddle1->setPosition(paddle1->getPosition().x, ofGetHeight()-paddleH);
	if (paddle1->getPosition().y < 0)
		paddle1->setPosition(paddle1->getPosition().x, 0);

	if (paddle2->getPosition().y > ofGetHeight()-paddleH)
		paddle2->setPosition(paddle2->getPosition().x, ofGetHeight()-paddleH);
	if (paddle2->getPosition().y < 0)
		paddle2->setPosition(paddle2->getPosition().x, 0);
	box2d.update();
}

//--------------------------------------------------------------
void testApp::resetBall(){
	printf("Resetting Ball!\n");
	ballX = ofGetWidth()/2;
	ballY = ofGetHeight()/2;
	ball.setPosition(ballX, ballY);
	ball.setVelocity(0,0);
}

//--------------------------------------------------------------
void testApp::draw(){

	if (bDrawDiagnostic == true){

        ofBackground(100,100,100);
        ofSetColor(0xffffff);
        colorImg.draw(0,0);
        grayImage.draw(320,0);
        grayBg.draw(0,240);
        grayDiff.draw(320,240);

        ofFill();
        ofSetColor(0x333333);
        ofRect(320,480,320,240);
        ofSetColor(0xffffff);

        // draw the incoming, the grayscale, the bg and the thresholded difference
        ofSetColor(0xffffff);
        for (int i = 0; i < contourFinder.nBlobs; i++){
            contourFinder.blobs[i].draw(320,480);
        }

        // finally, a report:

        ofSetColor(0xffffff);
        char reportStr[1024];
        sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\nnum blobs found %i, fps: %f", threshold, contourFinder.nBlobs, ofGetFrameRate());
        ofDrawBitmapString(reportStr, 20, 600);

    } else {
        // DRAW THE GAME
        ball.draw();
        for(int i=0; i<paddles.size(); i++) {
            paddles[i]->draw();
        }
        ofBackground(0,0,0);
        ofSetColor(255,255,255);
        ofFill();
        ofRect(ofGetWidth()-3,0,3, ofGetHeight());
        ofRect(0,0,3, ofGetHeight());
        ofRect(0,ofGetHeight()-3,ofGetWidth(), 3);
        ofRect(0,0,ofGetWidth(), 3);
        ofRect((ofGetWidth()/2)-5,0,5,ofGetHeight());
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if (key == ' ' ){
		bLearnBakground = true;
	} else if (key == 'd'){
		bDrawDiagnostic = !bDrawDiagnostic;
	}
	else if (key== '+')
		threshold++;
	else if (key== '-')
		threshold--;
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

