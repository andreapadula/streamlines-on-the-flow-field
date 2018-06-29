


#include "ofApp.h"

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataReader.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>


//--------------------------------------------------------------
void ofApp::setup(){
    size=0;
    ofPolyline line;
    for (int i=0; i < 500; i++) {
        Screenpolylines.push_back(line);
    }
  //Use VTK to read the .vti file
  vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    string path;
    ofFileDialogResult result = ofSystemLoadDialog("Load file");
    if(result.bSuccess) {
        path = result.getPath();
        reader->SetFileName((path).c_str());
    }
    else{
        return;
    }
  //Once Update() is called, the reader actually reads the file
  reader->Update();

  vtkSmartPointer<vtkImageData> image = reader->GetOutput();
  vtkSmartPointer<vtkDataArray> vectors = image->GetPointData()->GetVectors();
 
    image->GetSpacing(space);
    image->GetOrigin(orgin);
    image->GetBounds(bounds);
    minX=bounds[0];
    minY=bounds[2];
    maxX=bounds[1];
    maxY=bounds[3];
    offsetX=0;
    offsetY=0;
    scale2=1;
  
  image->GetDimensions(dims);
  std::cout << "Image dimensions: " << dims[0] << "x" << dims[1] << "x" << dims[2] << std::endl;
    
    
    //Get the scalar range (min/max) value
    
    vectors->GetRange(range);
    image_x = 500;
    image_y = 500;
    data2 = new float[3*image_x*image_y];
    ofVec2f* data = new ofVec2f[dims[0]*dims[1]];
    double* vec = new double[vectors->GetNumberOfComponents()];
    for (int i=0; i<dims[0]*dims[1]; i++) {
        
        vectors->GetTuple(i, vec);
        //vec[0] is the x-component of the vector
        data[i][0] = vec[0];
        //vec[1] is the y-component of the vector
        data[i][1] = vec[1];
        //cout<<data[i].length<<endl;
//        data[i].length
        
        rangeMax=MAX(data[i].length(),rangeMax);
        rangeMin=MIN(data[i].length(),rangeMin);
    }
    delete[] vec;
    arrayName = new ofVec2f* [ dims[0] ];
    arrayName[0] = new ofVec2f[ dims[0] * dims[1] ];
    for (int i = 1; i < dims[0]; i++ ) {
        arrayName[i] = arrayName[i-1] + dims[1];
    }
    int i=0;
    for (int y=0; y<dims[1]; y++) {
        
        for (int x=0; x<dims[0]; x++) {
            
            int Iy = i / dims[0];
            int Ix = i % dims[0];
            arrayName[Ix][Iy]=data[i];
            i++;
           // cout<<arrayName[Ix][Iy]<<endl;
        }
    }
    
    
    i=0;
    for (int iy=0; iy<image_y; iy++) {
        
        for (int ix=0; ix<image_x; ix++) {
            
            ofVec2f pos= ScreenToWord(ofVec2f(ix,iy));
            float wx = pos[0];
            float wy = pos[1];
            ofColor r =setColor(interpolate2(ofVec2f(wx,wy)).length());

            data2[3*i]=(float(r.r)) / (255);;///(range[1]-range[0]);
            data2[3*i+1]=(float(r.g)) / (255);;//(range[1]-range[0]);
            data2[3*i+2]=(float(r.b)) / (255);//(range[1]-range[0]);
            i++;
        }
    }

    
    
    test_image.setFromPixels(data2, image_x, image_y, OF_IMAGE_COLOR);
    ringButton.addListener(this, &ofApp::ringButtonPressed);
    slider.addListener(this, &ofApp::zoom);
    slider2.addListener(this, &ofApp::panX);
    slider3.addListener(this, &ofApp::panY);
    slider4.addListener(this, &ofApp::setStep);
    slider5.addListener(this, &ofApp::setDt);
    
    guiPanel.setup();
    guiPanel.add(slider.setup("Zoom", 1, 0.1, 10));
    guiPanel.add(slider2.setup("Pan X:", 0, -dims[0], dims[0]));
    guiPanel.add(slider3.setup("Pan Y:", 0, -dims[1], dims[1]));
    guiPanel.add(slider4.setup("Steps:", 100, 0, 500));
    guiPanel.add(slider5.setup("DT", 0.1, 0, 10));
    guiPanel.add(ringButton.setup("Erase Lines"));
    guiPanel.add(bboxToggle.setup("Sync lines", true));

}
void ofApp::setStep(int& s){
    steps=s;
    if(bboxToggle){
    if(size>0){
        size=0;
        polylines.clear();
        for (int i=0; i<startPoint.size(); i++) {
            rk4_integrate(startPoint[i],dt,steps);
        }
        
    }
    }
}
void ofApp::setDt(float& d){
    
    dt=d;
    if(bboxToggle){
    if(size>0){
        size=0;
        polylines.clear();
        for (int i=0; i<startPoint.size(); i++) {
            rk4_integrate(startPoint[i],dt,steps);
        }
        
    }
    }
}
void ofApp::ringButtonPressed(){
    if (size>0) {
        polylines.clear();
        startPoint.clear();
        size=0;
    }
    
}
ofColor ofApp::setColor(float num){
    ofColor color;
    color.r= ofMap(num, rangeMin, rangeMax, 227, 254);
    color.g= ofMap(num, rangeMin, rangeMax, 74, 232);
    color.b= ofMap(num, rangeMin, rangeMax, 51, 200);
    return color;
}
void ofApp::zoom(float& scaleFactor) {
    int i=0;
    scale2=1/scaleFactor;

    for (int iy=0; iy<image_y; iy++) {
        
        for (int ix=0; ix<image_x; ix++) {
            ofVec2f pos= ScreenToWord(ofVec2f(ix,iy));
            float wx = pos[0];
            float wy = pos[1];
            ofColor r =setColor(interpolate2(ofVec2f(wx,wy)).length());
            
            data2[3*i]=(float(r.r)) / (255);;///(range[1]-range[0]);
            data2[3*i+1]=(float(r.g)) / (255);;//(range[1]-range[0]);
            data2[3*i+2]=(float(r.b)) / (255);//(range[1]-range[0]);
            i++;
        }
    }
    test_image.setFromPixels(data2, image_x, image_y, OF_IMAGE_COLOR);
}


void ofApp::panX(float& offset) {
    int i=0;
    offsetX=offset;
    for (int iy=0; iy<image_y; iy++) {
        
        for (int ix=0; ix<image_x; ix++) {
            ofVec2f pos= ScreenToWord(ofVec2f(ix,iy));
            float wx = pos[0];
            float wy = pos[1];
            ofColor r =setColor(interpolate2(ofVec2f(wx,wy)).length());
            
            data2[3*i]=(float(r.r)) / (255);;///(range[1]-range[0]);
            data2[3*i+1]=(float(r.g)) / (255);;//(range[1]-range[0]);
            data2[3*i+2]=(float(r.b)) / (255);//(range[1]-range[0]);
            i++;
        }
    }
    test_image.setFromPixels(data2, image_x, image_y, OF_IMAGE_COLOR);
}



void ofApp::panY(float& offset) {
    int i=0;
    offsetY=offset;
    for (int iy=0; iy<image_y; iy++) {
        
        for (int ix=0; ix<image_x; ix++) {
            ofVec2f pos= ScreenToWord(ofVec2f(ix,iy));
            float wx = pos[0];
            float wy = pos[1];
            ofColor r =setColor(interpolate2(ofVec2f(wx,wy)).length());
            
            data2[3*i]=(float(r.r)) / (255);;///(range[1]-range[0]);
            data2[3*i+1]=(float(r.g)) / (255);;//(range[1]-range[0]);
            data2[3*i+2]=(float(r.b)) / (255);//(range[1]-range[0]);
            i++;
        }
    }
    test_image.setFromPixels(data2, image_x, image_y, OF_IMAGE_COLOR);
}







ofVec2f ofApp::interpolate2(const ofVec2f& pos){
    
    float wx=ofMap(pos[0],minX,maxX, 0,dims[0]-1);
    float wy=ofMap(pos[1],minY,maxY,0,dims[1]-1);
//    cout<<wx<<endl;
//    cout<<wy<<endl;
//    cout<<"????????????"<<endl;

    int x= floor(wx);
    int y= floor(wy);
    if(x<0|| x>=dims[0]-1)
        return ofVec2f(0,0);
    if(y<0|| y>=dims[1]-1)
        return ofVec2f(0,0);
    float u=wx-x;
    float t=wy-y;
    
    ofVec2f temp= (1-u)*arrayName[x][y]+(u)*arrayName[x+1][y];
    ofVec2f temp2= (1-u)*arrayName[x][y+1]+(u)*arrayName[x+1][y+1];
    ofVec2f result=(1-t)*temp+(t)*temp2;
    
    return result;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(ofColor(255, 255, 255));
    
    test_image.draw(250,200);
    ofSetColor(ofColor(0, 0, 0));
    convertPolyline();
    for (int i=0; i < size; i++) {
        Screenpolylines[i].draw();
    }

    guiPanel.draw();
    legend();
}

void ofApp::convertPolyline(){

    for(int i=0;i<size;i++){
        vector<ofPoint>& vertices = polylines[i].getVertices();
        ofPolyline line;
        for (int j=0; j<polylines[i].getVertices().size(); j++) {
            
            ofVec2f pos2;
            pos2=WordToScreen(ofVec2f(vertices[j][0],vertices[j][1]));
            if(pos2[0]<image_x+250){
                if(pos2[0]>250){
                    if (pos2[1]<image_y+200) {
                        if (pos2[1]>200) {
                            line.addVertex(pos2);
                        }
                    }
                }
            }
        }
    
       Screenpolylines[i]=line;
    }

}
void ofApp::legend(){

    for (int color=0; color<=255; color+=15){
        ofColor c;
        c.r=ofMap(color,0, 255, 227, 254);
        c.g=ofMap(color,0, 255, 74, 232);
        c.b=ofMap(color,0, 255, 51, 200);
        ofSetColor(c);
        ofRect(350+color,50,20,20);
    }
    ofSetColor(0);
    float nearest = rangeMin;
    float nearest2 = rangeMax;
    ofDrawBitmapString(std::to_string(nearest), 340, 80);
    ofDrawBitmapString(std::to_string(nearest2), 610, 80);

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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

ofPolyline ofApp::rk4_integrate(const ofVec2f& start, double dt, int num_steps){
    ofVec2f k1, k2, k3, k4;
    ofVec2f pos=start;
     ofPolyline line;
    line.addVertex(pos);
    float ax;
    float ay;
   
    for (int i=0; i<num_steps; i++)
    {
        
        k1 = dt*interpolate2(pos);
        k2 = dt* interpolate2(pos+ (k1/2));
        k3 = dt*interpolate2(pos+ (k2/2));
        k4 = dt*interpolate2(pos+ k3);

        pos = pos + (1.0/6.0)*(k1 + 2.0*k2 + 2.0*k3 + k4);
        line.addVertex(pos);
    }
    polylines.push_back(line);
    size++;
    return line;


}


ofVec2f ofApp::WordToScreen(ofVec2f p){
    float scale = scale2 * float(dims[0]) / image_x;
    //cout<<p[0]<<" "<<p[1]<<endl;
    float wx=ofMap(p[0],minX,maxX, 0,dims[0]-1);
    float wy=ofMap(p[1],minY,maxY,0,dims[1]-1);
    wx=(wx-offsetX)/scale+250;
    wy=(-1)*((wy-(dims[1]+offsetY))/(scale))+200;

    //cout<<wx<<" "<<wy<<endl;
    return(ofVec2f(wx,wy));
    
}
ofVec2f ofApp::ScreenToWord(ofVec2f p){
    float scale = scale2 * float(dims[0]) / image_x;
    float wx = (p[0]) * scale +offsetX;
    float wy = dims[1] - (p[1]) * scale+offsetY;
//    cout<<wx<<endl;
//    cout<<wy<<endl;
//    cout<<"========="<<endl;
    wx=ofMap(wx,0, dims[0]-1,minX,maxX);
    wy=ofMap(wy,0, dims[1]-1,minY,maxY);
//    cout<<wy<<endl;
    return(ofVec2f(wx,wy));

}

ofVec2f ofApp::ScreenToWord2(ofVec2f p){
    float scale = scale2 * float(dims[0]) / image_x;
    float wx = (p[0]-250) * scale +offsetX;
    float wy = dims[1] - (p[1]-200) * scale+offsetY;

    wx=ofMap(wx,0, dims[0]-1,minX,maxX);
    wy=ofMap(wy,0, dims[1]-1,minY,maxY);
    return(ofVec2f(wx,wy));
    
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofVec2f pos;
    pos[0]=x;
    pos[1]=y;

    pos=ScreenToWord2(pos);
    rk4_integrate(pos,dt,steps);
    startPoint.push_back(pos);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
