#include <stdexcept>
#include <QtGui/QOpenGLContext>
#include <QtCore/QMetaObject>

#include "MpvWidget.hpp"

static void wakeup( void *ctx ) {
    QMetaObject::invokeMethod( (MpvWidget *)ctx, "on_mpv_events", Qt::QueuedConnection );
}


static void *get_proc_address( void *ctx, const char *name ) {
    Q_UNUSED( ctx );
    QOpenGLContext *glctx = QOpenGLContext::currentContext();

    if ( !glctx ) {
        return nullptr;
    }

    return reinterpret_cast<void *>(glctx->getProcAddress( QByteArray( name ) ) );
}


MpvWidget::MpvWidget( QWidget *parent ) : QOpenGLWidget( parent ) {
    mpv = mpv_create();

    if ( !mpv ) {
        throw std::runtime_error( "could not create mpv context" );
    }

    mpv_set_option_string( mpv, "terminal",  "no" );
    mpv_set_option_string( mpv, "msg-level", "all=error" );

    /** Since mpv 0.38.0, we need to set this explicity */
    mpv_set_option_string( mpv, "vo",        "libmpv" );

    if ( mpv_initialize( mpv ) < 0 ) {
        throw std::runtime_error( "could not initialize mpv context" );
    }

    Mpv::Qt::set_option_variant( mpv, "hwdec", "auto" );

    mpv_observe_property( mpv, 0, "duration", MPV_FORMAT_DOUBLE );
    mpv_observe_property( mpv, 0, "time-pos", MPV_FORMAT_DOUBLE );
    mpv_set_wakeup_callback( mpv, wakeup, this );
}


MpvWidget::~MpvWidget() {
    makeCurrent();

    if ( mpv_gl ) {
        mpv_render_context_free( mpv_gl );
    }

    mpv_terminate_destroy( mpv );
}


void MpvWidget::command( const QVariant& params ) {
    Mpv::Qt::command_variant( mpv, params );
}


void MpvWidget::setProperty( const QString& name, const QVariant& value ) {
    Mpv::Qt::set_property_variant( mpv, name, value );
}


QVariant MpvWidget::getProperty( const QString& name ) const {
    return Mpv::Qt::get_property_variant( mpv, name );
}


void MpvWidget::initializeGL() {
    mpv_opengl_init_params gl_init_params[ 1 ] = { get_proc_address, nullptr };
    mpv_render_param       params[]{
        { MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL) },
        { MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params },
        { MPV_RENDER_PARAM_INVALID, nullptr }
    };

    if ( mpv_render_context_create( &mpv_gl, mpv, params ) < 0 ) {
        throw std::runtime_error( "failed to initialize mpv GL context" );
    }

    mpv_render_context_set_update_callback( mpv_gl, MpvWidget::on_update, reinterpret_cast<void *>(this) );
}


void MpvWidget::paintGL() {
    mpv_opengl_fbo mpfbo{ static_cast<int>(defaultFramebufferObject() ), width(), height(), 0 };
    int            flip_y{ 1 };

    mpv_render_param params[] = {
        { MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo  },
        { MPV_RENDER_PARAM_FLIP_Y,     &flip_y },
        { MPV_RENDER_PARAM_INVALID,    nullptr }
    };

    mpv_render_context_render( mpv_gl, params );
}


void MpvWidget::on_mpv_events() {
    // Process all events, until the event queue is empty.
    while ( mpv ) {
        mpv_event *event = mpv_wait_event( mpv, 0 );

        if ( event->event_id == MPV_EVENT_NONE ) {
            break;
        }

        handle_mpv_event( event );
    }
}


void MpvWidget::handle_mpv_event( mpv_event *event ) {
    switch ( event->event_id ) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *prop = (mpv_event_property *)event->data;

            if ( strcmp( prop->name, "time-pos" ) == 0 ) {
                if ( prop->format == MPV_FORMAT_DOUBLE ) {
                    double time = *(double *)prop->data;
                    Q_EMIT positionChanged( time );
                }
            }
            else if ( strcmp( prop->name, "duration" ) == 0 ) {
                if ( prop->format == MPV_FORMAT_DOUBLE ) {
                    double time = *(double *)prop->data;
                    Q_EMIT durationChanged( time );
                }
            }

            break;
        }

        default: { }
            // Ignore uninteresting or unknown events.
    }
}


void MpvWidget::maybeUpdate() {
    if ( window()->isMinimized() ) {
        makeCurrent();
        paintGL();
        context()->swapBuffers( context()->surface() );
        doneCurrent();
    }
    else {
        update();
    }
}


void MpvWidget::on_update( void *ctx ) {
    QMetaObject::invokeMethod( (MpvWidget *)ctx, "maybeUpdate" );
}
