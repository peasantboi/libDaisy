#include "daisy_terrarium.h"

using namespace daisy;

#ifndef SAMPLE_RATE
//#define SAMPLE_RATE DSY_AUDIO_SAMPLE_RATE
#define SAMPLE_RATE 48014.f
#endif

// Hardware related defines.
// Switches
#define SW_1_PIN 2
#define SW_2_PIN 1
#define SW_3_PIN 0
#define SW_4_PIN 6

// 5 and 6 are footswitches
#define SW_5_PIN 4
#define SW_6_PIN 5

// Knobs
#define PIN_KNOB_1 0
#define PIN_KNOB_2 2
#define PIN_KNOB_3 4
#define PIN_KNOB_4 1
#define PIN_KNOB_5 3
#define PIN_KNOB_6 5

// LEDS
#define LED1_PIN 22
#define LED2_PIN 23

static constexpr I2CHandle::Config terrarium_led_i2c_config
    = {I2CHandle::Config::Peripheral::I2C_1,
       {{DSY_GPIOB, LED1_PIN}, {DSY_GPIOB, LED2_PIN}},
       I2CHandle::Config::Speed::I2C_1MHZ};

enum LedOrder
{
    LED_FS_1,
    LED_FS_2,
    LED_LAST,
};

static LedDriverPca9685<2, true>::DmaBuffer DMA_BUFFER_MEM_SECTION
    petal_led_dma_buffer_a,
    petal_led_dma_buffer_b;

void DaisyTerrarium::Init()
{
    // Set Some numbers up for accessors.
    // Initialize the hardware.
    seed.Configure();
    seed.Init();
    dsy_tim_start();
    InitSwitches();
    InitLeds();
    InitAnalogControls();
    SetAudioBlockSize(48);
    //seed.usb_handle.Init(UsbHandle::FS_INTERNAL);
}

void DaisyTerrarium::DelayMs(size_t del)
{
    seed.DelayMs(del);
}

void DaisyTerrarium::StartAudio(AudioHandle::InterleavingAudioCallback cb)
{
    seed.StartAudio(cb);
}

void DaisyTerrarium::StartAudio(AudioHandle::AudioCallback cb)
{
    seed.StartAudio(cb);
}

void DaisyTerrarium::ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb)
{
    seed.ChangeAudioCallback(cb);
}

void DaisyTerrarium::ChangeAudioCallback(AudioHandle::AudioCallback cb)
{
    seed.ChangeAudioCallback(cb);
}

void DaisyTerrarium::StopAudio()
{
    seed.StopAudio();
}

void DaisyTerrarium::SetAudioBlockSize(size_t size)
{
    seed.SetAudioBlockSize(size);
}

size_t DaisyTerrarium::AudioBlockSize()
{
    return seed.AudioBlockSize();
}

void DaisyTerrarium::SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate)
{
    seed.SetAudioSampleRate(samplerate);
}

float DaisyTerrarium::AudioSampleRate()
{
    return seed.AudioSampleRate();
}

float DaisyTerrarium::AudioCallbackRate()
{
    return seed.AudioCallbackRate();
}

void DaisyTerrarium::StartAdc()
{
    seed.adc.Start();
}

void DaisyTerrarium::StopAdc()
{
    seed.adc.Stop();
}


void DaisyTerrarium::ProcessAnalogControls()
{
    for(size_t i = 0; i < KNOB_LAST; i++)
    {
        knob[i].Process();
    }
}

float DaisyTerrarium::GetKnobValue(Knob k)
{
    size_t idx;
    idx = k < KNOB_LAST ? k : KNOB_1;
    return knob[idx].Value();
}

void DaisyTerrarium::ProcessDigitalControls()
{
    for(size_t i = 0; i < SW_LAST; i++)
    {
        switches[i].Debounce();
    }
}


void DaisyTerrarium::ClearLeds()
{
    // Using Color
    //    Color c;
    //    c.Init(Color::PresetColor::OFF);
    //    for(size_t i = 0; i < RING_LED_LAST; i++)
    //    {
    //        ring_led[i].SetColor(c);
    //    }
    for(size_t i = 0; i < FOOTSWITCH_LED_LAST; i++)
    {
        SetFootswitchLed(static_cast<FootswitchLed>(i), 0.0f);
    }
}

void DaisyTerrarium::UpdateLeds()
{
    led_driver_.SwapBuffersAndTransmit();
}

void DaisyTerrarium::SetFootswitchLed(FootswitchLed idx, float bright)
{
    uint8_t fs_addr[FOOTSWITCH_LED_LAST]
        = {LED_FS_1, LED_FS_2};
    led_driver_.SetLed(fs_addr[idx], bright);
}

void DaisyTerrarium::InitSwitches()
{
    //    // button1
    //    button1.Init(seed.GetPin(SW_1_PIN), callback_rate_);
    //    // button2
    //    button2.Init(seed.GetPin(SW_2_PIN), callback_rate_);
    //
    //    buttons[BUTTON_1] = &button1;
    //    buttons[BUTTON_2] = &button2;
    uint8_t pin_numbers[SW_LAST] = {
        SW_1_PIN,
        SW_2_PIN,
        SW_3_PIN,
        SW_4_PIN,
        SW_5_PIN,
        SW_6_PIN,
    };

    for(size_t i = 0; i < SW_LAST; i++)
    {
        switches[i].Init(seed.GetPin(pin_numbers[i]), AudioCallbackRate());
    }
}

void DaisyTerrarium::InitLeds()
{
    // LEDs are on the LED Driver.

    // Need to figure out how we want to handle that.
    uint8_t   addr[2] = {0x00, 0x01};
    I2CHandle i2c;
    i2c.Init(terrarium_led_i2c_config);
    led_driver_.Init(i2c, addr, petal_led_dma_buffer_a, petal_led_dma_buffer_b);
    ClearLeds();
    UpdateLeds();
}

void DaisyTerrarium::InitAnalogControls()
{
    // Set order of ADCs based on CHANNEL NUMBER
    AdcChannelConfig cfg[KNOB_LAST];
    // Init with Single Pins
    cfg[KNOB_1].InitSingle(seed.GetPin(PIN_KNOB_1));
    cfg[KNOB_2].InitSingle(seed.GetPin(PIN_KNOB_2));
    cfg[KNOB_3].InitSingle(seed.GetPin(PIN_KNOB_3));
    cfg[KNOB_4].InitSingle(seed.GetPin(PIN_KNOB_4));
    cfg[KNOB_5].InitSingle(seed.GetPin(PIN_KNOB_5));
    cfg[KNOB_6].InitSingle(seed.GetPin(PIN_KNOB_6));

    seed.adc.Init(cfg, KNOB_LAST);
    // Make an array of pointers to the knob.
    for(int i = 0; i < KNOB_LAST; i++)
    {
        knob[i].Init(seed.adc.GetPtr(i), AudioCallbackRate());
    }
}
