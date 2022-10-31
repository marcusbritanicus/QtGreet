project(
	'QtGreet',
	'c',
	'cpp',
	version: '2.0.0',
	license: 'MIT',
	meson_version: '>=0.60.0',
	default_options: [
		'cpp_std=c++17',
		'c_std=c11',
		'warning_level=2',
		'werror=false',
	],
)

add_global_arguments( '-DPROJECT_VERSION="v@0@"'.format( meson.project_version() ), language : 'cpp' )
add_project_link_arguments( ['-rdynamic','-fPIC'], language:'cpp' )

if get_option( 'nodynpath' ) == false
	add_global_arguments( '-DDYNPATH="@0@"'.format( get_option( 'dynpath' ) ), language : 'cpp' )
else
	add_global_arguments( '-DDYNPATH=""'.format( get_option( 'dynpath' ) ), language : 'cpp' )
endif

add_global_arguments( '-DLOGPATH="@0@"'.format( get_option( 'logpath' ) ), language : 'cpp' )

# If use_qt_version is 'auto', we will first search for Qt6.
# If all Qt6 packages are available, we'll use it. Otherwise,
# we'll switch to Qt5.
if get_option('use_qt_version') == 'auto'
	# Check if Qt6 is available
	QtDeps = dependency( 'qt6', modules: [ 'Core', 'Gui', 'Widgets', 'DBus' ], required: false )
	wayqt  = dependency( 'wayqt-qt6',      required: false )
	DFApp  = dependency( 'df6application', required: false )
	DFIpc  = dependency( 'df6ipc',         required: false )
	DFUtl  = dependency( 'df6utils',       required: false )

	Qt6_is_Found = QtDeps.found() and wayqt.found() and DFApp.found() and DFIpc.found() and DFUtl.found()

	if Qt6_is_Found
		Qt = import( 'qt6' )

	else
		QtDeps = dependency( 'qt5', modules: [ 'Core', 'Gui', 'Widgets', 'DBus' ], required: true )
		Qt     = import( 'qt5' )

		wayqt  = dependency( 'wayqt',          required: true )
		DFApp  = dependency( 'df5application', required: true )
		DFIpc  = dependency( 'df5ipc',         required: true )
		DFUtl  = dependency( 'df5utils',       required: true )
	endif

# User specifically wants to user Qt5
elif get_option('use_qt_version') == 'qt5'
	QtDeps = dependency( 'qt5', modules: [ 'Core', 'Gui', 'Widgets', 'DBus' ], required: true )
	Qt     = import( 'qt5' )

	wayqt  = dependency( 'wayqt',          required: true )
	DFApp  = dependency( 'df5application', required: true )
	DFIpc  = dependency( 'df5ipc',         required: true )
	DFUtl  = dependency( 'df5utils',       required: true )

# User specifically wants to user Qt6
elif get_option('use_qt_version') == 'qt6'
	QtDeps = dependency( 'qt6', modules: [ 'Core', 'Gui', 'Widgets', 'DBus' ], required: false )
	Qt     = import( 'qt6' )

	wayqt  = dependency( 'wayqt-qt6',      required: false )
	DFApp  = dependency( 'df6application', required: false )
	DFIpc  = dependency( 'df6ipc',         required: false )
	DFUtl  = dependency( 'df6utils',       required: false )
endif

# Required for greetwl
wlr            = dependency( 'wlroots' )
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
	'src/widgets',
	'src/widgets/buttons',
	'src/widgets/clock',
	'src/widgets/container',
	'src/widgets/labels',
	'src/widgets/power',
	'src/widgets/session',
	'src/widgets/user',
	'src/widgets/factory',
]

Headers = [
	'src/Global.hpp',
	'src/backends/LoginManager.hpp',
	'src/backends/greetd/GreetdManager.hpp',
	'src/layoutmanager/parser/hjson.h',
	'src/layoutmanager/LayoutManager.hpp',
	'src/layoutmanager/LayoutUtils.hpp',
	'src/thememanager/ThemeManager.hpp',
	'src/ui/QtGreet.hpp',
	'src/widgets/factory/WidgetFactory.hpp',
	'src/widgets/buttons/buttons.hpp',
	'src/widgets/clock/ClockWidget.hpp',
	'src/widgets/container/Container.hpp',
	'src/widgets/labels/labels.hpp',
	'src/widgets/power/power.hpp',
	'src/widgets/session/session.hpp',
	'src/widgets/user/user.hpp'
]

Sources = [
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
	'src/widgets/factory/WidgetFactory.cpp',
	'src/widgets/buttons/buttons.cpp',
	'src/widgets/clock/ClockWidget.cpp',
	'src/widgets/container/Container.cpp',
	'src/widgets/labels/labels.cpp',
	'src/widgets/power/power.cpp',
	'src/widgets/session/session.cpp',
	'src/widgets/user/user.cpp'
]

Mocs = Qt.compile_moc(
 	headers : Headers,
 	dependencies: QtDeps
)

Resources = Qt.compile_resources(
	name: 'QtGreetRes',
	sources: 'src/QtGreet.qrc'
)

qtgreet = executable(
    'qtgreet', [ Sources, Mocs, Resources ],
    dependencies: [ QtDeps, wayqt, DFApp, DFIpc, DFUtl ],
	include_directories: [ Includes ],
    install: true
)

greetwl = executable(
    'greetwl', [ 'src/compositor/main.cpp', 'src/compositor/tinywl.c' ],
    dependencies: [ QtDeps, wlr, wayland_server, wayland_client, xkbcommon, rt, protos ],
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

# Configuration file path: create it only if it's not disabled
if get_option( 'nodynpath' ) == false
	install_emptydir(
		get_option( 'dynpath' ),
		install_mode: [ 'rwxr-xr-x', get_option( 'username' ), get_option( 'username' ) ],
	)
endif

summary = [
	'',
	'--------------',
	'QtGreet v@0@'.format( meson.project_version() ),
	'--------------',
	''
]
message( '\n'.join( summary ) )
