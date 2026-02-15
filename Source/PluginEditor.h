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
static const int verticalMargin = 30;
static const int nRows = 3;
static const int nCols = 6;

static const int windoWidth = 1000;
static const int windowHeight = 450;

struct AmpModelerLAF : public juce::LookAndFeel_V4 {
    
    AmpModelerLAF() {
        
        setColour(juce::Slider::rotarySliderOutlineColourId, widgetColor);
        setColour(juce::Slider::rotarySliderFillColourId, accentColor2);
        setColour(juce::Slider::thumbColourId, accentColor2);
        setColour(juce::Slider::backgroundColourId, widgetColor);
        setColour(juce::Slider::trackColourId, accentColor1);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        
        setColour(juce::ComboBox::backgroundColourId, widgetColor);
        setColour(juce::ComboBox::outlineColourId, widgetColor);
        setColour(juce::ComboBox::buttonColourId, widgetColor);
        setColour(juce::ComboBox::arrowColourId, accentColor1);
        setColour(juce::ComboBox::focusedOutlineColourId, juce::Colours::darkred);
        
        setColour(juce::Label::backgroundColourId, backgroundColor);
        // setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::outlineColourId, juce::Colours::white);
        setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);

        
        setColour(juce::PopupMenu::backgroundColourId, widgetColor);
        
        setColour(juce::TabbedComponent::outlineColourId, widgetColor);
        setColour(juce::TabbedButtonBar::tabOutlineColourId, backgroundColor);
        setColour(juce::TabbedButtonBar::frontOutlineColourId, widgetColor);
        
        setColour(juce::TextButton::buttonColourId, widgetColor);
        setColour(juce::TextButton::buttonOnColourId, accentColor1);
    }
    
    void drawLabel(juce::Graphics& g, juce::Label& label) {
        g.fillAll(label.findColour(juce::Label::backgroundColourId));
    
        if (!label.isBeingEdited()) {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const juce::Font font(getLabelFont(label));
    
            g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
            g.setFont(font);
    
            auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
    
            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                              max(1, (int)((float)textArea.getHeight() / font.getHeight())),
                              label.getMinimumHorizontalScale());

            g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));

        }
        else if (label.isEnabled()) {
            g.setColour(label.findColour(juce::Label::outlineColourId));
        }
    
        g.drawRect(label.getLocalBounds());
    }

    
    void drawComboBox(juce::Graphics& g, int width, int height, bool,
                                       int, int, int, int, juce::ComboBox& box)
    {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);
    
        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
    
        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
    
        juce::Rectangle<int> arrowZone(width - 30, 0, 20, height);
        juce::Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    
        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(2.0f));
    }


    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
        auto fill    = slider.findColour(juce::Slider::rotarySliderFillColourId);
    
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
    
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                               arcRadius, arcRadius,
                               0.0f, rotaryStartAngle, toAngle, true);
    
        g.setColour(fill.withMultipliedAlpha(slider.isEnabled() ? 1.0 : 0.5));
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
        
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider)
    {
    
        assert(!slider.isBar());

        // auto trackWidth = min(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);
        float trackWidth = 12.0f;

        juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
                                     slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
                                   slider.isHorizontal() ? startPoint.y : (float)y);

        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = { kx, ky };


        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        // auto thumbWidth = getSliderThumbRadius(slider);
        // g.setColour(slider.findColour(juce::Slider::thumbColourId));
        // g.fillEllipse(juce::Rectangle<float>(static_cast<float>(thumbWidth), static_cast<float>(thumbWidth)).withCentre(maxPoint));
    }

    juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider)
    {
        // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints
    
        int minXSpace = 0;
        int minYSpace = 0;
    
        auto textBoxPos = slider.getTextBoxPosition();
    
        minYSpace = 5;
    
        auto localBounds = slider.getLocalBounds();
    
        auto textBoxWidth  = max(0, min(slider.getTextBoxWidth(),  localBounds.getWidth() - minXSpace));
        auto textBoxHeight = max(0, min(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));
    
        juce::Slider::SliderLayout layout;
    
        // 2. set the textBox bounds
    
        if (slider.isBar()) {
            layout.textBoxBounds = localBounds;
        } else {
            layout.textBoxBounds.setWidth(textBoxWidth);
            layout.textBoxBounds.setHeight(textBoxHeight);

            layout.textBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);

            if (textBoxPos == juce::Slider::TextBoxAbove)      { layout.textBoxBounds.setY (0); }
            else if (textBoxPos == juce::Slider::TextBoxBelow) { layout.textBoxBounds.setY (localBounds.getHeight() - textBoxHeight); }
        }
    
        // 3. set the slider bounds
    
        layout.sliderBounds = localBounds;
    
        if (slider.isBar()) {
            layout.sliderBounds.reduce (1, 1);   // bar border
        } else {
            if (textBoxPos == juce::Slider::TextBoxAbove)      { layout.sliderBounds.removeFromTop (textBoxHeight); }
            else if (textBoxPos == juce::Slider::TextBoxBelow) { layout.sliderBounds.removeFromBottom (textBoxHeight); }
    
            const int thumbIndent = getSliderThumbRadius (slider);
    
            if (slider.isHorizontal())    { layout.sliderBounds.reduce(thumbIndent, 0); }
            else if (slider.isVertical()) { layout.sliderBounds.reduce(0, thumbIndent); }
        }

        return layout;
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown)
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);
    
        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                          .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);
    
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted) {
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);
        }
    
        g.setColour(baseColour);
    
        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();
    
        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom) {
            juce::Path path;
            path.addRoundedRectangle(bounds.getX(), bounds.getY(),
                                    bounds.getWidth(), bounds.getHeight(),
                                    cornerSize, cornerSize,
                                    !(flatOnLeft  || flatOnTop),
                                    !(flatOnRight || flatOnTop),
                                    !(flatOnLeft  || flatOnBottom),
                                    !(flatOnRight || flatOnBottom));
    
            g.fillPath(path);
    
            g.setColour(button.findColour(juce::ComboBox::outlineColourId));
            g.strokePath(path, juce::PathStrokeType (1.0f));
        } else {
            g.fillRoundedRectangle(bounds, cornerSize);
    
            g.setColour(button.findColour(juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        }
    }
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                         bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        juce::Font font(getTextButtonFont(button, button.getHeight()));
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                              : juce::TextButton::textColourOffId)
                           .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));
    
        const int yIndent = min(4, button.proportionOfHeight (0.3f));
        const int cornerSize = min(button.getHeight(), button.getWidth()) / 2;
    
        const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
        const int leftIndent  = min(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = min(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;
    
        if (textWidth > 0) {
            g.drawFittedText(button.getButtonText(),
                             leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                             juce::Justification::centred, 2);
        }
    }
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
    juce::Label stage0Label {"STAGE_0_LABEL", "Stage 0"};
    juce::Label stage1Label {"STAGE_1_LABEL", "Stage 1"};
    juce::Label stage2Label {"STAGE_2_LABEL", "Stage 2"};
    juce::Label stage3Label {"STAGE_3_LABEL", "Stage 3"};
    juce::Label stage4Label {"STAGE_4_LABEL", "Stage 4"};
    
    Knob stage0LPSlider;
    Knob stage0BypassSlider;
    Knob stage0BiasSlider;

    Knob stage1LPSlider;
    Knob stage1BypassSlider;
    Knob stage1BiasSlider;
    Knob stage1AttSlider;

    Knob stage2LPSlider;
    Knob stage2BypassSlider;
    Knob stage2BiasSlider;
    Knob stage2AttSlider;

    Knob stage3LPSlider;
    Knob stage3BypassSlider;
    Knob stage3BiasSlider;
    Knob stage3AttSlider;

    Knob stage4LPSlider;
    Knob stage4BypassSlider;
    Knob stage4BiasSlider;
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

    Knob highcutFreqKnob;
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
