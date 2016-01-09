# jack_synth
Practice making a synthesizer that interfaces with jack. The synthesizer is based off of the simple_client.c code found in the jack source code. 

To compile the program:

```gcc jack_synth.c -o jack_synth -ljack -lm```

To play the synth, use the numbers on your keyboard (0 through 9) to play different pitch. Each tone is the 1/n of the original tone played, where n is the number pressed on the keyboard.
