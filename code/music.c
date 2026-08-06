#define N000 00.00
#define N_CN 16.35
#define N_CS 17.32
#define N_DN 18.35
#define N_DS 19.45
#define N_EN 20.60
#define N_FN 21.83
#define N_FS 23.12
#define N_GN 24.50
#define N_GS 25.96
#define N_AN 27.50
#define N_AS 29.14
#define N_BN 30.87

// TODO: try these ideas for flute sounds. These are notes for the NES chip.
// 
// The "Breath" Attack (Volume Envelope): Real players cannot instantly hit
// maximum volume. For pulse wave woodwinds, create a volume envelope that
// starts at volume level 4, ramps up to 15 over a few frames, and tapers off
// gracefully.
// 
// Legato & Portamento (Sliding Notes): Wind players frequently slur notes
// together without breaking their breath. Use pitch-glide commands to smoothly
// bend the pitch between consecutive notes rather than re-triggering them
// sharply.
//
// Vibrato: Wind musicians use their embouchure or diaphragm to create pitch
// variance. Program a gentle pitch vibrato that kicks in slightly after a note
// is held down, rather than immediately at the start of the note.
// 
// Chiff / Air Sound: To add realism to the start of a flute note, rapidly play
// a single frame of low-volume white noise right as the triangle wave starts.
// This perfectly mimics the brief "chiff" of air hitting a physical embouchure
// hole.

typedef struct {
    f64 freq;
    f64 amp;
} MusicTrackNote;

f64 note(f64 note, i32 octave) {
    for(i32 i = 0; i < octave; i++) {
        note *= 2;
    }
    return (f64)note;
}
