#include "plugin.hpp"

// Constants
constexpr size_t bufferSize = 44100;
std::vector<float> audioBuffer(bufferSize, 0.0);
size_t bufferPosition = 0;

struct SebsPlug : Module {
    // Define your parameters, inputs, and outputs here
    enum ParamId {
        VOLUME_PARAM,
        STUTTER_PARAM,
        PUSH_PARAM,
        HOLD_PARAM,
        NUM_PARAMS
    };
    enum InputId {
        INPUT_1,
        INPUT_2,
        NUM_INPUTS
    };
    enum OutputId {
        OUTPUT_1,
        NUM_OUTPUTS
    };
   // enum LightId {
     //   STUTTER_LIGHT,
       // NUM_LIGHTS
   // };

    // Button state variables
    bool push = false;
    bool hold = false;

    SebsPlug() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS); //NUM_LIGHTS
        configParam(VOLUME_PARAM, 0.0, 1.0, 0.5, "Volume");
        configParam(STUTTER_PARAM, 0.0, 1.0, 0.0, "Stutter Effect");
        configParam(PUSH_PARAM, 0.0, 1.0, 0.0, "Push Button");
        configParam(HOLD_PARAM, 0.0, 1.0, 0.0, "Hold Button");
    }

    void process(const ProcessArgs& args) override {
        // Read the volume parameter
        float volume = params[VOLUME_PARAM].getValue();

        // Read the stutter effect button state
        bool stutterActive = params[STUTTER_PARAM].getValue() > 0.0;

        // Read the push button state
        push = params[PUSH_PARAM].getValue() > 0.0;

        // Read the hold button state
        hold = params[HOLD_PARAM].getValue() > 0.0;

        // Iterate over each sample in the current audio block
        for (size_t i = 0; i < args.sampleTime; i++) {
            // Read the incoming audio signal
            float inputSample = inputs[INPUT_1].getVoltage();

            // Stutter effect logic
            if (stutterActive) {
                // Store the current audio sample in the buffer
                audioBuffer[bufferPosition] = inputSample;

                // Advance the buffer position, looping back to the start if needed
                bufferPosition = (bufferPosition + 1) % bufferSize;

                // Read the audio data from the buffer and apply the stutter effect
                inputSample = audioBuffer[bufferPosition];
            }

            // Apply some processing to the input signal based on the volume level
            float processedSignal = inputSample * volume;

            // Output the processed audio sample
            outputs[OUTPUT_1].setVoltage(processedSignal);
        }
    }
};

struct SebsPlugWidget : ModuleWidget {
    SebsPlugWidget(SebsPlug* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/SebsPlug2.svg")));
        addInput(createInput<ThemedPJ301MPort>(Vec(10, 323), module, SebsPlug::INPUT_1));
        addOutput(createOutput<ThemedPJ301MPort>(Vec(60, 323), module, SebsPlug::OUTPUT_1));


        addChild(createWidget<ThemedScrew>(mm2px(Vec(1, 0))));
		addChild(createWidget<ThemedScrew>(mm2px(Vec(24.5, 0))));
		addChild(createWidget<ThemedScrew>(mm2px(Vec(1, 123))));
		addChild(createWidget<ThemedScrew>(mm2px(Vec(24.5, 123))));
        
        // Add the volume knob
        addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(16, 85)), module, SebsPlug::VOLUME_PARAM));

        // Create a button input for the stutter effect
        addParam(createParamCentered<LEDButton>(mm2px(Vec(16, 55)), module, SebsPlug::STUTTER_PARAM));



         // Create the manual gate's big button
        //addParam(createParamCentered<CountModulaLEDPushButtonMegaMomentary<CountModulaPBLight<GreenLight>>>(
        //mm2px(Vec(15, 65)), module, SebsPlug::PUSH_PARAM, SebsPlug::PUSH_PARAM_LIGHT));

        // You can add more controls here if needed
    }
};

Model* modelSebsPlug = createModel<SebsPlug, SebsPlugWidget>("SebsPlug");
