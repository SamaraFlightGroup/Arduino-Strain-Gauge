#include "Application.hpp"

Application::Application()
{
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    lastPrintTime = millis();

    midiVariables[SAMPLES_PER_MEAN] = 30;
    midiVariables[MEANS_PER_SEQUENCE] = 10;
}

Application::~Application()
{
}

void Application::runApp()
{
    setTogglesLow();
    // printMode = PRINT_RAW_DATA;
    // printMode = PRINT_CURRENT_MIDI;
    // printMode = PRINT_CURRENT_CURRENT_MEAN;
    receiveMidiInput();
    receiveAdcData();
    runStateMachine();
    checkPrintModeTransition();
    //latestMean = findMean();
    printCsv();
}

void Application::runStateMachine()
{
    
    printMode = PRINT_CURRENT_WEIGHT;
    if (state == IDLE)
    {
        checkTransitionFromIdle();
    }
    else if (state == DO_MEAN_ONCE)
    {
        latestMean = findMean();
        printMode = PRINT_CURRENT_CURRENT_MEAN;
        state = IDLE;
    }
    else if (state == MEASURE_AND_SET_OFFSET)
    {
        state = IDLE;
    }
    else if (state == SEQUENTIAL_MEANS)
    {
        sequentialMeans();
    }
    else if (state == RUNNING_SAMPLES)
    {
        // printMode = PRINT_RAW_DATA;
    }
}

void Application::sequentialMeans()
{

    if (sequencialMeanCounter < midiVariables[MEANS_PER_SEQUENCE] * 10)
    {
        if (printMode != PRINT_CURRENT_CURRENT_MEAN)
        {
            latestMean = findMean();
            sequencialMeanCounter++;
        }
        printMode = PRINT_CURRENT_CURRENT_MEAN;
    }
    else
    {
        state = IDLE;
        sequencialMeanCounter = 0;
    }
}

double Application::findMean()
{
    double sum = 0;
    int meanLength = midiVariables[SAMPLES_PER_MEAN] * 5;
    for (int i = 0; i < meanLength; i++)
    {
        receiveAdcData();
        sum += currentData;
    }
    double mean = sum / meanLength;
    return mean;
}

void Application::receiveMidiInput()
{
    if (Serial.available() > 0)
    {
        // Read the incoming data
        String data = Serial.readStringUntil('\n');
        // Serial.println(data);
        //  Find the comma separator
        int commaIndex = data.indexOf(',');

        if (commaIndex != -1)
        {
            // Extract the two parts of the string
            String part1 = data.substring(0, commaIndex);
            String part2 = data.substring(commaIndex + 1);

            // Convert the parts to integers
            uint8_t midiSwitch = part1.toInt();
            uint8_t midiVariable = part2.toInt();
            currentMidi[0] = midiSwitch;
            currentMidi[1] = midiVariable;

            // Update MIDI variables
            updateMidiVariables(midiSwitch, midiVariable);
        }
        else
        {
            // Handle the case where the data format is incorrect
            Serial.println("Error: Invalid data format");
        }
    }
}

void Application::checkTransitionFromIdle()
{
    if (currentMidi[0] == 48)
    {
        state = DO_MEAN_ONCE;
    }
    else if (currentMidi[0] == 50)
    {
        state = MEASURE_AND_SET_OFFSET;
    }
    else if (currentMidi[0] == 52)
    {
        state = SEQUENTIAL_MEANS;
    }
    else if (currentMidi[0] == 54)
    {
        state = RUNNING_SAMPLES;
    }
}

void Application::checkPrintModeTransition()
{
    if (currentMidi[0] == 36)
    {
        printMode = PRINT_NOTHING;
    }
    else if (currentMidi[0] == 37)
    {
        printMode = PRINT_CURRENT_MIDI;
    }
    else if (currentMidi[0] == 38)
    {
        printMode = PRINT_RAW_DATA;
    }
    else if (currentMidi[0] == 39)
    {
        printMode = PRINT_CURRENT_CURRENT_MEAN;
    }
    else if (currentMidi[0] == 40)
    {
        printMode = PRINT_CURRENT_WEIGHT;
    }
}

void Application::calculateWeight(int64_t devons)
{
    currentWeight = ((double)devons-123248) * 0.0023;
    // 123248 sets weight calculation offset
}

void Application::updateMidiVariables(uint8_t midiSwitch, uint8_t midiVariable)
{
    midiVariables[midiSwitch] = midiVariable;
}

void Application::receiveAdcData()
{
    if (scale.wait_ready_timeout(1000))
    {
        long reading = scale.read();
        currentData = reading - 0;
        // 0 is raw data offset
    }
    else
    {
        Serial.println("HX711 not found.");
    }
}

void Application::setTogglesLow()
{
    for (int i = LOWEST_NOTE; i < HIGHEST_NOTE; i++)
    {
        midiVariables[i] = 0;
    }

    currentMidi[0] = 0;
    currentMidi[1] = 0;
}

void Application::printCsv()
{
    if (millis() - lastPrintTime > printDelay)
    // change printDelay in Application.hpp 
    {

        if (printMode == PRINT_SWITCH_UNIFORMS)
        {
            Serial.print(midiVariables[SAMPLES_PER_MEAN]);
            Serial.print(", ");
            Serial.println(midiVariables[MEANS_PER_SEQUENCE]);
        }
        else if (printMode == PRINT_CURRENT_MIDI)
        {
            Serial.print(currentMidi[0]);
            Serial.print(", ");
            Serial.println(currentMidi[1]);
        }
        else if (printMode == PRINT_RAW_DATA)
        {
            Serial.println(currentData);
        }
        else if (printMode == PRINT_CURRENT_CURRENT_MEAN)
        {
            Serial.print("> ");
            Serial.println(latestMean);
            printMode = PRINT_NOTHING;
        }
        else if (printMode == PRINT_CURRENT_WEIGHT)
        {
            calculateWeight(currentData);
            Serial.print(currentData);
            Serial.print(", ");
            Serial.println(currentWeight);
        }

        lastPrintTime = millis();
    }
}
