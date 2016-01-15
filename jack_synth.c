/** @file jack_synth.c
 *
 * @brief Modified from the simple_client.c file found in the jack source code. Simple synthesizer. 
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <jack/jack.h>
#include <termios.h>

jack_port_t *output_port1, *output_port2;
jack_client_t *client;

int pitch_ctr;

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define MAX_TABLE_SIZE   (110)
typedef struct
{

    int left_phase;
    int right_phase;
    int table_size; 
    float sine[MAX_TABLE_SIZE];

}
paTestData;
paTestData current_data;

static void signal_handler(int sig)
{
	jack_client_close(client);
	fprintf(stderr, "signal received, exiting ...\n");
	exit(0);
}

//                                a/A,    w/A#,     s/B,      d/C,      r/C#,     f/D,      t/D#,     g/E,      h/F,      u/F#,     j/G,      i/G# 
//static char key_lookup[12][2] = {{97,0},  {119,1},  {115,2},  {100,3},  {114,4},  {102,5},  {116,6},  {103,7},  {104,8},  {117,9},  {106,10}, {105,11}   };
static char note_lookup[12] = {97,  119,  115,  100,  114,  102, 116,  103,  104,  117,  106, 105 };

static int table_size_for_pitch[12] = {109,103,97,91,87,82,77,73,69,65,61,58};

int getnote(int ascii_num){

  for(int i = 0; i < 12; i++){
 
    if(note_lookup[i] == ascii_num) return i;

  }

  return -1;

}


char getch() {
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
          perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
          perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
          perror ("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
          perror ("tcsetattr ~ICANON");
  return (buf);
}



/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client follows a simple rule: when the JACK transport is
 * running, copy the input port to the output.  When it stops, exit.
 */

int
process (jack_nframes_t nframes, void *arg)
{

	jack_default_audio_sample_t *out1, *out2;
	paTestData *data = (paTestData*)arg;
	int i;
	out1 = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port1, nframes);
	out2 = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port2, nframes);

	for( i=0; i<nframes; i++ )
    {
        out1[i] = data->sine[data->left_phase];  /* left */
        out2[i] = data->sine[data->right_phase];  /* right */
        data->left_phase += pitch_ctr;
        if( data->left_phase >= current_data.table_size ) data->left_phase -= current_data.table_size;
        data->right_phase += pitch_ctr;
        if( data->right_phase >= current_data.table_size ) data->right_phase -= current_data.table_size;
    }
  
	return 0;      
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	exit (1);
}

void populate_data_table(int new_table_size, int mode){

  current_data.table_size = new_table_size ;

  // Build a sine wave
  if (mode == 1){

    for(int i=0; i<current_data.table_size; i++ )
      {
        current_data.sine[i] = 0.2 * (float) sin( ((double)i/(double)MAX_TABLE_SIZE) * M_PI * 2.0 );
      }

  // Build a square wave
  }else if (mode == 2){
 
    for(int i=0; i<current_data.table_size; i++ ) {
    
      if (i < current_data.table_size / 2){
     
        current_data.sine[i] = 0.2;
      
      }else {
     
        current_data.sine[i] = - 0.2;
      
      }

    }

  // Build a sawtooth wave 
  }else if (mode == 3){
 
    for(int i=0; i<current_data.table_size; i++ ) {
    
      current_data.sine[i] = (0.4 * ((float) i / (float) current_data.table_size)) - 0.2;

    }
     
  
  }

    current_data.left_phase = current_data.right_phase = 0;
}

int
main (int argc, char *argv[])
{
	const char **ports;
	const char *client_name;
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	//paTestData data;
	int i;

  pitch_ctr = 1;

	if (argc >= 2) {		/* client name specified? */
		client_name = argv[1];
		if (argc >= 3) {	/* server name specified? */
			server_name = argv[2];
            int my_option = JackNullOption | JackServerName;
			options = (jack_options_t)my_option;
		}
	} else {			/* use basename of argv[0] */
		client_name = strrchr(argv[0], '/');
		if (client_name == 0) {
			client_name = argv[0];
		} else {
			client_name++;
		}
	}


  populate_data_table(table_size_for_pitch[0],1);
	/* open a client connection to the JACK server */

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	jack_set_process_callback (client, process, &current_data);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, 0);

	/* create two ports */

	output_port1 = jack_port_register (client, "ootpoot1",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	output_port2 = jack_port_register (client, "ootpoot2",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	if ((output_port1 == NULL) || (output_port2 == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}


	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */
 	
	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port1), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	if (jack_connect (client, jack_port_name (output_port2), ports[1])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	jack_free (ports);
    
    /* install a signal handler to properly quits jack client */
#ifdef WIN32
	signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);
	signal(SIGTERM, signal_handler);
#else
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
#endif

	/* keep running until the Ctrl+C */

  int current_mode = 1; 
  int nxt_candid, old_candid;  
	while (1) {
 
    nxt_candid = getch();

    if((char) nxt_candid == 'q') break; 
    else if (nxt_candid >= 49 && nxt_candid <= 51) { current_mode = nxt_candid - 48;  populate_data_table(current_data.table_size,current_mode); }
    else if( -1 != (nxt_candid = getnote(nxt_candid)) ) populate_data_table(table_size_for_pitch[nxt_candid],current_mode);

	}

	jack_client_close (client);
	exit (0);
}
