/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PluginProcessor.h"

const juce::String defaultIRText = "Default IR, made by JuanPabloZed with IR Maker \nhttps://github.com/JuanPabloZed/IR_Maker_Cpp";

const int knobSize = 100;
const int horizontalMargin = 25;
const int verticalMargin = 50;
const int nRows = 3;
const int nCols = 6;

using Apvts = juce::AudioProcessorValueTreeState;
using SliderAttachment = Apvts::SliderAttachment;
using ComboBoxAttachment = Apvts::ComboBoxAttachment;
using ButtonAttachment = Apvts::ButtonAttachment;

struct Knob : public juce::Slider {

    Knob(juce::String labelID, juce::String name, juce::Component* comp);
    
    void init(juce::String paramID, Apvts &apvts);
    
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct Slider : public juce::Slider {
    Slider(juce::String labelID, juce::String name, juce::Component* comp);
    
    void init(juce::String paramID, Apvts &apvts);
    
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct ComboBox : public juce::ComboBox {
    
    ComboBox(juce::String labelID, juce::String name, juce::Component* comp); 

    void init(juce::String paramID, Apvts &apvts);

    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


struct GateBoostPage : public juce::Component {
    GateBoostPage(Apvts &apvts);
    void resized();
    
    Knob gateKnob;
    Knob gateAttackKnob;
    Knob gateReleaseKnob; 
    Knob gateReturnKnob;

    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;

};


struct AmplifierPage : public juce::Component {
    AmplifierPage(Apvts &apvts);
    void resized();
    
    Knob gainKnob;
    Knob inputFilterKnob;
    Knob bassEQKnob;
    Knob midEQKnob;
    Knob trebbleEQKnob;
    Knob preampVolumeKnob;

    Knob resonanceKnob;
    Knob presenceKnob;

    ComboBox ampChannelBox;
    ComboBox toneStackModelBox;
    ComboBox channelConfigBox;    
};


struct GainStagesPage : public juce::Component {
    GainStagesPage() {
    
    }
    
    void resized() {
    
    }
};


struct IRLoaderPage : public juce::Component {
    IRLoaderPage(Processor &audioProcessor);
    void resized();
        
    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::TextButton nextIRButton {"Next"};
    juce::TextButton prevIRButton {"Previous"};

    juce::TextButton defaultIRButton {"Load default IR"};
    
    juce::ToggleButton bypassToggle {"Bypass IRloader"};
    std::unique_ptr<ButtonAttachment> bypassButtonAttachment;

};


struct MasterVolPanel : public juce::Component {

    MasterVolPanel(Processor &p) :     
        volumeSlider("MASTER_VOLUME_KNOB_LABEL", "Master Vol", this)
    {
        volumeSlider.init(ParamIDs[MASTER_VOLUME].toString(), p.apvts);
        volumeSlider.setTextValueSuffix(" dB");
        
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::darkgrey);
    }


    void resized() {
    
        volumeSlider.setBounds(0, 50, getWidth(), 300);
        volumeSlider.label.setBounds(volumeSlider.getX(), 
                                    volumeSlider.getY() - 15, 
                                    volumeSlider.getWidth(), 
                                    20);
    } 


    Slider volumeSlider;

};


struct Editor : public juce::AudioProcessorEditor
{
    Editor (Processor&);

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::TabbedComponent tabs {juce::TabbedButtonBar::TabsAtTop};
    juce::Component statusBar;

    GateBoostPage gateBoostPage;
    AmplifierPage ampPage;
    GainStagesPage gainStagesPage;
    IRLoaderPage irLoaderPage;
    
    MasterVolPanel volumePanel;

    Processor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
