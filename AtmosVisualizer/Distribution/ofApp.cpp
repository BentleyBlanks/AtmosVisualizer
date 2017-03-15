#include "ofApp.h"

//--------------------------------------------------------------
enum SceneName
{
    SAMPLING1D,
    SAMPLING2D
};

//--------------------------------------------------------------
void ofApp::setup(){
    sceneName = SAMPLING2D;

    draw3D = true;

    setupSampling1D();

    setupSampling2D();

    gui.setup();
}

//--------------------------------------------------------------
void ofApp::setupSampling1D()
{
    //coordinateSample.setAxisLength(400, 400);
    //coordinateSample.setOrigin(ofGetWindowWidth() / 3 + 40, 700);

    coordinateSample.setAxisLength(400, 400);
    coordinateSample.setOrigin(40, 700);

    coordinateLeft.setAxisLength(400, 400);
    coordinateLeft.setOrigin(40, 700);

    //coordinateRight.setOrigin(ofGetWindowWidth() * 2 / 3 + 40, 700);
    //coordinateRight.setAxisLength(400, 400);

    coordinateRight.setOrigin(ofGetWindowWidth() / 2 + 40, 700);
    coordinateRight.setAxisLength(400, 400);

    numOfSampled1D = 10;
    // 非负
    float pdfTemp[] = {30, 55, 125, 216, 50, 80, 40, 136};

    // 从数组初始化
    numOfData = sizeof(pdfTemp) / sizeof(pdfTemp[0]);
    pdf = new vector<float>(pdfTemp, pdfTemp + numOfData);

    distribution1D = new a3Distribution1D(*pdf);

    sampled.resize(numOfData);

    for(int i = 0; i < numOfSampled1D; i++)
    {
        float u = ofRandomuf();
        float x = distribution1D->sampleContinuous(u, NULL);

        int index = x * numOfData;
        sampled[index]++;
    }

    //for(int i = 0; i < sampled.size(); i++)
    //    printf("%f,", sampled[i]);
}

//--------------------------------------------------------------
void ofApp::setupSampling2D()
{
    graph = new Graph3D(400, 400, 100, 100);

    drawOriginal = true;
    drawSample = true;

    //image.load("1.exr");
    hdrImage.load("1.exr");

    //imageData.push_back(200);
    //imageData.push_back(15);
    //imageData.push_back(100);
    //imageData.push_back(30);
    //imageData.push_back(10);
    //imageData.push_back(50);
    //imageData.push_back(5);
    //imageData.push_back(250);
    //imageData.push_back(220);

    //dataWidth = image.getWidth();
    //dataHeight = image.getHeight();

    dataWidth = hdrImage.getWidth();
    dataHeight = hdrImage.getHeight();

    for(int i = 0; i < dataHeight; i++)
    {
        for(int j = 0; j < dataWidth; j++)
        {
            ofFloatColor c = hdrImage.getColor(j, i);

            // 0.299*R + 0.587*G + 0.114*B

            float y = c.r * 0.299 + c.g * 0.587 + c.b * 0.114;

            imageData.push_back(y);
        }
    }

    //dataWidth = 4;
    //dataHeight = 2;

    samplingImage = new a3SamplingImage(imageData, hdrImage);

    samplingImage->setOrigin(40, 40);
    samplingImage->setDisplayDimension(512, 768);

    numOfSampled2D = 100;

    distribution2D = new a3Distribution2D(imageData, dataWidth, dataHeight);

    points.resize(numOfSampled2D);

    for(int i = 0; i < numOfSampled2D; i++)
    {
        float u0 = ofRandomuf(), u1 = ofRandomuf();
        
        float x0 = 0.0f, x1 = 0.0f, pdf = 0.0f;

        distribution2D->sampleContinuous(u0, u1, &x0, &x1, &pdf);

        points.push_back(ofVec2f(x0, x1));
    }

    //for(int i = 0; i < points.size(); i++)
    //    printf("%f, ", points[i]);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    switch(sceneName)
    {
    case SAMPLING1D:
        drawSampling1D();
        break;

    case SAMPLING2D:

        if(draw3D)
        {
            cam.begin();

            graph->draw();

            ofDrawAxis(100);

            cam.end();
        }
        else
            drawSampling2D();


        break;
    }

    guiControl();
}

//--------------------------------------------------------------

void ofApp::guiControl()
{
    gui.begin();

    //if(ImGui::SliderInt("1D", &numOfSampled1D, 10.0f, 100000.0f))
    //{
    //    for(int i = 0; i < sampled.size(); i++)
    //        sampled[i] = 0.0f;

    //    for(int i = 0; i < numOfSampled1D; i++)
    //    {
    //        float u = ofRandomuf();
    //        float x = distribution1D->sampleContinuous(u, NULL);

    //        int index = x * numOfData;
    //        sampled[index]++;
    //    }
    //    int j = sampled.size();
    //    for(int i = 0; i < sampled.size(); i++)
    //        printf("%f,", sampled[i]);
    //}

    if(ImGui::SliderInt("2D", &numOfSampled2D, 10.0f, 100000.0f))
    {
        points.resize(numOfSampled2D);

        for(int i = 0; i < numOfSampled2D; i++)
        {
            float u0 = ofRandomuf(), u1 = ofRandomuf();

            float x0 = 0.0f, x1 = 0.0f, pdf = 0.0f;

            distribution2D->sampleContinuous(u0, u1, &x0, &x1, &pdf);

            points.push_back(ofVec2f(x0, x1));
        }
    }

    if(ImGui::Button("Original"))
        drawOriginal = !drawOriginal;

    if(ImGui::Button("Samples"))
        drawSample = !drawSample;
    
    if(ImGui::Button("Draw 2D/3D"))
        draw3D = !draw3D;

    gui.end();
}


//--------------------------------------------------------------
void ofApp::drawSampling1D()
{
    //ofDrawBitmapString("PDF", ofPoint(ofGetWindowWidth() / 6, 40));

    // 分割线
    //ofDrawBitmapString("Sampled", ofPoint(ofGetWindowWidth() / 2, 40));

    //ofDrawBitmapString("CDF", ofPoint(ofGetWindowWidth() * 5 / 6, 40));

    //ofDrawLine(ofPoint(ofGetWindowWidth() / 3, 0), ofPoint(ofGetWindowWidth() / 3, ofGetWindowHeight()));

    //ofDrawLine(ofPoint(ofGetWindowWidth() * 2 / 3, 0), ofPoint(ofGetWindowWidth() * 2 / 3, ofGetWindowHeight()));

    ofDrawLine(ofPoint(ofGetWindowWidth() / 2, 0), ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight()));

    ofDrawLine(ofPoint(ofGetWindowWidth() / 2, 0), ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight()));

    // PDF
    ofPushStyle();

    ofSetColor(ofColor(0));

    coordinateLeft.drawAxis();

    ofSetColor(ofColor(200, 40, 40));

    coordinateLeft.drawPiecewise(*pdf);

    ofSetColor(ofColor(40, 200, 40));

    coordinateSample.drawPiecewise(sampled);

    ofPopStyle();

    // Sampled
    //ofPushStyle();

    //ofSetColor(ofColor(0));

    //coordinateSample.drawAxis();

    //ofSetColor(ofColor(40, 200, 40));

    //coordinateSample.drawPiecewise(sampled);

    //ofPopStyle();

    // CDF
    ofPushStyle();

    ofSetColor(ofColor(0));

    coordinateRight.drawAxis();

    ofSetColor(ofColor(40, 40, 200));

    coordinateRight.drawLinear(distribution1D->cdf);

    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawSampling2D()
{
    if(drawOriginal)
        samplingImage->drawImage();
    else
        samplingImage->drawImageData();

    if(drawSample)
        samplingImage->drawSamples(points);
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

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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
