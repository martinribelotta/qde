#include <QWidget>

#include "../include/plugins.h"

class MyWidget: public QWidget {
public:
	MyWidget() {
		setAttribute( Qt::WA_NoSystemBackground );
		setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
	}
};

DECLARE_QUIQUE_CREATOR() {
	return new MyWidget();
}
