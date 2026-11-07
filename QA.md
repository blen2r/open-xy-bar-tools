# Testing Bar tools on an OP-XY

Checks to run on a device with a firmware that includes Bar tools, from top to bottom, in one project:
each section leaves the project ready for the next. To run only some sections, set up what their first
line says. Host tests (`python ../open-xy/sdk.py test --extension extensions/bar_tools`) don't replace
these: they can't hear the timing at the loop point or see the screen.

Report a failure with the section, the line and what you saw or heard. A photo of the screen helps.

**Firmware.** Bar tools needs the SDK's `bar_menu_m3` and `shift_play` claims, its `loop_bars` and
`play_from` actions and its `key_label` and `sequencer_change` events (branch `sdk/bar-loop` of the SDK
until they are merged). Build it with `sdk.py build ... --extension extensions/bar_tools`. Last full
pass: OS 1.2.1, SDK `sdk/bar-loop` at a4c6d1f, 2026-11-04.

AI agents working on the extension keep this file up to date as they go: each change that adds, removes
or alters something a person would check on the device updates its lines here.

**Words used here.** The *bar menu* is what shows while BAR (the right column's 4th key) is held: M1
"clr notes", M2 "clr params", M3 Bar tools' label, M4 "clr all". *Picking a bar* is tapping BAR, which
shows the next bar's steps. A track's *shown bar* is the one its step keys show and edit.

## Setup

- Install the firmware. Create a new project, **plain**, give track 1 a sound and a note, and save it.
- Create another new project, **qa**: don't use a project you care about.
- Track 1, 4 bars, a different pitch on step 1 of each bar (C, D, E, F) and a short note on step 16 of
  each bar (a higher pitch), so you can hear which bar plays and the end of each bar.
- Track 2, 2 bars: one note on step 1 of each bar, two different pitches (G, A).
- Track 3, 1 bar: a drum hit on steps 1, 5, 9 and 13.
- Track 4, 3 bars: one note on step 1 of each bar (three pitches).
- MIDI clock (only if you have another MIDI device; skip the clock lines otherwise): a device that
  follows MIDI clock and start/stop, such as a DAW, a sequencer or a synth with a clock-synced
  arpeggiator, connected to the OP-XY's MIDI out (and, for the external-clock line, sending clock to
  its MIDI in). No notes are needed. Leave the OP-XY on its own clock until that line.
- Scenes and songs, for the song lines. (The OP-XY has no song mode switch: a song plays
  through its scenes when it has any, and an empty song just repeats the current scene.) **Scene 1** is what you just made: pattern 1 on
  tracks 1 to 4 (4 bars long, its longest track). **Scene 2**: track 1 on pattern 2, 1 bar, a high C on
  steps 1, 5, 9 and 13 and nothing else; tracks 2 to 4 on their empty pattern 2. So scene 2 is just
  four high Cs, easy to tell from scene 1. **Song 1**: scene 1, then scene 2. **Song 2**: scene 2 only.
  **Song 3**: empty (no scenes). **Song 4**: scene 1, 2, 2, 1, 2. Select song 3 and scene 1 until a line
  asks for another song.
- Select track 1. Press play, and leave it playing unless a step says otherwise.

## Firmware basics

- [ ] The OP-XY boots, and the sequencer, stock engines and project save/load work
- [ ] Hold BAR (playing): the bar menu shows M1 "clr notes", M2 "clr params", M4 "clr all" as before,
      and M3 "loop selected" in the same font, row and spacing, not cut off or overlapping M2/M4
- [ ] Playing, hold BAR and Shift: M3 says "loop playing"; stopped, with Shift or not: "loop selected"
- [ ] In the bar menu, "+" and "-" still add and remove bars, and M1, M2 and M4 still ask and clear
- [ ] M3 outside the bar menu (on other pages) does what stock does there
- [ ] With nothing looping, the bar menu's bar boxes look as in stock (no grey box)
- [ ] Stopped, the bar menu's numbers read 1, 2, 3, 4 as in stock (only as many as the track has bars)
- [ ] Playing (no loop), hold BAR: the number of the bar playing has a dot ("3.") and the dot moves on as
      playback crosses into the next bar while you hold BAR. If it only moves when you reopen the menu,
      note it. Select another track: the dot follows that track's playing bar. Stop: the dot goes
- [ ] The dot fits: "3." in the same font, not cut off, the number still centred enough to read

## Looping while playing

- [ ] During track 1's bar 3 (E): hold BAR, press M3. A popup says "bar loop on" and M3 changes to
      "end loop" straight away, while the menu is still open. Track 1 repeats bar 3; releasing BAR
      doesn't stop it
- [ ] The other tracks repeat the bar they were in: track 2 its bar 1 (G), track 3 its only bar, track
      4 its bar 3
- [ ] At the loop point, over at least 8 passes: the bar's step 1 note plays every pass, once (not
      doubled, not missed, not late), step 16's note plays before it, no note hangs
- [ ] The step keys show track 1's bar 3 and the playhead runs over it every pass
- [ ] Select track 2: its step keys show its bar 1 and its playhead stays there
- [ ] Hold BAR: track 1's bar 3 box is grey (a dimmer shade of the other boxes), and still shows that
      it is the shown bar. It is clear which bar loops
- [ ] Hold BAR, press M3: "bar loop off", M3 back to "loop selected", the grey box back to normal. Playback
      carries on into the next bar (F) and the pattern plays through as normal

## Other bars while looping

Start with track 1's bar 3 looping, playing.

- [ ] Tap BAR (pick a bar): track 1 shows bar 4, the loop keeps playing bar 3, no popup. The step keys
      show bar 4's steps without the running light
- [ ] Hold BAR: bar 4 shows as the shown bar, bar 3 is grey and its number has the dot ("3."): you can
      see which bar loops and which is playing while you edit another; M3 still says "end loop"
- [ ] Pick bars round to bar 3 again: its steps and the running light are back, the loop never broke
- [ ] On bar 1 (shown, not looping): add a note, change one, add a p-lock. None plays while bar 3 loops;
      all of them play once the loop ends
- [ ] Select track 2 (looping its bar 1) and pick its bar 2: same, its loop holds
- [ ] Hold BAR and Shift together: M3 changes to "loop selected" at once, and back to "end loop" when you let
      go of Shift (BAR still held). Order doesn't matter (Shift then BAR, BAR then Shift). Shift first
      keeps the bar menu on screen after BAR is released (stock); the label and M3 work the same there
- [ ] Show track 1's bar 1, hold Shift + BAR, press M3: "bar loop on", the loop moves to bar 1 straight
      away, in time (same place within the bar, no gap, no double note); the grey box moves to bar 1;
      track 2 moves to its bar 1, track 4 to its bar 1
- [ ] Show bar 2 and do it again: everything moves to bar 2 (track 4 its bar 2, track 2 its bar 2)
- [ ] Nothing looping, playing, showing a bar other than the playing one: BAR + M3 ("loop selected")
      loops the shown bar (every track jumps there, in time); Shift + BAR + M3 ("loop playing") loops
      the bars playing
- [ ] Stopped, armed on bar 3: show bar 4, Shift + BAR + M3: "bar loop armed" on bar 4; Play starts at
      bar 4. Stopped, plain BAR + M3 while armed: ends it
- [ ] Shift + BAR on their own (no M3) do what stock does with them, if anything; nothing freezes

## Looping from stop

- [ ] Stop. Pick a bar until track 1 shows bar 4. Hold BAR, press M3: "bar loop armed", M3 shows "end
      loop"
- [ ] Press play: track 1 starts at bar 4 (F) and repeats it; its step keys show bar 4 from the first
      pass
- [ ] Track 2 repeats its bar 2 (A), track 3 its bar, track 4 its bar 1 (bar 4 of a 4-bar track lines
      up with bar 1 of a 3-bar track)
- [ ] Stop, end the loop (BAR + M3), show track 1's bar 3, arm, play: track 2 its bar 1 (G), track 4
      its bar 3
- [ ] Select track 2, show its bar 2, arm, play: track 2 plays bar 2 (A); track 1 lines up with it
      (bar 2, D); track 4 bar 2
- [ ] Armed, then stop again without playing: still armed (M3 shows "end loop"); BAR + M3 ends it
- [ ] Looping, press stop, then play: playback restarts in the looped bars (stopping doesn't end the
      loop)

End the loop.

## Shift + Play

Start each line stopped, with no loop armed.

- [ ] Show track 1's bar 3, press Shift + Play: track 1 starts at bar 3 (E), then plays bar 4 (F) and
      carries on to bar 1, 2... as normal. Its step keys show bar 3, then follow the playhead
- [ ] The other tracks start where they would be if the song had played from the top: track 2 its bar 1
      (G), track 3 its bar, track 4 its bar 3; over two full passes all of them stay in time
- [ ] Bar 3's step 1 note plays at the start (not skipped), with nothing doubled
- [ ] Bar 1 shown: Shift + Play starts like Play
- [ ] Select track 4 (3 bars), show its bar 2: it starts there; the 4-bar track 1 starts on its bar 2 (D)
- [ ] Another step scale: set track 4 to scale 2 (half speed: each of its bars lasts two bars of track
      1). Select track 4, show its bar 2, press Shift + Play. Track 4 starts on its bar 2's note. Its
      bar 2 begins two bars into the song, so track 1 starts on bar 3 (E), track 2 on its bar 1 (G),
      track 3 on its step 1. Over two full passes of track 1, all of them stay in time. Set track 4
      back to its normal scale
- [ ] Plain Play still starts from the top, and while playing still restarts from the top (as stock)
- [ ] Playing anywhere, show track 1's bar 3, press Shift + Play: playback restarts at once from bar 3
      (E), every track where it lines up (track 2 its bar 1, track 4 its bar 3), as cleanly as Play's
      own restart: no stuck notes, nothing doubled
- [ ] Playing with a loop on (track 1's bar 3): show bar 1, Shift + Play: the loop moves to bar 1 and
      plays it from its first step; every track loops the bar lining up with bar 1 (track 4 its bar
      1). Hold BAR: bar 1 is grey
- [ ] Then plain Play: restarts the loop where it is (bar 1), it doesn't move
- [ ] Looping bar 1, with bar 1 shown: Shift + Play restarts bar 1, as Play does
- [ ] Looping bar 1, show bar 3, then change track 4's scale: tracks line up with bar 3 (the moved
      loop), not bar 1
- [ ] Count-in on (metronome settings), recording not armed: Shift + Play starts at once from the bar,
      as Play starts at once from the top
- [ ] Recording armed (Record + Play), count-in on: Play counts in, then starts from the top. Arm again,
      show bar 3, Shift + Play: the same count-in (same length, same clicks), then playback and
      recording start at bar 3 (E), every track lined up; a note played during bar 3 records into bar 3
- [ ] The same from bar 4, playing a note just before the count-in ends: it records on step 1 of bar 4
      (as stock records it on step 1 of bar 1 from the top), not on bar 1. During the count-in the step
      keys already show bar 4
- [ ] Hold Record, then Shift, then press Play, with track 1's bar 3 shown (stopped, no loop): recording
      arms as with Record + Play and the step keys show bar 3 (stock's red waiting box still reads "1": a
      known limit). Play a note: playback and recording start at bar 3, the note on its step 1; tracks 2 and 4 line up (bar 1 (G), bar 3)
- [ ] The same, but press Play instead of a note: the count-in, then bar 3, as Shift + Play with
      recording armed
- [ ] Record + Shift + Play, then disarm (as stock disarms), then plain Play: starts from the top
- [ ] Shift then Record (undo, as stock): no arming, no start
- [ ] Shift + Play with recording armed, then Stop during the count-in, then plain Play (armed again):
      counts in and starts from the top, not from bar 3
- [ ] During a count-in (started with Play or Shift + Play), Shift + Play does nothing. (Plain Play
      there turns recording off and plays on, as stock)
- [ ] Recording armed, a loop armed on bar 3, bar 3 shown, Shift + Play: counts in, then loops bar 3
- [ ] Recording armed, a loop armed on bar 3, show bar 2, Shift + Play: counts in, then loops bar 2
- [ ] A loop armed on bar 3, show bar 2, Record + Shift + Play, then a note: playback and recording
      start in bar 2, which loops; the note lands in bar 2
- [ ] The metronome clicks on the beat from the start, with the accent on the bar's first beat
- [ ] A tempo-synced LFO (e.g. 1 bar, a ramp) on a track: its phase follows the bar, from Play, Shift +
      Play, and with looping on or off: at the top of each bar's ramp, at the bottom by the bar's end
- [ ] Envelopes and note lengths. Set up track 5 (4 bars) with a synth sound that has a slow attack
      (about half a second) and a long release (about a second), and give it:
      - bar 2, step 13: a note 8 steps long (it runs into bar 3)
      - bar 3, steps 1 to 4: four 1-step notes (different pitches)
      - bar 3, step 16: a note 4 steps long (it runs into bar 4)
      Listen to bars 3 and 4 from Play first, then from Shift + Play with bar 3 shown (track 5 selected
      or track 1, either way bar 3), and compare:
- [ ] Bar 3's step 1 note swells in with the slow attack, as from Play; steps 2 to 4 are each as short
      as from Play, none cut early or held on
- [ ] The step 16 note runs its 4 steps into bar 4 and then fades with the release, as from Play
- [ ] Bar 2's long note, which started before the start point, doesn't sound from Shift + Play (it
      did from Play). Nothing hangs, and nothing sounds that isn't on bar 3 or 4
- [ ] While bar 3's step 16 note is held, press Shift + Play again (a restart): the held note releases
      normally (no stuck note), and the restart plays bar 3 as before
- [ ] Stop during the step 16 note: it releases as it does when you stop during Play
- [ ] Song 1 (scene 1 then scene 2) selected, stopped in scene 1. Show track 1's bar 3, Shift +
      Play: bar 3 (E) and bar 4 (F) of scene 1, then scene 2's four high Cs right after bar 4, in time
      (no gap, no extra bar), then the song carries on as it does from Play. Compare with Play from the
      top: the move to scene 2 comes at the same point in bar 4
- [ ] Clock out (see Setup; the OP-XY sending clock and start/stop in its MIDI settings): Play, then
      Stop, then Shift + Play from bar 3: each time the other device starts with the OP-XY and stays
      in tempo, and stops with it. Its own sequence starts from its beginning (expected: stock sends no
      song position). Shift + Play while playing (a restart): note what the other device does, compared
      with Play's restart from the top
- [ ] Following a running external clock (see Setup): loop track 1's bar 3 for a few passes, move it,
      end it: it all behaves as on the OP-XY's own clock, in time with the other device
- [ ] Clock out while looping (track 1's bar 3): the other device stays in tempo across the loop point,
      over several passes
- [ ] External clock (see Setup). Set the OP-XY to follow incoming MIDI clock and start/stop. Start the
      other device: the OP-XY plays from the top as in stock. Stop it. Then, with the other device
      stopped, show bar 3 and press Shift + Play on the OP-XY: it plays from bar 3 as without the
      other device. Then with the other device running (it started the OP-XY from bar 1): Shift + Play
      from bar 3 restarts there at once, the tempo stays in sync, but the restart lands wherever you
      pressed, so the two sequences are no longer aligned. Set the OP-XY back to its
      own clock
- [ ] Loop armed on track 1's bar 3 (stopped, BAR + M3), bar 3 shown, then Shift + Play: plays bar 3
      and keeps repeating it, as Play would with the loop armed
- [ ] Loop armed on track 1's bar 3, show bar 4, Shift + Play: the loop moves, plays bar 4 and keeps
      repeating it
- [ ] Shift + Play, stop, Play: the second start is from the top

## Editing in the looped bar

- [ ] Loop track 1's bar 3. Add a note on step 9 of the shown bar: it plays in the loop, in this pass if
      the playhead hasn't reached step 9 yet, otherwise on the next
- [ ] Remove it: gone from the same point (this pass if before step 9, otherwise the next)
- [ ] Song 3 (empty) selected. Record live into the loop (hold Record, play a note): the loop holds, the
      note lands in bar 3 and plays on the next pass
- [ ] The same with song 1 selected (scene 1 then scene 2): the loop holds over several passes, the song
      stays in scene 1 (no high Cs), the note lands in bar 3
- [ ] Record live into the loop and hold a note across the loop point (press it on bar 3's step 13 or so,
      keep holding past step 16, release in the next pass): the note wraps, running on into the start
      of the same bar up to where you released it, with no second note and nothing hanging or freezing
     . Stock stores only the note's start and length, so once the loop ends the same note
      runs on into bar 4 instead (a known limit). Stock
      can't make a note longer than the bar by editing, so this is the only way to reach the loop point
      with a held note. Do it with song 3, then once with song 1
- [ ] Wait-for-note recording (stock's Record + Play, then the first note starts the sequencer; count-in
      off), stopped with a loop armed on track 1's bar 3: Record + Play moves to step 1 of bar 3 (the step
      keys show bar 3; stock's red waiting box still reads "1", a known limit), and the first note starts playback there and records on bar 3's step 1, not bar 1.
      The loop then holds. Also with the loop on while playing: Record + Play behaves as stock there
- [ ] Undo after a looped edit: the edit is undone, the loop holds
- [ ] Last, with song 1 selected: end the loop (BAR + M3). The song carries on: bar 4 (F), then scene 2's
      high Cs right after it, in time, with what you recorded in bar 3 playing on later passes. With
      song 3 (empty), ending the loop just plays on through the pattern

## Parameter locks

- [ ] Lock a sound control on step 5 of the looped bar (hold the step, turn a knob): it plays on step 5
      every pass, and the page shows the lock while the step is held
- [ ] After step 5, the control goes back to the track's own value at the start of each pass (the next
      step 1 plays with the track's value, not the lock), as a 1-bar pattern does
- [ ] A lock on step 1: it plays on every pass's first step, from the first pass
- [ ] Locks on another bar (bar 1) never play while bar 3 loops; they play again once the loop ends
- [ ] Clear the locks (M2 "clr params" in the bar menu): the control plays its own value on the next
      pass. Clearing doesn't end the loop

## Step components

Put components on track 1's bar 3 while looping it. Each should stay inside the looped bar.

- [ ] A go-to on step 9 ("go to step 5" or similar): the playhead jumps to step 5 of bar 3, not of bar 1
- [ ] Go-to step 1, 9 and 13 (each in turn): step 1, 9 or 13 of bar 3
- [ ] A go-to on bar 3's last step (16): it jumps where it says, in bar 3, rather than looping back to
      step 1
- [ ] Components on bar 1's steps never fire while bar 3 loops; they work again once the loop ends
- [ ] With a go-to that skips step 1, the bar's step-1 note doesn't play on those passes, and nothing
      double-plays
- [ ] A go-to on one track only (e.g. "go to step 5" on track 1's step 11; it jumps once step 11 has
      played) shortens that track's pass, so over several passes it drifts out of step with the other
      tracks. That's expected, as it is without a loop. After the loop ends the tracks stay out of step
      until the scene starts over (with song 1, when the song changes scene), which brings them back in
      step
- [ ] Remove the components

## What ends the loop

Each of these should end the loop, with no popup: M3 shows "loop selected" the next time you hold BAR, and
playback carries on as stock would. Start each one with track 1's bar 3 looping.

- [ ] Changing track 1's pattern by hand (to a second pattern): the loop ends
- [ ] Changing another looped track's pattern (track 2): the loop ends for every track
- [ ] Changing to scene 2 by hand (track 1 plays another pattern there): the loop ends. A scene whose
      looped tracks keep the same patterns would keep the loop
- [ ] Loading a project (**plain**, then **qa** again) while playing: the loop ends, nothing freezes
- [ ] Removing the selected track's looped bar (track 1 loops bar 4, hold BAR, "-"): the loop ends for
      every track, no freeze

## Other settings while looping

- [ ] Change the tempo while looping: the loop holds, the loop point stays in time
- [ ] Adding a bar ("+") to track 4 while looping track 1's bar 4 (select track 4, hold BAR, "+"): no
      freeze; the loop holds and track 4 (now 4 bars) moves to its new bar 4, lining up with track 1
- [ ] Removing a bar from track 4 ("-", back to 3 bars) while track 1 loops bar 4: track 4 moves to
      its bar 1 (bar 4 of song time lines up with bar 1 of 3), the loop holds
- [ ] Loop track 1's bar 3, then remove track 4's bar 3 ("-", 2 bars left: the bar it loops is gone):
      the loop holds, track 4 moves to its bar 1 (bar 3 lines up with bar 1 of 2), others unchanged
- [ ] Select track 1 again. Add a bar to it while it loops bar 2 (track 1 selected when the loop was
      made): track 1 keeps bar 2, nothing else moves; remove a bar it doesn't loop (its bar 4): same
- [ ] Any of the bar changes above while armed (stopped), then play: every track starts on the bar that
      lines up with track 1's armed bar, as if the bars had been set first
- [ ] Step scale set first: track 4 on scale 2 (half speed), stopped, select track 1, show bar 3, BAR +
      M3 (armed), play: track 1 plays bar 3 (E), track 4 its bar 2, track 2 its bar 1 (G), all in time
      over several passes. Stop, end the loop, set track 4 back to scale 1
- [ ] The same, but set track 4 to scale 2 after arming: the tracks line up again with track 1's bar, so
      play gives the same bars as above (track 4 its bar 2, not its bar 3)
- [ ] The same while playing: loop track 1's bar 3, set track 4 to scale 2: track 4 moves to its bar 2
      straight away, in time, no stuck or doubled notes; set it back: it returns to its bar 3
- [ ] The selected track's own scale changing (track 1 to scale 2 while looping its bar 3): track 1
      keeps bar 3, now at half speed; its bar 3 begins four normal bars in, so track 2 moves to its bar
      1 (G) and track 4 to its bar 2. Set track 1 back: track 4 returns to its bar 3
- [ ] Mute a looped track and unmute it: the loop holds
- [ ] An aux track with a pattern (MIDI or CV), optional: looping holds it in its bar too
- [ ] The metronome while looping: no change to the loop

## Songs

Song 1 is scene 1 then scene 2; song 3 is empty (see Setup). Recording into a loop with a song is in
"Editing in the looped bar".

- [ ] Song 1 selected, playing, loop track 1's bar 3 in scene 1: the song stays in scene 1 for as long
      as the loop is on, several passes past where scene 2 would have come (no high Cs)
- [ ] Song 1 selected, playing, no loop: BAR + M3 on bar 2, then show bar 4 and Shift + BAR + M3 (move
      the loop to the scene's last bar): the loop holds on bar 4 over several passes, the song stays in
      scene 1. The move keeps the place within the
      bar, by design. Also loop bar 4 directly, and loop the only bar of a 1-bar scene
- [ ] Song 1 selected, looping bar 3: show bar 1, Shift + BAR + M3 (move the loop to bar 1), then end it:
      bars 2, 3, 4 play, then scene 2, right after bar 4 (the song follows the tracks, not the time spent
      looping)
- [ ] A song entry other than the first. Select song 4 (scene 1, 2, 2, 1, 2). On the song screen, Shift +
      Oct+ moves to an entry: go to the second "scene 1" (the 4th entry; it blinks). Go back to track 1
      and show bar 3, then Shift + Play: playback starts at bar 3 of that 4th entry (the song screen shows
      it playing there, not at the first entry), then plays the last entry (scene 2)
- [ ] Same setup, but loop bar 3 (BAR + M3) and press Play: the loop plays in that 4th entry; end the
      loop: the song carries on from there, to the 5th entry (scene 2)
- [ ] Song 3 (empty) selected, looping in scene 1: switching to song 1 ends the loop too (another song).
      Scene 1 then finishes, and song 1 starts from its first entry, which is scene 1 again, so scene 1
      plays once more before scene 2 (stock's own song start: it does the same without a loop)
- [ ] Song 1 selected, looping track 1's bar 3 in scene 1 for a while (longer than the 4-bar scene):
      switching to song 2 (scene 2) ends the loop at once (M3 back to "loop selected", the grey box
      gone). Scene 1 plays on from the looped bar to the end of the scene, bar 4 (F) only, then song
      2's high Cs, in time: stock finishes the current scene before switching songs, and the time
      spent looping doesn't count towards it. Play
      instead starts the new song at once, as in stock

## Saving and loading

- [ ] Save **qa** while looping, power off and on, load it: the project loads with the loop off (the
      loop is not saved), M3 "loop selected" (stopped), every note and lock as you left it
- [ ] Load **plain**: plays as stock; Bar tools' M3 label shows "loop selected"
- [ ] Create a new project: hold BAR, M3 shows "loop selected" (stopped); looping works in it

## Known limits

Not failures, but worth confirming they haven't changed.

- A note held across the loop point while recording wraps into the start of the looped bar, but stock
  stores it as one note with a length: after the loop ends it runs on into the next bar instead.
- A go-to on one track shortens its pass, so it drifts out of step with the others, as without a loop;
  the scene starting over brings them back in step.
- A scene change that leaves every looped track on the same pattern keeps the loop (only a change of
  what a looped track plays ends it).
- Switching songs ends the loop, and, as in stock, the current scene plays to its end before the new
  song starts (Play starts it at once).
- Shift + Play starts from the selected track's selected bar (pick it first), counts in when recording
  is armed (as Play), restarts at once while playing, and does nothing during a count-in. It sends no
  MIDI song position, so other gear's own sequences start from their beginning, and under a running
  external clock a restart falls out of step with them (a later idea: wait for the next bar line).
- Waiting to record from a bar (Record + Play with a loop armed, or Record + Shift + Play), stock's red
  waiting box reads "1" until the first note. The note and playback are on the right bar, and the box is
  right once playback starts.
