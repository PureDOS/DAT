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
| `mem_size`      | Total RAM size in kilobytes                      | __16384__                                                   |
| `mem_xms`       | Whether to use XMS (Extended Memory)             | __true__ / false                                            |
| `mem_ems`       | Whether to use EMS (Expanded Memory)             | __true__ / false                                            |
| `mem_umb`       | Whether to use UMB (Upper Memory Blocks)         | __true__ / false                                            |
| `mem_doslimit`  | Whether to limit free conventional memory        | Default to all available, specified in kilobytes < 640      |
| `video_card`    | Type of video card/machine to emulate            | __generic_svga__ / generic_hercules / generic_cga / generic_tandy / generic_pcjr / generic_ega / generic_vga / svga_s3_trio / svga_tseng_et3000 / svga_tseng_et4000 / svga_paradise_pvga1a |
| `video_memory`  | Amount of video memory                           | __2048__ / 512 / 1024 / 4096                                |
| `video_voodoo`  | If a Voodoo 3dfx expansion card is present       | __true__ / false                                            |
| `sound_card`    | Type of sound card to emulate                    | __sb16__ / sb1 / sb2 / sbpro1 / sbpro2 / gameblaster / none |
| `sound_port`    | The IO address of the sound card                 | __220__ / 240 / 260 / 280 / 2A0 / 2C0 / 2E0 / 300           |
| `sound_irq`     | The IRQ number of the sound card                 | __7__ / 5 / 3 / 9 / 10 / 11 / 12                            |
| `sound_dma`     | The DMA number of the sound card                 | __1__ / 5 / 3 / 6 / 7                                       |
| `sound_hdma`    | The High DMA number of the sound card            | __5__ / 6 / 7                                               |
| `sound_midi`    | If a MPU401 MIDI Processing Unit is present      | __true__ / false / path to a ZIP with a soundfont           |
| `sound_mt32`    | If a MT-32 MIDI synthesizer module is present    | __false__ / true / path to a ZIP with a MT-32 ROM (if set overrides `sound_midi`) |
| `sound_gus`     | If a Gravis UltraSound expansion card is present | __false__ / true                                            |
| `sound_tandy`   | If a Tandy Sound Device is present               | __false__ / true (always on with video_card: generic_tandy) |
| `run_path`      | Default program to launch                        | Full path, i.e. C:\PROGRAM.EXE                              |
| `run_boot`      | Default disk image to boot (floppy or hard disk) | Path relative to DOSZ, i.e. DISKS\DISK1.IMG                 |
| `run_mount`     | Default disk image to mount (floppy or CD-ROM)   | Path relative to DOSZ, i.e. CD\GAME.CHD                     |
| `run_input`     | Input sequence performed at startup              | [See below for details](#run-input-sequence)                |

## Run Input Sequence
An input sequence is composed of typed characters, pressed keyboard keys or special commands.

The sequence is processed in order, one command at a time.

Typed characters are performed as a key down, key up event to type the specific characters.
Examples for typing characters are `abc`, `ABC`, `123` or ``-=[];\`,./``.

Pressed keyboard keys are either performed as both down and up events, or just a single event when specifying `:DOWN` or `:UP`.
Examples for keyboard key presses are `(ENTER)`, `(SPACE)` or `(leftctrl:DOWN)c(leftctrl:UP)`.
When performing a single event (with DOWN or UP) no time is waited and the sequence continues immediately.
Otherwise the time between pressing and releasing a key is fixed at 70 milliseconds and the sequence continues after waiting a configurable delay time.

Special commands are WAIT and DELAY.
Wait will do nothing for a given number of milliseconds. An emulator should fast forward that wait time so only for the emulated software
the time will pass while in real time it should be as quick as possible. This can be used to skip loading times or wait for an input prompt.
Delay by itself will not do anything but it sets how many milliseconds are waited after releasing a key until pressing the next key in the
sequence. The default delay time is 70 milliseconds. Example `(WAIT:2000)(DELAY:15)i5j`

List of keyboard key names: `1`,`2`,`3`,`4`,`5`,`6`,`7`,`8`,`9`,`0`,`q`,`w`,`e`,`r`,`t`,`y`,`u`,`i`,`o`,`p`,`a`,`s`,`d`,`f`,`g`,`h`,`j`,`k`,`l`,`z`,`x`,`c`,`v`,`b`,`n`,`m`,`F1`,`F2`,`F3`,`F4`,`F5`,`F6`,`F7`,`F8`,`F9`,`F10`,`F11`,`F12`,`ESC`,`TAB`,`BACKSPACE`,`ENTER`,`SPACE`,`LEFTALT`,`RIGHTALT`,`LEFTCTRL`,`RIGHTCTRL`,`LEFTSHIFT`,`RIGHTSHIFT`,`CAPSLOCK`,`SCROLLLOCK`,`NUMLOCK`,`GRAVE`,`MINUS`,`EQUALS`,`BACKSLASH`,`LEFTBRACKET`,`RIGHTBRACKET`,`SEMICOLON`,`QUOTE`,`PERIOD`,`COMMA`,`SLASH`,`EXTRA_LT_GT`,`PRINTSCREEN`,`PAUSE`,`INSERT`,`HOME`,`PAGEUP`,`DELETE`,`END`,`PAGEDOWN`,`LEFT`,`UP`,`DOWN`,`RIGHT`,`KP1`,`KP2`,`KP3`,`KP4`,`KP5`,`KP6`,`KP7`,`KP8`,`KP9`,`KP0`,`KPDIVIDE`,`KPMULTIPLY`,`KPMINUS`,`KPPLUS`,`KPENTER`,`KPPERIOD`
