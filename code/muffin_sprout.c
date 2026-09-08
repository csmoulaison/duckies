/*

Minigame level
- Hardcoded this level just doesn't accept the ability to move out of -x or -y
  bounds.

Honking
- Honking unlocks with a bool.
- Each entity tracks whether it has a honk "queued" for the next follower.
  Followers consume honks. Last marcher needs to know not to queue.
  - Probably will work to just sequence this so that followers are told to
    honk explicitly by the leader before the leader clears their honk state.
- When ducks honk, they choose a note from the music.
  
*/

void update_muffin_minigame(Game* game) {
    LevelState* state = &game->state;
    MuffinGame* minigame = &state->muffin_game;

    if(minigame->mode == MUFFIN_GAME_INACTIVE) {
        return;
    }
    if(minigame->mode == MUFFIN_GAME_LOST) {
        return;
    }
    if(minigame->mode == MUFFIN_GAME_WON) {
        return;
    }

    bool won = true;
    for(i32 i = 0; i < 9; i++) {
        if(minigame->muffins[i].state != MUFFIN_ASLEEP) {
            won = false;
            break;
        }
    }
    if(won) {
        minigame->mode = MUFFIN_GAME_WON;
        Msg msg[3] = {
            new_msg(MSG_MUFFIN, string_const("YOU DID IT!")),
            new_msg(MSG_MUFFIN, string_const("THE MUFFIN SPROUTS ARE ALL TUCKED IN TO BED.")),
            new_msg(MSG_MUFFIN, string_const("I'LL MEET YOU UP AHEAD FOR YOUR REWARD."))
        };
        start_msg(game, msg, 3, MODE_GAME);
    }

    if(game->new_cycle_this_frame) {
        // Choose a new tribute
        minigame->spawn_cooldown--;
        i32 down_count = 0;
        i32 sleep_count = 0;
        for(i32 i = 0; i < 9; i++) {
            Muffin* muffin = &minigame->muffins[i];
            if(muffin->state == MUFFIN_DOWN) {
                down_count++;
            }
            sleep_count += muffin->sleep_count;
        }
        if(down_count > 0) {
            if(minigame->spawn_cooldown < 0) {
                minigame->spawn_cooldown = 9 - sleep_count / 2;
                i32 tribute_down_index = random_i32(down_count - 1);
                assert(tribute_down_index < down_count);

                i32 count_to_tribute_index = 0;
                for(i32 i = 0; i < 9; i++) {
                    Muffin* muffin = &minigame->muffins[i];
                    if(muffin->state== MUFFIN_DOWN) {
                        if(count_to_tribute_index == tribute_down_index) {
                            muffin->state = MUFFIN_WARN;
                            muffin->state_counter = 0;
                            muffin->transition_t = 0.0;
                            break;
                        }
                        count_to_tribute_index++;
                    }
                }
            }
        }

        // Update muffins
        for(i32 i = 0; i < 9; i++) {
            iv2 pos = muffin_pos_from_index(i);
            Muffin* muffin = &minigame->muffins[i];

            // Put the boys to sleep.
            if(muffin->state == MUFFIN_UP && any_honk_adjacent(state, pos)) {
                muffin->sleep_count++;
                if(muffin->sleep_count > 1) {
                    muffin->state = MUFFIN_ASLEEP;
                } else {
                    muffin->state = MUFFIN_DOWN;
                }
                muffin->transition_t = 0.0;
                continue;
            }

            // Update state counter
            muffin->state_counter++;
            if(muffin->state == MUFFIN_WARN && muffin->state_counter >= 6) {
                muffin->state = MUFFIN_UP;
                muffin->state_counter = 0;
                muffin->transition_t = 0.0;
            } else if(muffin->state == MUFFIN_UP && muffin->state_counter >= 10) {
                minigame->mode = MUFFIN_GAME_LOST;
            }
        }
    }
}
