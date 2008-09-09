#include <QColumnView>
#include <QDirModel>

#include "../include/plugins.h"

class FileViewer: public QColumnView {
public:
	FileViewer( QWidget *parent=0l ): QColumnView( parent ) {
		setModel( model = new QDirModel( this ) );
	}
private:
	QDirModel *model;
};

DECLARE_MOQOID_CREATOR() {
	return new FileViewer( parent );
}
