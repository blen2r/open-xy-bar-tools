#include <opxy/extension.hpp>
#include "policy.hpp"

// Hold BAR and press M3 to loop the selected (shown) bar; again to stop. With
// Shift, M3 loops the bars playing instead, or, looping, moves the loop to the
// selected bar. Picking bars to view or
// edit them keeps the loop; changing a track's length or scale lines the
// tracks up again; the SDK stops it when the pattern, the scene or the
// project changes. Shift + Play plays from the shown bar, moving a loop there.
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
void queued_control(const Context& context, const Control& control, const Actions& actions) {
    // Stock ignores Shift + Play; start (playing: restart) from the selected
    // track's shown bar. A loop (on or armed) moves there with it, every track
    // to the bar that lines up, as Shift + BAR + M3 does; Play alone still
    // restarts the loop where it is. play_from only asks the transport to
    // start, so the context stays current for the move, and a refused start
    // (a count-in under way) leaves the loop alone.
    if (control.key == keys::play) {
        if (!control.pressed || !control.shift || !control.accepted) return;
        if (!actions.play_from(context, shown_start(context)) || !looping(context.sequencer)) return;
        const BarLoop loop = choose(context, true);
        if (!already_loops(loop, context.sequencer) && actions.loop_bars(context, loop)) hold(context, loop);
        return;
    }
    if (control.key != keys::m3 || !control.pressed || control.screen != screens::bar_menu || !control.accepted) return;
    if (looping(context.sequencer) && !control.shift) {
        if (actions.loop_bars(context, none())) actions.notify("bar loop off");
        return;
    }
    // Not looping: the selected bar, or with Shift the bars playing. Looping,
    // Shift moves the loop to the selected bar.
    const bool selected = looping(context.sequencer) || !control.shift;
    const BarLoop loop = choose(context, selected);
    if (actions.loop_bars(context, loop)) {
        hold(context, loop);
        actions.notify(context.playing ? "bar loop on" : "bar loop armed");
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
// The bar menu's M3 says what it does, like M1, M2 and M4: "loop selected"
// (the shown bar), with Shift while playing "loop playing"; while bars loop
// (or are armed) "end loop", with Shift "loop selected" (move the loop).
const char* key_label(const Context& context, u32 key) {
    if (key != keys::m3) return nullptr;
    if (looping(context.sequencer)) return context.shift ? "loop selected" : "end loop";
    return context.playing && context.shift ? "loop playing" : "loop selected";
}
}
extern constexpr Hooks hooks =
    Hooks().with_queued_control(queued_control).with_key_label(key_label).with_sequencer_change(sequencer_change);
}}}
