#include "testApp.h"
#include "ofxBox2d.h"


void testApp::resetBall(){
	cout << "resetting ball";
	ballX = ofGetWidth()/2;
	ballY = ofGetHeight()/2;
	ball.setPosition(ballX, ballY);
	ball.setVelocity(0,0);
}
//--------------------------------------------------------------
void testApp::setup(){	
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	
	/*for (int i = 0; i < 1000; i++){
		particle myParticle;
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
	}
	
	VF.setupField(60,40,ofGetWidth(), ofGetHeight());
	*/
	
	#ifdef _USE_LIVE_VIDEO
		vidGrabber.setVerbose(true);
		vidGrabber.initGrabber(320,240);
	#else
		vidPlayer.loadMovie("fingers.mov");
		vidPlayer.play();
	#endif
		
    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);
	grayDiffSmall.allocate(60,40);
	
	bLearnBakground = true;
	threshold = 50;
	bDrawDiagnostic = true;
	bForceInward	= false; // do we push the particles away...
	
	roi[0]=0;
	roi[1]=0;
	roi[2]=320;
	roi[3]=240;
	bRoi=false;
	roiCt=0;
	
	ofBackground(255,255,255);
	
	setupGame();
}

//--------------------------------------------------------------
void testApp::setupGame() {
	dir = 1;
	counter = 0;
	paddleW = 20;
	paddleH = 100;
	ballRadius = 20;
	ballX = 300;
	ballY = 300;
//	goUp1 = goUp2 = goDown1 = goDown2 = false;
		
	box2d.init();
	box2d.setGravity(0, 0);
	box2d.createFloor();
	box2d.checkBounds(true);
	box2d.setFPS(30.0);
	
	ball.setPhysics(1, 1.1, 0.0);
	ball.setup(box2d.getWorld(), ballX, ballY, ballRadius);
	
	/*paddle1.setPhysics(3.0, 0.53, 0.1);
	paddle1.setup(box2d.getWorld(),  100, 100, 15, 50, true);
	boxes.push_back(paddle1);
	
	paddle2.setPhysics(3.0, 0.53, 0.1);
	paddle2.setup(box2d.getWorld(), 500, 100, 15, 50, true);
	boxes.push_back(paddle2);
	*/
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
	
	#ifdef _USE_LIVE_VIDEO
		vidGrabber.grabFrame();
		bNewFrame = vidGrabber.isFrameNew();
	#else
		vidPlayer.idleMovie();
		bNewFrame = vidPlayer.isFrameNew();
	#endif
	
	if (bNewFrame){
		
		#ifdef _USE_LIVE_VIDEO
				colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
		#else
				colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
		#endif
		
        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}
		
		// take the abs value of the difference between background and incoming and then threshold:
		
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
		
		contourFinder.findContours(grayDiff,200, (320*240)/6, 2, false);
		
		grayDiffSmall.scaleIntoMe(grayDiff);
		grayDiffSmall.blur(5); // really blur the image alot!
		VF.setFromPixels(grayDiffSmall.getPixels(), bForceInward, 0.05f);
	}
	
	
	
	// on every frame 
	// we reset the forces
	// add in any forces on the particle
	// perfom damping and
	// then update
	
	
	/*for (int i = 0; i < particles.size(); i++){
		particles[i].resetForce();
		// get the force from the vector field: 
		ofxVec2f frc;
		frc = VF.getForceFromPos(particles[i].pos.x, particles[i].pos.y);
		particles[i].addForce(frc.x, frc.y);
		particles[i].addDampingForce();
		particles[i].update();
	
	}*/
	
	updateGame();

}

void testApp::updateGame() {
	counter++;
	if (counter < 60){
	}
	else if (counter == 60){
		cout << "YEAH" << endl;
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
			cout << "Player 1 wins";
			dir = -1;
			resetBall();
			counter = 0;
		}
		else if (ballX < 0){
			dir = 1;
			resetBall();
			counter = 0;
			cout << "Player 2 wins";
		}
	}	
	
/*	if(goUp1){
		if (paddle1y>paddleH)
			paddle1y -=10;
		else 
			paddle1y = paddleH;
	}
	if(goDown1){
		if (paddle1y<ofGetHeight()-paddleH) 
			paddle1y += 10;
		else
			paddle1y = ofGetHeight()-paddleH;
	}
	
	if(goUp2){
		if (paddle2y>paddleH)
			paddle2y -=10;
		else 
			paddle2y = paddleH;
	}
	if(goDown2){
		if (paddle2y<ofGetHeight()-paddleH) 
			paddle2y += 10;
		else
			paddle2y = ofGetHeight()-paddleH;
	}
	
	paddle1.setPosition(ofPoint(paddle1x, paddle1y, 0));
	paddle2.setPosition(ofPoint(paddle2x, paddle2y, 0));
*/	
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
void testApp::draw(){
	
	if (bDrawDiagnostic == true){
		
		// draw the incoming, the grayscale, the bg and the thresholded difference
		ofSetColor(0xffffff);
		colorImg.draw(20,20);	
		grayImage.draw(360,20);
		grayBg.draw(20,280);
	
		grayDiff.draw(360,280);
		for(int i=0; i<contourFinder.nBlobs;i++){
			contourFinder.blobs[i].draw(0,0);
		}
		// draw the blurry image
		grayDiffSmall.draw(360, 540, 320, 240);
		
    } else {
		
		/*ofEnableAlphaBlending();
		ofSetColor(255,255,255, 50);
		colorImg.draw(0,0,ofGetWidth(), ofGetHeight());	
		ofSetColor(0,130,130, 200);
		VF.draw();
		ofSetColor(0x000000);
		for (int i = 0; i < particles.size(); i++){
			particles[i].draw();
		}*/
		ofBackground(0,0,0);
		ofSetColor(255,255,255);
		ofRect((ofGetWidth()/2)-5,0,5,ofGetHeight());
		
		/*for(int i=0; i<paddles.size(); i++) {
			paddles[i]->destroyShape();
			delete paddles[i];
		}

		paddles.clear();
		*/
		
		//for(int i=0;i<contourFinder.nBlobs;i++){
			//float nw = ofGetWidth()/320 * contourFinder.blobs[i].boundingRect.width;
			/*contourFinder.blobs[0].boundingRect.width= contourFinder.blobs[0].boundingRect.width*3.2;
			contourFinder.blobs[0].boundingRect.height=contourFinder.blobs[0].boundingRect.height*3.2;
			contourFinder.blobs[0].boundingRect.x=contourFinder.blobs[0].boundingRect.x*3.2;
			contourFinder.blobs[0].boundingRect.y=contourFinder.blobs[0].boundingRect.y*3.2;*/
			
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
				paddle1->setPosition(100, blob1.boundingRect.y*3.5);
				//blob2.draw(0,0);
				paddle2->setPosition(900, blob2.boundingRect.y*3.5);
				}
		
		
			
			//blob = contourFinder.blobs[1];
			//paddle2->setPosition(900, blob.boundingRect.y*4);
			/*if(blob.boundingRect.x<150 || blob.boundingRect.x>ofGetWidth()-150 &&
			   (blob.boundingRect.width>50 && blob.boundingRect.height>50) &&
			   (blob.boundingRect.width<200 && blob.boundingRect.height<200)){
				ofSetColor(255, 0, 0);
				ofxBox2dRect* paddle= new ofxBox2dRect();
				paddle->setPhysics(3.0, 0.53, 0.1);
				paddle->setup(box2d.getWorld(),  blob.boundingRect.x, blob.boundingRect.y, blob.boundingRect.width, blob.boundingRect.height, true);
				paddles.push_back(paddle);
			}*/
			
			//ofRect(blob.boundingRect.x, contourFinder.blobs[i].boundingRect.y, contourFinder.blobs[i].boundingRect.width, contourFinder.blobs[i].boundingRect.height);
		//}
		
		drawGame();
	}
	
	/*ofSetColor(0,130,130, 200);
	ofRect(10,550,330,130);
	ofSetColor(0,0,0);
	ofDrawBitmapString("press ' ' (space) to snap background\npress 'd' to toggle diagnostic\ndrag mouse to add particles\npress 'r' to reset particles\npress 't' to toggle force direction", 20, 600);
	*/
}

//--------------------------------------------------------------
void testApp::drawGame() {
	 ball.draw();
	for(int i=0; i<paddles.size(); i++) {
		paddles[i]->draw();
	}
	/*
	ofSetColor(255,255,255);
	ofFill();
	ofPoint pos = ball.getPosition();
	ofCircle(pos.x, pos.y, ballRadius);
	pos = paddle1->getPosition();
	ofRect(pos.x-paddleW, pos.y-paddleH, paddleW*2, paddleH*2);
	pos = paddle2->getPosition();
	ofRect(pos.x-paddleW, pos.y-paddleH, paddleW*2, paddleH*2);
	*/
	ofSetColor(255,255,255);
	ofFill();
	ofRect(ofGetWidth()-3,0,3, ofGetHeight());
	ofRect(0,0,3, ofGetHeight());
	ofRect(0,ofGetHeight()-3,ofGetWidth(), 3);
	ofRect(0,0,ofGetWidth(), 3);
	//box2d.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	if (key == ' ' ){
		bLearnBakground = true;
	} else if (key == 'd'){	
		bDrawDiagnostic = !bDrawDiagnostic;
	} else if (key == 'r'){
		for (int i = 0; i < particles.size(); i++){
			particles[i].setInitialCondition(ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()), 0,0);
		}
	} else if (key == 't'){
		bForceInward = !bForceInward;
	}else if(key=='s'){
		bRoi=!bRoi;
	}
	else if (key== 't')
		threshold++;
	else if (key== 'g')
		threshold--;
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){ 
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	/*particles.erase(particles.begin());
	particle pt;
	pt.setInitialCondition(x, y, 0, 0);
	particles.push_back(pt);*/
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if(bRoi){
		if(roiCt==0){
			roi[0]=x;
			roi[1]=y;
			roiCt++;
		}else{
			roi[2]=x-roi[0];
			roi[3]=y-roi[1];
			bRoi=false;
			roiCt=0;
			printf("x: %f y: %f w: %f h: %f\n",roi[0],roi[1],roi[2],roi[3]);
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
}
