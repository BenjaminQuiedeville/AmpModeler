/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PluginProcessor.h"

const juce::Colour backgroundColor {0xFF0F0F0F}; // noir sombre
const juce::Colour widgetColor     {0xFF232D3F}; // gris bleuté
const juce::Colour accentColor1    {0XFF005B41}; // vert bouteille
const juce::Colour accentColor2    {0XFF008170}; // bleu plus clair

static const juce::String defaultIRText = "Default IR, made by JuanPabloZed with IR Maker \nhttps://github.com/JuanPabloZed/IR_Maker_Cpp";

static const int knobSize = 100;
static const int horizontalMargin = 25;
static const int verticalMargin = 20;
static const int nRows = 3;
static const int nCols = 6;

static const int windoWidth = 850;
static const int windowHeight = 450;

struct AmpModelerLAF : public juce::LookAndFeel_V4 {
    
    AmpModelerLAF();
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    void drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override;
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider);
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

// changer pour des ButtonParameterAttachment et autres, ca retire une couche d'allocation
using Apvts = juce::AudioProcessorValueTreeState;
using SliderAttachment = Apvts::SliderAttachment;
using ComboBoxAttachment = Apvts::ComboBoxAttachment;
using ButtonAttachment = Apvts::ButtonAttachment;

struct Editor; 

struct Knob : public juce::Slider {
    Knob(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix);

    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};



struct VSlider : public juce::Slider {
    VSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix);

    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};

struct HSlider : public juce::Slider {
    HSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix);

    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
    juce::Identifier paramID;
};

struct ComboBox : public juce::ComboBox {
    ComboBox(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts);

    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


struct GateBoostPage : public juce::Component {
    GateBoostPage(Editor *editor, Processor &p);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(backgroundColor);
    }

    Editor *main_editor = nullptr;

    Knob gateKnob;
    // Knob gateAttackKnob;
    // Knob gateReleaseKnob;
    // Knob gateReturnKnob;

    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;

    Knob inputGainKnob;

    juce::TextButton gateToggle {"Activate Gate/Boost"};
    juce::TextButton preampToggle {"Activate Preamp"};
    juce::TextButton tonestackToggle {"Activate Tonestack"};
    juce::TextButton EQToggle {"Activate EQ"};

    std::unique_ptr<ButtonAttachment> gateToggleAttachment;
    std::unique_ptr<ButtonAttachment> preampToggleAttachment;
    std::unique_ptr<ButtonAttachment> tonestackToggleAttachment;
    std::unique_ptr<ButtonAttachment> EQToggleAttachment;

};


struct AmplifierPage : public juce::Component {
    AmplifierPage(Editor *editor, Processor &p);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(backgroundColor);
    }

    Editor *main_editor = nullptr;

    Knob gain1Knob;
    Knob gain2Knob;
    Knob inputFilterKnob;
    Knob bassEQKnob;
    Knob midEQKnob;
    Knob trebbleEQKnob;
    Knob preampVolumeKnob;

    Knob resonanceKnob;
    Knob presenceKnob;

    ComboBox ampChannelBox;
    ComboBox toneStackModelBox;
    ComboBox stereoSettingBox;

    juce::TextButton brightToggle {"Bright Switch"};
    std::unique_ptr<ButtonAttachment> brightToggleAttachment;
};


struct GainStagesPage : public juce::Component {
    GainStagesPage(Editor *editor, Processor &p);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(backgroundColor);
    }

    Editor *main_editor = nullptr;

    juce::TextButton resetButton {"Reset"};
    juce::Label stage1Label {"STAGE_1_LABEL", "Stage 1"};
    juce::Label stage2Label {"STAGE_2_LABEL", "Stage 2\nChannel 1"};
    juce::Label stage3Label {"STAGE_3_LABEL", "Stage 3\nChannel 2"};
    juce::Label stage4Label {"STAGE_4_LABEL", "Stage 4\nChannel 3"};
    
    Knob stage1LP;
    Knob stage1Bypass;
    Knob stage1Bias;
    Knob stage1Gain;

    Knob stage2LP;
    Knob stage2Bypass;
    Knob stage2Bias;
    Knob stage2Gain;

    Knob stage3LP;
    Knob stage3Bypass;
    Knob stage3Bias;
    Knob stage3Gain;

    Knob stage4LP;
    Knob stage4Bypass;
    Knob stage4Bias;
    Knob stage4Gain;
};


struct IRLoaderPage : public juce::Component {
    IRLoaderPage(Editor *editor, Processor &audioProcessor);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(backgroundColor);
    }

    Editor *main_editor = nullptr;

    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::TextButton nextIRButton {"Next"};
    juce::TextButton prevIRButton {"Previous"};

    juce::TextButton defaultIRButton {"Load default IR"};

    juce::TextButton bypassToggle {"Activate IRloader"};
    std::unique_ptr<ButtonAttachment> bypassButtonAttachment;
};


struct EQPage : public juce::Component {
    EQPage(Editor *editor, Processor &audioProcessor);
    
    void paint(juce::Graphics &g) {
        g.fillAll(backgroundColor);
    }
    
    void resized();

    Editor *main_editor = nullptr;

    juce::TextButton resetButton {"Reset Gains"};

    Knob lowcutFreqKnob;

    Knob lowShelfFreqKnob;
    Knob lowShelfGainKnob;

    Knob lowMidFreqKnob;
    Knob lowMidGainKnob;
    Knob lowMidQKnob;

    Knob midFreqKnob;
    Knob midGainKnob;
    Knob midQKnob;

    Knob highFreqKnob;
    Knob highGainKnob;
    Knob highQKnob;

    Knob highShelfFreqKnob;
    Knob highShelfGainKnob;

    Knob highCutFreqKnob;
};


struct MasterVolPanel : public juce::Component {

    MasterVolPanel(Processor &p) :
        volumeSlider("MASTER_VOLUME_SLIDER_LABEL", "Master Vol", this, paramInfos[MASTER_VOLUME].id.toString(), p.apvts, " dB")
    {
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(backgroundColor);
    }

    void resized() {
        volumeSlider.setBounds(0, 50, getWidth(), 300);
        volumeSlider.label.setBounds(volumeSlider.getX(),volumeSlider.getY() - 15,
                                    volumeSlider.getWidth(), 20);
    }

    VSlider volumeSlider;
};


struct Editor : public juce::AudioProcessorEditor
{
    Editor(Processor&);
    ~Editor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::TabbedComponent tabs {juce::TabbedButtonBar::TabsAtTop};
    juce::Component statusBar;
    AmpModelerLAF lookAndFeel;
    
    GateBoostPage gateBoostPage;
    AmplifierPage ampPage;
    GainStagesPage gainStagesPage;
    EQPage eqPage;
    IRLoaderPage irLoaderPage;

    MasterVolPanel volumePanel;

    Processor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
