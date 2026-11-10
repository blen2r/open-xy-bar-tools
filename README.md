# Bar tools

An Open-XY firmware extension for the OP-XY (OS 1.2.1) for working one bar at a time, with everything
else about playback as stock:

- **Play from a bar:** Shift + Play plays from the bar you're on instead of from the top.
- **Loop a bar:** BAR + Shift + Play does the same and keeps repeating that bar, while you keep
  editing any bar. BAR + Play loops the bars playing, and ends a loop.
- **Choose how they take over while playing:** at once, from the bar's first step, or when the playing
  bar ends, so you stay on the beat.
- **See which bar is playing:** the bar menu marks the playing bar with a dot and the looped bar in
  grey.
- **Change bars with the bar menu locked open:** a tap of BAR shows the next bar there too, and
  Shift + BAR closes the menu.

## What stock already has: the playing bar and the selected bar

A pattern on the OP-XY has up to 4 bars of 16 steps. Two of them matter here, and both are stock's:

- **The playing bar** is the bar the playhead is in. The running light on the step keys shows where
  it is.
- **The selected bar** is the bar whose steps the 16 step keys show and edit. Tap **BAR** to select
  the next bar. While the step keys follow the playhead, the selected bar moves with
  playback, so most of the time the two are the same. Select another bar and you can edit it while
  something else plays; its step keys then show no running light.

Hold **BAR** to open stock's bar menu. It shows the track's bars as boxes, the selected one
highlighted, with M1 "clr notes", M2 "clr params", M4 "clr all" (each clears once you've held the key
for a moment), and "+" and "-" to add or remove bars. Bar tools adds Play to the menu (BAR + Play and
BAR + Shift + Play); the rest stays as stock's.

## What Bar tools adds

### The shortcuts

Shift + Play and BAR + Shift + Play both play from the selected bar: the first without looping, the
second looping it. BAR + Play loops what's playing, or ends the loop.

```
                     Stopped                          Playing
Shift + Play         play from the selected bar       go to the selected bar, no loop
                     (no loop)                        (a loop ends)
BAR + Shift + Play   play from the selected bar,      loop the selected bar
                     looping it                       (a loop moves there)
BAR + Play           arm a loop on the selected bar   loop the bars playing
                     (armed: end it)                  (looping: end it)
```

"Playing" changes follow the [transition](#transition) setting: at once by default. Popups
say what happened ("bar loop on", "bar loop armed", "bar loop off", "loop at bar end"). Shift works
whichever you press first.

**Bar menu locked open.** Pressing Shift before BAR keeps the bar menu open after you let go of both
(stock). Play alone then starts playback as in stock, so you can watch the bars play, and Shift + Play
plays from the selected bar. Hold BAR for the BAR shortcuts: the menu stays open. **A tap of BAR shows
the next bar**, as it does with the menu held open, and **Shift + BAR closes the menu**, the way
Shift + BAR opened it (in stock, BAR closes it and you can't change bars there). Both act when you let
go of BAR, and only if you pressed nothing else meanwhile, so BAR + Play and the other BAR shortcuts
never close the menu or change the bar.

### Play from the selected bar: Shift + Play

Stock ignores Shift + Play. With Bar tools it plays from the selected track's selected bar, for example
to hear bar 3 run into bar 4 without sitting through bars 1 and 2. Every track starts where it would
be had the song played from the top (see [How tracks line up](#how-tracks-line-up)).

- **Stopped:** starts from that bar's first step.
- **Playing:** goes to that bar, as the transition says: at once at the same place within the
  bar (the default), at once from its first step, or when the playing bar ends.
- **With a loop on or armed:** the loop ends, and playback goes to the selected bar as above.
- **Recording armed** (Record + Play): counts in exactly as Play does, then playback and recording
  start at that bar. A note played just before the count-in ends lands on the bar's first step, as it
  would on bar 1 (stock behavior).
- **Record, then Shift + Play** (press Record first: Shift + Record is stock's undo): arms recording
  as Record + Play does, with the step keys already on that bar. Then, as in stock, press Play to
  count in, or just play: your first note starts playback and recording at that bar.
- **During a count-in:** does nothing (Play there turns recording off and plays on, as in stock).

The metronome, tempo-synced LFOs and envelopes start as on any play; a 1-bar LFO lines up with the bar.

### Loop the selected bar: BAR + Shift + Play

Hold BAR and Shift and press Play. The selected track loops its selected bar, and every other track
loops the bar that lines up with it (see [How tracks line up](#how-tracks-line-up)).

- **Stopped:** playback starts in that bar and keeps repeating it ("bar loop on"). With recording
  armed, it counts in first, as Shift + Play does.
- **Playing:** the tracks go there as the transition says. At once (the default), they jump in
  time to the same place within the bar; if the selected bar is the playing bar, nothing jumps: the
  bar you're hearing just keeps repeating.
- **Looping another bar:** the loop moves to the selected bar, every track to the bar that lines up
  with it, the same way.
- **On the bar already looping:** nothing changes, except with the restart transition, which plays it
  again from its first step.

### Loop the bars playing, or end the loop: BAR + Play

- **Playing, nothing looping:** every track keeps repeating the bar it's playing now, whichever bar
  you're editing ("bar loop on"). With the restart transition, from the first step of the selected
  track's playing bar.
- **Stopped, nothing looping:** the selected bar is armed ("bar loop armed"). Press Play and playback
  starts in the looped bar, its steps shown for editing.
- **A loop on or armed:** it ends ("bar loop off"), at once whatever the transition. Playback carries on
  from the looped bar into the next one.

Plain Play still restarts a loop where it is, and stopping doesn't end it: Play starts it again.

### Keep working while it loops

A loop doesn't stop you using the rest of the pattern. Tap BAR to select other bars, see and edit
their notes and parameter locks, then come back; the loop keeps playing throughout. Edits in the
looped bar are heard from the next time the playhead reaches them. Record into the loop as usual.

### What the bar menu shows

In the bar boxes, the **looped bar is grey**, and while playing the **playing bar's number has a dot**
("3."). So while you edit bar 1 of a loop on bar 3, the menu shows bar 1 highlighted, bar 3 grey,
and "3." playing.

### Settings

Stock's settings (COM) get an **open-xy** page after "system" and "midi"; click knob 2 on "bar tools" to
open its page there (M1 or knob 1 goes back). Changes take effect at once
and are kept like stock's settings, for every project.

```
transition        instant / restart / next bar      (instant)
--
shift+play        play highlighted
bar+shift+play    play+loop highl.
bar+play          toggle loop
```

The rows after "--" are a reminder of the shortcuts; they can't be changed.

#### Transition

How Shift + Play, BAR + Shift + Play and BAR + Play take over while playing:

- **instant:** at once, at the same place within the bar, so the beat carries on.
- **restart:** at once, from the bar's first step, as Play restarts from the top.
- **next bar:** when the selected track's playing bar ends, from the new bar's first step. A popup says
  "loop at bar end" (or "loop off at bar end" for Shift + Play leaving a loop). Until then, another
  shortcut or Play cancels it. Looping the bars playing (BAR + Play) has
  nothing to wait for, so it happens at once.

Ending a loop with BAR + Play is always at once: playback simply carries on into the next bar. Stopped,
the setting doesn't matter.

## Tips

**Leave a loop at the bar you choose.** BAR + Play ends a loop and lets playback carry on into the next
bar. Shift + Play ends it somewhere else: say bar 2 loops and your fill is on bar 4. Show bar 4 and
press Shift + Play. With the transition on "next bar", bar 2 finishes its pass and bar 4 takes over
on the beat; on "instant", playback jumps there at once, at the same place within the bar.

**Prepare the next part while one loops.** Tap BAR to show another bar and change its notes and
parameter locks while the loop keeps playing. When it's ready, BAR + Shift + Play moves the loop
there. With "next bar", the switch waits for the bar playing to end.

**Start on a loop.** Stopped, BAR + Play arms the selected bar, and Play starts it looping. When the
intro has gone on long enough, BAR + Play lets the pattern run on from there.

**Work on a spot deep in a song.** To rehearse a transition without playing the song from the top: on
the song screen, Shift + Oct+ moves to a song entry (stock; it blinks). Go back to a track, show the
bar just before the change and press Shift + Play: playback starts at that bar of that entry, then runs
on into the next one. To keep working on it, loop the bar with BAR + Shift + Play instead; end the loop
with BAR + Play and the song carries on from there.

**Stutter a bar.** With the transition on "restart", BAR + Shift + Play on the bar already looping
plays it again from its first step. Press it in rhythm for a build-up.

**Keep the bar menu in view.** Press Shift, then BAR, and let go of both: the menu stays open, showing
the grey looped bar and the playing bar's dot as you play. Tap BAR to move to the next bar, or hold it
for the shortcuts; the menu stays open. Shift + BAR closes it.

**Record from a bar, with a count-in.** Arm recording (Record + Play), show the bar and press
Shift + Play: the count-in plays, then playback and recording start at that bar. A
note played just before the count-in ends lands on the bar's first step, as it would on bar 1.

**Loop-record one bar.** The same with BAR + Shift + Play: after the count-in, the bar loops while you
record, and every pass adds to it. End the loop with BAR + Play when it's done.

**Record from a bar, starting on your first note.** Press Record, then Shift + Play (Record first:
Shift + Record is stock's undo). Recording waits at the selected bar, its steps shown, and nothing plays
until you play a note: that note starts playback and recording from the bar's first step.

## How it works in detail

### How tracks line up

Tracks can have different lengths (1 to 4 bars) and step scales. Stock keeps them in step by playing
them all from the top together, and Bar tools uses the same rule: when you pick a bar on the selected
track, every other track takes the bar it would be playing at that moment had the song played from
the top.

Say track 1 has 4 bars, track 2 has 2 and track 3 has 3. Played from the top, they go:

```
song time  │ bar 1 │ bar 2 │ bar 3 │ bar 4 │
───────────┼───────┼───────┼───────┼───────┤
track 1    │   1   │   2   │ [ 3 ] │   4   │   4 bars
track 2    │   1   │   2   │ [ 1 ] │   2   │   2 bars: starts over after its bar 2
track 3    │   1   │   2   │ [ 3 ] │   1   │   3 bars: starts over after its bar 3
```

Loop bar 3 of track 1, and each track loops the bar in its column: track 2 its bar 1, track 3 its
bar 3. Loop bar 4 instead, and they'd loop bar 2 and bar 1.

A track on a slower step scale has longer bars. On scale 2 (half speed) each of its bars lasts two
bars of song time:

```
song time         │ bar 1 │ bar 2 │ bar 3 │ bar 4 │
──────────────────┼───────┼───────┼───────┼───────┤
track 1           │   1   │   2   │ [ 3 ] │   4   │
track 4 (scale 2) │       1       │ [     2     ] │   2 bars, each twice as long
```

Looping bar 3 of track 1, track 4 loops its bar 2: track 1 repeats bar 3 twice for each pass of
track 4's bar 2.

Looping the bars playing (BAR + Play) doesn't use this: each track keeps the bar it's playing.

### Changing lengths and step scales while looping

Add or remove bars, or change a track's step scale, while a loop is on or armed, and the tracks line
up again with the bar the selected track loops, as if you had made the change first. The selected
track is the one selected when the loop started; it keeps its bar.

For example, looping bar 3 of track 1, remove a bar from track 3 (3 bars down to 2):

```
song time         │ bar 1 │ bar 2 │ bar 3 │ bar 4 │
──────────────────┼───────┼───────┼───────┼───────┤
track 1           │   1   │   2   │ [ 3 ] │   4   │
track 3, 3 bars   │   1   │   2   │ [ 3 ] │   1   │   before: loops its bar 3
track 3, 2 bars   │   1   │   2   │ [ 1 ] │   2   │   after: moves to its bar 1
```

Track 3 moves to its bar 1, in time, and keeps looping. Track 1 keeps its bar 3. Removing the selected
track's own looped bar ends the loop.

### Scenes and songs

A song (one with scenes) stays in its current scene while bars loop, however long you loop. When the
loop ends, the song moves on when the tracks reach the end of the scene, counted from where the tracks
are rather than from the time spent looping. In a 4-bar scene:

- Loop bar 3 and end the loop there: you hear bar 4, then the next scene.
- Loop bar 3, move the loop to bar 1 and end it there: you hear bars 2, 3 and 4, then the next scene.

An empty song just repeats the current scene, as in stock.

### What ends a loop

- BAR + Play.
- Shift + Play, which plays on from the selected bar without the loop.
- A looped track playing another pattern: changing its pattern by hand, or a scene change that does
  (a scene change that leaves every looped track on the same pattern keeps the loop).
- Selecting another song. As in stock, the current scene then plays to its end before the new song
  starts; press Play to start it at once.
- Loading or creating a project. Loops aren't saved with the project.
- Removing the selected track's looped bar.

Selecting other bars, editing, recording, muting, undo and tempo changes don't end it.

### Recording

Live recording into a loop works as normal: notes land in the looped bar and play from the next pass.
A note held across the loop point wraps into the start of the bar. Stock stores a note as a start and
a length, so once the loop ends that note plays its full length on into the next bar.

### Step components and parameter locks

Go-to step components ("go to step 5", and the like) stay inside the looped bar: step 5 means step 5
of that bar. Components on other bars don't fire while they aren't playing. A go-to on one track
shortens that track's pass, so it drifts out of step with the others, as it does without a loop; the
scene starting over brings them back.

Parameter locks work as usual in the looped bar: each pass plays them just as the pattern does when it
loops, so every lock is heard on its step, every pass. Locks on other bars wait until those bars play
again.

### MIDI

MIDI clock out keeps running through loops and Shift + Play starts. Bar tools sends no song position,
so another device's own sequence starts from its beginning. Following an external clock, loops work
as on the OP-XY's own clock, in time with the other device. Shift + Play and BAR + Shift + Play start
from the selected bar when the other device is stopped. While it runs, the instant and next bar
transitions keep the two in step; restart stays in tempo but lands wherever you press it, so the two
sequences fall out of step.

## Limits

- A note held across the loop point while recording runs on into the next bar once the loop ends
  (above).
- Waiting to record from a bar (Record + Play with a loop armed, or Record + Shift + Play), stock's red
  waiting box reads "1" until your first note. The note and playback land on the right bar, and the box
  is right once playback starts.

## Requirements

Bar tools needs the Open-XY SDK's `bar_menu` and `shift_play` claims, its `loop_bars`, `notify` and
`play_from` actions (with `notify_for` and `loop_at_bar_end`), its `sequencer_change` and
`setting` events, and its settings pages.

## Checks

On a device: [QA.md](QA.md), from top to bottom.

On the host: clone the SDK next to this repository, then from here:

```sh
python ../open-xy/sdk.py test --extension extensions/bar_tools
```

Build it into a firmware with `--extension extensions/bar_tools` beside `--machine` on the SDK's
`build` command.
