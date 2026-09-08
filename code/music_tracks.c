MusicTrackNote melody_a[24];
MusicTrackNote melody_b_1[24];
MusicTrackNote melody_b_2[24];

MusicTrackNote bass_a[24];
MusicTrackNote bass_b_1[24];
MusicTrackNote bass_b_2[24];

MusicTrackNote melody_track_a[96];

void init_music_tracks() {
    // melody_a
    melody_a[0]  = (MusicTrackNote){ note(N_DN, 2), 1.0, false };
    melody_a[1]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[2]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[3]  = (MusicTrackNote){ note(N_AN, 3), 2.0, false };
    melody_a[4]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[5]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[6]  = (MusicTrackNote){ note(N_GN, 3), 2.0, false };
    melody_a[7]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[8]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[9]  = (MusicTrackNote){ note(N_AN, 2), 2.0, false };
    melody_a[10] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[11] = (MusicTrackNote){ note(N_AN, 2), 2.0, false };

    melody_a[12] = (MusicTrackNote){ note(N_DN, 2), 1.0, false };
    melody_a[13] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[14] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[15] = (MusicTrackNote){ note(N_AN, 3), 2.0, false };
    melody_a[16] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[17] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[18] = (MusicTrackNote){ note(N_GN, 3), 2.0, false };
    melody_a[19] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[20] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[21] = (MusicTrackNote){ note(N_AN, 2), 2.0, false };
    melody_a[22] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[23] = (MusicTrackNote){ note(N_AN, 2), 2.0, false };

    // melody_b_1 NOW: not actually tracked. its melody a right now.
    melody_a[0]  = (MusicTrackNote){ note(N_DN, 2), 1.0, false };
    melody_a[1]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[2]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[3]  = (MusicTrackNote){ note(N_AN, 3), 2.0, false };
    melody_a[4]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[5]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[6]  = (MusicTrackNote){ note(N_GN, 3), 2.0, false };
    melody_a[7]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[8]  = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[9]  = (MusicTrackNote){ note(N_AN, 2), 2.0, false };
    melody_a[10] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[11] = (MusicTrackNote){ note(N_AN, 2), 2.0, false };

    melody_a[12] = (MusicTrackNote){ note(N_DN, 2), 1.0, false };
    melody_a[13] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[14] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[15] = (MusicTrackNote){ note(N_AN, 3), 2.0, false };
    melody_a[16] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[17] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[18] = (MusicTrackNote){ note(N_GN, 3), 2.0, false };
    melody_a[19] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[20] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[21] = (MusicTrackNote){ note(N_AN, 2), 2.0, false };
    melody_a[22] = (MusicTrackNote){ note(N000, 0), 0.0, false };
    melody_a[23] = (MusicTrackNote){ note(N_AN, 2), 2.0, false };
}

