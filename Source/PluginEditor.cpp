/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginEditor.h"


static inline int computeXcoord(float col, int width) {
    int usableWidth = width - 2*horizontalMargin - knobSize;
    return horizontalMargin + int(usableWidth/nCols * (int)col);
}

static inline int computeYcoord(float row, int height) {
    return verticalMargin + int((height - 2*verticalMargin)/nRows * (int)row);
}

static inline void placeKnob(Knob *knob, int x, int y, int size) {
    knob->setBounds(x, y, size, size);
    knob->label.setBounds(knob->getX(), knob->getY() - 15, size, 20);
}

AmpModelerLAF::AmpModelerLAF() {
    
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
    setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    // setColour(juce::Label::outlineColourId, juce::Colours::white);
    setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);

    
    setColour(juce::PopupMenu::backgroundColourId, widgetColor);
    
    setColour(juce::TabbedComponent::outlineColourId, widgetColor);
    setColour(juce::TabbedButtonBar::tabOutlineColourId, backgroundColor);
    setColour(juce::TabbedButtonBar::frontOutlineColourId, widgetColor);
    
    setColour(juce::TextButton::buttonColourId, widgetColor);
    setColour(juce::TextButton::buttonOnColourId, accentColor1);
}

void AmpModelerLAF::drawLabel(juce::Graphics& g, juce::Label& label) {
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


void AmpModelerLAF::drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
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


void AmpModelerLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                     float sliderPos, float rotaryStartAngle,
                                     float rotaryEndAngle, juce::Slider& slider)
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

    g.setColour(fill.withMultipliedAlpha(slider.isEnabled() ? 1.0 : 0.3));
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}
    
void AmpModelerLAF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                     float sliderPos, float minSliderPos, float maxSliderPos,
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

juce::Slider::SliderLayout AmpModelerLAF::getSliderLayout (juce::Slider& slider)
{
    // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

    int minXSpace = 0;
    int minYSpace = 0;

    auto textBoxPos = slider.getTextBoxPosition();

    minYSpace = 5;

    auto localBounds = slider.getLocalBounds();
    auto sliderBounds = slider.getBounds();

    static const int textBoxWidth = 120;
    static const int textBoxHeight = 30;

    juce::Slider::SliderLayout layout;
    

    // 2. set the textBox bounds

    if (slider.isBar()) {
        layout.textBoxBounds = localBounds;
    } else {
        layout.textBoxBounds.setWidth(textBoxWidth);
        layout.textBoxBounds.setHeight(textBoxHeight);

        layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

        if (textBoxPos == juce::Slider::TextBoxAbove)      { layout.textBoxBounds.setY(0); }
        else if (textBoxPos == juce::Slider::TextBoxBelow) { layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight - 15); }
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

void AmpModelerLAF::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
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

void AmpModelerLAF::drawButtonText(juce::Graphics& g, juce::TextButton& button,
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


GateBoostPage::GateBoostPage(Editor *editor, Processor &p) :
    gateKnob(       "GATE_KNOB_LABEL",         "Gate Thresh",     this, paramInfos[GATE_THRESH].id.toString(),     p.apvts, " dB"),
    // gateAttackKnob( "GATE_Attack_LABEL",       "Gate Attack",     this, paramInfos[GATE_ATTACK].id.toString(),     p.apvts, " ms"),
    // gateReleaseKnob("GATE_RELEASE_LABEL",      "Gate Release",    this, paramInfos[GATE_RELEASE].id.toString(),    p.apvts, " ms"),
    // gateReturnKnob( "GATE_RETURN_LABEL",       "Gate Hysteresis", this, paramInfos[GATE_RETURN].id.toString(),     p.apvts, " dB"),
    boostAttackKnob("BOOST_ATTACK_KNOB_LABEL", "Boost Top",       this, paramInfos[SCREAMER_AMOUNT].id.toString(), p.apvts, " dB"),
    boostFreqKnob(  "BOOST_FREQ_KNOB_LABEL",   "Boost Freq",      this, paramInfos[SCREAMER_FREQ].id.toString(),   p.apvts, " Hz"),
    boostTightKnob( "BOOST_TIGHT_KNOB_LABEL",  "Boost Tight",     this, paramInfos[TIGHT].id.toString(),           p.apvts, " Hz"),
    inputGainKnob(  "INPUT_GAIN_KNOB_LABEL",   "Input Gain",      this, paramInfos[INPUT_GAIN].id.toString(),      p.apvts, " dB")

{
    this->main_editor = editor; 

    gateToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[DO_GATE].id.toString(), gateToggle
    );

    gateToggle.setToggleable(true);
    gateToggle.setClickingTogglesState(true);

    // disable/enable the matching knobs on the different pages
    gateToggle.onStateChange = [this]() {
        bool state = gateToggle.getToggleState();
        
        gateKnob.setEnabled(state);
    };
        
    addAndMakeVisible(gateToggle);
    
    boostToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[DO_BOOST].id.toString(), boostToggle
    );
    
    boostToggle.setToggleable(true);
    boostToggle.setClickingTogglesState(true);
    
    boostToggle.onStateChange = [this]() {
        bool state = boostToggle.getToggleState();
        
        boostAttackKnob.setEnabled(state);
        boostFreqKnob.setEnabled(state);
        boostTightKnob.setEnabled(state);        
    };
    
    addAndMakeVisible(boostToggle);
}


void GateBoostPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();

    placeKnob(&inputGainKnob, computeXcoord(0, width), computeYcoord(0, height), knobSize);
    placeKnob(&gateKnob, computeXcoord(0, width), computeYcoord(1, height), knobSize);

    // gateAttackKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    // gateAttackKnob.label.setBounds(gateAttackKnob.getX(), gateAttackKnob.getY() - 15, knobSize, 20);

    // gateReleaseKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReleaseKnob.label.setBounds(gateReleaseKnob.getX(), gateReleaseKnob.getY() - 15, knobSize, 20);

    // gateReturnKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReturnKnob.label.setBounds(gateReturnKnob.getX(), gateReturnKnob.getY() - 15, knobSize, 20);


    placeKnob(&boostAttackKnob, computeXcoord(2, width), computeYcoord(0, height), knobSize);
    placeKnob(&boostFreqKnob, computeXcoord(3, width), computeYcoord(0, height), knobSize);
    placeKnob(&boostTightKnob, computeXcoord(2, width), computeYcoord(1, height), knobSize);

    gateToggle.setBounds(boostFreqKnob.getRight() + 10, boostFreqKnob.getY() + 10, 130, 30);
    boostToggle.setBounds(gateToggle.getBounds() + juce::Point(0, gateToggle.getHeight() + 10));
}


static void updateGainStageKnobsState(Editor *editor, int channel) {
    
    editor->gainStagesPage.stage2LP.setEnabled(channel >= 1);
    editor->gainStagesPage.stage2Bypass.setEnabled(channel > 1);
    editor->gainStagesPage.stage2Bias.setEnabled(channel >= 1);
    editor->gainStagesPage.stage2Gain.setEnabled(channel > 1);
    
    editor->gainStagesPage.stage3LP.setEnabled(channel >= 2);
    editor->gainStagesPage.stage3Bypass.setEnabled(channel > 2);
    editor->gainStagesPage.stage3Bias.setEnabled(channel >= 2);
    editor->gainStagesPage.stage3Gain.setEnabled(channel > 2);
    
    editor->gainStagesPage.stage4LP.setEnabled(channel >= 3);
    editor->gainStagesPage.stage4Bypass.setEnabled(channel > 3);
    editor->gainStagesPage.stage4Bias.setEnabled(channel >= 3);
    editor->gainStagesPage.stage4Gain.setEnabled(channel > 3);

    editor->gainStagesPage.stage5LP.setEnabled(channel == 4);
    editor->gainStagesPage.stage5Bias.setEnabled(channel == 4);
    
    editor->ampPage.gain2Knob.setEnabled(channel > 1);
}

AmplifierPage::AmplifierPage(Editor *editor, Processor &p) :
    gain1Knob(         "GAIN1_KNOB_LABEL",         "Gain 1",          this, paramInfos[GAIN1].id.toString(),             p.apvts, ""),
    gain2Knob(         "GAIN2_KNOB_LABEL",         "Gain 2",          this, paramInfos[GAIN2].id.toString(),             p.apvts, ""),
    inputFilterKnob(   "INPUT_KNOB_FILTER_LABEL",  "Input Filter",    this, paramInfos[INPUT_FILTER].id.toString(),      p.apvts, " Hz"),
    bassEQKnob(        "BASS_EQ_KNOB_LABEL",       "Bass",            this, paramInfos[TONESTACK_BASS].id.toString(),    p.apvts, ""),
    midEQKnob(         "MIDDLE_EQ_KNOB_LABEL",     "Mid",             this, paramInfos[TONESTACK_MIDDLE].id.toString(),  p.apvts, ""),
    trebbleEQKnob(     "TREBBLE_EQ_KNOB_LABEL",    "Trebble",         this, paramInfos[TONESTACK_TREBBLE].id.toString(), p.apvts, ""),
    preampVolumeKnob(  "PREMP_VOLUME_KNOB_LABEL",  "Post Gain",       this, paramInfos[PREAMP_VOLUME].id.toString(),     p.apvts, " dB"),
    resonanceKnob(     "RESONANCE_KNOB_LABEL",     "Resonance",       this, paramInfos[RESONANCE].id.toString(),         p.apvts, ""),
    presenceKnob(      "PRESENCE_KNOB_LABEL",      "Presence",        this, paramInfos[PRESENCE].id.toString(),          p.apvts, ""),
    ampChannelBox(     "AMP_CHANNEL_BOX_LABEL",    "Amp Channel",     this, paramInfos[CHANNEL].id.toString(),           p.apvts),
    toneStackModelBox( "TONE_MODEL_BOX_LABEL",     "Tonestack Model", this, paramInfos[TONESTACK_MODEL].id.toString(),   p.apvts),
    stereoSettingBox(  "CHANNEL_CONFIG_BOX_LABEL", "Channel config",  this, paramInfos[CHANNEL_CONFIG].id.toString(),    p.apvts)
{
    this->main_editor = editor;

    gain1Knob.setNumDecimalPlacesToDisplay(2);
    gain2Knob.setNumDecimalPlacesToDisplay(2);

    preampToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[DO_PREAMP].id.toString(), preampToggle
    );

    preampToggle.setToggleable(true);
    preampToggle.setClickingTogglesState(true);
    
    preampToggle.onStateChange = [this, editor]() {
        bool state = preampToggle.getToggleState();
        
        editor->ampPage.gain1Knob.setEnabled(state);
        editor->ampPage.gain2Knob.setEnabled(state);
        editor->ampPage.inputFilterKnob.setEnabled(state);
        editor->ampPage.preampVolumeKnob.setEnabled(state);

        editor->ampPage.resonanceKnob.setEnabled(state);
        editor->ampPage.presenceKnob.setEnabled(state);

        editor->ampPage.ampChannelBox.setEnabled(state);
        editor->ampPage.toneStackModelBox.setEnabled(state);
        
        updateGainStageKnobsState(editor, ampChannelBox.getSelectedId());
    };


    addAndMakeVisible(preampToggle);

    tonestackToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[DO_TONESTACK].id.toString(), tonestackToggle
    );
    
    tonestackToggle.setToggleable(true);
    tonestackToggle.setClickingTogglesState(true);

    tonestackToggle.onStateChange = [this, editor]() {
        bool state = tonestackToggle.getToggleState();

        editor->ampPage.bassEQKnob.setEnabled(state);
        editor->ampPage.midEQKnob.setEnabled(state);
        editor->ampPage.trebbleEQKnob.setEnabled(state);
    };

    addAndMakeVisible(tonestackToggle);

    ampChannelBox.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[CHANNEL].id),
                                 juce::NotificationType::dontSendNotification);
    
    ampChannelBox.onChange = [this, editor]() {
        int channel = ampChannelBox.getSelectedId();

        updateGainStageKnobsState(editor, channel);
    }; 
    
    toneStackModelBox.addItemList({"Normal", "Scooped", "Mids!"}, 1);
    toneStackModelBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[TONESTACK_MODEL].id) + 1,
                                     juce::NotificationType::dontSendNotification);

    stereoSettingBox.addItemList({"Mono","Fake Stereo", "Stereo"}, 1);
    stereoSettingBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[CHANNEL_CONFIG].id) + 1,
                                    juce::NotificationType::dontSendNotification);
    
    // stereoSettingBox.valueChanged = [&, this](juce::Value &value) {
        
    brightToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[BRIGHT_CAP].id.toString(), brightToggle
    );
    brightToggle.setToggleable(true);    
    brightToggle.setClickingTogglesState(true);    
    addAndMakeVisible(brightToggle);
}


void AmplifierPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();

    static const int labelHeight = 20;
    static const int horizontalSpacing = 0;
    static const int verticalSpacing = 20;
        
    stereoSettingBox.setBounds(horizontalMargin, verticalMargin + labelHeight, 120, 30);
    stereoSettingBox.label.setBounds(stereoSettingBox.getX(), stereoSettingBox.getY() - 20, knobSize, 20);

    updateGainStageKnobsState(this->main_editor, ampChannelBox.getSelectedId());
    ampChannelBox.setBounds(stereoSettingBox.getRight() + 5, stereoSettingBox.getY(), 120, 30);
    ampChannelBox.label.setBounds(ampChannelBox.getX(), ampChannelBox.getY() - 20, knobSize, 20);

    toneStackModelBox.setBounds(ampChannelBox.getRight() + 5, ampChannelBox.getY(), 120, 30);
    toneStackModelBox.label.setBounds(toneStackModelBox.getX(), toneStackModelBox.getY() - 20, knobSize, 20);

    preampToggle.setBounds(toneStackModelBox.getBounds() + juce::Point(toneStackModelBox.getWidth() + 10, 0));
    tonestackToggle.setBounds(preampToggle.getBounds() + juce::Point(preampToggle.getWidth() + 10, 0));


    placeKnob(&gain1Knob, ampChannelBox.getX(), ampChannelBox.getBottom() + verticalSpacing + 10, knobSize);
    placeKnob(&gain2Knob, gain1Knob.getX(), gain1Knob.getBottom() + verticalSpacing, knobSize);
    placeKnob(&inputFilterKnob, gain1Knob.getX() - knobSize - horizontalSpacing, gain1Knob.getY() + knobSize/2, knobSize);
    
    placeKnob(&bassEQKnob, gain1Knob.getRight() + horizontalSpacing,  gain1Knob.getY() + knobSize/2, knobSize);
    placeKnob(&midEQKnob, bassEQKnob.getRight() + horizontalSpacing, gain1Knob.getY() + knobSize/2, knobSize);
    placeKnob(&trebbleEQKnob, midEQKnob.getRight() + horizontalSpacing,  gain1Knob.getY() + knobSize/2, knobSize);

    placeKnob(&presenceKnob, trebbleEQKnob.getRight() + horizontalSpacing, gain1Knob.getY(), knobSize);
    placeKnob(&resonanceKnob, trebbleEQKnob.getRight() + horizontalSpacing, gain2Knob.getY(), knobSize);

    placeKnob(&preampVolumeKnob, presenceKnob.getRight() + horizontalSpacing, trebbleEQKnob.getY(),  knobSize);

    brightToggle.setBounds(inputFilterKnob.getX(), inputFilterKnob.getY() - labelHeight*2 - 10, knobSize, 30);
}


GainStagesPage::GainStagesPage(Editor *editor, Processor &p) :
    stage1LP(      "STAGE1_LP_SLIDER_LABEL",      "LP filter",       this, paramInfos[STAGE1_LP].id.toString(),     p.apvts, " Hz"),
    stage1Bypass(  "STAGE1_BYPASS_SLIDER_LABEL",  "Cathode bypass",  this, paramInfos[STAGE1_BYPASS].id.toString(), p.apvts, " dB"),
    stage1Bias(    "STAGE1_BIAS_SLIDER_LABEL",    "Tube bias",       this, paramInfos[STAGE1_BIAS].id.toString(),   p.apvts, ""),
    stage1Gain(    "STAGE1_GAIN_SLIDER_LABEL",    "Gain 1 (mirror)", this, paramInfos[GAIN1].id.toString(),         p.apvts, ""),
    
    stage2LP(      "STAGE2_LP_SLIDER_LABEL",      "LP filter",       this, paramInfos[STAGE2_LP].id.toString(),     p.apvts, " Hz"),
    stage2Bypass(  "STAGE2_BYPASS_SLIDER_LABEL",  "Cathode bypass",  this, paramInfos[STAGE2_BYPASS].id.toString(), p.apvts, " dB"),
    stage2Bias(    "STAGE2_BIAS_SLIDER_LABEL",    "Tube bias",       this, paramInfos[STAGE2_BIAS].id.toString(),   p.apvts, ""),
    stage2Gain(    "STAGE2_GAIN_SLIDER_LABEL",    "Gain2 (mirror)",  this, paramInfos[GAIN2].id.toString(),         p.apvts, ""),
    
    stage3LP(      "STAGE3_LP_SLIDER_LABEL",      "LP filter",       this, paramInfos[STAGE3_LP].id.toString(),     p.apvts, " Hz"),
    stage3Bypass(  "STAGE3_BYPASS_SLIDER_LABEL",  "Cathode bypass",  this, paramInfos[STAGE3_BYPASS].id.toString(), p.apvts, " dB"),
    stage3Bias(    "STAGE3_BIAS_SLIDER_LABEL",    "Tube bias",       this, paramInfos[STAGE3_BIAS].id.toString(),   p.apvts, ""),
    stage3Gain(    "STAGE3_GAIN_SLIDER_LABEL",    "Gain 3",          this, paramInfos[GAIN3].id.toString(),         p.apvts, ""),
    
    stage4LP(      "STAGE4_LP_SLIDER_LABEL",      "LP filter",       this, paramInfos[STAGE4_LP].id.toString(),     p.apvts, " Hz"),
    stage4Bypass(  "STAGE4_BYPASS_SLIDER_LABEL",  "Cathode bypass",  this, paramInfos[STAGE4_BYPASS].id.toString(), p.apvts, " dB"),
    stage4Bias(    "STAGE4_BIAS_SLIDER_LABEL",    "Tube bias",       this, paramInfos[STAGE4_BIAS].id.toString(),   p.apvts, ""),
    stage4Gain(    "STAGE4_GAIN_SLIDER_LABEL",    "Gain 4",          this, paramInfos[GAIN4].id.toString(),         p.apvts, ""),
    
    stage5LP(      "STAGE5_LP_SLIDER_LABEL",      "LP filter",       this, paramInfos[STAGE5_LP].id.toString(),     p.apvts, " Hz"),
    stage5Bias(    "STAGE5_BIAS_SLIDER_LABEL",    "Tube bias",       this, paramInfos[STAGE5_BIAS].id.toString(),   p.apvts, ""),
    ampChannelBox( "AMP_CHANNEL_BOX_LABEL2",      "Amp Channel",     this, paramInfos[CHANNEL].id.toString(),       p.apvts)

{
    this->main_editor = editor;
    
    resetButton.onClick = [&p, this]() {
    
        Knob* slider_refs[] = { 
            &stage1LP, &stage1Bypass, &stage1Bias, &stage1Gain,
            &stage2LP, &stage2Bypass, &stage2Bias, &stage2Gain,
            &stage3LP, &stage3Bypass, &stage3Bias, &stage3Gain,
            &stage4LP, &stage4Bypass, &stage4Bias, &stage4Gain,
            &stage5LP,                &stage5Bias,
        };
    
        for (int paramIndex = STAGE1_LP; paramIndex <= STAGE5_BIAS; paramIndex++) {
            p.apvts.getParameter(paramInfos[paramIndex].id.toString())->setValueNotifyingHost(paramInfos[paramIndex].defaultValue);        
            slider_refs[paramIndex - STAGE1_LP]->setValue(paramInfos[paramIndex].defaultValue);
        }
    };
    
    addAndMakeVisible(resetButton);
    
    
    ampChannelBox.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[CHANNEL].id),
                                 juce::NotificationType::dontSendNotification);
    
    ampChannelBox.onChange = [this, editor]() {
        int channel = ampChannelBox.getSelectedId();

        updateGainStageKnobsState(editor, channel);
    }; 

    stage1Label.setJustificationType(juce::Justification::centred);
    stage2Label.setJustificationType(juce::Justification::centred);
    stage3Label.setJustificationType(juce::Justification::centred);
    stage4Label.setJustificationType(juce::Justification::centred);
    stage5Label.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(stage1Label);
    addAndMakeVisible(stage2Label);
    addAndMakeVisible(stage3Label);
    addAndMakeVisible(stage4Label);
    addAndMakeVisible(stage5Label);
}

void GainStagesPage::resized() {
    ZoneScoped;

    static const int horizontalSpacing = 20;
    static const int verticalSpacing = 10;
    static const int stageKnobSize = 95;
    static const int labelWidth = stageKnobSize;

    static const int knobCenterOffset = (labelWidth - stageKnobSize) / 2;
    
    updateGainStageKnobsState(main_editor, ampChannelBox.getSelectedId());

    ampChannelBox.setBounds(horizontalMargin, verticalMargin + 10, 100, 30);
    resetButton.setBounds(ampChannelBox.getBounds() + juce::Point(0, ampChannelBox.getHeight() + 10)); 
    
    stage1Label.setBounds(resetButton.getRight() + horizontalSpacing, verticalMargin - 20, labelWidth, 40);
    stage2Label.setBounds(stage1Label.getRight() + horizontalSpacing, stage1Label.getY(), labelWidth, 40);
    stage3Label.setBounds(stage2Label.getRight() + horizontalSpacing, stage1Label.getY(), labelWidth, 40);
    stage4Label.setBounds(stage3Label.getRight() + horizontalSpacing, stage1Label.getY(), labelWidth, 40);
    stage5Label.setBounds(stage4Label.getRight() + horizontalSpacing, stage1Label.getY(), labelWidth, 40);
    
    placeKnob(&stage1LP,     stage1Label.getX() + knobCenterOffset, stage1Label.getBottom() + verticalSpacing, stageKnobSize);
    placeKnob(&stage1Bypass, stage1LP.getX(),                       stage1LP.getBottom(),                       stageKnobSize);
    placeKnob(&stage1Bias,   stage1Bypass.getX(),                   stage1Bypass.getBottom(), stageKnobSize);
    placeKnob(&stage1Gain,   stage1Bias.getX(),                     stage1Bias.getBottom(), stageKnobSize);

    placeKnob(&stage2LP,     stage2Label.getX() + knobCenterOffset, stage2Label.getBottom() + verticalSpacing, stageKnobSize);
    placeKnob(&stage2Bypass, stage2LP.getX(),                       stage2LP.getBottom(),                       stageKnobSize);
    placeKnob(&stage2Bias,   stage2Bypass.getX(),                   stage2Bypass.getBottom(), stageKnobSize);
    placeKnob(&stage2Gain,   stage2Bias.getX(),                     stage2Bias.getBottom(), stageKnobSize);

    placeKnob(&stage3LP,     stage3Label.getX() + knobCenterOffset, stage3Label.getBottom() + verticalSpacing, stageKnobSize);
    placeKnob(&stage3Bypass, stage3LP.getX(),                       stage3LP.getBottom(),                       stageKnobSize);
    placeKnob(&stage3Bias,   stage3Bypass.getX(),                   stage3Bypass.getBottom(), stageKnobSize);
    placeKnob(&stage3Gain,   stage3Bias.getX(),                     stage3Bias.getBottom(), stageKnobSize);

    placeKnob(&stage4LP,     stage4Label.getX() + knobCenterOffset, stage4Label.getBottom() + verticalSpacing, stageKnobSize);
    placeKnob(&stage4Bypass, stage4LP.getX(),                       stage4LP.getBottom(),                       stageKnobSize);
    placeKnob(&stage4Bias,   stage4Bypass.getX(),                   stage4Bypass.getBottom(), stageKnobSize);
    placeKnob(&stage4Gain,   stage4Bias.getX(),                     stage4Bias.getBottom(), stageKnobSize);
    
    placeKnob(&stage5LP,     stage5Label.getX() + knobCenterOffset, stage5Label.getBottom() + verticalSpacing, stageKnobSize);
    placeKnob(&stage5Bias,   stage5LP.getX(),                       stage4Bias.getY(),                         stageKnobSize);
}

IRLoaderPage::IRLoaderPage(Editor *editor, Processor &audioProcessor) {

    bypassButtonAttachment = std::make_unique<ButtonAttachment>(
        audioProcessor.apvts, paramInfos[DO_IR].id.toString(), bypassToggle
    );

    irLoadButton.onClick = [&audioProcessor, this]() {
        auto chooser = std::make_unique<juce::FileChooser>("Choose a .wav File to open", juce::File(), "*.wav");

        bool fileChoosed = chooser->browseForFileToOpen();
        if (!fileChoosed) {
            return;
        }

        juce::File returnedFile = chooser->getResult();

        if (returnedFile.getFileExtension() != ".wav") {
            return;
        }

        audioProcessor.irLoader.irFile = returnedFile;
        audioProcessor.irLoader.defaultIR = false;
        IRLoaderError error = audioProcessor.irLoader.loadIR();

        if (error == IRLoaderError::Error) { return; }

        irNameLabel.setText(returnedFile.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);

        audioProcessor.valueTree.setProperty(irPath1, returnedFile.getFullPathName(), nullptr);

        // get the list of files in the same directory
        juce::File parentFolder = returnedFile.getParentDirectory();
        audioProcessor.irLoader.directoryWavFiles = parentFolder.findChildFiles(
            juce::File::TypesOfFileToFind::findFiles,
            false,
            "*.wav",
            juce::File::FollowSymlinks::no
        );

        audioProcessor.irLoader.indexOfCurrentFile = audioProcessor.irLoader.directoryWavFiles.indexOf(returnedFile);
    };

    addAndMakeVisible(irLoadButton);

    nextIRButton.onClick = [&audioProcessor, this]() {

        // if the defualt ir is loader, ignore the button
        if (audioProcessor.irLoader.defaultIR) { return; }

        audioProcessor.irLoader.indexOfCurrentFile = (audioProcessor.irLoader.indexOfCurrentFile + 1) % audioProcessor.irLoader.directoryWavFiles.size();

        juce::File fileToLoad = audioProcessor.irLoader.directoryWavFiles[audioProcessor.irLoader.indexOfCurrentFile];
        audioProcessor.irLoader.irFile = fileToLoad;
        IRLoaderError error = audioProcessor.irLoader.loadIR();

        if (error == IRLoaderError::Error) { return; }

        irNameLabel.setText(fileToLoad.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);

        audioProcessor.valueTree.setProperty(irPath1, fileToLoad.getFullPathName(), nullptr);

    };

    prevIRButton.onClick = [&audioProcessor, this]() {

        if (audioProcessor.irLoader.defaultIR) { return; }

        audioProcessor.irLoader.indexOfCurrentFile--;
        if (audioProcessor.irLoader.indexOfCurrentFile < 0) {
            audioProcessor.irLoader.indexOfCurrentFile +=  audioProcessor.irLoader.directoryWavFiles.size();
        }

        juce::File fileToLoad = audioProcessor.irLoader.directoryWavFiles[audioProcessor.irLoader.indexOfCurrentFile];
        audioProcessor.irLoader.irFile = fileToLoad;
        IRLoaderError error = audioProcessor.irLoader.loadIR();

        if (error == IRLoaderError::Error) { return; }

        irNameLabel.setText(fileToLoad.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);

        audioProcessor.valueTree.setProperty(irPath1, fileToLoad.getFullPathName(), nullptr);
    };

    addAndMakeVisible(nextIRButton);
    addAndMakeVisible(prevIRButton);

    irNameLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    irNameLabel.setJustificationType(juce::Justification::left);
    irNameLabel.setFont(15.0f);

    juce::String irNameText = audioProcessor.irLoader.irFile.getFileNameWithoutExtension();


    if (audioProcessor.irLoader.defaultIR) {
        irNameLabel.setText(defaultIRText, juce::NotificationType::dontSendNotification);
    } else {
        irNameLabel.setText(irNameText, juce::NotificationType::dontSendNotification);
    }

    addAndMakeVisible(irNameLabel);

    defaultIRButton.onClick = [&audioProcessor, this]() {
        audioProcessor.irLoader.defaultIR = true;
        IRLoaderError error = audioProcessor.irLoader.loadIR();

        if (error == IRLoaderError::Error) { return; }

        irNameLabel.setText(defaultIRText, juce::NotificationType::dontSendNotification);

        audioProcessor.valueTree.setProperty(irPath1, "", nullptr);
    };

    addAndMakeVisible(defaultIRButton);

    bypassToggle.setToggleState(audioProcessor.irLoader.active, juce::NotificationType::dontSendNotification);
    bypassToggle.setToggleable(true);    
    bypassToggle.setClickingTogglesState(true);    

    bypassToggle.onClick = [&audioProcessor, this]() {
        audioProcessor.irLoader.active = bypassToggle.getToggleState();
    };

    addAndMakeVisible(bypassToggle);
}

void IRLoaderPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();
    
    static const int horizontalSpacing = 10;
    static const int verticalSpacing = 10;

    irLoadButton.setBounds(horizontalMargin, verticalMargin, 100, 50);

    bypassToggle.setBounds(irLoadButton.getRight() + horizontalSpacing, irLoadButton.getY() - 10, 120, 30);
    defaultIRButton.setBounds(bypassToggle.getX(), bypassToggle.getBottom() + verticalSpacing, 120, 30);

    irNameLabel.setBounds(irLoadButton.getX(), irLoadButton.getBottom() + 15, 400, 50);

    nextIRButton.setBounds(irNameLabel.getX(), irNameLabel.getBottom() + verticalSpacing, 120, 30);
    prevIRButton.setBounds(nextIRButton.getX(), nextIRButton.getBottom() + verticalSpacing, 120, 30);
}

EQPage::EQPage(Editor *editor, Processor &p) :
    lowcutFreqKnob("LOW_CUT_FREQ_KNOB_LABEL",       "Low cut",         this, paramInfos[LOW_CUT_FREQ].id.toString(),    p.apvts, " Hz"),
    lowShelfFreqKnob("LOW_SHELF_FREQ_KNOB_LABEL",   "Low Shelf Freq",  this, paramInfos[LOW_SHELF_FREQ].id.toString(),  p.apvts, " Hz"),
    lowShelfGainKnob("LOW_SHELF_GAIN_KNOB_LABEL",   "Gain",            this, paramInfos[LOW_SHELF_GAIN].id.toString(),  p.apvts, " dB"),
    lowMidFreqKnob("LOWMID_FREQ_KNOB_LABEL",        "Freq",            this, paramInfos[LOWMID_FREQ].id.toString(),     p.apvts, " Hz"),
    lowMidGainKnob("LOWMID_GAIN_KNOB_LABEL",        "Gain",            this, paramInfos[LOWMID_GAIN].id.toString(),     p.apvts, " dB"),
    lowMidQKnob("LOWMID_Q_KNOB_LABEL",              "Q",               this, paramInfos[LOWMID_Q].id.toString(),        p.apvts, ""),
    midFreqKnob("MID_FREQ_KNOB_LABEL",              "Freq",            this, paramInfos[MID_FREQ].id.toString(),        p.apvts, " Hz"),
    midGainKnob("MID_GAIN_KNOB_LABEL",              "Gain",            this, paramInfos[MID_GAIN].id.toString(),        p.apvts, " dB"),
    midQKnob("MID_Q_KNOB_LABEL",                    "Q",               this, paramInfos[MID_Q].id.toString(),           p.apvts, ""),
    highFreqKnob("HIGH_FREQ_KNOB_LABEL",            "Freq",            this, paramInfos[HIGH_FREQ].id.toString(),       p.apvts, " Hz"),
    highGainKnob("HIGH_GAIN_KNOB_LABEL",            "Gain",            this, paramInfos[HIGH_GAIN].id.toString(),       p.apvts, " dB"),
    highQKnob("HIGH_Q_KNOB_LABEL",                  "Q",               this, paramInfos[HIGH_Q].id.toString(),          p.apvts, ""),
    highShelfFreqKnob("HIGH_SHELF_FREQ_KNOB_LABEL", "High Shelf Freq", this, paramInfos[HIGH_SHELF_FREQ].id.toString(), p.apvts, " Hz"),
    highShelfGainKnob("HIGH_SHELF_GAIN_KNOB_LABEL", "Gain",            this, paramInfos[HIGH_SHELF_GAIN].id.toString(), p.apvts, " dB"),
    highCutFreqKnob("HIGH_CUT_FREQ_KNOB_LABEL",     "High cut",        this, paramInfos[HIGH_CUT_FREQ].id.toString(),   p.apvts, " Hz")
{
    resetButton.onClick = [&p, this]() {
    
        p.apvts.getParameter(paramInfos[LOW_SHELF_GAIN].id.toString())->setValueNotifyingHost(paramInfos[LOW_SHELF_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[LOWMID_GAIN].id.toString())->setValueNotifyingHost(paramInfos[LOWMID_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[MID_GAIN].id.toString())->setValueNotifyingHost(paramInfos[MID_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[HIGH_GAIN].id.toString())->setValueNotifyingHost(paramInfos[HIGH_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[HIGH_SHELF_GAIN].id.toString())->setValueNotifyingHost(paramInfos[HIGH_SHELF_GAIN].defaultValue);
    
        lowShelfGainKnob.setValue(paramInfos[LOW_SHELF_GAIN].defaultValue);
        lowMidGainKnob.setValue(paramInfos[LOWMID_GAIN].defaultValue);
        midGainKnob.setValue(paramInfos[MID_GAIN].defaultValue);
        highGainKnob.setValue(paramInfos[HIGH_GAIN].defaultValue);
        highShelfGainKnob.setValue(paramInfos[HIGH_SHELF_GAIN].defaultValue);
    };

    addAndMakeVisible(resetButton);

    EQToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[DO_EQ].id.toString(), EQToggle
    );


    EQToggle.setToggleable(true);
    EQToggle.setClickingTogglesState(true);

    EQToggle.onStateChange = [this, editor]() {
        bool state = EQToggle.getToggleState();

        editor->eqPage.lowcutFreqKnob.setEnabled(state);
        editor->eqPage.lowShelfFreqKnob.setEnabled(state);
        editor->eqPage.lowShelfGainKnob.setEnabled(state);
        editor->eqPage.lowMidFreqKnob.setEnabled(state);
        editor->eqPage.lowMidGainKnob.setEnabled(state);
        editor->eqPage.lowMidQKnob.setEnabled(state);
        editor->eqPage.midFreqKnob.setEnabled(state);
        editor->eqPage.midGainKnob.setEnabled(state);
        editor->eqPage.midQKnob.setEnabled(state);
        editor->eqPage.highFreqKnob.setEnabled(state);
        editor->eqPage.highGainKnob.setEnabled(state);
        editor->eqPage.highQKnob.setEnabled(state);
        editor->eqPage.highShelfFreqKnob.setEnabled(state);
        editor->eqPage.highShelfGainKnob.setEnabled(state);
        editor->eqPage.highCutFreqKnob.setEnabled(state);
    };

    addAndMakeVisible(EQToggle);

}

void EQPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();

    static const int eqKnobSize = 100;
    static const int labelHeight = 20;
    static const int verticalSpacing = 5;
    static const int horizontalSpacing = 0;
    
    EQToggle.setBounds(horizontalMargin, verticalMargin + 20, 100, 30);
    resetButton.setBounds(EQToggle.getBounds() + juce::Point(0, EQToggle.getHeight() + verticalSpacing));
    
    placeKnob(&lowShelfFreqKnob, resetButton.getRight() + horizontalSpacing + 20, verticalMargin, eqKnobSize);
    placeKnob(&lowShelfGainKnob, lowShelfFreqKnob.getX(), lowShelfFreqKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
    placeKnob(&lowcutFreqKnob,   lowShelfFreqKnob.getX(), lowShelfGainKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
    
    placeKnob(&lowMidFreqKnob, lowShelfFreqKnob.getRight() + horizontalSpacing, lowShelfFreqKnob.getY(), eqKnobSize);
    placeKnob(&lowMidGainKnob, lowMidFreqKnob.getX(), lowMidFreqKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
    placeKnob(&lowMidQKnob,    lowMidFreqKnob.getX(), lowMidGainKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);

    placeKnob(&midFreqKnob, lowMidFreqKnob.getRight() + horizontalSpacing, lowMidFreqKnob.getY(), eqKnobSize);
    placeKnob(&midGainKnob, midFreqKnob.getX(), midFreqKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
    placeKnob(&midQKnob,    midFreqKnob.getX(), midGainKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);

    placeKnob(&highFreqKnob, midFreqKnob.getRight() + horizontalSpacing, midFreqKnob.getY(), eqKnobSize);
    placeKnob(&highGainKnob, highFreqKnob.getX(), highFreqKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
    placeKnob(&highQKnob,    highFreqKnob.getX(), highGainKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);

    placeKnob(&highShelfFreqKnob, highFreqKnob.getRight() + horizontalSpacing, highFreqKnob.getY(), eqKnobSize);
    placeKnob(&highShelfGainKnob, highShelfFreqKnob.getX(), highShelfFreqKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
    placeKnob(&highCutFreqKnob,   highShelfFreqKnob.getX(), highShelfGainKnob.getBottom() + verticalSpacing + labelHeight, eqKnobSize);
}


//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    gateBoostPage(this, p),
    ampPage(this, p),
    gainStagesPage(this, p),
    irLoaderPage(this, p),
    eqPage(this, p),
    volumePanel(p)
{

    juce::Colour tabColour = juce::Colours::black;
    tabs.setColour(juce::TabbedComponent::ColourIds::backgroundColourId, tabColour);
    tabs.setColour(juce::TabbedComponent::ColourIds::outlineColourId, tabColour);
    
    tabs.addTab("Input",       tabColour, &gateBoostPage,  true);
    tabs.addTab("Amplifier",   tabColour, &ampPage,        true);
    tabs.addTab("Gain Stages", tabColour, &gainStagesPage, true);
    tabs.addTab("EQ",          tabColour, &eqPage,         true);
    tabs.addTab("IR Loader",   tabColour, &irLoaderPage,   true);
    tabs.setCurrentTabIndex(1);
    
    addAndMakeVisible(tabs);
    addAndMakeVisible(volumePanel);

    setLookAndFeel(&this->lookAndFeel);
    setSize(windoWidth, windowHeight);
    setResizable(false, false);
}

Editor::~Editor() {
    setLookAndFeel(nullptr);
}

//==============================================================================
void Editor::paint (juce::Graphics& g) {
    ZoneScoped;
	g.fillAll(backgroundColor);
}

void Editor::resized() {
    ZoneScoped;

    juce::Rectangle<int> bounds = getLocalBounds();

    volumePanel.setBounds(bounds.removeFromRight(100));

    gateBoostPage.setBounds(bounds);
    ampPage.setBounds(bounds);
    eqPage.setBounds(bounds);
    irLoaderPage.setBounds(bounds);

    tabs.setBounds(bounds);

    gateBoostPage.resized();
    ampPage.resized();
    gainStagesPage.resized();
    irLoaderPage.resized();
    eqPage.resized();
}


Knob::Knob(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix)
: label{labelID, name}
{
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    setTextValueSuffix(suffix);
    comp->addAndMakeVisible(*this);

    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);

    sliderAttachment = std::make_unique<SliderAttachment>(apvts, paramID, *this);
}

VSlider::VSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix)
: label{labelID, name}
{
    setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    setTextValueSuffix(suffix);
    comp->addAndMakeVisible(*this);

    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);


    sliderAttachment = std::make_unique<SliderAttachment>(apvts, paramID, *this);
}

HSlider::HSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String parameterID, Apvts &apvts, juce::String suffix)
: label{labelID, name}
{
    setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    setTextValueSuffix(suffix);
    comp->addAndMakeVisible(*this);

    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);

    sliderAttachment = std::make_unique<SliderAttachment>(apvts, parameterID, *this);
    
    paramID = parameterID;
}

ComboBox::ComboBox(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts)
         : label {labelID, name}
{
    setEditableText(false);
    setJustificationType(juce::Justification::centred);
    comp->addAndMakeVisible(*this);

    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);

    comp->addAndMakeVisible(label);

    boxAttachment = std::make_unique<ComboBoxAttachment>(apvts, paramID, *this);
}
