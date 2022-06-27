#include "MainComponent.h"

MainComponent::MainComponent()
    : Component("Main Window Central Component")
{
    this->setSize(600, 400);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
}

void MainComponent::resized()
{
}