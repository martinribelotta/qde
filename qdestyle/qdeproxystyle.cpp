#include <QIcon>

#include "qdeproxystyle.h"

QIcon QDEProxyStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *opt,
		const QWidget *widget) const {
	QIcon result;
	QMetaObject::invokeMethod(const_cast<QStyle*>(m_style),
		"standardIconImplementation", Qt::DirectConnection,
		Q_RETURN_ARG(QIcon, result),
		Q_ARG(StandardPixmap, standardIcon),
		Q_ARG(const QStyleOption*, opt),
		Q_ARG(const QWidget*, widget));
	return result;
}

int QDEProxyStyle::layoutSpacingImplementation(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
		Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const {
	int layoutSpacingIndex = m_style->metaObject()->indexOfMethod(
		"layoutSpacingImplementation(QSizePolicy::ControlType,QSizePolicy::ControlType,"
		"Qt::Orientation,const QStyleOption*,const QWidget*)" );
	if (layoutSpacingIndex < 0)
		return -1;

	int result;
	void *param[] = {&result, &control1, &control2, &orientation, &option, &widget};

	const_cast<QStyle *>(m_style)->qt_metacall(QMetaObject::InvokeMetaMethod, layoutSpacingIndex, param);
	return result;
}
