# QtGreet
Qt based greeter for [greetd](https://git.sr.ht/~kennylevinsen/greetd), to be run under wayfire or similar wlr-based compositors.

![image](Screenshots/Greeter.jpg)

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

### Configuration of QtGreet

- We first paint a base color over the entire screen over which the wallpaper is draw with 50% opacity. This is to improve the legibility.
- You can set the base color and the wallpaper in /etc/qtgreet/config.ini file.
- `BaseColor` key defines the base color to be painted. It takes RGB hex values as single continuous string (HTML format without the leading #).
  * Ex: ffffff (white), ff0000 (red), 008080 (teal), and so on.
- `Background` key defines the background image. All image formats supported by Qt on your system are supported by QtGreet. Typically
jpg/png/svg files should be fine. Some default background files can be found in /usr/share/qtgreet/backgrounds. The path should be absolute.
- If you wish not to use a background image, you may set `Background = none`, in which case only the base color will be painted on the screen.

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
