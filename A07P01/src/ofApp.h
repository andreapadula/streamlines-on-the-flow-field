
#pragma once

#include "ofMain.h"
#include "ofxGui.h"



class ofApp : public ofBaseApp{

  public:
    float offsetX;
    float offsetY;
    float scale2;
    void setup();
    void update();
    void draw();
    int size;
    float* data2;
    vector<ofPolyline> polylines;
    vector<ofPolyline> Screenpolylines;
    vector<ofVec2f> startPoint;
    ofVec2f ** arrayName;
    double range[2];
    double rangeMax=0;
    double rangeMin=0;
    
    int dims[3];
    int image_x;
    int image_y;
    ofVec2f WordToScreen(ofVec2f p);
    ofVec2f ScreenToWord(ofVec2f p);
    ofVec2f ScreenToWord2(ofVec2f p);

    double bounds[6];
    double space[3];
    double orgin[3];
    float minX,maxX,minY,maxY;
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    ofPolyline rk4_integrate(const ofVec2f& start, double dt, int num_steps);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void zoom(float& scale);
    void panX(float& offset);
    ofColor setColor(float num);
    void panY(float& offset);
    void setStep(int& s);
    void setDt(float& d);
    ofVec2f interpolate(const ofVec2f& pos);
    ofVec2f interpolate2(const ofVec2f& pos);
    void ringButtonPressed();
    void convertPolyline();
    ofFloatImage test_image;
    int steps;
    double dt;
    void legend();
    ofxPanel guiPanel;
    ofxFloatSlider slider;
    ofxFloatSlider slider2;
    ofxFloatSlider slider3;
    ofxIntSlider slider4;
    ofxFloatSlider slider5;
    ofxToggle bboxToggle;
    ofxButton ringButton;
};
