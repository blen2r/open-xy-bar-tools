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
bool refuse;  // play_from refused (a count-in under way)
bool play(const Context&, u32 at) { ++starts; started_at = at; return !refuse; }
u32 shown_ms;
void notify_for(const char* text, u32 ms) { notify(text); shown_ms = ms; }
u32 waits, wait_at;
BarLoop waiting;
bool at_bar_end(const Context&, const BarLoop& l, u32 at) { ++waits; waiting = l; wait_at = at; return !refuse; }
const Actions actions = {insert, notify, replace, loop, play, notify_for, at_bar_end};

bool same(const char* a, const char* b) {
    if (!a || !b) return a == b;
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
// With BAR held the bar menu is on screen, and Shift hides it from `held`;
// `menu` alone is the bar menu locked open (Shift + BAR), BAR not held. The SDK
// takes Play there (`taken`) only with BAR held.
void key(const Context& c, u32 k, bool pressed, u32 held = keys::none, bool accepted = true, bool shift = false,
         bool menu = false) {
    const u32 screen = menu || held == keys::bar ? screens::bar_menu : 0;
    const bool taken = pressed && accepted && k == keys::play && held == keys::bar;
    const Control control = {k, pressed, accepted, held == keys::none && !shift, 0, shift ? keys::none : held, shift,
                             screen, taken};
    bar_tools::hooks.queued_control(c, control, actions);
}
// Hold BAR (and Shift, or not), press a key, let go of both.
void menu_key(const Context& c, u32 k, bool shift = false) {
    key(c, keys::bar, true, keys::none, true, shift); key(c, k, true, keys::bar, true, shift, true);
    key(c, k, false, keys::bar, true, shift, true); key(c, keys::bar, false, keys::none, true, shift);
}
Context looped(Context c, const BarLoop& l) {
    for (u32 t = 0; t < sequencer_tracks; ++t) c.sequencer.track[t].loop = l.bar[t];
    return c;
}
}

int main() {
    // In the bar menu only Play does anything; the M keys stay stock's.
    {
        Context c = context(true);
        track(c, 1, 4, 2);
        const u32 others[] = {keys::m1, keys::m2, keys::m3, keys::m4, keys::record};
        for (u32 k : others) {
            menu_key(c, k); menu_key(c, k, true);
        }
        CHECK(loops == 0 && popups == 0);
    }

    // Playing, BAR + Play: each track keeps the bar it is in. Track 1 (selected, 4
    // bars) is in bar 3; a 2-bar track is in its bar 1; an empty track is left alone.
    Context playing = context(true);
    track(playing, 1, 4, 2); track(playing, 0, 2, 0); track(playing, 4, 3, 2);
    menu_key(playing, keys::play);
    CHECK(loops == 1 && last.bar[1] == 2 && last.bar[0] == 0 && last.bar[4] == 2 && last.bar[5] == no_bar);
    CHECK(popups == 1 && same(said, "bar loop on"));
    // BAR + Play again: off, with a popup; releasing BAR after it is not a bar pick.
    const Context on = looped(playing, last);
    menu_key(on, keys::play);
    CHECK(loops == 2 && last.bar[1] == no_bar && popups == 2 && same(said, "bar loop off"));

    // Picking bars (BAR on its own) to view or edit them keeps the loop.
    key(on, keys::bar, true); key(on, keys::bar, false);
    key(playing, keys::bar, true); key(playing, keys::bar, false);
    CHECK(loops == 2 && popups == 2);

    // BAR + Shift + Play while looping moves the loop to the bar shown: track 1
    // loops bar 3 but shows bar 1, so every track moves to the bar lining up with it.
    Context moved = on; moved.sequencer.track[1].shown = 0;
    menu_key(moved, keys::play, true);
    CHECK(loops == 3 && last.bar[1] == 0 && last.bar[0] == 0 && last.bar[4] == 0 && last.bar[5] == no_bar);
    CHECK(popups == 3 && same(said, "bar loop on"));
    // Again on the bar it already loops: nothing changes.
    menu_key(looped(moved, last), keys::play, true);
    CHECK(loops == 3 && popups == 3);
    // The menu kept open by pressing Shift first, BAR let go: Play isn't taken
    // there, so Shift + Play is the one from outside the menu (plays from the
    // shown bar, the loop ending).
    Context kept = looped(moved, last); kept.sequencer.track[1].shown = 3;
    const u32 starts_then = starts;
    key(kept, keys::play, true, keys::none, true, true, true);
    CHECK(starts == starts_then + 1 && started_at == 3 * 7680);
    CHECK(loops == 4 && last.bar[1] == no_bar && popups == 4 && same(said, "bar loop off"));
    starts = starts_then;
    // Plain Play outside the menu, and Play held with another key: stock's.
    key(kept, keys::play, true);
    key(kept, keys::play, true, keys::m1);
    CHECK(loops == 4 && popups == 4);
    // With nothing looping, playing, BAR + Shift + Play: the selected (shown)
    // bar, not the playing one.
    Context fresh = playing; fresh.sequencer.track[1].shown = 1;
    menu_key(fresh, keys::play, true);
    CHECK(loops == 5 && last.bar[1] == 1 && last.bar[0] == 1 && last.bar[4] == 1);

    // Stopped, BAR + Play arms the selected track's shown bar, and the bars that line up
    // with it. Selected track 1 has 4 bars and shows bar 3 (index 2).
    Context stopped = context(false);
    track(stopped, 1, 4, 0, 2);
    track(stopped, 0, 1, 0);   // 1 bar: bar 1
    track(stopped, 2, 2, 0);   // 2 bars: 1-2-1 → bar 1
    track(stopped, 3, 3, 0);   // 3 bars: bar 3
    track(stopped, 4, 4, 0);   // 4 bars: bar 3
    track(stopped, 5, 3, 0, 0, 2560);  // a third of the scale: 3 of its bars per bar → (2*7680) % 7680 / 2560 = 0
    menu_key(stopped, keys::play);
    CHECK(loops == 6 && last.bar[1] == 2 && last.bar[0] == 0 && last.bar[2] == 0 && last.bar[3] == 2 && last.bar[4] == 2);
    CHECK(last.bar[5] == 0 && same(said, "bar loop armed"));
    // Shown bar 4 of the 4-bar track: a 3-bar track lines up on its bar 1.
    stopped.sequencer.track[1].shown = 3;
    menu_key(stopped, keys::play);
    CHECK(loops == 7 && last.bar[1] == 3 && last.bar[3] == 0 && last.bar[2] == 1);
    // Stopped with a loop armed, BAR + Play ends it.
    menu_key(looped(stopped, last), keys::play);
    CHECK(loops == 8 && last.bar[1] == no_bar && same(said, "bar loop off"));

    // Shift + Play, stopped: from the selected track's shown bar (track 1 shows
    // bar 4), without a loop.
    const u32 loops_before = loops;
    key(stopped, keys::play, true, keys::none, true, true);
    CHECK(starts == 1 && started_at == 3 * 7680 && loops == loops_before);
    // Plain Play, a release, or not taken by stock: stock's.
    key(stopped, keys::play, true);
    key(stopped, keys::play, false, keys::none, true, true);
    key(stopped, keys::play, true, keys::none, false, true);
    CHECK(starts == 1);
    // The bar menu locked open (BAR not held): Play is stock's (playback
    // starts), Shift + Play still plays from the shown bar.
    {
        const u32 loops_then = loops, popups_then = popups;
        key(stopped, keys::play, true, keys::none, true, false, true);
        CHECK(starts == 1 && loops == loops_then && popups == popups_then);
        key(stopped, keys::play, true, keys::none, true, true, true);
        CHECK(starts == 2 && started_at == 3 * 7680 && loops == loops_then);
        starts = 1;
    }
    // Playing: a restart from the shown bar (track 1 shows bar 2).
    Context running = playing; running.sequencer.track[1].shown = 1; running.sequencer.track[1].bar_ticks = 7680;
    key(running, keys::play, true, keys::none, true, true);
    CHECK(starts == 2 && started_at == 7680);
    // The shown bar of the selected track only: another selection, another start.
    Context other = stopped; other.recording.track = 2; other.sequencer.track[2].shown = 1;
    key(other, keys::play, true, keys::none, true, true);
    CHECK(starts == 3 && started_at == 7680);
    // A refused start does nothing more.
    const u32 loops_off = loops;
    refuse = true;
    key(stopped, keys::play, true, keys::none, true, true);
    CHECK(starts == 4 && loops == loops_off);
    refuse = false;

    // A scale change while armed: the other tracks line up again with the
    // selected track's bar. Track 1 (4 bars) arms bar 3; track 4 (3 bars) lines
    // up on its bar 3, then at half speed on its bar 2, as if set first.
    Context armed = context(false);
    track(armed, 1, 4, 0, 2); track(armed, 4, 3, 0);
    menu_key(armed, keys::play);
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

    // Shift + Play goes to the shown bar, ending a loop, as "transition"
    // says. Track 1 (4 bars) loops bar 3, a quarter
    // of the way in (1920 ticks), and shows bar 1; a 3-bar track loops its bar 3.
    Context playing_loop = context(true);
    track(playing_loop, 1, 4, 2, 0); track(playing_loop, 3, 3, 2);
    playing_loop.sequencer.track[1].offset = 1920;
    BarLoop held = bar_tools::none(); held.bar[1] = 2; held.bar[3] = 2;
    Context loop_on = looped(playing_loop, held);
    u32 n = loops, s0 = starts;
    // Instant: bar 1 at the same place within it; the loop ends.
    key(loop_on, keys::play, true, keys::none, true, true);
    CHECK(starts == s0 + 1 && started_at == 1920 && loops == n + 1 && last.bar[1] == no_bar);
    CHECK(same(said, "bar loop off"));
    // Nothing looping: the same jump, nothing to end.
    key(playing_loop, keys::play, true, keys::none, true, true);
    CHECK(starts == s0 + 2 && started_at == 1920 && loops == n + 1);
    // Restart: from bar 1's first step.
    bar_tools::hooks.setting(0, 1);
    key(loop_on, keys::play, true, keys::none, true, true);
    CHECK(starts == s0 + 3 && started_at == 0 && loops == n + 2 && last.bar[1] == no_bar);
    // Next bar: once the playing bar ends, from bar 1 with no loop.
    bar_tools::hooks.setting(0, 2);
    const u32 w1 = waits;
    key(loop_on, keys::play, true, keys::none, true, true);
    CHECK(waits == w1 + 1 && waiting.bar[1] == no_bar && waiting.bar[3] == no_bar && wait_at == 0);
    CHECK(starts == s0 + 3 && loops == n + 2 && same(said, "loop off at bar end"));
    bar_tools::hooks.setting(0, 0);
    // Plain Play while looping: stock's (restarts the loop where it is).
    key(loop_on, keys::play, true);
    CHECK(starts == s0 + 3 && loops == n + 2);
    // Start refused (a count-in under way): the loop stays.
    refuse = true;
    key(loop_on, keys::play, true, keys::none, true, true);
    refuse = false;
    CHECK(starts == s0 + 4 && loops == n + 2);
    // Stopped with a loop armed: from bar 1's first step, the armed loop ended.
    Context armed_loop = loop_on; armed_loop.playing = false;
    key(armed_loop, keys::play, true, keys::none, true, true);
    CHECK(starts == s0 + 5 && started_at == 0 && loops == n + 3 && last.bar[1] == no_bar);
    // BAR + Shift + Play moves a loop to the shown bar in place (instant); the
    // moved loop is the new anchor, so a scale change lines up with track 1's
    // bar 1 (the 3-bar track on its bar 1).
    key(loop_on, keys::play, true, keys::bar, true, true, true);
    CHECK(starts == s0 + 5 && loops == n + 4 && last.bar[1] == 0 && last.bar[3] == 0);
    Context after = looped(loop_on, last);
    after.sequencer.track[3].bar_ticks = 2 * 7680;
    bar_tools::hooks.sequencer_change(after, actions);
    CHECK(loops == n + 5 && last.bar[1] == 0 && last.bar[3] == 0);

    // BAR + Play.
    {
        // Playing, nothing looping: the bars playing loop; no restart.
        Context bar_playing = context(true);
        track(bar_playing, 1, 4, 2); track(bar_playing, 3, 3, 1);
        const u32 l0 = loops, p0 = starts;
        key(bar_playing, keys::play, true, keys::bar);
        CHECK(starts == p0 && loops == l0 + 1 && last.bar[1] == 2 && last.bar[3] == 1 && same(said, "bar loop on"));
        // A loop on (or armed): BAR + Play ends it.
        key(looped(bar_playing, last), keys::play, true, keys::bar);
        CHECK(starts == p0 && loops == l0 + 2 && last.bar[1] == no_bar && same(said, "bar loop off"));
        Context armed_play = looped(bar_playing, held); armed_play.playing = false;
        key(armed_play, keys::play, true, keys::bar);
        CHECK(starts == p0 && loops == l0 + 3 && last.bar[1] == no_bar);
        // Stopped, nothing looping: the shown bar is armed (track 1 shows bar
        // 4; the 3-bar track lines up on its bar 1), without starting.
        Context bar_stopped = context(false);
        track(bar_stopped, 1, 4, 2, 3); track(bar_stopped, 3, 3, 1);
        key(bar_stopped, keys::play, true, keys::bar);
        CHECK(starts == p0 && loops == l0 + 4 && last.bar[1] == 3 && last.bar[3] == 0 && last.bar[5] == no_bar);
        CHECK(same(said, "bar loop armed"));
        // BAR + Shift + Play: from the shown bar, looping it.
        key(bar_stopped, keys::play, true, keys::bar, true, true, true);
        CHECK(starts == p0 + 1 && started_at == 3 * 7680 && loops == l0 + 5 && last.bar[1] == 3 && last.bar[3] == 0);
        // A refused start loops nothing.
        refuse = true;
        key(bar_stopped, keys::play, true, keys::bar, true, true, true);
        refuse = false;
        CHECK(starts == p0 + 2 && loops == l0 + 5);
        // A release, or a press stock didn't accept, does nothing.
        key(bar_stopped, keys::play, false, keys::bar);
        key(bar_stopped, keys::play, true, keys::bar, false);
        CHECK(starts == p0 + 2 && loops == l0 + 5);
    }

    // "transition" (instant by default): while playing, new and moved
    // loops keep their place, or with restart play from their first step.
    {
        Context run = context(true);
        track(run, 1, 4, 2, 0); track(run, 3, 3, 1);
        const u32 l0 = loops, p0 = starts;
        // Off: BAR + Shift + Play loops the shown bar (bar 1) without a restart.
        key(run, keys::play, true, keys::bar, true, true, true);
        CHECK(starts == p0 && loops == l0 + 1 && last.bar[1] == 0 && last.bar[3] == 0);
        bar_tools::hooks.setting(0, 1);
        // On: from the shown bar's first step.
        key(run, keys::play, true, keys::bar, true, true, true);
        CHECK(starts == p0 + 1 && started_at == 0 && loops == l0 + 2 && last.bar[1] == 0);
        // BAR + Play: the bars playing, from the first step of track 1's bar 3.
        key(run, keys::play, true, keys::bar);
        CHECK(starts == p0 + 2 && started_at == 2 * 7680 && loops == l0 + 3 && last.bar[1] == 2 && last.bar[3] == 1);
        // A refused restart loops nothing.
        refuse = true;
        key(run, keys::play, true, keys::bar);
        refuse = false;
        CHECK(starts == p0 + 3 && loops == l0 + 3);
        bar_tools::hooks.setting(0, 0);
        // Stopped, BAR + Play still only arms.
        Context halt = run; halt.playing = false;
        key(halt, keys::play, true, keys::bar);
        CHECK(starts == p0 + 3 && loops == l0 + 4 && same(said, "bar loop armed"));
    }

    // Every Bar tools popup stays up 2 s; BAR itself shows nothing.
    {
        Context run = context(true); track(run, 1, 4, 0);
        const u32 n0 = popups;
        key(run, keys::bar, true);
        CHECK(popups == n0);
        key(run, keys::play, true, keys::bar);
        CHECK(popups == n0 + 1 && shown_ms == 2000);
    }

    // "transition" next bar: while playing, the selected bar waits for the
    // playing bar to end; stopped, and for the bars playing, as instant.
    {
        bar_tools::hooks.setting(0, 2);
        Context run = context(true);
        track(run, 1, 4, 2, 0); track(run, 3, 3, 1);
        const u32 l0 = loops, p0 = starts, w0 = waits;
        key(run, keys::play, true, keys::bar, true, true, true);
        CHECK(waits == w0 + 1 && waiting.bar[1] == 0 && waiting.bar[3] == 0 && wait_at == 0);
        CHECK(loops == l0 && starts == p0 && same(said, "loop at bar end"));
        // Moving a loop waits too; the selected bar already looping: nothing.
        BarLoop on = bar_tools::none(); on.bar[1] = 2; on.bar[3] = 2;
        Context moving = looped(run, on); moving.sequencer.track[1].shown = 3;
        key(moving, keys::play, true, keys::bar, true, true, true);
        CHECK(waits == w0 + 2 && waiting.bar[1] == 3 && wait_at == 3 * 7680);
        Context same_bar = looped(run, on); same_bar.sequencer.track[1].shown = 2;
        const u32 l1 = loops;
        key(same_bar, keys::play, true, keys::bar, true, true, true);
        CHECK(waits == w0 + 2 && loops == l1);
        // BAR + Play: the bars playing loop at once; stopped, BAR + Shift + Play plays.
        key(run, keys::play, true, keys::bar);
        CHECK(waits == w0 + 2 && loops == l0 + 1 && same(said, "bar loop on"));
        Context halt = run; halt.playing = false;
        key(halt, keys::play, true, keys::bar, true, true, true);
        CHECK(waits == w0 + 2 && starts == p0 + 1);
        // A refused request says nothing.
        const u32 n = popups;
        refuse = true;
        key(run, keys::play, true, keys::bar, true, true, true);
        refuse = false;
        CHECK(waits == w0 + 3 && popups == n);
        bar_tools::hooks.setting(0, 0);
    }

    // The bar already looping, looped again: left as it is (instant, next
    // bar), or with restart played from its first step.
    {
        Context run = context(true);
        track(run, 1, 4, 2, 2); track(run, 3, 3, 2);
        run.sequencer.track[1].offset = 5000;
        BarLoop on = bar_tools::none(); on.bar[1] = 2; on.bar[3] = 2;
        Context looping_now = looped(run, on);
        const u32 l0 = loops, w0 = waits, p0 = starts;
        bar_tools::hooks.setting(0, 0);
        key(looping_now, keys::play, true, keys::bar, true, true, true);
        bar_tools::hooks.setting(0, 2);
        key(looping_now, keys::play, true, keys::bar, true, true, true);
        CHECK(loops == l0 && waits == w0 && starts == p0);
        bar_tools::hooks.setting(0, 1);
        key(looping_now, keys::play, true, keys::bar, true, true, true);
        CHECK(loops == l0 && starts == p0 + 1 && started_at == 2 * 7680);
        bar_tools::hooks.setting(0, 0);
        // Stopped, the armed bar: BAR + Shift + Play plays from it, still armed.
        Context armed = looping_now; armed.playing = false;
        key(armed, keys::play, true, keys::bar, true, true, true);
        CHECK(loops == l0 && starts == p0 + 2 && started_at == 2 * 7680);
    }
    return 0;
}
