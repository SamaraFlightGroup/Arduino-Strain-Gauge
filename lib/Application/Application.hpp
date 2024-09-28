#ifndef Application_hpp
#define Application_hpp

#include <Arduino.h>
#include <HX711.h>

#define LOWEST_NOTE 36
#define HIGHEST_NOTE 96

#define NUMBER_OF_MIDI_SWITCHES 128
#define NUMBERS_PER_TRANSACTION 2

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

enum State
{
    IDLE,
    DO_MEAN_ONCE,
    MEASURE_AND_SET_OFFSET,
    SEQUENTIAL_MEANS,
    RUNNING_SAMPLES
};

enum SwitchUniforms
{
    SAMPLES_PER_MEAN = 31,
    MEANS_PER_SEQUENCE = 33
};

enum PrintMode
{
    PRINT_NOTHING,
    PRINT_RAW_DATA,
    PRINT_SWITCH_UNIFORMS,
    PRINT_CURRENT_MIDI,
    PRINT_CURRENT_CURRENT_MEAN,
    PRINT_CURRENT_WEIGHT
};

class Application
{
private:
    HX711 scale;

    uint8_t state = IDLE;
    uint8_t midiVariables[NUMBER_OF_MIDI_SWITCHES];
    uint8_t currentMidi[NUMBERS_PER_TRANSACTION];
    int64_t currentData;
    double currentWeight = 0;
    uint32_t sequencialMeanCounter = 0;
    PrintMode printMode = PRINT_NOTHING;

    double latestMean = 0;

    uint64_t printDelay = 40;
    uint64_t lastPrintTime = 0;

public:
    Application();
    ~Application();

    void runApp();
    void runStateMachine();
    double findMean();
    void sequentialMeans();

    void receiveMidiInput();
    void updateMidiVariables(uint8_t midiSwitch, uint8_t midiVariable);

    void checkTransitionFromIdle();
    void checkPrintModeTransition();

    void receiveAdcData();
    void printCsv();
    void logCsv();

    void setTogglesLow();
    void calculateWeight(int64_t devons);
};

#endif // Application_hpp