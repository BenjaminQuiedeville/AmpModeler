/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PluginProcessor.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

struct Editor;

struct Knob : public juce::Slider {

    Knob(juce::String labelID, juce::String name, juce::Component* comp);
    
    void init(juce::String paramID, Editor *editor);
    
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct ComboBox : public juce::ComboBox {
    
    ComboBox(juce::String labelID, juce::String name, juce::Component* comp); 

    void init(juce::String paramID, Editor *editor);

    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


struct GateBoostPage : public juce::Component {
    GateBoostPage() {
    
    }
    
    void resized() {
    
    }
};


struct AmplifierPage : public juce::Component {
    AmplifierPage() {
    
    }
    
    void resized() {
    
    }
};


struct GainStagesPage : public juce::Component {
    GainStagesPage() {
    
    }
    
    void resized() {
    
    }
};


struct IRLoaderPage : public juce::Component {
    IRLoaderPage() {
    
    }
    
    void resized() {
    
    }
};


// struct TabsComponent : public juce::TabbedComponent {
    
//     TabsComponent() : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop) {
        
//         auto colour = findColour (juce::ResizableWindow::backgroundColourId);
        
//         addTab("Gate Boost", colour, new GateBoostPage(),  true);
//         addTab("Amp",        colour, new AmplifierPage(),  true);
//         addTab("GainStages", colour, new GainStagesPage(), true);
//         addTab("IRLoader",   colour, new IRLoaderPage(),   true);

//     }
    
//     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabsComponent)
// };


struct Editor : public juce::AudioProcessorEditor
{
    Editor (Processor&);
    ~Editor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    Knob gateKnob;
    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;
    Knob gainKnob;
    Knob inputFilterKnob;
    Knob bassEQKnob;
    Knob midEQKnob;
    Knob trebbleEQKnob;
    Knob preampVolumeKnob;

    Knob resonanceKnob;
    Knob presenceKnob;

    Knob volumeKnob;

    ComboBox ampChannelBox;
    ComboBox toneStackModelBox;
    ComboBox channelConfigBox;
    
    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::TextButton irLoaderDefaultIRButton {"Load default IR"};
    juce::ToggleButton irLoaderBypassToggle {"Bypass IRloader"};

    juce::TabbedComponent tabs {juce::TabbedButtonBar::TabsAtTop};
    juce::Component statusBar;

    GateBoostPage gateBoostPage;
    AmplifierPage ampPage;
    GainStagesPage gainStagesPage;
    IRLoaderPage irLoaderPage;


    Processor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
