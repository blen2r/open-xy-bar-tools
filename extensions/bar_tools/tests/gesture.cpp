// Bar tools driven as the SDK runtime calls it, with fake actions standing in
// for the SDK's loop and popup.
#include "extension.cpp"

using namespace opxy;
using namespace opxy::extensions;
#define CHECK(value) do { if (!(value)) return __LINE__; } while (false)

namespace {
u32 loops, popups;
BarLoop last;
char said[32];
u32 insert(const Context&, u32, const Note*, u32) { return 0; }
u32 replace(const Context&, u32, const Note*, u32) { return 0; }
void notify(const char* text) {
    ++popups;
    u32 i = 0;
    for (; text[i] && i < 31; ++i) said[i] = text[i];
    said[i] = 0;
}
bool loop(const Context&, const BarLoop& l) { ++loops; last = l; return true; }
u32 starts, started_at;
bool play(const Context&, u32 at) { ++starts; started_at = at; return true; }
const Actions actions = {insert, notify, replace, loop, play};

bool same(const char* a, const char* b) {
    while (*a && *a == *b) { ++a; ++b; }
    return *a == *b;
}
Context context(bool playing) {
    Context c = {};
    c.recording = {1, 2, 1, 16 * 480, 3, 0, true, true};
    c.playing = playing;
    for (u32 t = 0; t < sequencer_tracks; ++t) c.sequencer.track[t].loop = no_bar;
    return c;
}
void track(Context& c, u32 t, u8 bars, u8 bar, u8 shown = 0, u32 ticks = 7680) {
    c.sequencer.track[t].bars = bars; c.sequencer.track[t].bar = bar;
    c.sequencer.track[t].shown = shown; c.sequencer.track[t].bar_ticks = ticks;
}
// With BAR held the bar menu is on screen, and Shift hides it from `held`.
void key(const Context& c, u32 k, bool pressed, u32 held = keys::none, bool accepted = true, bool shift = false,
         bool menu = false) {
    const u32 screen = menu || held == keys::bar ? screens::bar_menu : 0;
    const Control control = {k, pressed, accepted, held == keys::none && !shift, 0, shift ? keys::none : held, shift,
                             screen};
    bar_tools::hooks.queued_control(c, control, actions);
}
// Hold BAR, press M3 (with Shift, or not), let go of both.
void toggle(const Context& c, bool shift = false) {
    key(c, keys::bar, true, keys::none, true, shift); key(c, keys::m3, true, keys::bar, true, shift);
    key(c, keys::m3, false, keys::bar, true, shift); key(c, keys::bar, false, keys::none, true, shift);
}
Context looped(Context c, const BarLoop& l) {
    for (u32 t = 0; t < sequencer_tracks; ++t) c.sequencer.track[t].loop = l.bar[t];
    return c;
}
}

int main() {
    // Playing, Shift: each track keeps the bar it is in. Track 1 (selected, 4
    // bars) is in bar 3; a 2-bar track is in its bar 1; an empty track is left alone.
    Context playing = context(true);
    track(playing, 1, 4, 2); track(playing, 0, 2, 0); track(playing, 4, 3, 2);
    toggle(playing, true);
    CHECK(loops == 1 && last.bar[1] == 2 && last.bar[0] == 0 && last.bar[4] == 2 && last.bar[5] == no_bar);
    CHECK(popups == 1 && same(said, "bar loop on"));
    // Again: off, with a popup; releasing BAR after M3 is not a bar pick.
    const Context on = looped(playing, last);
    toggle(on);
    CHECK(loops == 2 && last.bar[1] == no_bar && popups == 2 && same(said, "bar loop off"));

    // Picking bars (BAR on its own) to view or edit them keeps the loop.
    key(on, keys::bar, true); key(on, keys::bar, false);
    key(playing, keys::bar, true); key(playing, keys::bar, false);
    CHECK(loops == 2 && popups == 2);

    // Shift + BAR + M3 loops the bar shown, moving a loop there: track 1 loops
    // bar 3 but shows bar 1, so every track moves to the bar lining up with it.
    Context moved = on; moved.sequencer.track[1].shown = 0;
    toggle(moved, true);
    CHECK(loops == 3 && last.bar[1] == 0 && last.bar[0] == 0 && last.bar[4] == 0 && last.bar[5] == no_bar);
    CHECK(popups == 3 && same(said, "bar loop on"));
    // Shift pressed first keeps the menu open without BAR: M3 still works there.
    Context kept = looped(moved, last); kept.sequencer.track[1].shown = 3;
    key(kept, keys::m3, true, keys::none, true, true, true);
    CHECK(loops == 4 && last.bar[1] == 3 && popups == 4);
    key(kept, keys::m3, true, keys::none, true, false, false);  // menu closed: stock's
    CHECK(loops == 4);
    // With nothing looping, playing, no Shift: the selected (shown) bar, not the playing one.
    Context fresh = playing; fresh.sequencer.track[1].shown = 1;
    toggle(fresh);
    CHECK(loops == 5 && last.bar[1] == 1 && last.bar[0] == 1 && last.bar[4] == 1);

    // M3 outside the bar menu (alone, or with another key held) or not accepted: stock's.
    key(playing, keys::m3, true);
    key(playing, keys::m3, true, keys::bar, false);
    key(playing, keys::m3, true, keys::m1);
    CHECK(loops == 5);

    // Stopped: the selected track's shown bar, and the bars that line up with
    // it. Selected track 1 has 4 bars and shows bar 3 (index 2).
    Context stopped = context(false);
    track(stopped, 1, 4, 0, 2);
    track(stopped, 0, 1, 0);   // 1 bar: bar 1
    track(stopped, 2, 2, 0);   // 2 bars: 1-2-1 → bar 1
    track(stopped, 3, 3, 0);   // 3 bars: bar 3
    track(stopped, 4, 4, 0);   // 4 bars: bar 3
    track(stopped, 5, 3, 0, 0, 2560);  // a third of the scale: 3 of its bars per bar → (2*7680) % 7680 / 2560 = 0
    toggle(stopped);
    CHECK(loops == 6 && last.bar[1] == 2 && last.bar[0] == 0 && last.bar[2] == 0 && last.bar[3] == 2 && last.bar[4] == 2);
    CHECK(last.bar[5] == 0 && same(said, "bar loop armed"));
    // Shown bar 4 of the 4-bar track: a 3-bar track lines up on its bar 1.
    stopped.sequencer.track[1].shown = 3;
    toggle(stopped);
    CHECK(loops == 7 && last.bar[1] == 3 && last.bar[3] == 0 && last.bar[2] == 1);

    // M3's label in the bar menu follows the loop, armed included; other keys keep stock's.
    CHECK(same(bar_tools::hooks.key_label(playing, keys::m3), "loop selected"));
    CHECK(same(bar_tools::hooks.key_label(stopped, keys::m3), "loop selected"));
    CHECK(same(bar_tools::hooks.key_label(on, keys::m3), "end loop"));
    // Looping, Shift moves the loop to the selected bar; playing without a loop, Shift loops the bars playing.
    Context on_shift = on; on_shift.shift = true;
    Context playing_shift = playing; playing_shift.shift = true;
    CHECK(same(bar_tools::hooks.key_label(on_shift, keys::m3), "loop selected"));
    CHECK(same(bar_tools::hooks.key_label(playing_shift, keys::m3), "loop playing"));
    CHECK(same(bar_tools::hooks.key_label(looped(stopped, last), keys::m3), "end loop"));
    CHECK(bar_tools::hooks.key_label(on, keys::record) == nullptr);

    // Shift + Play, stopped: from the selected track's shown bar (track 1 shows bar 4).
    key(stopped, keys::play, true, keys::none, true, true);
    CHECK(starts == 1 && started_at == 3 * 7680);
    // Plain Play, a release, or not taken by stock: stock's.
    key(stopped, keys::play, true);
    key(stopped, keys::play, false, keys::none, true, true);
    key(stopped, keys::play, true, keys::none, false, true);
    CHECK(starts == 1);
    // Playing: a restart from the shown bar (track 1 shows bar 2).
    Context running = playing; running.sequencer.track[1].shown = 1; running.sequencer.track[1].bar_ticks = 7680;
    key(running, keys::play, true, keys::none, true, true);
    CHECK(starts == 2 && started_at == 7680);
    // The shown bar of the selected track only: another selection, another start.
    Context other = stopped; other.recording.track = 2; other.sequencer.track[2].shown = 1;
    key(other, keys::play, true, keys::none, true, true);
    CHECK(starts == 3 && started_at == 7680);

    // A scale change while armed: the other tracks line up again with the
    // selected track's bar. Track 1 (4 bars) arms bar 3; track 4 (3 bars) lines
    // up on its bar 3, then at half speed on its bar 2, as if set first.
    Context armed = context(false);
    track(armed, 1, 4, 0, 2); track(armed, 4, 3, 0);
    toggle(armed);
    CHECK(last.bar[1] == 2 && last.bar[4] == 2);
    Context slower = looped(armed, last);
    slower.sequencer.track[4].bar_ticks = 2 * 7680;
    const u32 before = loops;
    bar_tools::hooks.sequencer_change(slower, actions);
    CHECK(loops == before + 1 && last.bar[1] == 2 && last.bar[4] == 1);
    // A bar removed from another track (track 4 down to 2 bars, its looped bar
    // gone): it lines up again, on its bar 1.
    Context shorter = looped(armed, last);
    shorter.sequencer.track[4].bars = 2; shorter.sequencer.track[4].loop = no_bar;
    bar_tools::hooks.sequencer_change(shorter, actions);
    CHECK(loops == before + 2 && last.bar[1] == 2 && last.bar[4] == 0);
    // The selected track's own looped bar removed: the loop ends, and later
    // changes leave everything alone.
    Context gone = looped(armed, last);
    gone.sequencer.track[1].bars = 2; gone.sequencer.track[1].loop = no_bar;
    bar_tools::hooks.sequencer_change(gone, actions);
    CHECK(loops == before + 3 && last.bar[4] == no_bar && last.bar[1] == no_bar);
    bar_tools::hooks.sequencer_change(slower, actions);
    CHECK(loops == before + 3);
    return 0;
}
