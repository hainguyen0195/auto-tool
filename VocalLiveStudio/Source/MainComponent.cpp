#include "MainComponent.h"
#include <iomanip>
#include <sstream>

namespace VocalStudio
{

// ---------------------------------------------------------------------------
// Studio Look and Feel
// ---------------------------------------------------------------------------
StudioLookAndFeel::StudioLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff12141e));
    setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff2b3044));
    setColour(juce::GroupComponent::textColourId, juce::Colour(0xff00e5ff));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xff00e5ff));
    setColour(juce::Slider::trackColourId, juce::Colour(0xff1f2434));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe0e6ed));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff161924));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff2d3348));
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
    setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1c2132));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffe0e6ed));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff333a52));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff191d2c));
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xffe0e6ed));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff9d4edd));
}

void StudioLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& /*slider*/)
{
    auto radius = (float) juce::jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Dark track background
    g.setColour(juce::Colour(0xff1a1e2d));
    g.fillEllipse(rx, ry, rw, rw);
    g.setColour(juce::Colour(0xff2d354d));
    g.drawEllipse(rx, ry, rw, rw, 1.5f);

    // Glowing arc track
    juce::Path trackPath;
    trackPath.addCentredArc(centreX, centreY, radius - 4.0f, radius - 4.0f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff22283a));
    g.strokePath(trackPath, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active fill arc
    juce::Path activePath;
    activePath.addCentredArc(centreX, centreY, radius - 4.0f, radius - 4.0f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(juce::Colour(0xff00e5ff));
    g.strokePath(activePath, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Dial needle
    juce::Path p;
    auto pointerLength = radius * 0.65f;
    p.addRoundedRectangle(-2.0f, -radius + 4.0f, 4.0f, pointerLength * 0.5f, 2.0f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(juce::Colours::white);
    g.fillPath(p);
}

void StudioLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                        const juce::Slider::SliderStyle /*style*/, juce::Slider& slider)
{
    bool isHorizontal = slider.isHorizontal();
    auto bounds = juce::Rectangle<float>((float) x, (float) y, (float) width, (float) height);

    if (isHorizontal)
    {
        auto trackY = bounds.getCentreY() - 3.0f;
        auto trackH = 6.0f;

        // Background track
        g.setColour(juce::Colour(0xff181c2b));
        g.fillRoundedRectangle(bounds.getX(), trackY, bounds.getWidth(), trackH, 3.0f);

        // Filled active track
        g.setColour(juce::Colour(0xff00e5ff).withAlpha(0.85f));
        g.fillRoundedRectangle(bounds.getX(), trackY, sliderPos - bounds.getX(), trackH, 3.0f);

        // Thumb
        g.setColour(juce::Colours::white);
        g.fillRoundedRectangle(sliderPos - 5.0f, bounds.getCentreY() - 9.0f, 10.0f, 18.0f, 3.0f);
        g.setColour(juce::Colour(0xff9d4edd));
        g.drawRoundedRectangle(sliderPos - 5.0f, bounds.getCentreY() - 9.0f, 10.0f, 18.0f, 3.0f, 1.5f);
    }
    else
    {
        auto trackX = bounds.getCentreX() - 3.0f;
        auto trackW = 6.0f;

        g.setColour(juce::Colour(0xff181c2b));
        g.fillRoundedRectangle(trackX, bounds.getY(), trackW, bounds.getHeight(), 3.0f);

        g.setColour(juce::Colour(0xff00e5ff).withAlpha(0.85f));
        g.fillRoundedRectangle(trackX, sliderPos, trackW, bounds.getBottom() - sliderPos, 3.0f);

        g.setColour(juce::Colours::white);
        g.fillRoundedRectangle(bounds.getCentreX() - 9.0f, sliderPos - 5.0f, 18.0f, 10.0f, 3.0f);
        g.setColour(juce::Colour(0xff9d4edd));
        g.drawRoundedRectangle(bounds.getCentreX() - 9.0f, sliderPos - 5.0f, 18.0f, 10.0f, 3.0f, 1.5f);
    }
}

void StudioLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    auto baseCol = backgroundColour;

    if (shouldDrawButtonAsDown)
        baseCol = baseCol.brighter(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseCol = baseCol.brighter(0.1f);

    g.setColour(baseCol);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(shouldDrawButtonAsHighlighted ? juce::Colour(0xff00e5ff) : juce::Colour(0xff3a425c));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void StudioLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                        bool /*shouldDrawButtonAsHighlighted*/,
                                        bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto tickWidth = 16.0f;
    auto tickRect = juce::Rectangle<float>(bounds.getX() + 2.0f, bounds.getCentreY() - tickWidth * 0.5f, tickWidth, tickWidth);

    bool isOn = button.getToggleState();

    g.setColour(juce::Colour(0xff191d2c));
    g.fillRoundedRectangle(tickRect, 3.0f);

    g.setColour(isOn ? juce::Colour(0xff00e5ff) : juce::Colour(0xff39415c));
    g.drawRoundedRectangle(tickRect, 3.0f, 1.5f);

    if (isOn)
    {
        g.setColour(juce::Colour(0xff00e5ff));
        g.fillRoundedRectangle(tickRect.reduced(3.0f), 2.0f);
    }

    g.setColour(isOn ? juce::Colours::white : juce::Colour(0xffa0abbd));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    auto textRect = bounds.withTrimmedLeft(tickWidth + 8.0f);
    g.drawFittedText(button.getButtonText(), textRect.toNearestInt(), juce::Justification::centredLeft, 1);
}

// ---------------------------------------------------------------------------
// Peak Meter Component
// ---------------------------------------------------------------------------
PeakMeterComponent::PeakMeterComponent(const juce::String& labelText, bool vertical)
    : label(labelText), isVertical(vertical)
{
}

void PeakMeterComponent::setLevel(float levelDb)
{
    currentLevelDb = juce::jlimit(-60.0f, 3.0f, levelDb);
    repaint();
}

void PeakMeterComponent::setClipped(bool isClipped)
{
    clipped = isClipped;
    repaint();
}

void PeakMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark meter frame
    g.setColour(juce::Colour(0xff10121a));
    g.fillRoundedRectangle(bounds, 3.0f);
    g.setColour(juce::Colour(0xff282e42));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    float norm = (currentLevelDb + 60.0f) / 63.0f; // 0.0 to 1.0
    norm = juce::jlimit(0.0f, 1.0f, norm);

    auto barArea = bounds.reduced(2.0f);

    if (isVertical)
    {
        float fillHeight = barArea.getHeight() * norm;
        auto fillRect = juce::Rectangle<float>(barArea.getX(), barArea.getBottom() - fillHeight, barArea.getWidth(), fillHeight);

        juce::ColourGradient grad(juce::Colour(0xff00e5ff), barArea.getX(), barArea.getBottom(),
                                  juce::Colour(0xffff3d00), barArea.getX(), barArea.getY(), false);
        grad.addColour(0.75, juce::Colour(0xff00e676));
        grad.addColour(0.90, juce::Colour(0xffffea00));
        g.setGradientFill(grad);
        g.fillRoundedRectangle(fillRect, 2.0f);
    }
    else
    {
        float fillWidth = barArea.getWidth() * norm;
        auto fillRect = juce::Rectangle<float>(barArea.getX(), barArea.getY(), fillWidth, barArea.getHeight());

        juce::ColourGradient grad(juce::Colour(0xff00e676), barArea.getX(), barArea.getY(),
                                  juce::Colour(0xffff1744), barArea.getRight(), barArea.getY(), false);
        grad.addColour(0.70, juce::Colour(0xff00e5ff));
        grad.addColour(0.88, juce::Colour(0xffffea00));
        g.setGradientFill(grad);
        g.fillRoundedRectangle(fillRect, 2.0f);
    }

    if (clipped)
    {
        g.setColour(juce::Colour(0xffff1744));
        g.drawRoundedRectangle(bounds, 3.0f, 2.0f);
    }
}

// ---------------------------------------------------------------------------
// Main Window Component Implementation
// ---------------------------------------------------------------------------
MainComponent::MainComponent(AudioEngine& engine)
    : audioEngine(engine)
{
    setLookAndFeel(&studioLookAndFeel);
    setupUI();
    setSize(1240, 820);
    startTimerHz(30); // 30 FPS UI refresh
}

MainComponent::~MainComponent()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MainComponent::setupUI()
{
    // 1. Header Bar
    titleLabel.setText("VOCAL LIVE STUDIO  v1.0", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00e5ff));
    addAndMakeVisible(titleLabel);

    presetLabel.setText("PRESET:", juce::dontSendNotification);
    presetLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xff9d4edd));
    addAndMakeVisible(presetLabel);

    addAndMakeVisible(presetComboBox);
    const auto& presets = audioEngine.getPresetManager().getFactoryPresets();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        presetComboBox.addItem(presets[i].name, static_cast<int>(i + 1));
    }
    presetComboBox.setSelectedId(1, juce::dontSendNotification);
    presetComboBox.addListener(this);

    savePresetButton.addListener(this);
    addAndMakeVisible(savePresetButton);

    loadPresetButton.addListener(this);
    addAndMakeVisible(loadPresetButton);

    audioSettingsButton.addListener(this);
    audioSettingsButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2b3248));
    addAndMakeVisible(audioSettingsButton);

    liveRoutingButton.addListener(this);
    liveRoutingButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a148c));
    addAndMakeVisible(liveRoutingButton);

    // Master Volume & Clip
    masterVolLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    masterVolLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(masterVolLabel);

    masterVolSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolSlider.setRange(0.0, 1.5, 0.01);
    masterVolSlider.setValue(audioEngine.getMasterVolume());
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    masterVolSlider.addListener(this);
    addAndMakeVisible(masterVolSlider);

    addAndMakeVisible(masterMeter);

    clipResetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff251a24));
    clipResetButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffff5252));
    clipResetButton.addListener(this);
    addAndMakeVisible(clipResetButton);

    // 2. Microphone Input Group
    addAndMakeVisible(micGroup);

    micGainLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    micGainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(micGainLabel);

    micGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    micGainSlider.setRange(0.0, 3.0, 0.01);
    micGainSlider.setValue(audioEngine.getMicGain());
    micGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    micGainSlider.addListener(this);
    addAndMakeVisible(micGainSlider);

    addAndMakeVisible(micMeter);

    monitorToggle.setToggleState(audioEngine.isInputMonitoringEnabled(), juce::dontSendNotification);
    monitorToggle.addListener(this);
    addAndMakeVisible(monitorToggle);

    safeModeToggle.setToggleState(audioEngine.isSafeModeEnabled(), juce::dontSendNotification);
    safeModeToggle.addListener(this);
    addAndMakeVisible(safeModeToggle);

    // 3. Auto-Tune Group
    addAndMakeVisible(autoTuneGroup);

    naturalModeButton.addListener(this);
    hardTuneModeButton.addListener(this);
    bypassModeButton.addListener(this);
    naturalModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff00e5ff));
    naturalModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    addAndMakeVisible(naturalModeButton);
    addAndMakeVisible(hardTuneModeButton);
    addAndMakeVisible(bypassModeButton);

    keyLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    keyLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00e5ff));
    addAndMakeVisible(keyLabel);

    addAndMakeVisible(keyComboBox);
    const auto& noteNames = NoteMapper::getNoteNames();
    for (size_t i = 0; i < noteNames.size(); ++i)
        keyComboBox.addItem(noteNames[i], static_cast<int>(i + 1));
    keyComboBox.setSelectedId(1, juce::dontSendNotification);
    keyComboBox.addListener(this);

    scaleLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    scaleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00e5ff));
    addAndMakeVisible(scaleLabel);

    addAndMakeVisible(scaleComboBox);
    const auto& scales = NoteMapper::getAvailableScales();
    for (size_t i = 0; i < scales.size(); ++i)
        scaleComboBox.addItem(scales[i].name, static_cast<int>(i + 1));
    scaleComboBox.setSelectedId(2, juce::dontSendNotification); // Major default
    scaleComboBox.addListener(this);

    // Custom Note Pills
    for (int i = 0; i < 12; ++i)
    {
        auto pill = std::make_unique<juce::TextButton>(noteNames[static_cast<size_t>(i)]);
        pill->setClickingTogglesState(true);
        pill->setToggleState(true, juce::dontSendNotification);
        pill->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1e2336));
        pill->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff9d4edd));
        pill->addListener(this);
        addChildComponent(pill.get());
        customNotePills.push_back(std::move(pill));
    }

    // Retune Speed Slider
    retuneSpeedLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    addAndMakeVisible(retuneSpeedLabel);
    retuneSpeedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    retuneSpeedSlider.setRange(0.0, 300.0, 1.0);
    retuneSpeedSlider.setValue(audioEngine.getAutoTune().getRetuneSpeedMs());
    retuneSpeedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 18);
    retuneSpeedSlider.setTextValueSuffix(" ms");
    retuneSpeedSlider.addListener(this);
    addAndMakeVisible(retuneSpeedSlider);

    // Correction Strength Slider
    strengthLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    addAndMakeVisible(strengthLabel);
    strengthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    strengthSlider.setRange(0.0, 100.0, 1.0);
    strengthSlider.setValue(audioEngine.getAutoTune().getCorrectionStrength() * 100.0f);
    strengthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 18);
    strengthSlider.setTextValueSuffix(" %");
    strengthSlider.addListener(this);
    addAndMakeVisible(strengthSlider);

    // Humanize Slider
    humanizeLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    addAndMakeVisible(humanizeLabel);
    humanizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    humanizeSlider.setRange(0.0, 100.0, 1.0);
    humanizeSlider.setValue(audioEngine.getAutoTune().getHumanize() * 100.0f);
    humanizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 18);
    humanizeSlider.setTextValueSuffix(" %");
    humanizeSlider.addListener(this);
    addAndMakeVisible(humanizeSlider);

    // Live Pitch Display Card
    pitchDisplayTitle.setFont(juce::Font(12.0f, juce::Font::bold));
    pitchDisplayTitle.setColour(juce::Label::textColourId, juce::Colour(0xff00e5ff));
    addAndMakeVisible(pitchDisplayTitle);

    pitchDetectedLabel.setText("Detected: --- Hz", juce::dontSendNotification);
    pitchDetectedLabel.setFont(juce::Font(14.0f, juce::Font::plain));
    pitchDetectedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(pitchDetectedLabel);

    pitchTargetLabel.setText("Target: ---", juce::dontSendNotification);
    pitchTargetLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    pitchTargetLabel.setColour(juce::Label::textColourId, juce::Colour(0xff9d4edd));
    addAndMakeVisible(pitchTargetLabel);

    pitchDetuneLabel.setText("Detune: 0 cents", juce::dontSendNotification);
    pitchDetuneLabel.setFont(juce::Font(13.0f, juce::Font::plain));
    pitchDetuneLabel.setColour(juce::Label::textColourId, juce::Colour(0xff80d8ff));
    addAndMakeVisible(pitchDetuneLabel);

    // 4. Vocal Effects Rack
    addAndMakeVisible(effectsGroup);

    // Gate
    gateToggle.setToggleState(audioEngine.getEffects().gate.enabled.load(), juce::dontSendNotification);
    gateToggle.addListener(this);
    addAndMakeVisible(gateToggle);
    gateThreshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gateThreshSlider.setRange(-80.0, 0.0, 1.0);
    gateThreshSlider.setValue(audioEngine.getEffects().gate.thresholdDb.load());
    gateThreshSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    gateThreshSlider.setTextValueSuffix(" dB");
    gateThreshSlider.addListener(this);
    addAndMakeVisible(gateThreshSlider);

    // DeEsser
    deEsserToggle.setToggleState(audioEngine.getEffects().deEsser.enabled.load(), juce::dontSendNotification);
    deEsserToggle.addListener(this);
    addAndMakeVisible(deEsserToggle);
    deEsserThreshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    deEsserThreshSlider.setRange(-40.0, 0.0, 1.0);
    deEsserThreshSlider.setValue(audioEngine.getEffects().deEsser.thresholdDb.load());
    deEsserThreshSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    deEsserThreshSlider.setTextValueSuffix(" dB");
    deEsserThreshSlider.addListener(this);
    addAndMakeVisible(deEsserThreshSlider);

    // 3-Band EQ
    eqToggle.setToggleState(audioEngine.getEffects().eq.enabled.load(), juce::dontSendNotification);
    eqToggle.addListener(this);
    addAndMakeVisible(eqToggle);
    eqLowSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    eqLowSlider.setRange(-15.0, 15.0, 0.5);
    eqLowSlider.setValue(audioEngine.getEffects().eq.lowGainDb.load());
    eqLowSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    eqLowSlider.setTextValueSuffix(" L");
    eqLowSlider.addListener(this);
    addAndMakeVisible(eqLowSlider);

    eqMidSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    eqMidSlider.setRange(-15.0, 15.0, 0.5);
    eqMidSlider.setValue(audioEngine.getEffects().eq.midGainDb.load());
    eqMidSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    eqMidSlider.setTextValueSuffix(" M");
    eqMidSlider.addListener(this);
    addAndMakeVisible(eqMidSlider);

    eqHighSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    eqHighSlider.setRange(-15.0, 15.0, 0.5);
    eqHighSlider.setValue(audioEngine.getEffects().eq.highGainDb.load());
    eqHighSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    eqHighSlider.setTextValueSuffix(" H");
    eqHighSlider.addListener(this);
    addAndMakeVisible(eqHighSlider);

    // Compressor
    compToggle.setToggleState(audioEngine.getEffects().comp.enabled.load(), juce::dontSendNotification);
    compToggle.addListener(this);
    addAndMakeVisible(compToggle);
    compThreshSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    compThreshSlider.setRange(-40.0, 0.0, 1.0);
    compThreshSlider.setValue(audioEngine.getEffects().comp.thresholdDb.load());
    compThreshSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    compThreshSlider.setTextValueSuffix(" dB");
    compThreshSlider.addListener(this);
    addAndMakeVisible(compThreshSlider);

    compRatioSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    compRatioSlider.setRange(1.0, 12.0, 0.5);
    compRatioSlider.setValue(audioEngine.getEffects().comp.ratio.load());
    compRatioSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    compRatioSlider.setTextValueSuffix(" :1");
    compRatioSlider.addListener(this);
    addAndMakeVisible(compRatioSlider);

    // Delay
    delayToggle.setToggleState(audioEngine.getEffects().delay.enabled.load(), juce::dontSendNotification);
    delayToggle.addListener(this);
    addAndMakeVisible(delayToggle);
    delayTimeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    delayTimeSlider.setRange(10.0, 800.0, 10.0);
    delayTimeSlider.setValue(audioEngine.getEffects().delay.timeMs.load());
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    delayTimeSlider.setTextValueSuffix(" ms");
    delayTimeSlider.addListener(this);
    addAndMakeVisible(delayTimeSlider);

    delayMixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    delayMixSlider.setRange(0.0, 100.0, 1.0);
    delayMixSlider.setValue(audioEngine.getEffects().delay.mix.load() * 100.0f);
    delayMixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    delayMixSlider.setTextValueSuffix(" %");
    delayMixSlider.addListener(this);
    addAndMakeVisible(delayMixSlider);

    // Reverb
    reverbToggle.setToggleState(audioEngine.getEffects().reverb.enabled.load(), juce::dontSendNotification);
    reverbToggle.addListener(this);
    addAndMakeVisible(reverbToggle);
    reverbSizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    reverbSizeSlider.setRange(0.0, 100.0, 1.0);
    reverbSizeSlider.setValue(audioEngine.getEffects().reverb.roomSize.load() * 100.0f);
    reverbSizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    reverbSizeSlider.setTextValueSuffix(" sz");
    reverbSizeSlider.addListener(this);
    addAndMakeVisible(reverbSizeSlider);

    reverbMixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    reverbMixSlider.setRange(0.0, 100.0, 1.0);
    reverbMixSlider.setValue(audioEngine.getEffects().reverb.mix.load() * 100.0f);
    reverbMixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 18);
    reverbMixSlider.setTextValueSuffix(" %");
    reverbMixSlider.addListener(this);
    addAndMakeVisible(reverbMixSlider);

    // Limiter
    limiterToggle.setToggleState(audioEngine.getEffects().limiter.enabled.load(), juce::dontSendNotification);
    limiterToggle.addListener(this);
    addAndMakeVisible(limiterToggle);

    // 5. Karaoke Player Group (Bottom)
    addAndMakeVisible(karaokeGroup);

    loadTrackButton.addListener(this);
    loadTrackButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1976d2));
    addAndMakeVisible(loadTrackButton);

    trackNameLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    trackNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(trackNameLabel);

    playButton.addListener(this);
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2e7d32));
    addAndMakeVisible(playButton);

    pauseButton.addListener(this);
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xfff57f17));
    addAndMakeVisible(pauseButton);

    stopButton.addListener(this);
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffc62828));
    addAndMakeVisible(stopButton);

    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    timeLabel.setFont(juce::Font(12.0f, juce::Font::plain));
    timeLabel.setColour(juce::Label::textColourId, juce::Colour(0xff90caf9));
    addAndMakeVisible(timeLabel);

    backingVolLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    addAndMakeVisible(backingVolLabel);
    backingVolSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    backingVolSlider.setRange(0.0, 1.5, 0.01);
    backingVolSlider.setValue(audioEngine.getKaraokePlayer().getVolume());
    backingVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 45, 18);
    backingVolSlider.addListener(this);
    addAndMakeVisible(backingVolSlider);
    addAndMakeVisible(karaokeMeter);

    pitchShiftLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    addAndMakeVisible(pitchShiftLabel);
    pitchShiftSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    pitchShiftSlider.setRange(-6, 6, 1);
    pitchShiftSlider.setValue(audioEngine.getKaraokePlayer().getPitchSemitones());
    pitchShiftSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 45, 18);
    pitchShiftSlider.setTextValueSuffix(" st");
    pitchShiftSlider.addListener(this);
    addAndMakeVisible(pitchShiftSlider);

    tempoLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    addAndMakeVisible(tempoLabel);
    tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    tempoSlider.setRange(0.8, 1.2, 0.01);
    tempoSlider.setValue(audioEngine.getKaraokePlayer().getSpeedRatio());
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 45, 18);
    tempoSlider.setTextValueSuffix(" x");
    tempoSlider.addListener(this);
    addAndMakeVisible(tempoSlider);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff11131c));

    // Subtle dark gradient background
    juce::ColourGradient grad(juce::Colour(0xff161926), 0.0f, 0.0f,
                              juce::Colour(0xff0d0e17), 0.0f, (float) getHeight(), false);
    g.setGradientFill(grad);
    g.fillRect(getLocalBounds());

    // Top Header bar separator
    g.setColour(juce::Colour(0xff23283c));
    g.drawHorizontalLine(54, 0.0f, (float) getWidth());
}

void MainComponent::resized()
{
    int w = getWidth();
    int h = getHeight();

    // 1. Header Bar (y: 8 to 46)
    titleLabel.setBounds(16, 10, 270, 34);

    presetLabel.setBounds(290, 14, 60, 26);
    presetComboBox.setBounds(355, 14, 150, 26);
    savePresetButton.setBounds(512, 14, 55, 26);
    loadPresetButton.setBounds(572, 14, 55, 26);

    audioSettingsButton.setBounds(645, 14, 100, 26);
    liveRoutingButton.setBounds(752, 14, 125, 26);

    masterVolLabel.setBounds(895, 14, 85, 26);
    masterVolSlider.setBounds(980, 14, 130, 26);
    masterMeter.setBounds(1115, 17, 60, 20);
    clipResetButton.setBounds(1180, 14, 45, 26);

    // 2. Microphone Input Group (Top-Left)
    micGroup.setBounds(16, 62, 380, 115);
    micGainLabel.setBounds(30, 88, 70, 22);
    micGainSlider.setBounds(100, 88, 175, 22);
    micMeter.setBounds(280, 91, 100, 16);

    monitorToggle.setBounds(30, 128, 160, 26);
    safeModeToggle.setBounds(205, 128, 110, 26);

    // 3. Auto-Tune Group (Left-Center)
    autoTuneGroup.setBounds(16, 185, 595, 430);

    int modeBtnW = 180;
    naturalModeButton.setBounds(30, 215, modeBtnW, 32);
    hardTuneModeButton.setBounds(220, 215, modeBtnW, 32);
    bypassModeButton.setBounds(410, 215, modeBtnW, 32);

    keyLabel.setBounds(30, 260, 45, 24);
    keyComboBox.setBounds(75, 260, 80, 24);
    scaleLabel.setBounds(175, 260, 60, 24);
    scaleComboBox.setBounds(235, 260, 155, 24);

    // Custom Scale Note Pills (arranged in a row)
    int pillW = 42;
    int pillX = 30;
    for (size_t i = 0; i < customNotePills.size(); ++i)
    {
        customNotePills[i]->setBounds(pillX + static_cast<int>(i) * (pillW + 4), 295, pillW, 26);
    }
    updateCustomScalePillsVisibility();

    // Dials: Retune Speed, Strength, Humanize
    int dialY = 340;
    int dialW = 100;
    int dialH = 100;

    retuneSpeedLabel.setBounds(50, dialY - 20, dialW, 18);
    retuneSpeedSlider.setBounds(50, dialY, dialW, dialH);

    strengthLabel.setBounds(245, dialY - 20, dialW, 18);
    strengthSlider.setBounds(245, dialY, dialW, dialH);

    humanizeLabel.setBounds(440, dialY - 20, dialW, 18);
    humanizeSlider.setBounds(440, dialY, dialW, dialH);

    // Live Pitch Display Box
    int cardY = 475;
    pitchDisplayTitle.setBounds(30, cardY, 200, 20);
    pitchDetectedLabel.setBounds(30, cardY + 25, 170, 24);
    pitchTargetLabel.setBounds(215, cardY + 20, 160, 30);
    pitchDetuneLabel.setBounds(390, cardY + 25, 190, 24);

    // 4. Vocal Effects Rack (Right Section)
    effectsGroup.setBounds(625, 62, 600, 553);

    int effX = 645;
    int effRowY = 95;
    int effStepY = 58;

    // Gate
    gateToggle.setBounds(effX, effRowY, 110, 24);
    gateThreshSlider.setBounds(effX + 120, effRowY, 440, 24);

    // De-Esser
    effRowY += effStepY;
    deEsserToggle.setBounds(effX, effRowY, 110, 24);
    deEsserThreshSlider.setBounds(effX + 120, effRowY, 440, 24);

    // 3-Band EQ
    effRowY += effStepY;
    eqToggle.setBounds(effX, effRowY, 110, 24);
    int eqW = 140;
    eqLowSlider.setBounds(effX + 120, effRowY, eqW, 24);
    eqMidSlider.setBounds(effX + 120 + eqW + 10, effRowY, eqW, 24);
    eqHighSlider.setBounds(effX + 120 + (eqW + 10) * 2, effRowY, eqW, 24);

    // Compressor
    effRowY += effStepY;
    compToggle.setBounds(effX, effRowY, 110, 24);
    compThreshSlider.setBounds(effX + 120, effRowY, 215, 24);
    compRatioSlider.setBounds(effX + 345, effRowY, 215, 24);

    // Delay
    effRowY += effStepY;
    delayToggle.setBounds(effX, effRowY, 110, 24);
    delayTimeSlider.setBounds(effX + 120, effRowY, 215, 24);
    delayMixSlider.setBounds(effX + 345, effRowY, 215, 24);

    // Reverb
    effRowY += effStepY;
    reverbToggle.setBounds(effX, effRowY, 110, 24);
    reverbSizeSlider.setBounds(effX + 120, effRowY, 215, 24);
    reverbMixSlider.setBounds(effX + 345, effRowY, 215, 24);

    // Limiter
    effRowY += effStepY;
    limiterToggle.setBounds(effX, effRowY, 200, 24);

    // 5. Karaoke Player (Bottom)
    int kY = 625;
    karaokeGroup.setBounds(16, kY, w - 32, h - kY - 16);

    loadTrackButton.setBounds(30, kY + 30, 110, 28);
    trackNameLabel.setBounds(150, kY + 30, 360, 28);

    playButton.setBounds(520, kY + 30, 65, 28);
    pauseButton.setBounds(592, kY + 30, 65, 28);
    stopButton.setBounds(664, kY + 30, 65, 28);

    positionSlider.setBounds(30, kY + 68, 620, 22);
    timeLabel.setBounds(660, kY + 68, 120, 22);

    int kRightX = 790;
    backingVolLabel.setBounds(kRightX, kY + 25, 75, 22);
    backingVolSlider.setBounds(kRightX + 80, kY + 25, 170, 22);
    karaokeMeter.setBounds(kRightX + 260, kY + 28, 80, 16);

    pitchShiftLabel.setBounds(kRightX, kY + 58, 95, 22);
    pitchShiftSlider.setBounds(kRightX + 100, kY + 58, 150, 22);

    tempoLabel.setBounds(kRightX, kY + 90, 95, 22);
    tempoSlider.setBounds(kRightX + 100, kY + 90, 150, 22);
}

void MainComponent::updateCustomScalePillsVisibility()
{
    bool isCustom = (scaleComboBox.getSelectedId() == static_cast<int>(ScaleType::Custom) + 1);
    for (auto& pill : customNotePills)
    {
        pill->setVisible(isCustom);
    }
}

void MainComponent::timerCallback()
{
    // Update live peak meters
    micMeter.setLevel(audioEngine.getMicInputLevelDb());
    karaokeMeter.setLevel(audioEngine.getKaraokeLevelDb());
    masterMeter.setLevel(audioEngine.getMasterOutputLevelDb());

    // Check clipping
    if (audioEngine.hasOutputClipped())
    {
        masterMeter.setClipped(true);
        clipResetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff1744));
        clipResetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }

    // Update Live Pitch Monitor
    const auto& at = audioEngine.getAutoTune();
    float pitchHz = at.getLiveDetectedPitchHz();
    bool voiced = at.isLiveVoiced();

    if (voiced && pitchHz > 40.0f)
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << pitchHz << " Hz";
        pitchDetectedLabel.setText("Detected: " + juce::String(ss.str()), juce::dontSendNotification);

        pitchTargetLabel.setText("Target: " + at.getLiveNoteName(), juce::dontSendNotification);

        float detune = at.getLiveDetuneCents();
        std::ostringstream ss2;
        ss2 << (detune > 0.0f ? "+" : "") << std::fixed << std::setprecision(1) << detune << " cents";
        pitchDetuneLabel.setText("Detune: " + juce::String(ss2.str()), juce::dontSendNotification);

        pitchTargetLabel.setColour(juce::Label::textColourId, std::abs(detune) < 15.0f ? juce::Colour(0xff00e676) : juce::Colour(0xff00e5ff));
    }
    else
    {
        pitchDetectedLabel.setText("Detected: --- Hz", juce::dontSendNotification);
        pitchTargetLabel.setText("Target: ---", juce::dontSendNotification);
        pitchDetuneLabel.setText("Detune: 0 cents", juce::dontSendNotification);
        pitchTargetLabel.setColour(juce::Label::textColourId, juce::Colour(0xff607d8b));
    }

    // Update Karaoke timeline
    auto& kp = audioEngine.getKaraokePlayer();
    double totalSec = kp.getLengthInSeconds();
    if (totalSec > 0.0)
    {
        double currentSec = kp.getPosition();
        if (!positionSlider.isMouseButtonDown())
        {
            positionSlider.setValue(currentSec / totalSec, juce::dontSendNotification);
        }

        int curMin = static_cast<int>(currentSec) / 60;
        int curS = static_cast<int>(currentSec) % 60;
        int totMin = static_cast<int>(totalSec) / 60;
        int totS = static_cast<int>(totalSec) % 60;

        char buf[64];
        std::snprintf(buf, sizeof(buf), "%02d:%02d / %02d:%02d", curMin, curS, totMin, totS);
        timeLabel.setText(buf, juce::dontSendNotification);
    }
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &clipResetButton)
    {
        masterMeter.setClipped(false);
        clipResetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff251a24));
        clipResetButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffff5252));
    }
    else if (button == &naturalModeButton)
    {
        audioEngine.getAutoTune().setMode(AutoTuneMode::Natural);
        naturalModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff00e5ff));
        naturalModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        hardTuneModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
        hardTuneModeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
        bypassModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
        bypassModeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
    }
    else if (button == &hardTuneModeButton)
    {
        audioEngine.getAutoTune().setMode(AutoTuneMode::HardTune);
        hardTuneModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff9d4edd));
        hardTuneModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        naturalModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
        naturalModeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
        bypassModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
        bypassModeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
    }
    else if (button == &bypassModeButton)
    {
        audioEngine.getAutoTune().setMode(AutoTuneMode::Bypass);
        bypassModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff455a64));
        bypassModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        naturalModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
        naturalModeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
        hardTuneModeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202538));
        hardTuneModeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff0f4f8));
    }
    else if (button == &monitorToggle)
    {
        audioEngine.setInputMonitoringEnabled(monitorToggle.getToggleState());
    }
    else if (button == &safeModeToggle)
    {
        audioEngine.setSafeModeEnabled(safeModeToggle.getToggleState());
    }
    else if (button == &audioSettingsButton)
    {
        openAudioSettings();
    }
    else if (button == &liveRoutingButton)
    {
        openLivestreamGuide();
    }
    else if (button == &savePresetButton)
    {
        openSavePresetDialog();
    }
    else if (button == &loadPresetButton)
    {
        openLoadPresetDialog();
    }
    else if (button == &loadTrackButton)
    {
        loadKaraokeFile();
    }
    else if (button == &playButton)
    {
        audioEngine.getKaraokePlayer().start();
    }
    else if (button == &pauseButton)
    {
        audioEngine.getKaraokePlayer().pause();
    }
    else if (button == &stopButton)
    {
        audioEngine.getKaraokePlayer().stop();
        positionSlider.setValue(0.0, juce::dontSendNotification);
    }
    else
    {
        // Custom scale pill clicked
        for (size_t i = 0; i < customNotePills.size(); ++i)
        {
            if (button == customNotePills[i].get())
            {
                audioEngine.getAutoTune().setCustomNoteEnabled(static_cast<int>(i), customNotePills[i]->getToggleState());
                break;
            }
        }

        // Effect toggle buttons
        if (button == &gateToggle)
            audioEngine.getEffects().gate.enabled.store(gateToggle.getToggleState());
        else if (button == &deEsserToggle)
            audioEngine.getEffects().deEsser.enabled.store(deEsserToggle.getToggleState());
        else if (button == &eqToggle)
            audioEngine.getEffects().eq.enabled.store(eqToggle.getToggleState());
        else if (button == &compToggle)
            audioEngine.getEffects().comp.enabled.store(compToggle.getToggleState());
        else if (button == &delayToggle)
            audioEngine.getEffects().delay.enabled.store(delayToggle.getToggleState());
        else if (button == &reverbToggle)
            audioEngine.getEffects().reverb.enabled.store(reverbToggle.getToggleState());
        else if (button == &limiterToggle)
            audioEngine.getEffects().limiter.enabled.store(limiterToggle.getToggleState());
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &masterVolSlider)
        audioEngine.setMasterVolume(static_cast<float>(slider->getValue()));
    else if (slider == &micGainSlider)
        audioEngine.setMicGain(static_cast<float>(slider->getValue()));
    else if (slider == &retuneSpeedSlider)
        audioEngine.getAutoTune().setRetuneSpeedMs(static_cast<float>(slider->getValue()));
    else if (slider == &strengthSlider)
        audioEngine.getAutoTune().setCorrectionStrength(static_cast<float>(slider->getValue()) * 0.01f);
    else if (slider == &humanizeSlider)
        audioEngine.getAutoTune().setHumanize(static_cast<float>(slider->getValue()) * 0.01f);
    else if (slider == &gateThreshSlider)
        audioEngine.getEffects().gate.thresholdDb.store(static_cast<float>(slider->getValue()));
    else if (slider == &deEsserThreshSlider)
        audioEngine.getEffects().deEsser.thresholdDb.store(static_cast<float>(slider->getValue()));
    else if (slider == &eqLowSlider)
        audioEngine.getEffects().eq.lowGainDb.store(static_cast<float>(slider->getValue()));
    else if (slider == &eqMidSlider)
        audioEngine.getEffects().eq.midGainDb.store(static_cast<float>(slider->getValue()));
    else if (slider == &eqHighSlider)
        audioEngine.getEffects().eq.highGainDb.store(static_cast<float>(slider->getValue()));
    else if (slider == &compThreshSlider)
        audioEngine.getEffects().comp.thresholdDb.store(static_cast<float>(slider->getValue()));
    else if (slider == &compRatioSlider)
        audioEngine.getEffects().comp.ratio.store(static_cast<float>(slider->getValue()));
    else if (slider == &delayTimeSlider)
        audioEngine.getEffects().delay.timeMs.store(static_cast<float>(slider->getValue()));
    else if (slider == &delayMixSlider)
        audioEngine.getEffects().delay.mix.store(static_cast<float>(slider->getValue()) * 0.01f);
    else if (slider == &reverbSizeSlider)
        audioEngine.getEffects().reverb.roomSize.store(static_cast<float>(slider->getValue()) * 0.01f);
    else if (slider == &reverbMixSlider)
        audioEngine.getEffects().reverb.mix.store(static_cast<float>(slider->getValue()) * 0.01f);
    else if (slider == &backingVolSlider)
        audioEngine.getKaraokePlayer().setVolume(static_cast<float>(slider->getValue()));
    else if (slider == &pitchShiftSlider)
        audioEngine.getKaraokePlayer().setPitchSemitones(static_cast<int>(slider->getValue()));
    else if (slider == &tempoSlider)
        audioEngine.getKaraokePlayer().setSpeedRatio(static_cast<float>(slider->getValue()));
    else if (slider == &positionSlider)
    {
        double posRatio = slider->getValue();
        double totalSec = audioEngine.getKaraokePlayer().getLengthInSeconds();
        if (totalSec > 0.0)
            audioEngine.getKaraokePlayer().setPosition(posRatio * totalSec);
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &presetComboBox)
    {
        int index = presetComboBox.getSelectedId() - 1;
        const auto& presets = audioEngine.getPresetManager().getFactoryPresets();
        if (index >= 0 && index < static_cast<int>(presets.size()))
        {
            syncUIToPreset(presets[static_cast<size_t>(index)]);
        }
    }
    else if (comboBox == &keyComboBox)
    {
        audioEngine.getAutoTune().setRootKey(keyComboBox.getSelectedId() - 1);
    }
    else if (comboBox == &scaleComboBox)
    {
        auto scale = static_cast<ScaleType>(scaleComboBox.getSelectedId() - 1);
        audioEngine.getAutoTune().setScaleType(scale);
        updateCustomScalePillsVisibility();
    }
}

void MainComponent::syncUIToPreset(const PresetData& preset)
{
    audioEngine.getPresetManager().applyPreset(preset, audioEngine.getAutoTune(), audioEngine.getEffects());

    // Sync AutoTune controls
    if (preset.autoTuneMode == 0)
        buttonClicked(&naturalModeButton);
    else if (preset.autoTuneMode == 1)
        buttonClicked(&hardTuneModeButton);
    else
        buttonClicked(&bypassModeButton);

    retuneSpeedSlider.setValue(preset.retuneSpeedMs, juce::dontSendNotification);
    strengthSlider.setValue(preset.correctionStrength * 100.0f, juce::dontSendNotification);
    humanizeSlider.setValue(preset.humanize * 100.0f, juce::dontSendNotification);

    // Sync Effects toggles & sliders
    gateToggle.setToggleState(preset.gateEnabled, juce::dontSendNotification);
    gateThreshSlider.setValue(preset.gateThresholdDb, juce::dontSendNotification);

    deEsserToggle.setToggleState(preset.deEsserEnabled, juce::dontSendNotification);
    deEsserThreshSlider.setValue(preset.deEsserThreshDb, juce::dontSendNotification);

    eqToggle.setToggleState(preset.eqEnabled, juce::dontSendNotification);
    eqLowSlider.setValue(preset.eqLowGainDb, juce::dontSendNotification);
    eqMidSlider.setValue(preset.eqMidGainDb, juce::dontSendNotification);
    eqHighSlider.setValue(preset.eqHighGainDb, juce::dontSendNotification);

    compToggle.setToggleState(preset.compEnabled, juce::dontSendNotification);
    compThreshSlider.setValue(preset.compThresholdDb, juce::dontSendNotification);
    compRatioSlider.setValue(preset.compRatio, juce::dontSendNotification);

    delayToggle.setToggleState(preset.delayEnabled, juce::dontSendNotification);
    delayTimeSlider.setValue(preset.delayTimeMs, juce::dontSendNotification);
    delayMixSlider.setValue(preset.delayMix * 100.0f, juce::dontSendNotification);

    reverbToggle.setToggleState(preset.reverbEnabled, juce::dontSendNotification);
    reverbSizeSlider.setValue(preset.reverbRoomSize * 100.0f, juce::dontSendNotification);
    reverbMixSlider.setValue(preset.reverbMix * 100.0f, juce::dontSendNotification);

    limiterToggle.setToggleState(preset.limiterEnabled, juce::dontSendNotification);
}

void MainComponent::openAudioSettings()
{
    auto* selector = new juce::AudioDeviceSelectorComponent(
        audioEngine.getDeviceManager(),
        1, 2, // min/max inputs
        2, 2, // min/max outputs
        true, true, true, false
    );
    selector->setSize(520, 360);

    juce::DialogWindow::LaunchOptions opt;
    opt.dialogTitle = "Audio Hardware & Device Setup";
    opt.content.setOwned(selector);
    opt.resizable = false;
    opt.dialogBackgroundColour = juce::Colour(0xff161924);
    opt.launchAsync();
}

void MainComponent::openLivestreamGuide()
{
    juce::String guide = 
        "=== LIVESTREAM AUDIO ROUTING GUIDE ===\n\n"
        "1. OBS STUDIO:\n"
        "   - In Vocal Live Studio: Go to [Audio Setup] and set Output Device to 'CABLE Input' (VB-Audio Virtual Cable).\n"
        "   - In OBS Studio: Settings -> Audio -> Mic/Auxiliary Audio -> Select 'CABLE Output'.\n"
        "   - All Auto-Tune vocals and backing music will stream live into OBS with zero hum!\n\n"
        "2. TIKTOK LIVE STUDIO:\n"
        "   - Set Vocal Live Studio Output to 'CABLE Input'.\n"
        "   - In TikTok Live Studio -> Audio Settings -> Microphone -> Select 'CABLE Output'.\n\n"
        "3. LATENCY RECOMMENDATION:\n"
        "   - Use Buffer Size 128 or 256 samples (2.6ms - 5.3ms latency).\n"
        "   - Select WASAPI Exclusive or ASIO for lowest latency.";

    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Livestream Virtual Routing Setup",
        guide,
        "Got It"
    );
}

void MainComponent::openSavePresetDialog()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Save Custom Preset",
        audioEngine.getPresetManager().getPresetsDirectory().getChildFile("MyPreset.json"),
        "*.json"
    );

    auto folderChooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;
    fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file != juce::File())
        {
            auto currentPreset = audioEngine.getPresetManager().captureCurrentState(
                file.getFileNameWithoutExtension(),
                audioEngine.getAutoTune(),
                audioEngine.getEffects()
            );
            audioEngine.getPresetManager().savePresetToFile(currentPreset, file);
        }
    });
}

void MainComponent::openLoadPresetDialog()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Load Custom Preset",
        audioEngine.getPresetManager().getPresetsDirectory(),
        "*.json"
    );

    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file.existsAsFile())
        {
            PresetData loadedPreset;
            if (audioEngine.getPresetManager().loadPresetFromFile(file, loadedPreset))
            {
                syncUIToPreset(loadedPreset);
            }
        }
    });
}

void MainComponent::loadKaraokeFile()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select Backing Track (MP3, WAV, FLAC, AIFF)",
        juce::File::getSpecialLocation(juce::File::userMusicDirectory),
        "*.mp3;*.wav;*.flac;*.aif;*.aiff"
    );

    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file.existsAsFile())
        {
            if (audioEngine.getKaraokePlayer().loadFile(file))
            {
                trackNameLabel.setText(file.getFileName(), juce::dontSendNotification);
                positionSlider.setValue(0.0, juce::dontSendNotification);
            }
        }
    });
}

} // namespace VocalStudio
