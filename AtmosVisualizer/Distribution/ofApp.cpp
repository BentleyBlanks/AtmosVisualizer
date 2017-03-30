#include "ofApp.h"

enum a3UniformDiskType
{
    CONCENTRIC,
    AREA
};

enum a3CosineSphereType
{
    PROJECTED = 0,
    DEFAULT
};

ofVec2f a3UniformSampleDisk(const float sampleU, const float sampleV, a3UniformDiskType type = CONCENTRIC)
{
    if(type == CONCENTRIC)
    {
        float phi, r, u, v;
        // (a,b) is now on [-1,1]ˆ2
        float a = 2 * sampleU - 1;
        float b = 2 * sampleV - 1;

        // region 1 or 2
        if(a > -b)
        {
            // region 1, also |a| > |b|
            if(a > b)
            {
                r = a;
                phi = (PI / 4) * (b / a);
            }
            // region 2, also |b| > |a|
            else
            {
                r = b;
                phi = (PI / 4) * (2 - (a / b));
            }
        }
        // region 3 or 4
        else
        {
            // region 3, also |a| >= |b|, a != 0
            if(a < b)
            {
                r = -a;
                phi = (PI / 4) * (4 + (b / a));
            }
            // region 4, |b| >= |a|, but a==0 and b==0 could occur.
            else
            {
                r = -b;
                if(b != 0)
                    phi = (PI / 4) * (6 - (a / b));
                else
                    phi = 0;
            }
        }

        u = r* cos(phi);
        v = r* sin(phi);
        return ofVec2f(u, v);
    }
    else if(type == AREA)
    {
        float r = sqrtf(sampleU);

        float theta = 2.0f * PI * sampleV;

        float u = r * cosf(theta);
        float v = r * sinf(theta);

        return ofVec2f(u, v);
    }
    else
    {
        // a3Log
        return ofVec2f::zero();
    }
}

ofVec3f a3UniformSampleSphere(float u1, float u2)
{
    // z=1-(1-2\xi^2)
    float z = 1.f - 2.f * u1;
    float r = sqrtf(max(0.f, 1.f - z*z));
    float phi = 2.f * PI * u2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    return ofVec3f(x, y, z);
}

ofVec3f a3UniformSampleHemisphere(float u1, float u2)
{
    float z = u1;
    float r = sqrtf(max(0.f, 1.f - z*z));
    float phi = 2 * PI * u2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    return ofVec3f(x, y, z);
}

ofVec3f a3UniformSampleCone(float u1, float u2, float cosThetaMax)
{
    // cos\theta = 1 - \xi + \xi * costhetamax
    float costheta = 1.0f - u1 + u1 * cosThetaMax;
    float sintheta = sqrtf(1.0f - costheta*costheta);
    // \phi = 2 \pi psi
    float phi = 2 * PI * u2;

    float x = cosf(phi) * sintheta;
    float y = sinf(phi) * sintheta;
    float z = costheta;
    return ofVec3f(x, y, z);
}

ofVec3f a3CosineSampleHemisphere(float u1, float u2, a3CosineSphereType type = PROJECTED)
{
    if(type == PROJECTED)
    {
        ofVec2f p = a3UniformSampleDisk(u1, u2);

        float z = sqrtf(max(0.f, 1.f - p.x*p.x - p.y*p.y));

        return ofVec3f(p.x, p.y, z);
    }
    else if(type == DEFAULT)
    {
        float sintheta = sinf(0.5 * acos(1 - 2 * u1));
        float costheta = cosf(0.5 * acos(1 - 2 * u1));

        float v = 2 * PI * u2;

        float x = sintheta * cosf(v);
        float y = sintheta * sinf(v);

        float z = costheta;

        return ofVec3f(x, y, z);
    }
    else
    {
        // a3Log
        return ofVec3f::zero();
    }
}

//--------------------------------------------------------------
enum SceneName
{
    SAMPLING_1D = 0,
    SAMPLING_2D,
    SAMPLING_COSINE_HEMISPHERE,
    SAMPLING_SPHERE,
    SAMPLING_UNIFORM_HEMISPHERE,
    SAMPLING_UNIFORM_CONE
};

//--------------------------------------------------------------
void ofApp::setup(){
    sceneName = SAMPLING_UNIFORM_CONE;

    draw3D = true;

    setupSampling1D();

    setupSampling2D();

    setupSamplingHemisphere();

    setupSamplingSphere();

    setupSamplingCone();

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
void ofApp::setupSamplingHemisphere()
{
    hemiRadius = 200.0f;
    hemiPointNum = 1000;
    hemiSampleType = PROJECTED;

    for(int i = 0; i < hemiPointNum; i++)
    {
        hemiPoints1.push_back(a3CosineSampleHemisphere(ofRandomuf(), ofRandomuf(), PROJECTED));
        hemiPoints2.push_back(a3CosineSampleHemisphere(ofRandomuf(), ofRandomuf(), DEFAULT));
        hemiUniPoints.push_back(a3UniformSampleHemisphere(ofRandomuf(), ofRandomuf()));
    }
}

//--------------------------------------------------------------
void ofApp::setupSamplingSphere()
{
    sphereRadius = 200.0f;
    spherePointNum = 1000;

    for(int i = 0; i < spherePointNum; i++)
    {
        spherePoints.push_back(a3UniformSampleSphere(ofRandomuf(), ofRandomuf()));
    }
}

//--------------------------------------------------------------
void ofApp::setupSamplingCone()
{
    coneRadius = 200.0f;
    conePointNum = 1000;
    cosThetaMax = 0.1f;

    for(int i = 0; i < conePointNum; i++)
    {
        conePoints.push_back(a3UniformSampleCone(ofRandomuf(), ofRandomuf(), cosThetaMax));
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    switch(sceneName)
    {
    case SAMPLING_1D:
        drawSampling1D();
        break;

    case SAMPLING_2D:
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

    case SAMPLING_COSINE_HEMISPHERE:
        cam.begin();

        graph->draw();

        ofDrawAxis(100);

        drawSamplingHemisphere();

        cam.end();
        break;
    case SAMPLING_SPHERE:
        cam.begin();

        graph->draw();

        ofDrawAxis(100);

        drawSamplingSphere();

        cam.end();
        break;
    case SAMPLING_UNIFORM_HEMISPHERE:
        cam.begin();

        graph->draw();

        ofDrawAxis(100);

        drawSamplingUniformHemisphere();

        cam.end();
        break;
    case SAMPLING_UNIFORM_CONE:
        cam.begin();

        graph->draw();

        ofDrawAxis(100);

        drawSamplingCone();

        cam.end();
        break;
    }

    guiControl();
}

//--------------------------------------------------------------
void ofApp::guiControl()
{
    gui.begin();

    ImGui::Combo("Scene Name", &sceneName, "TwoD\0ThreeD\0Hemisphere\0Sphere\0Uniform Hemisphere\0Cone");

    if(sceneName == SAMPLING_COSINE_HEMISPHERE)
    {
        ImGui::DragFloat("HemiSphere Radius", &hemiRadius, 1.0f, 10.0f, 500.0f);
        ImGui::Combo("Sample Hemisphere Type", &hemiSampleType, "Projected\0Default");
        ImGui::DragInt("Hemisphere Points Number", &hemiPointNum, 1, 200, 1500.0f);

        if(ImGui::Button("Calculate Hemisphere"))
        {
            hemiPoints1.clear();
            hemiPoints2.clear();
            for(int i = 0; i < hemiPointNum; i++)
            {
                hemiPoints1.push_back(a3CosineSampleHemisphere(ofRandomuf(), ofRandomuf(), PROJECTED));
                hemiPoints2.push_back(a3CosineSampleHemisphere(ofRandomuf(), ofRandomuf(), DEFAULT));
            }
        }
    }
    else if(sceneName == SAMPLING_SPHERE)
    {
        ImGui::DragFloat("Sphere Radius", &sphereRadius, 1.0f, 10.0f, 500.0f);
        ImGui::DragInt("Sphere Points Number", &spherePointNum, 1, 200, 1500.0f);

        if(ImGui::Button("Calculate Sphere"))
        {
            spherePoints.clear();
            for(int i = 0; i < spherePointNum; i++)
            {
                spherePoints.push_back(a3UniformSampleSphere(ofRandomuf(), ofRandomuf()));
            }
        }
    }
    else if(sceneName == SAMPLING_UNIFORM_HEMISPHERE)
    {
        ImGui::DragFloat("HemiSphere Radius", &hemiRadius, 1.0f, 10.0f, 500.0f);
        ImGui::DragInt("Hemisphere Points Number", &hemiPointNum, 1, 200, 1500.0f);

        if(ImGui::Button("Calculate Uniform Hemisphere"))
        {
            hemiUniPoints.clear();
            for(int i = 0; i < hemiPointNum; i++)
                hemiUniPoints.push_back(a3UniformSampleHemisphere(ofRandomuf(), ofRandomuf()));
        }
    }
    else if(sceneName == SAMPLING_UNIFORM_CONE)
    {
        ImGui::DragFloat("Cone Radius", &coneRadius, 1.0f, 10.0f, 500.0f);
        ImGui::DragFloat("Cone CosThetaMax", &cosThetaMax, 0.01f, 0.0f, 1.0f);
        ImGui::DragInt("Cone Points Number", &conePointNum, 1, 200, 1500.0f);

        if(ImGui::Button("Calculate Uniform Cone"))
        {
            conePoints.clear();
            for(int i = 0; i < conePointNum; i++)
                conePoints.push_back(a3UniformSampleCone(ofRandomuf(), ofRandomuf(), cosThetaMax));
        }
    }
    else
    {
        //if(ImGui::SliderInt("1D", &numOfSampled1D, 10.0f, 100000.0f))
        //{
        //    for(int i = 0; i < sampled.size(); i++)
        //        sampled[i] = 0.0f;
        //
        //    for(int i = 0; i < numOfSampled1D; i++)
        //    {
        //        float u = ofRandomuf();
        //        float x = distribution1D->sampleContinuous(u, NULL);
        //
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
    }

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
void ofApp::drawSamplingHemisphere()
{
    ofPushStyle();
    ofSetColor(0);
    if(hemiSampleType == PROJECTED)
    {
        for(auto p : hemiPoints1)
        {
            ofDrawSphere(p * hemiRadius, 2);
        }
    }
    else
    {
        for(auto p : hemiPoints2)
        {
            ofDrawSphere(p * hemiRadius, 2);
        }
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawSamplingUniformHemisphere()
{
    ofPushStyle();
    ofSetColor(0);
    for(auto p : hemiUniPoints)
    {
        ofDrawSphere(p * hemiRadius, 2);
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawSamplingSphere()
{
    ofPushStyle();
    ofSetColor(0);
    for(auto p : spherePoints)
    {
        ofDrawSphere(p * sphereRadius, 2);
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawSamplingCone()
{
    ofPushStyle();
    ofSetColor(0);    
    for(auto p : conePoints)
    {
        ofDrawSphere(p * coneRadius, 2);
    }
    ofPopStyle();
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
