/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PdBase.hpp"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    // Patch loading
    void reloadPatch(double sampleRate);
    void setPatchFile(File file);
    File getPatchFile();
    String status = "Select a pure data patch file...";
    bool patchLoadError = false;
    bool isPdComputingAudio = false;
    
private:
    //==============================================================================
    // Your private member variables go here...
    Random random;
    
    int numOutputs = 2;
    int numInputs = 2;
    
    ScopedPointer<pd::PdBase> pd;
    int pos;
    
    AudioPlayHead::CurrentPositionInfo positionInfo;
    File patchfile;
    
    pd::Patch patch;
    HeapBlock<float> pdInBuffer, pdOutBuffer;
    double cachedSampleRate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
