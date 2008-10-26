#ifndef QDEPROXYSTYLE_H
#define QDEPROXYSTYLE_H

#include <QStyle>

class QDEProxyStyle : public QStyle
{
	Q_OBJECT
public:
	QDEProxyStyle( QStyle *managed=0l ): m_style( managed ) { }

	virtual ~QDEProxyStyle() {
		deleteManagedStyle();
	}

	virtual void polish(QWidget *w) {
		managedStyle()->polish(w);
	}
	virtual void unpolish(QWidget *w) {
		managedStyle()->unpolish(w);
	}

	virtual void polish(QApplication *a) {
		managedStyle()->unpolish(a);
	}
	virtual void unpolish(QApplication *a) {
		managedStyle()->unpolish(a);
	}

	virtual void polish(QPalette &pal) {
		managedStyle()->polish(pal);
	}

	virtual QRect itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled,
				const QString &text) const {
		return managedStyle()->itemTextRect( fm, r, flags, enabled, text );
	}

	virtual QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const {
		return managedStyle()->itemPixmapRect( r, flags, pixmap );
	}

	virtual void drawItemText(QPainter *painter, const QRect &rect,
				int flags, const QPalette &pal, bool enabled,
				const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const {
		managedStyle()->drawItemText( painter, rect, flags, pal, enabled, text, textRole );
	}

	// pure
	virtual void drawItemPixmap(QPainter *painter, const QRect &rect,
				int alignment, const QPixmap &pixmap) const {
		managedStyle()->drawItemPixmap( painter, rect, alignment, pixmap );
	}

	virtual QPalette standardPalette() const {
		return managedStyle()->standardPalette();
	}

	// pure
	virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
				const QWidget *w = 0) const {
		managedStyle()->drawPrimitive( pe, opt, p, w );
	}

	// pure
	virtual void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
				const QWidget *w = 0) const {
		managedStyle()->drawControl( element, opt, p, w );
	}

	// pure
	virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
				const QWidget *widget = 0) const {
		return managedStyle()->subElementRect( subElement, option, widget );
	}

	// pure
	virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
				const QWidget *widget = 0) const {
		managedStyle()->drawComplexControl( cc, opt, p, widget );
	}

	// pure
	virtual SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
				const QPoint &pt, const QWidget *widget = 0) const {
		return managedStyle()->hitTestComplexControl( cc, opt, pt, widget );
	}

	// pure
	virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
				SubControl sc, const QWidget *widget = 0) const {
		return managedStyle()->subControlRect( cc, opt, sc, widget );
	}

	// pure
	virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
				const QWidget *widget = 0) const {
		return managedStyle()->pixelMetric( metric, option, widget );
	}

	// pure
	virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
				const QSize &contentsSize, const QWidget *w = 0) const {
		return managedStyle()->sizeFromContents( ct, opt, contentsSize, w );
	}

	// pure
	virtual int styleHint(StyleHint stylehint, const QStyleOption *opt = 0,
				const QWidget *widget = 0, QStyleHintReturn* returnData = 0) const {
		return managedStyle()->styleHint( stylehint, opt, widget, returnData );
	}

	// pure
	virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt = 0,
				const QWidget *widget = 0) const {
		return managedStyle()->standardPixmap( standardPixmap, opt, widget );
	}

	// pure
	virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
				const QStyleOption *opt) const {
		return managedStyle()->generatedIconPixmap( iconMode, pixmap, opt );
	}

protected Q_SLOTS:
	QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *opt = 0,
		const QWidget *widget = 0) const;

	int layoutSpacingImplementation(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
		Qt::Orientation orientation, const QStyleOption *option = 0, const QWidget *widget = 0) const;

protected:
	QStyle *managedStyle() const { return m_style; }
	virtual void deleteManagedStyle() { if ( m_style ) delete m_style; }
	virtual void setManagedStyle( QStyle *style ) { m_style = style; }
private:
	QStyle *m_style;
};

#endif // QDEPROXYSTYLE_H
