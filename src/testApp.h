#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		ofVideoGrabber 		    vidGrabber;

        ofxCvColorImage		    colorImg;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
		ofxCvGrayscaleImage     grayDiffSmall;

        ofxCvContourFinder 	    contourFinder;

		int 				    threshold;
		bool				    bLearnBakground;

        float                   scaleX, scaleY;

        int                     blur;


        //HUMAN PONG RELATED STUFF

		void updateGame();
		void drawGame();
        void resetBall();

        vector<ofxBox2dRect*>	paddles;
        ofxBox2dRect*			paddle1, *paddle2;
        ofxBox2dCircle			ball;
        int						paddleW,paddleH;
        float					ballX, ballY;
        int						ballRadius;
        int						counter;
        int						dir;
        ofxBox2d                box2d;
        ofxCvBlob               blob;

        bool                    bGamePaused, bDrawBlobs;

        //WARP

        		ofPoint             warpPoints[4];
		int whichCorner;
		int bgColor;
};
#endif
