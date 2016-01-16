# jack_synth
Practice making a synthesizer that interfaces with jack. The synthesizer is based off of the simple_client.c code found in the jack source code. 

## Requirements

You need to have a working install of JACK. Jack stands for Jack Audio Connection Kit. It basically is a common interface that professional audio programs can write to. For example, you can route the audio from a youtube video through an Equilizer. Or, if you have an audio interface, you can route your guitar through the same Equilizer software and then through an virtual guitar amp. It's a really powerful open source library that I've used for the last year or so. 

Setting up JACK is beyond the scope of this document, but if you use Arch Linux (or other distros) like me, you can follow this splendid [tutorial on the Arch Linux Wiki](https://wiki.archlinux.org/index.php/JACK_Audio_Connection_Kit) . I also know that JACK is available on Windows and Mac OSX, so I'm sure you can dig up documentation on that. 

## Compiling the program on Linux:

```gcc jack_synth.c -o jack_synth -ljack -lm```

## Playing the synth

There are three modes. Sine wave, square wave and sawtooth wave. When you start the program, it defaults to the sine wave, but you can switch to different wave forms by using the 1-3 keys on your keyboard:

1. Sine
2. Square
3. Sawtooth

You can play one octave, starting with an "A", by hitting the "A" key on your keyboard. The rest of the keys are:

| Keyboard letter | Musical note  |
|-----------------|---------------|
| A               | A             |
| W               | A#            |
| S               | B             |
| D               | C             |
| R               | C#            |
| F               | D             |
| T               | D#            |
| G               | E             |
| H               | F             |
| U               | F#            |
| J               | G             |
| I               | G#            |

## Future inhancements 

For some reason, the Sine wave becomes slightly distorted once you go higher than A. Something to investigate. 
