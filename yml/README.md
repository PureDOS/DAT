# DOS.YML
DOS.YML files describe hardware and software configuration for the DOS emulator to make running a game correctly as seamless as possible.

## Format
The format of the DOS.YML file is very simple, it is just pairs of `KEY: VALUE` followed by a line break.
Line breaks in DOS.YML files use the DOS format (CR + LF).

The following settings are supported, the default value is the first one listed in __bold__:

| Key             | Description                                      | Possible values                                             |
| --------------- |--------------------------------------------------|-------------------------------------------------------------|
| `cpu_type`      | Type of emulated CPU                             | __auto__ / generic_386 / generic_486 / generic_pentium      |
| `cpu_year`      | Emulate speed of CPU released that year          | Default unset, must be between 1980 and 2000                |
| `cpu_hz`        | Emulate speed of CPU with frequency              | Default unset, must be 500 or more                          |
| `cpu_cycles`    | Emulate speed of CPU by DOSBox cycles            | Default unset, must be 100 or more                          |
| `cpu_max_year`  | Emulate CPU as fast as possible with upper limit | Default unset, must be between 1980 and 2000                |
| `cpu_max_hz`    | Emulate CPU as fast as possible with upper limit | Default unset, must be 500 or more                          |
| `cpu_max_cycles`| Emulate CPU as fast as possible with upper limit | Default unset, must be 100 or more                          |
| `mem_size`      | Total RAM size in kilobytes                      | __16384__                                                   |
| `mem_xms`       | Whether to use XMS (Extended Memory)             | __true__ / false                                            |
| `mem_ems`       | Whether to use EMS (Expanded Memory)             | __true__ / false                                            |
| `mem_umb`       | Whether to use UMB (Upper Memory Blocks)         | __true__ / false                                            |
| `mem_doslimit`  | Whether to limit free conventional memory        | Default to all available, specified in kilobytes < 640      |
| `video_card`    | Type of video card/machine to emulate            | __generic_svga__ / generic_hercules / generic_cga / generic_ega / generic_vga / tandy / pcjr / svga_s3_trio / svga_tseng_et3000 / svga_tseng_et4000 / svga_paradise_pvga1a |
| `video_memory`  | Amount of video memory                           | __2048__ / 512 / 1024 / 4096                                |
| `video_voodoo`  | Type of Voodoo 3dfx expansion card               | __v1_8mb__ / v1_4mb / none                                  |
| `sound_card`    | Type of sound card to emulate                    | __sb16__ / sb1 / sb2 / sbpro1 / sbpro2 / gameblaster / none |
| `sound_port`    | The IO address of the sound card                 | __220__ / 240 / 260 / 280 / 2A0 / 2C0 / 2E0 / 300           |
| `sound_irq`     | The IRQ number of the sound card                 | __7__ / 5 / 3 / 9 / 10 / 11 / 12                            |
| `sound_dma`     | The DMA number of the sound card                 | __1__ / 5 / 3 / 6 / 7                                       |
| `sound_hdma`    | The High DMA number of the sound card            | __5__ / 6 / 7                                               |
| `sound_midi`    | If a MPU401 MIDI Processing Unit is present      | __true__ / false / name of DOSX with a soundfont            |
| `sound_mt32`    | If a MT-32 MIDI synthesizer module is present    | __false__ / true / name of DOSX with a MT-32 ROM (if set overrides `sound_midi`) |
| `sound_gus`     | If a Gravis UltraSound expansion card is present | __false__ / true                                            |
| `sound_tandy`   | If a Tandy Sound Device is present               | __false__ / true (always on with video_card: tandy)         |
| `input_directmouse`    | Emulator support for direct mouse cursor mode                 | __false__ / true                          |
| `input_mousespeed`     | Speed factor for all virtual mouse movement in percent        | __100__                                   |
| `input_mousexfactor`   | Speed factor for horizontal virtual mouse movement in percent | __100__                                   |
| `input_mousewheelup`   | Action to assign to rotating the mouse wheel up               | [Assignable Actions](#assignable-actions) |
| `input_mousewheeldown` | Action to assign to rotating the mouse wheel down             | [Assignable Actions](#assignable-actions) |
| `input_padmousespeed`  | Speed factor for mouse movement done by game pad in percent   | __100__                                   |
| `input_pad_*`          | Defines a mapping for binding controls to a modern gamepad    | [See below for details](#gamepad-mapping) |
| `input_wheel_*`        | Defines options for an action-wheel for additional inputs     | [See below for details](#action-wheel)    |
| `run_path`      | Default program to launch                        | Full path, i.e. C:\PROGRAM.EXE                              |
| `run_boot`      | Default disk image to boot (floppy or hard disk) | Path relative to DOSZ, i.e. DISKS\DISK1.IMG                 |
| `run_mount`     | Default disk image to mount (floppy or CD-ROM)   | Path relative to DOSZ, i.e. CD\GAME.CHD                     |
| `run_input`     | Input sequence performed at startup              | [See below for details](#run-input-sequence)                |
| `run_utility`   | Mark a configuration to not start automatically  | __false__ / true                                            |

## Run Input Sequence
An input sequence is composed of typed characters, pressed keyboard keys or special commands.

The sequence is processed in order, one command at a time.

Typed characters are performed as a key down, key up event to type the specific characters.
Examples for typing characters are `abc`, `ABC`, `123` or ``-=[];\`,./``.

Pressed keyboard keys are either performed as both down and up events, or just a single event when specifying `:DOWN` or `:UP`.
Examples for keyboard key presses are `(ENTER)`, `(SPACE)` or `(leftctrl:DOWN)c(leftctrl:UP)`.
When performing a single event (with DOWN or UP) no time is waited and the sequence continues immediately.
Otherwise the time between pressing and releasing a key is fixed at 70 milliseconds and the sequence continues after waiting a configurable delay time.

Special commands are WAIT, WAITMODECHANGE and DELAY.
WAIT will do nothing for a given number of milliseconds. An emulator should fast forward that wait time so only for the emulated software
the time will pass while in real time it should be as quick as possible. This can be used to skip loading times or wait for an input prompt.
WAITMODECHANGE will wait until the video mode changes. This can be used to skip the console at the very startup until the game changes
into a graphics mode. Like WAIT an emulator will fast forward this time as best as it can. Can be repeated to skip multiple mode changes.
DELAY by itself will not do anything but it sets how many milliseconds are waited after releasing a key until pressing the next key in the
sequence. The default delay time is 70 milliseconds. Example `(WAITMODECHANGE)(WAIT:500)(DELAY:15)i5j`

List of keyboard key names: `1`,`2`,`3`,`4`,`5`,`6`,`7`,`8`,`9`,`0`,`q`,`w`,`e`,`r`,`t`,`y`,`u`,`i`,`o`,`p`,`a`,`s`,`d`,`f`,`g`,`h`,`j`,`k`,`l`,`z`,`x`,`c`,`v`,`b`,`n`,`m`,`F1`,`F2`,`F3`,`F4`,`F5`,`F6`,`F7`,`F8`,`F9`,`F10`,`F11`,`F12`,`ESC`,`TAB`,`BACKSPACE`,`ENTER`,`SPACE`,`LEFTALT`,`RIGHTALT`,`LEFTCTRL`,`RIGHTCTRL`,`LEFTSHIFT`,`RIGHTSHIFT`,`CAPSLOCK`,`SCROLLLOCK`,`NUMLOCK`,`GRAVE`,`MINUS`,`EQUALS`,`BACKSLASH`,`LEFTBRACKET`,`RIGHTBRACKET`,`SEMICOLON`,`QUOTE`,`PERIOD`,`COMMA`,`SLASH`,`EXTRA_LT_GT`,`PRINTSCREEN`,`PAUSE`,`INSERT`,`HOME`,`PAGEUP`,`DELETE`,`END`,`PAGEDOWN`,`LEFT`,`UP`,`DOWN`,`RIGHT`,`KP1`,`KP2`,`KP3`,`KP4`,`KP5`,`KP6`,`KP7`,`KP8`,`KP9`,`KP0`,`KPDIVIDE`,`KPMULTIPLY`,`KPMINUS`,`KPPLUS`,`KPENTER`,`KPPERIOD`

## Gamepad Mapping
DOS.YML can define a mapping between DOS inputs (keyboard, joystick, mouse) and a modern gamepad.

The modern gamepad is defined to have a d-pad (up/down/left/right), 4 face buttons (B in the bottom, A on the right, X at the top and Y on the left),
4 shoulder buttons (L1, R1, L2, R2), 2 analog sticks that also act as buttons (L3, R3), a Select and a Start button.
The emulator will then map any actual gamepad to physically match that layout. Therefore X doesn't always mean the X button on the actual gamepad,
it will always mean the face button physically at the top (so controls will be the same regardless of button naming).

Up to 4 DOS inputs can be mapped to one gamepad input. For example, one gamepad button can be made to press 2 keyboard keys at the same time.
Optionally a name can be defined for each mapping, which will be shown in an emulators menu as a reference for players.

Example mapping:
```yml
input_pad_up: up Move Up
input_pad_x: space Jump
input_pad_start: leftctrl+f10 Open Menu
input_pad_l: wheel
```
This will map the d-pad up direction to pressing the up arrow keyboard key, the X button to the spacebar key, the Start button to pressing both
Left Ctrl and F10 simultaneously and the left shoulder button to showing the [action wheel](#action-wheel).

Full list of mappable gamepad inputs: `input_pad_up`, `input_pad_down`, `input_pad_left`, `input_pad_right`, `input_pad_b`, `input_pad_a`, `input_pad_x`, `input_pad_y`, `input_pad_l`, `input_pad_r`, `input_pad_l2`, `input_pad_r2`, `input_pad_l3`, `input_pad_r3`, `input_pad_select`, `input_pad_start`, `input_pad_lstick_left`, `input_pad_lstick_right`, `input_pad_lstick_up`, `input_pad_lstick_down`, `input_pad_rstick_left`, `input_pad_rstick_right`, `input_pad_rstick_up`, `input_pad_rstick_down`

## Action Wheel
An emulator can support what is called an action wheel which can offer additional inputs beyond the 24 gamepad inputs.
The action wheel will represent all defined options in a radial menu.
This is similar to modern games which sometimes feature a weapon wheel to simplify weapon selection with a gamepad.

Example mapping:
```yml
input_wheel_1: 1 Fists
input_wheel_2: 2 Pistol
input_wheel_3: 3 Machine Gun
```
This will create a wheel with 3 options named Fists, Pistol and Machine Gun which when activated will press the 1, 2 or 3 number key on the emulated DOS keyboard.

Then defining an action wheel, the [gamepad mapping](#gamepad-mapping) must define an input bound to the `wheel` action.
Generally it is recommended to assign the left shoulder button to show the action wheel.

## Assignable Actions
These actions can be assigned to the [gamepad mapping](#gamepad-mapping) and [action wheel](#action-wheel).

| Device   | Actions |
|----------|---------|
| Keyboard | `1`,`2`,`3`,`4`,`5`,`6`,`7`,`8`,`9`,`0`,`q`,`w`,`e`,`r`,`t`,`y`,`u`,`i`,`o`,`p`,`a`,`s`,`d`,`f`,`g`,`h`,`j`,`k`,`l`,`z`,`x`,`c`,`v`,`b`,`n`,`m`,`F1`,`F2`,`F3`,`F4`,`F5`,`F6`,`F7`,`F8`,`F9`,`F10`,`F11`,`F12`,`ESC`,`TAB`,`BACKSPACE`,`ENTER`,`SPACE`,`LEFTALT`,`RIGHTALT`,`LEFTCTRL`,`RIGHTCTRL`,`LEFTSHIFT`,`RIGHTSHIFT`,`CAPSLOCK`,`SCROLLLOCK`,`NUMLOCK`,`GRAVE`,`MINUS`,`EQUALS`,`BACKSLASH`,`LEFTBRACKET`,`RIGHTBRACKET`,`SEMICOLON`,`QUOTE`,`PERIOD`,`COMMA`,`SLASH`,`EXTRA_LT_GT`,`PRINTSCREEN`,`PAUSE`,`INSERT`,`HOME`,`PAGEUP`,`DELETE`,`END`,`PAGEDOWN`,`LEFT`,`UP`,`DOWN`,`RIGHT`,`KP1`,`KP2`,`KP3`,`KP4`,`KP5`,`KP6`,`KP7`,`KP8`,`KP9`,`KP0`,`KPDIVIDE`,`KPMULTIPLY`,`KPMINUS`,`KPPLUS`,`KPENTER`,`KPPERIOD` |
| Mouse    | `mouse_move_up`,`mouse_move_down`,`mouse_move_left`,`mouse_move_right`,`mouse_left_click`,`mouse_right_click`,`mouse_middle_click`,`mouse_speed_up`,`mouse_speed_down` |
| Joystick | `joy_up`,`joy_down`,`joy_left`,`joy_right`,`joy_button1`,`joy_button2`,`joy_button3`,`joy_button4`,`joy_hat_up`,`joy_hat_down`,`joy_hat_left`,`joy_hat_right`,`joy_2_up`,`joy_2_down`,`joy_2_left`,`joy_2_right` |
| Special  | `wheel` (show [Action Wheel](#action-wheel)),`none` (clear assignment) |

## CPU Speeds
| CPU Year | Comparable CPU Model  | Comparable DOSBox Cycles |
|----------|-----------------------|--------------------------|
| 1981     | 8088 @ 4.77 MHz       | 315                      |
| 1982     | 8088 @ 8 MHz          | 900                      |
| 1983     | 286 @ 10 MHz          | 1500                     |
| 1984     | 286 @ 12 MHz          | 2100                     |
| 1985     | 386 @ 16 Mhz          | 2750                     |
| 1986     | 386 @ 20 MHz          | 3800                     |
| 1987     | 386 @ 33 MHz          | 4800                     |
| 1988     | 386DX @ 33 MHz        | 6300                     |
| 1989     | 486SX @ 25 MHz        | 7800                     |
| 1990     | 486DX @ 33 MHz        | 14000                    |
| 1991     | 486DX @ 50 MHz        | 23800                    |
| 1992     | 486DX @ 66 MHz        | 27000                    |
| 1993     | Pentium @ 60 MHz      | 44000                    |
| 1994     | Pentium @ 75 MHz      | 55000                    |
| 1995     | Pentium @ 100 MHz     | 66800                    |
| 1996     | Pentium @ 166 MHz     | 93000                    |
| 1997     | Pentium @ 200 MHz     | 125000                   |
| 1998     | Pentium II @ 300 MHz  | 200000                   |
| 1999     | Pentium II @ 450 MHz  | 350000                   |
| 2000     | Pentium III @ 600 MHz | 500000                   |
