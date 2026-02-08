/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PluginProcessor.h"

static const juce::String defaultIRText = "Default IR, made by JuanPabloZed with IR Maker \nhttps://github.com/JuanPabloZed/IR_Maker_Cpp";

static const int knobSize = 100;
static const int horizontalMargin = 25;
static const int verticalMargin = 30;
static const int nRows = 3;
static const int nCols = 6;

static const int windoWidth = 1000;
static const int windowHeight = 450;


struct AmpModelerLAF : public juce::LookAndFeel_V4 {

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
        auto fill    = juce::Colours::darkred;
    
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    
        auto radius = min(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = min(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;
    
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                    arcRadius, arcRadius,
                                    0.0f, rotaryStartAngle, rotaryEndAngle,
                                    true);
    
        g.setColour(outline);
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
        if (slider.isEnabled()) {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                   arcRadius, arcRadius,
                                   0.0f, rotaryStartAngle, toAngle, true);
    
            g.setColour(fill);
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    
        auto thumbWidth = lineW * 2.0f;
        // juce::Point<float>thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        //                              bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
    
        // g.setColour(slider.findColour(juce::Slider::thumbColourId));
        // g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
    }
};


using Apvts = juce::AudioProcessorValueTreeState;
using SliderAttachment = Apvts::SliderAttachment;
using ComboBoxAttachment = Apvts::ComboBoxAttachment;
using ButtonAttachment = Apvts::ButtonAttachment;

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
    GateBoostPage(Processor &p);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(juce::Colours::black);
    }

    Knob gateKnob;
    // Knob gateAttackKnob;
    // Knob gateReleaseKnob;
    // Knob gateReturnKnob;

    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;

    Knob inputGainKnob;

    juce::ToggleButton gateToggle {"Activate Gate/Boost"};
    juce::ToggleButton preampToggle {"Activate Preamp"};
    juce::ToggleButton tonestackToggle {"Activate Tonestack"};
    juce::ToggleButton EQToggle {"Activate EQ"};

    std::unique_ptr<ButtonAttachment> gateToggleAttachment;
    std::unique_ptr<ButtonAttachment> preampToggleAttachment;
    std::unique_ptr<ButtonAttachment> tonestackToggleAttachment;
    std::unique_ptr<ButtonAttachment> EQToggleAttachment;

};


struct AmplifierPage : public juce::Component {
    AmplifierPage(Processor &p);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(juce::Colours::black);
    }

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

    juce::ToggleButton brightToggle {"Bright Switch"};
    std::unique_ptr<ButtonAttachment> brightToggleAttachment;
};


struct GainStagesPage : public juce::Component {
    GainStagesPage(Processor &p);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(juce::Colours::black);
    }

    juce::TextButton resetButton {"Reset"};

    HSlider stage0LPSlider;
    HSlider stage0BypassSlider;
    HSlider stage0BiasSlider;

    HSlider stage1LPSlider;
    HSlider stage1BypassSlider;
    HSlider stage1BiasSlider;
    HSlider stage1AttSlider;

    HSlider stage2LPSlider;
    HSlider stage2BypassSlider;
    HSlider stage2BiasSlider;
    HSlider stage2AttSlider;

    HSlider stage3LPSlider;
    HSlider stage3BypassSlider;
    HSlider stage3BiasSlider;
    HSlider stage3AttSlider;

    HSlider stage4LPSlider;
    HSlider stage4BypassSlider;
    HSlider stage4BiasSlider;
};


struct IRLoaderPage : public juce::Component {
    IRLoaderPage(Processor &audioProcessor);
    void resized();

    void paint(juce::Graphics &g) {
        g.fillAll(juce::Colours::black);
    }

    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::TextButton nextIRButton {"Next"};
    juce::TextButton prevIRButton {"Previous"};

    juce::TextButton defaultIRButton {"Load default IR"};

    juce::ToggleButton bypassToggle {"Activate IRloader"};
    std::unique_ptr<ButtonAttachment> bypassButtonAttachment;
};


struct EQPage : public juce::Component {
    EQPage(Processor &audioProcessor);
    
    void paint(juce::Graphics &g) {
        g.fillAll(juce::Colours::black);
    }
    
    void resized();

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

    Knob highcutFreqKnob;
};


struct MasterVolPanel : public juce::Component {

    MasterVolPanel(Processor &p) :
        volumeSlider("MASTER_VOLUME_SLIDER_LABEL", "Master Vol", this, paramInfos[MASTER_VOLUME].id.toString(), p.apvts, " dB")
    {
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black);
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
