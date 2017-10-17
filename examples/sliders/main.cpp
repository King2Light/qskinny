/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Slider.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskRgbValue.h>
#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskFocusIndicator.h>
#include <QskLinearBox.h>
#include <QskSlider.h>
#include <QskSeparator.h>
#include <QskPushButton.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxShapeMetrics.h>

#include <QGuiApplication>
#include <QtMath>

class OtherSlider : public QskSlider
{
public:
    /*
        Slider overriding many hints from the skin.
        "Preserved" is for horizontal, "Transposed" for vertical orientation
     */

    OtherSlider( QQuickItem* parentItem = nullptr ):
        QskSlider( parentItem )
    {
        using namespace QskAspect;
        using namespace QskRgbValue;

        const qreal h = 30;
        const qreal w = 2.0 * h;
        const qreal paddingW = 0.5 * w + 1;

        // Panel

        for ( auto placement : { Preserved, Transposed } )
        {
            const Aspect aspect = Panel | placement;

            setMetric( aspect | Size, h );
            setBoxBorderHint( aspect, 1 );
            setBoxShapeHint( aspect, 4 );
            setBoxBorderColorHint( aspect, Grey900 );
            setGradientHint( aspect, Grey400 );

            if ( placement == Preserved )
                setMarginsHint( aspect | Padding, QMarginsF( paddingW, 0, paddingW, 0 ) );
            else
                setMarginsHint( aspect | Padding, QMarginsF( 0, paddingW, 0, paddingW ) );
        }

        // Groove

        for ( auto placement : { Preserved, Transposed } )
        {
            const Aspect aspect = Groove | placement;

            setMetric( aspect | Size, 4 );
            setBoxBorderHint( aspect, 0 );
            setBoxShapeHint( aspect, 1 );

            setGradientHint( aspect, Qt::black );
        }

        // no Fill
        for ( auto placement : { Preserved, Transposed } )
        {
            const Aspect aspect = Fill | placement;
            setMetric( aspect | Size, 0 );
        }
    
        // Handle

        for ( auto placement : { Preserved, Transposed } )
        {
            const Aspect aspect = Handle | placement;
        
            setBoxBorderHint( aspect, 1 );
            setBoxShapeHint( aspect, 4 );

            const qreal m = 0.5 * qCeil( 0.5 * ( w - h ) ) + 1;

            if ( placement == Preserved )
                setMarginsHint( aspect | Margin, QMarginsF( -m, 0, -m, 0 ) );
            else
                setMarginsHint( aspect | Margin, QMarginsF( 0, -m, 0, -m ) );

            for ( auto state : { NoState, Pressed } )
            {
                setBoxBorderColorHint( aspect | state, Grey600 );
                setGradientHint( aspect | state, Blue400 );
            }
        }
    }
};

class SliderBox : public QskLinearBox
{
public:
    SliderBox( QQuickItem* parentItem = nullptr ):
        QskLinearBox( Qt::Vertical, parentItem )
    {
        auto slider = new QskSlider( this );
        slider->setMinimum( 0 );
        slider->setMaximum( 1000 );
        slider->setPageSize( 10 );
        slider->setStepSize( 10 );
        slider->setSnap( true );

        (void) new QskSeparator( Qt::Horizontal, this );

        auto otherSlider = new OtherSlider( this );

        otherSlider->setMinimum( 0 );
        otherSlider->setMaximum( 10 );
        otherSlider->setStepSize( 1 );

        auto customSlider = new Slider( this );
        customSlider->setMargins( QMarginsF( 0, 15, 0, 15 ) );
        customSlider->setSnap( true );
        customSlider->setMinimum( 0 );
        customSlider->setMaximum( 2000 );
        customSlider->setStepSize( 10 );
        customSlider->setPageSize( 10 );

        for ( int i = 0; i < itemCount(); i++ )
        {
            if ( QskSlider* slider = qobject_cast< QskSlider* >( itemAtIndex( i ) ) )
            {
                slider->setObjectName( "Slider " + QString::number(i + 1) );
                slider->setValue( slider->minimum() +
                    0.5 * ( slider->maximum() - slider->minimum() ) );
#if 0
                connect( slider, &QskSlider::valueChanged,
                    [ = ]( qreal value ) { qDebug() << value; } );
#endif
                setAlignment( slider, Qt::AlignCenter );
            }
        }
    }

    void flip()
    {
        setOrientation( inverted( orientation() ) );

        for ( int i = 0; i < itemCount(); i++ )
        {
            if ( QskSlider* slider = qobject_cast< QskSlider* >( itemAtIndex( i ) ) )
            {
                const Qt::Orientation orientation = inverted( slider->orientation() );

                slider->setOrientation( orientation );

                if ( i >= itemCount() - 1 )
                {
                    // we didn't implement the vertical mode of the heavily
                    // customized slider yet.

                    slider->setVisible( orientation == Qt::Horizontal );
                }
            }
            else if ( QskSeparator* separator = qobject_cast< QskSeparator* >( itemAtIndex( i ) ) )
            {
                separator->setOrientation( inverted( separator->orientation() ) );
            }
        }
    }

private:
    Qt::Orientation inverted( Qt::Orientation orientation ) const
    {
        return ( orientation == Qt::Horizontal ) ? Qt::Vertical : Qt::Horizontal;
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskPushButton* buttonFlip = new QskPushButton( "Flip" );
    buttonFlip->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    buttonFlip->setFocus( true );

    SliderBox* sliderBox = new SliderBox();
    sliderBox->setBackgroundColor( QskRgbValue::PeachPuff );

    QObject::connect( buttonFlip, &QskPushButton::clicked,
        sliderBox, &SliderBox::flip );

    QskLinearBox* mainBox = new QskLinearBox( Qt::Vertical );
    mainBox->setMargins( 10 );
    mainBox->setSpacing( 10 );
    mainBox->addItem( buttonFlip, Qt::AlignLeft );
    mainBox->addItem( sliderBox );
    mainBox->setStretchFactor( sliderBox, 10 );

    QskFocusIndicator* focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );

    QskWindow window;
    window.addItem( mainBox );
    window.addItem( focusIndicator );
    window.resize( 800, qMax( 480, int( mainBox->sizeHint().height() + 0.5 ) ) );
    window.show();

    return app.exec();
}
