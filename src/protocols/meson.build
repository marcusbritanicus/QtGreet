wayland_scanner = find_program( 'wayland-scanner' )

wayland_scanner_code = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.c',
	arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_server = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.h',
	arguments: ['server-header', '@INPUT@', '@OUTPUT@'],
)

protocols = [
    'xdg-shell.xml',
]

wl_protos_src = []
wl_protos_headers = []

foreach p : protocols
	xml = join_paths( p )
	wl_protos_src += wayland_scanner_code.process( xml )
	wl_protos_headers += wayland_scanner_server.process( xml )
endforeach

lib_wl_protos = static_library(
    'wl_protos', wl_protos_src + wl_protos_headers,
	dependencies: [wayland_client],
) # for the include directory

protos = declare_dependency(
	link_with: lib_wl_protos,
	sources: wl_protos_headers,
)
