#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "particle.h"
#include "vectorField.h"

#include "ofxOpenCv.h"
#include "ofxBox2d.h"

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file


class testApp : public ofSimpleApp{
	
	public:
		
		void setup();
		void setupGame();
		void update();
		void updateGame();
		void draw();
		void drawGame();
		
		void keyPressed  (int key);
		void keyReleased (int key);
		
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
	void resetBall();
	vector		<ofxBox2dRect*>		paddles;
	ofxBox2dRect*					paddle1, *paddle2;
	ofxBox2dCircle					ball;
	int								paddleW,paddleH;
	float							ballX, ballY;
	int								ballRadius;
	int								counter;
	int								dir;
	ofxBox2d box2d;
	ofxCvBlob blob;
		
		// let's make a vector of them
		vector <particle> particles;
		vectorField VF;
	
		float prevMouseX;
		float prevMouseY;
		float roi[3];
		bool bRoi;
		int roiCt;
	
		bool bDrawDiagnostic;
	
		#ifdef _USE_LIVE_VIDEO
			ofVideoGrabber 		vidGrabber;
		#else
			ofVideoPlayer 		vidPlayer;
		#endif
		
		ofxCvColorImage		colorImg;
		
		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
		ofxCvGrayscaleImage		grayDiffSmall;
		
		int 				threshold;
		bool				bLearnBakground;
	
		bool				bForceInward;
	
	ofxCvContourFinder contourFinder;
	
	
};

#endif
	
