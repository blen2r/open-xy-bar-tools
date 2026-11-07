// Which bar each track loops. Pure functions of the context, so tests drive
// them directly.
#pragma once
#include <opxy/extension.hpp>

namespace opxy { namespace extensions { namespace bar_tools {

inline BarLoop none() {
    BarLoop loop;
    for (u32 t = 0; t < sequencer_tracks; ++t) loop.bar[t] = no_bar;
    return loop;
}
inline bool looping(const Sequencer& s) {
    for (u32 t = 0; t < sequencer_tracks; ++t)
        if (s.track[t].loop != no_bar) return true;
    return false;
}

// Whether every track already loops the bar `loop` gives it.
inline bool already_loops(const BarLoop& loop, const Sequencer& s) {
    for (u32 t = 0; t < sequencer_tracks; ++t)
        if (loop.bar[t] != s.track[t].loop) return false;
    return true;
}

// The bar a track plays `at` song ticks after the patterns started together:
// its bar N lines up with bar N mod its length of the others (in song time,
// so tracks on other scales line up too).
inline u8 aligned(u32 at, const SequencerTrack& track) {
    if (!track.bars || !track.bar_ticks) return 0;
    return static_cast<u8>((at % (track.bars * track.bar_ticks)) / track.bar_ticks);
}

// Where the selected track's shown bar starts, in song ticks (0 without one).
inline u32 shown_start(const Context& context) {
    const u32 selected = context.recording.track;
    if (selected >= sequencer_tracks) return 0;
    const SequencerTrack& anchor = context.sequencer.track[selected];
    return anchor.bars && anchor.shown < anchor.bars ? anchor.shown * anchor.bar_ticks : 0;
}

// Every track on the bar that lines up with `bar` of `anchor` (the anchor on
// that bar itself); nothing if the anchor has no such bar.
inline BarLoop aligned_to(const Context& context, u32 anchor, u32 bar) {
    BarLoop loop = none();
    if (anchor >= sequencer_tracks) return loop;
    const SequencerTrack& a = context.sequencer.track[anchor];
    if (bar >= a.bars) return loop;
    const u32 at = bar * a.bar_ticks;
    for (u32 t = 0; t < sequencer_tracks; ++t)
        if (context.sequencer.track[t].bars) loop.bar[t] = aligned(at, context.sequencer.track[t]);
    return loop;
}

// Playing: every track keeps the bar it is in. Stopped, or with `shown`: the
// selected track's shown bar, and every other track the bar that lines up
// with it; play then starts there (playing, each track moves to its bar at
// the same place within it).
inline BarLoop choose(const Context& context, bool shown = false) {
    BarLoop loop = none();
    const Sequencer& s = context.sequencer;
    const u32 at = shown_start(context);
    for (u32 t = 0; t < sequencer_tracks; ++t) {
        const SequencerTrack& track = s.track[t];
        if (!track.bars) continue;
        loop.bar[t] = context.playing && !shown ? track.bar : aligned(at, track);
    }
    return loop;
}
}}}
