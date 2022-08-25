project(
	'QtGreet',
	'c',
	'cpp',
	version: '2.0.0',
	license: 'MIT',
	meson_version: '>=0.59.0',
	default_options: [
		'cpp_std=c++17',
		'c_std=c11',
		'warning_level=2',
		'werror=false',
	],
)

add_global_arguments( '-DPROJECT_VERSION="v@0@"'.format( meson.project_version() ), language : 'cpp' )
add_project_link_arguments( ['-rdynamic','-fPIC'], language:'cpp' )

Qt5     = import( 'qt5' )
Qt5Deps = dependency( 'qt5', modules: [ 'Core', 'Gui', 'Widgets', 'DBus', 'Network', 'WaylandCompositor' ] )
json    = dependency( 'json-c' )
wlr     = dependency( 'wlroots' )
wlrqt   = dependency( 'wayqt' )

wayland_server = dependency( 'wayland-server' )
wayland_client = dependency( 'wayland-client' )
xkbcommon      = dependency( 'xkbcommon' )
rt             = meson.get_compiler( 'cpp' ).find_library( 'rt' )

subdir( 'src/protocols' )

Includes = [
	'src',
	'src/backends',
	'src/backends/greetd',
	'src/layoutmanager',
	'src/layoutmanager/parser',
	'src/thememanager',
	'src/ui',
	'src/widgetfactory',
	'src/widgets',
	'src/widgets/buttons',
	'src/widgets/clock',
	'src/widgets/container',
	'src/widgets/labels',
	'src/widgets/power',
	'src/widgets/session',
	'src/widgets/user'
]

Headers = [
	'src/Global.hpp',
	'src/backends/LoginManager.hpp',
	'src/backends/greetd/proto.h',
	'src/backends/greetd/GreetdManager.hpp',
	'src/layoutmanager/parser/hjson.h',
	'src/layoutmanager/LayoutManager.hpp',
	'src/layoutmanager/LayoutUtils.hpp',
	'src/thememanager/ThemeManager.hpp',
	'src/ui/QtGreet.hpp',
	'src/widgetfactory/WidgetFactory.hpp',
	'src/widgets/buttons/buttons.hpp',
	'src/widgets/clock/ClockWidget.hpp',
	'src/widgets/container/Container.hpp',
	'src/widgets/labels/labels.hpp',
	'src/widgets/power/power.hpp',
	'src/widgets/session/session.hpp',
	'src/widgets/user/user.hpp'
]

Sources = [
	'src/backends/greetd/proto.c',
	'src/backends/LoginManager.cpp',
	'src/backends/greetd/GreetdManager.cpp',
	'src/layoutmanager/LayoutManager.cpp',
	'src/layoutmanager/LayoutUtils.cpp',
	'src/layoutmanager/parser/hjson_decode.cpp',
	'src/layoutmanager/parser/hjson_encode.cpp',
	'src/layoutmanager/parser/hjson_parsenumber.cpp',
	'src/layoutmanager/parser/hjson_value.cpp',
	'src/thememanager/ThemeManager.cpp',
	'src/Main.cpp',
	'src/ui/QtGreet.cpp',
	'src/widgetfactory/WidgetFactory.cpp',
	'src/widgets/buttons/buttons.cpp',
	'src/widgets/clock/ClockWidget.cpp',
	'src/widgets/container/Container.cpp',
	'src/widgets/labels/labels.cpp',
	'src/widgets/power/power.cpp',
	'src/widgets/session/session.cpp',
	'src/widgets/user/user.cpp'
]

Mocs = Qt5.compile_moc(
 	headers : Headers,
 	dependencies: Qt5Deps
)

Resources = Qt5.compile_resources(
	name: 'QtGreetRes',
	sources: 'src/QtGreet.qrc'
)

qtgreet = executable(
    'qtgreet', [ Sources, Mocs, Resources ],
    dependencies: [ Qt5Deps, json, wlr, wlrqt, wayland_server, wayland_client, xkbcommon, rt, protos ],
	include_directories: [ Includes ],
    install: true
)

tinywl = executable(
    'greetwl', [ 'src/compositor/main.cpp', 'src/compositor/tinywl.c' ],
    dependencies: [ Qt5Deps, wlr, wayland_server, wayland_client, xkbcommon, rt, protos ],
	include_directories: [ 'src/compositor' ],
    install: true
)

# Backgrounds
install_subdir(
	'backgrounds',
	install_dir : get_option( 'datadir' ) / 'qtgreet'
)

# Backgrounds
install_subdir(
	'themes',
	install_dir : get_option( 'datadir' ) / 'qtgreet'
)

# Readme, Changelog, etc
install_data(
	[ 'README.md', 'Changelog', 'ReleaseNotes' ],
	install_dir : get_option( 'datadir' ) / 'qtgreet'
)

# Configuration files
install_data(
	[ 'configs/config.ini', 'configs/wayfire.ini', 'configs/users.conf', 'configs/sway.cfg' ],
	install_dir : join_paths( get_option( 'sysconfdir' ), 'qtgreet' )
)

summary = [
	'',
	'-------------',
	'QtGreet @0@'.format( meson.project_version() ),
	'-------------',
	''
]
message( '\n'.join( summary ) )
