#include <opxy/extension.hpp>
#include "policy.hpp"

// BAR + Play arms the selected (shown) bar while stopped, loops the bars
// playing while playing, and ends a loop. BAR + Shift + Play loops the selected
// bar (stopped: playing from it) or moves a loop there. Shift + Play plays
// from the selected bar, ending a loop.
// While playing, these follow
// "transition": at the same place within the bar (instant), from its
// first step (restart), or once the playing bar ends (next bar). The bar
// menu's M keys stay stock's, and with the bar menu locked open (Shift + BAR)
// Play alone is stock's; BAR + Play there keeps the menu open. Popups stay up 2 s, as stock's do; the settings
// page also lists the shortcuts. Picking
// bars to view or edit them keeps the loop; changing a track's length or scale
// lines the tracks up again; the SDK stops it when the pattern, the song or
// the project changes.
namespace opxy { namespace extensions { namespace bar_tools {
namespace {
// The selected track when the loop was made, and the bar it loops: when a
// track's length or scale changes, the others line up with it again.
struct Anchor { u32 track; u8 bar; };
Anchor anchor = {sequencer_tracks, no_bar};
// Settings rows (extension.json): 0 how a loop made, moved or left while
// playing takes over (instant, restart, next bar). The rows after it (a
// separator, then the shortcuts) are only shown.
enum Transition : u32 { instant, restart, next_bar };
const u32 popup_ms = 2000;
u32 transition = instant;
void setting(u32 row, u32 value) {
    if (row == 0) transition = value;
}
void say(const Actions& actions, const char* text) { actions.notify_for(text, popup_ms); }

void hold(const Context& context, const BarLoop& loop) {
    const u32 t = context.recording.track;
    anchor = {t, t < sequencer_tracks ? loop.bar[t] : no_bar};
}
void take(const Context& context, const Actions& actions, const BarLoop& loop) {
    if (!actions.loop_bars(context, loop)) return;
    hold(context, loop);
    say(actions, context.playing ? "bar loop on" : "bar loop armed");
}
void end_loop(const Context& context, const Actions& actions) {
    if (actions.loop_bars(context, none())) say(actions, "bar loop off");
}
// Start (playing: restart) from the selected track's shown bar. A loop (on or
// armed) moves there with it, every track to the bar that lines up; with
// `loops`, a loop starts there when none is on. play_from only asks the
// transport to start, so the context stays current for the move, and a
// refused start (a count-in under way) leaves the loop alone.
void play_from_shown(const Context& context, const Actions& actions, bool loops) {
    if (!actions.play_from(context, shown_start(context))) return;
    const bool was_looping = looping(context.sequencer);
    if (!was_looping && !loops) return;
    const BarLoop loop = choose(context, true);
    if (already_loops(loop, context.sequencer) || !actions.loop_bars(context, loop)) return;
    hold(context, loop);
    if (!was_looping) say(actions, "bar loop on");
}
// The selected bar, every other track on the bar lining up with it: a new
// loop, or a running one moved there. Stopped, it is armed, or with `start`
// played from. Playing, as "transition" says: it keeps the place in the
// bar, plays from its first step, or waits for the playing bar to end; on the
// bar already looping, only restart does anything (it restarts the bar).
void loop_selected(const Context& context, const Actions& actions, bool start) {
    const BarLoop loop = choose(context, true);
    const bool again = already_loops(loop, context.sequencer);
    if ((!context.playing && start) || (context.playing && transition == restart)) {
        play_from_shown(context, actions, true);
        return;
    }
    if (again) return;
    if (!context.playing || transition == instant) return take(context, actions, loop);
    if (!actions.loop_at_bar_end(context, loop, shown_start(context))) return;
    hold(context, loop);
    say(actions, "loop at bar end");
}
// Playing: every track keeps the bar it is in, or with "transition" on
// restart plays from the first step of the selected track's bar. (Next bar is
// instant here: the bar playing loops once it ends either way.)
void loop_playing(const Context& context, const Actions& actions) {
    const BarLoop loop = choose(context);
    if (transition == restart && !actions.play_from(context, playing_start(context))) return;
    take(context, actions, loop);
}
// Shift + Play: play from the selected bar, a
// loop (on or armed) ending. Stopped, from its first step; playing, as "loop
// transition" says: at the same place within the bar, from its first step, or
// once the playing bar ends.
void go_to_selected(const Context& context, const Actions& actions) {
    const bool was_looping = looping(context.sequencer);
    const u32 start = shown_start(context);
    if (context.playing && transition == next_bar) {
        if (actions.loop_at_bar_end(context, none(), start) && was_looping) say(actions, "loop off at bar end");
        return;
    }
    u32 at = start;
    const u32 t = context.recording.track;
    if (context.playing && transition == instant && t < sequencer_tracks) at += context.sequencer.track[t].offset;
    if (!actions.play_from(context, at)) return;
    if (was_looping) end_loop(context, actions);
}
void queued_control(const Context& context, const Control& control, const Actions& actions) {
    // Play: BAR + Play and BAR + Shift + Play (the bar_menu claim takes Play
    // in the bar menu with BAR held, so stock's never runs); otherwise Shift +
    // Play, which stock ignores (Play alone still restarts a loop where it is,
    // or starts playback with the bar menu locked open).
    if (!control.pressed || !control.accepted || control.key != keys::play) return;
    if (control.taken) {
        if (control.shift) loop_selected(context, actions, true);
        else if (looping(context.sequencer)) end_loop(context, actions);
        else if (context.playing) loop_playing(context, actions);
        else loop_selected(context, actions, false);
    } else if (control.shift) {
        go_to_selected(context, actions);
    }
}
// A track's bar count or scale changed while looping: line every track up
// with the anchor again, as if the change had come first. Removing the
// anchor's own looped bar ends the loop.
void sequencer_change(const Context& context, const Actions& actions) {
    if (anchor.track >= sequencer_tracks) return;
    if (context.sequencer.track[anchor.track].loop != anchor.bar) {
        actions.loop_bars(context, none());
        anchor = {sequencer_tracks, no_bar};
        return;
    }
    const BarLoop loop = aligned_to(context, anchor.track, anchor.bar);
    if (loop.bar[anchor.track] != no_bar) actions.loop_bars(context, loop);
}
}
extern constexpr Hooks hooks =
    Hooks().with_queued_control(queued_control).with_sequencer_change(sequencer_change)
           .with_setting(setting);
}}}
