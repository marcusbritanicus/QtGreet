# QtGreet
Qt based greeter for [greetd](https://git.sr.ht/~kennylevinsen/greetd), to be run under wayfire or similar wlr-based compositors.

Connect with us via [Matrix/IRC](https://app.element.io/#/room/#qtgreet:matrix.org) (#qtgreet:matrix.org)

<img src="screenshots/Greeter.jpg" width="800" /><br /><br />
<img src="screenshots/Sessions.jpg" width="800" /><br /><br />
<img src="screenshots/Users.jpg" width="800" />

### Dependencies:
- Qt5 (duh...)
- WayQt (https://gitlab.com/desqktop-frameworks/wayqt/)
- A wayland based compositor (wayfire, sway, cage etc..)
- Meson (to build this project)
- Ninja (to compile this project)

### Compiling and installing

- Get the sources of QtGreet
  * Git: `git clone https://gitlab.com/marcusbritanicus/QtGreet.git`
- Enter the `QtGreet` folder
  * `cd QtGreet`
- Configure the project - we use meson for project management
  * `meson .build --prefix=/usr --buildtype=release`
- Compile and install - we use ninja
  * `ninja -C .build -k 0 -j $(nproc) && sudo ninja -C .build install`

### Configure greetd to run QtGreet using wayfire

If you wish to make use of greetd, then you will have to configure it to use QtGreet.
You can either use the *inbuilt* `greeetwl` compositor (a fork of `tinywl`), or use a full-fledged compositor like wayfire or sway.

- To use `greetwl` compositor,
  * Open `/etc/greetd/config.toml` for editing with your favorite editor.
  * Under `[default_session]`, set `command = "greetwl"`

- To use wayfire,
  * Open `/etc/greetd/config.toml` for editing with your favorite editor.
  * Under `[default_session]`, set `command = "wayfire --config /etc/qtgreet/wayfire.ini"`

- Similarly for sway,
  * Open `/etc/greetd/config.toml` for editing with your favorite editor.
  * Under `[default_session]`, set `command = "sway --config /etc/qtgreet/sway.cfg"`

### Configuring the keyboard layout for wayfire
Sometimes, it would be easier if the keyboard layout was not the default `us`. For wayfire, simply add the following line at the end of
`/etc/qtgreet/wayfire.ini` file:

```ini
[input]
xkb_layout = us
```
