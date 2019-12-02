/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    
    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
//        setAudioChannels (2, 2);
        setAudioChannels (numInputs, numOutputs); // no inputs, two outputs
    }
    
    
    patchfile = File::getSpecialLocation(File::currentApplicationFile).getChildFile("main.pd");
    
    String message;
    message << "setting pure data file with size " << newLine;
    message << File::getCurrentWorkingDirectory().getFullPathName() << newLine;
    message << File::getSpecialLocation(File::currentApplicationFile).getChildFile("main.pd").getFullPathName() << newLine;
    message << patchfile.getSize() << newLine;
    
    
#if JUCE_IOS
    Array<File> results;
    File::getSpecialLocation( File::SpecialLocationType::currentApplicationFile).findChildFiles(results, File::findFilesAndDirectories, true);
    for( auto& result : results )
    {
        DBG( "result: " << result.getFullPathName() );
    }
#endif
    
    Logger::getCurrentLogger()->writeToLog (message);
    setPatchFile(patchfile);
    reloadPatch(NULL);
    Logger::getCurrentLogger()->writeToLog (status);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.
    
    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.
    
    // For more details, see the help for AudioProcessor::prepareToPlay()
    
    String message;
    message << "Preparing to play audio..." << newLine;
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine;
    message << " sampleRate = " << sampleRate;
    Logger::getCurrentLogger()->writeToLog (message);
    
    
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    
    if (isPdComputingAudio) {
        
        int len = bufferToFill.numSamples;
        int idx = 0;
        while (len > 0)
        {
            int max = jmin (len, pd->blockSize());
                        
            pd->processFloat (1, pdInBuffer.getData(), pdOutBuffer.getData());
            
            /* write-back */
            
            {
                const float* srcBuffer = pdOutBuffer.getData();
                for (int i = 0; i < max; ++i)
                {
                    for (int channelIndex = 0; channelIndex < bufferToFill.buffer->getNumChannels(); ++channelIndex)
                        bufferToFill.buffer->getWritePointer(channelIndex) [idx + i] = *srcBuffer++;
                }
            }
            
            idx += max;
            len -= max;
        }
    } else {
        bufferToFill.clearActiveBufferRegion();
    }
    
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    
    // For more details, see the help for AudioProcessor::releaseResources()
    
    Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}


//==============================================================================
// loading patch
void MainComponent::reloadPatch (double sampleRate)
{
    if (sampleRate) {
        cachedSampleRate = sampleRate;
    } else {
        sampleRate = cachedSampleRate;
    }
    
    if (pd) {
        pd->computeAudio(false);
        isPdComputingAudio = false;
        pd->closePatch(patch);
    }
    
    pd = new pd::PdBase;
    pd->init (numInputs, numOutputs, sampleRate);
    
    int numChannels = jmin (numInputs, numOutputs);
    pdInBuffer.calloc (pd->blockSize() * numChannels);
    pdOutBuffer.calloc (pd->blockSize() * numChannels);
    
    
    if (!patchfile.exists()) {
        if (patchfile.getFullPathName().toStdString() != "") {
            status = "File does not exist";
        }
        // else keeps select patch message
        return;
    }
    
    if (patchfile.isDirectory()) {
        status = "You selected a directory";
        return;
    }
    
    patch = pd->openPatch (patchfile.getFileName().toStdString(), patchfile.getParentDirectory().getFullPathName().toStdString());
    
    if (patch.isValid()) {
        
        if(!patchLoadError) {
            status = "Patch loaded successfully";
            pd->computeAudio (true);
            isPdComputingAudio = true;
        }

        patchLoadError = false;
    } else {
        status = "Selected patch is not valid";
        patchLoadError = true;
    }

}

void MainComponent::setPatchFile(File file)
{
    patchfile = file;
}

File MainComponent::getPatchFile()
{
    return patchfile;
}



