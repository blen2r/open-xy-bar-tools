#include <opxy/extension.hpp>
#include "policy.hpp"

// In the bar menu (hold BAR), M1 loops the selected (shown) bar, or moves a
// running loop there; M2 loops the bars playing, or ends a loop. M3 and M4 are
// left empty. With Shift, the M keys are stock's clears, as the SDK gives
// them back. Picking bars to view or edit them keeps the loop; changing a
// track's length or scale lines the tracks up again; the SDK stops it when
// the pattern, the song or the project changes. Shift + Play plays from the
// shown bar, moving a loop there.
namespace opxy { namespace extensions { namespace bar_tools {
namespace {
// The selected track when the loop was made, and the bar it loops: when a
// track's length or scale changes, the others line up with it again.
struct Anchor { u32 track; u8 bar; };
Anchor anchor = {sequencer_tracks, no_bar};

void hold(const Context& context, const BarLoop& loop) {
    const u32 t = context.recording.track;
    anchor = {t, t < sequencer_tracks ? loop.bar[t] : no_bar};
}
void take(const Context& context, const Actions& actions, const BarLoop& loop) {
    if (!actions.loop_bars(context, loop)) return;
    hold(context, loop);
    actions.notify(context.playing ? "bar loop on" : "bar loop armed");
}
void queued_control(const Context& context, const Control& control, const Actions& actions) {
    // Stock ignores Shift + Play; start (playing: restart) from the selected
    // track's shown bar. A loop (on or armed) moves there with it, every track
    // to the bar that lines up, as M1 does; Play alone still restarts the loop
    // where it is. play_from only asks the transport to start, so the context
    // stays current for the move, and a refused start (a count-in under way)
    // leaves the loop alone.
    if (control.key == keys::play) {
        if (!control.pressed || !control.shift || !control.accepted) return;
        if (!actions.play_from(context, shown_start(context)) || !looping(context.sequencer)) return;
        const BarLoop loop = choose(context, true);
        if (!already_loops(loop, context.sequencer) && actions.loop_bars(context, loop)) hold(context, loop);
        return;
    }
    // The bar menu's M keys without Shift (with Shift they are stock's).
    if (!control.pressed || control.shift || control.screen != screens::bar_menu || !control.accepted) return;
    if (control.key == keys::m1) {
        // The selected bar, every other track on the bar lining up with it: a
        // new loop, or a running one moved there.
        const BarLoop loop = choose(context, true);
        if (!already_loops(loop, context.sequencer)) take(context, actions, loop);
    } else if (control.key == keys::m2) {
        // Looping (or armed): end it. Playing: every track keeps the bar it is
        // in. Stopped with nothing looping there is nothing playing to loop.
        if (looping(context.sequencer)) {
            if (actions.loop_bars(context, none())) actions.notify("bar loop off");
        } else if (context.playing) {
            take(context, actions, choose(context));
        }
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
// The bar menu's labels say what each key does: M1 "loop bar"; M2 "end
// loop" while bars loop (or are armed), otherwise "loop playing", greyed out
// while stopped; M3 and M4 empty.
KeyLabel key_label(const Context& context, u32 key) {
    if (key == keys::m1) return {"loop bar", true};
    if (key != keys::m2) return {nullptr, true};
    if (looping(context.sequencer)) return {"end loop", true};
    return {"loop playing", context.playing};
}
}
extern constexpr Hooks hooks =
    Hooks().with_queued_control(queued_control).with_key_label(key_label).with_sequencer_change(sequencer_change);
}}}
