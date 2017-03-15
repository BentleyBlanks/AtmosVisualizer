#include "a3SamplingImage.h"

a3SamplingImage::a3SamplingImage(vector<float> data, ofFloatImage image)
    :data(data), image(image), displayWidth(100), displayHeight(100), width(image.getWidth()), height(image.getHeight())
{

}

a3SamplingImage::~a3SamplingImage()
{
}

void a3SamplingImage::setOrigin(int x, int y)
{
    origin.set(x, y);
}

void a3SamplingImage::setDisplayDimension(int displayWidth, int displayHeight)
{
    this->displayWidth = displayWidth;
    this->displayHeight = displayHeight;

    if(width && height)
    {
        displayIntervalX = displayWidth / width;
        displayIntervalY = displayHeight / height;
    }
}

void a3SamplingImage::drawSamples(vector<ofVec2f> samples)
{
    ofPushStyle();

    ofSetColor(ofColor(220, 20, 20));
    int j = samples.size();
    for(int i = 0; i < samples.size(); i++)
        ofDrawCircle(samples[i].x * displayWidth + origin.x, samples[i].y * displayHeight + origin.y, 1.5);

    ofPopStyle();
}

void a3SamplingImage::drawImageData()
{
    ofPushStyle();

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            ofSetColor(data[i*width + j]);

            float startX = j * displayIntervalX + origin.x, startY = i *displayIntervalY + origin.y;

            ofDrawRectangle(ofPoint(startX, startY), displayIntervalX, displayIntervalY);
        }
    }

    ofPopStyle();
}

void a3SamplingImage::drawImage()
{
    image.draw(origin, displayWidth, displayHeight);
}
