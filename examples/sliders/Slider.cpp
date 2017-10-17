#include "Slider.h"
#include "SliderSkinlet.h"

#include <QskAspect.h>
#include <QskRgbValue.h>
#include <QskAnimationHint.h>

QSK_SUBCONTROL( Slider, Scale )
QSK_SUBCONTROL( Slider, Decoration )

Slider::Slider( QQuickItem* parentItem ):
    QskSlider( parentItem )
{
    using namespace QskAspect;

    setMetric( QskSlider::Handle | Size, 80 );

    setColor( Fill, QskRgbValue::Grey700 );
    setColor( Scale, qRgb( 178, 178, 178 ) ); // for the ticks

    setColor( QskSlider::Handle, QskRgbValue::Grey800 );

    for ( auto state : { Pressed, Focused | Hovered, Hovered, Focused } )
        setColor( QskSlider::Handle | Color | state, QskRgbValue::Orange600 );

    setAnimation( QskSlider::Handle | Color, 1000 );
    for ( auto state : { Focused | Hovered, Hovered, Focused } )
        setAnimation( QskSlider::Handle | Color | state, 300 );

    // using an individual skinlet, not known by the skin

    auto skinlet = new SliderSkinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );
}

QSizeF Slider::contentsSizeHint() const
{
    const qreal extra = 40;
    return Inherited::contentsSizeHint() + QSizeF( 0, extra );
}

