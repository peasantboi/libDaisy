#pragma once
#ifndef DSY_TERRARIUM_H
#define DSY_TERRARIUM_H /**< & */

#include "daisy_seed.h"

namespace daisy
{
/**
   @brief Helpers and hardware definitions for daisy petal.
   @ingroup boards
*/
class DaisyTerrarium
{
  public:
    /** Switches */
    enum Sw
    {
        SW_1,    /**< Toggle */
        SW_2,    /**< Toggle */
        SW_3,    /**< Toggle */
        SW_4,    /**< Toggle */
	SW_5,    /**< Toggle */
	SW_6,    /**< Toggle */
        SW_LAST, /**< Last enum item */
    };

    /** Knobs */
    enum Knob
    {
        KNOB_1,    /**< & */
        KNOB_2,    /**< & */
        KNOB_3,    /**< & */
        KNOB_4,    /**< & */
        KNOB_5,    /**< & */
        KNOB_6,    /**< & */
        KNOB_LAST, /**< & */
    };

    /** footswitch leds */
    enum FootswitchLed
    {
        FOOTSWITCH_LED_1,    /**< & */
        FOOTSWITCH_LED_2,    /**< & */
	FOOTSWITCH_LED_LAST, /**< & */
    };

    /** Constructor */
    DaisyTerrarium() {}
    /** Destructor */
    ~DaisyTerrarium() {}

    /** Initialize daisy petal */
    void Init();

    /**
       Wait before moving on.
       \param del Delay time in ms.
     */
    void DelayMs(size_t del);


    /** Starts the callback
    \cb Interleaved callback function
    */
    void StartAudio(AudioHandle::InterleavingAudioCallback cb);

    /** Starts the callback
    \cb multichannel callback function
    */
    void StartAudio(AudioHandle::AudioCallback cb);

    /**
       Switch callback functions
       \param cb New interleaved callback function.
    */
    void ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb);

    /**
       Switch callback functions
       \param cb New multichannel callback function.
    */
    void ChangeAudioCallback(AudioHandle::AudioCallback cb);

    /** Stops the audio if it is running. */
    void StopAudio();

    /** Updates the Audio Sample Rate, and reinitializes.
     ** Audio must be stopped for this to work.
     */
    void SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate);

    /** Returns the audio sample rate in Hz as a floating point number.
     */
    float AudioSampleRate();

    /** Sets the number of samples processed per channel by the audio callback.
       \param size Audio block size
     */
    void SetAudioBlockSize(size_t size);

    /** Returns the number of samples per channel in a block of audio. */
    size_t AudioBlockSize();

    /** Returns the rate in Hz that the Audio callback is called */
    float AudioCallbackRate();

    /** Start analog to digital conversion. */
    void StartAdc();

    /** Stops Transfering data from the ADC */
    void StopAdc();

    /** Call at the same frequency as controls are read for stable readings.*/
    void ProcessAnalogControls();

    /** Process Analog and Digital Controls */
    inline void ProcessAllControls()
    {
        ProcessAnalogControls();
        ProcessDigitalControls();
    }


    /** Get value per knob.
    \param k Which knob to get
    \return Floating point knob position.
    */
    float GetKnobValue(Knob k);

    /** & */
    float GetExpression();

    /** Process digital controls */
    void ProcessDigitalControls();

    /** Turn all leds off */
    void ClearLeds();

    /** Update Leds to values you had set. */
    void UpdateLeds();

    /**
       Set footswitch LED
       \param idx Led Index
       \param bright Brightness
     */
    void SetFootswitchLed(FootswitchLed idx, float bright);

    DaisySeed seed;    /**< & */

    AnalogControl knob[KNOB_LAST]; /**< & */
    AnalogControl expression;      /**< & */
    Switch        switches[SW_LAST] /**< & */;

    RgbLed ring_led[8];       /**< & */
    Led    footswitch_led[4]; /**< & */

  private:
    void InitSwitches();
    void InitLeds();
    void InitAnalogControls();

    inline uint16_t* adc_ptr(const uint8_t chn) { return seed.adc.GetPtr(chn); }

    LedDriverPca9685<2, true> led_driver_;
};

} // namespace daisy

#endif
