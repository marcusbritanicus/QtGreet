# QtGreet
Qt based greeter for [greetd](https://git.sr.ht/~kennylevinsen/greetd), to be run under wayfire or similar wlr-based compositors.

### Dependencies:
- Qt5 (duh...)
- wlroots
- WLR based compositor (wayfire, sway etc..)
- CMake (to build this project)
- Make (to compile this project)

### Compiling and installing

- Get the sources
  * Git: `git clone https://gitlab.com/marcusbritanicus/QtGreet.git`
- Enter the `QtGreet` and create and enter `build`.
  * `cd QtGreet && mkdir build && cd build`
- Configure the project - we use cmake for project management
  * `cmake ..`
- Compile and install - we use make
  * `make -kj$(nproc) && sudo make install`

### Configure greetd to run QtGreet using wayfire

- Firstly, set the command `greetd` should run as `wayfire --config /etc/greetd/wayfire.ini`
  * To do this, you'll need to edit `/etc/greetd/config.toml`.
  * Under `[default_session]`, set `command = "wayfire --config /etc/greetd/wayfire.ini"`

- Then, you'll need a configuration file `wayfire.ini` which is supplied to wayfire.
  * Create a file `/etc/greetd/wayfire.ini` and set the contents to:
    ```
    [autostart]
    autostart_wf_shell = false
    dm = qtgreet && wayland-logout

    [core]
    plugins = autostart
    vheight = 1
    vwidth = 1
    ```

### Tips and tricks

- The users are automatically divined from `/etc/passwd`, UID_MIN and UID_MAX are read from `/etc/login.defs`.
- The users are sorted alphabetically.
- You may select the user by pressing the left/right buttons available on either side of the user name.
- Alternatively, the user can click on the username (it's a button) - and select the user from the dialog.
- The sessions are read from `/usr/share/wayland-sessions` first, then from `/usr/share/xsessions`
- You may select the sessions by pressing the left/right buttons available on either side of the session name.
- Alternatively, the user can click on the session name (it's a button) - and select the session from the dialog.
- An option to enter custom command is available at the end.
- The field below the session name shows the command that will be run. If you select the session as "Custom",
  you'll be able to enter the command.
