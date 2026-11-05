# Bar tools

An Open-XY firmware extension for the OP-XY (OS 1.2.1) for working one bar at a time, with everything
else about playback as stock:

- **Loop a bar:** hold BAR, press M3, and the bar you're on repeats until you press M3 again, while you
  keep editing any bar.
- **Play from a bar:** Shift + Play starts (or restarts) playback from the bar you're on instead of
  from the top, with the count-in when recording.
- **See which bar is playing:** the bar menu marks the playing bar with a dot and the looped bar in
  grey.

## What stock already has: the playing bar and the selected bar

A pattern on the OP-XY has up to 4 bars of 16 steps. Two of them matter here, and both are stock's:

- **The playing bar** is the bar the playhead is in. The running light on the step keys shows where
  it is.
- **The selected bar** is the bar whose steps the 16 step keys show and edit. Tap **BAR** to select
  the next bar. While the step keys follow the playhead, the selected bar moves with
  playback, so most of the time the two are the same. Select another bar and you can edit it while
  something else plays; its step keys then show no running light.

Hold **BAR** to open stock's bar menu. It shows the track's bars as boxes, the selected one
highlighted, with M1 "clr notes", M2 "clr params", M4 "clr all", and "+" and "-" to add or remove
bars. Stock leaves M3 unused there; Bar tools uses it.

## What Bar tools adds

### Loop the selected bar: BAR + M3

Hold BAR and press M3 ("loop selected"). The selected track loops its selected bar, and every other
track loops the bar that lines up with it (see [How tracks line up](#how-tracks-line-up)).

- **Playing:** the tracks go there straight away, in time, at the same place within the bar. If the
  selected bar is the playing bar, nothing jumps: the bar you're hearing just keeps repeating.
- **Stopped:** the loop is armed. Press Play and playback starts in the looped bar, its steps shown for
  editing.

A popup says "bar loop on" (playing) or "bar loop armed" (stopped).

### Loop the bars that are playing: Shift + BAR + M3

Playing with nothing looping, hold Shift too and M3 reads "loop playing": every track keeps repeating
the bar it's in, whatever bar the step keys show.

### Move or end a loop

- **End it:** BAR + M3 again ("end loop"). Playback carries on from the looped bar into the next one.
  Popup: "bar loop off".
- **Move it:** while looping, Shift + BAR + M3 ("loop selected") moves the loop to the selected bar:
  every track moves to the bar that lines up with it, at once and in time.

Shift works whichever you press first. Pressing Shift before BAR keeps the bar menu open after you let
go of BAR (stock), and M3 works the same there.

### Keep working while it loops

A loop doesn't stop you using the rest of the pattern. Tap BAR to select other bars, see and edit
their notes and parameter locks, then come back; the loop keeps playing throughout. Edits in the
looped bar are heard from the next time the playhead reaches them. Record into the loop as usual.

### What the bar menu shows

| M3 label | When | What M3 does |
| --- | --- | --- |
| loop selected | nothing looping | loops the selected bar |
| loop playing | nothing looping, playing, Shift held | loops the bars playing |
| end loop | looping or armed | ends the loop |
| loop selected | looping or armed, Shift held | moves the loop to the selected bar |

In the bar boxes, the **looped bar is grey**, and while playing the **playing bar's number has a dot**
("3."). So while you edit bar 1 of a loop on bar 3, the menu shows bar 1 highlighted, bar 3 grey,
and "3." playing.

### Play from the selected bar: Shift + Play

Stock ignores Shift + Play. With Bar tools it plays from the selected track's selected bar, for example
to hear bar 3 run into bar 4 without sitting through bars 1 and 2. Every track starts where it would
be had the song played from the top (see [How tracks line up](#how-tracks-line-up)).

- **Stopped:** starts from that bar.
- **Playing:** restarts from that bar at once, as Play restarts from the top.
- **Recording armed** (Record + Play): counts in exactly as Play does, then playback and recording
  start at that bar. A note played just before the count-in ends lands on the bar's first step, as it
  would on bar 1.
- **Record, then Shift + Play** (press Record first: Shift + Record is stock's undo): arms recording
  as Record + Play does, with the step keys already on that bar. Then, as in stock, press Play to
  count in, or just play: your first note starts playback and recording at that bar.
- **During a count-in:** does nothing (Play there turns recording off and plays on, as in stock).
- **With a loop armed:** the loop holds, as with Play.

The metronome, tempo-synced LFOs and envelopes start as on any play; a 1-bar LFO lines up with the bar.

## How it works in detail

### How tracks line up

Tracks can have different lengths (1 to 4 bars) and step scales. Stock keeps them in step with one
scene clock, and Bar tools uses the same rule: a bar of the selected track starts at some point of song
time, and every other track takes the bar it would be playing at that point had playback started from
the top. With track 1 on 4 bars, track 2 on 2 and track 4 on 3, bar 3 of track 1 lines up with bar 1
of track 2 and bar 3 of track 4; bar 4 of track 1 lines up with bar 2 and bar 1. A track on scale 2
(half speed) has bars twice as long, so bar 2 of that track begins two bars into the song.

Looping the playing bars (Shift) doesn't use this: each track keeps the bar it's in.

### Changing lengths and step scales while looping

Add or remove bars, or change a track's step scale, while a loop is on or armed, and the tracks line
up again with the bar the selected track loops, as if you had made the change first. The selected
track is the one selected when the loop started; it keeps its bar. If you remove a bar another track
was looping, that track moves to the bar that lines up instead. If you remove the selected track's
looped bar, the loop ends.

### Scenes and songs

A song (one with scenes) stays in its current scene while bars loop, however long you loop. When the
loop ends, the song moves on when the tracks reach the end of the scene, counted from where the tracks
are rather than from the time spent looping. So if you loop bar 3 of a 4-bar scene, move the loop to
bar 1 and end it there, you hear bars 2, 3 and 4, then the next scene.

An empty song just repeats the current scene, as in stock.

### What ends a loop

- BAR + M3 ("end loop").
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

Each pass restarts parameter locks as a pattern loop does: a lock doesn't carry from the end of the
bar into its next pass. Locks on other bars wait until those bars play again.

### MIDI

MIDI clock out keeps running through loops and Shift + Play starts. Bar tools sends no song position,
so another device's own sequence starts from its beginning. Following an external clock, Shift + Play
starts from the selected bar when the other device is stopped; while it runs, a restart stays in tempo
but lands wherever you press it, so the two sequences fall out of step.

## Limits

- Parameter locks reset on each pass, and a held note's length runs on after the loop ends (above).
- Shift + Play restarts at once wherever you press it; it doesn't wait for a bar line.
- External MIDI clock sync hasn't been tried with loops.
- The popups repeat what M3's label already shows.

## Requirements

Bar tools needs the Open-XY SDK's `bar_menu_m3` and `shift_play` claims, its `loop_bars` and
`play_from` actions, and its `key_label` and `sequencer_change` events.

## Checks

On a device: [QA.md](QA.md), from top to bottom.

On the host: clone the SDK next to this repository, then from here:

```sh
python ../open-xy/sdk.py test --extension extensions/bar_tools
```

Build it into a firmware with `--extension extensions/bar_tools` beside `--machine` on the SDK's
`build` command.
