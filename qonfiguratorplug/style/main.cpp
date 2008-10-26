#include "../interface.h"

#include <QtCore>
#include <QtGui>

#include <QtUiTools>
#include <QCopChannel>

#include <QtDebug>

class StyleConfiguratorWidget: public QWidget {
	Q_OBJECT
public:
	StyleConfiguratorWidget( QWidget *parent = 0l ): QWidget( parent ) {
		QUiLoader loader;
		QFile f( ":/formStyle.ui" );
		if ( f.open( QFile::ReadOnly ) ) {
			QWidget *form = loader.load( &f, this );
			QHBoxLayout *layout = new QHBoxLayout( this );
			layout->addWidget( form );

			styleListWidget = qFindChild<QListWidget*>( form, "styleListWidget" );
			stylePreviewer = qFindChild<QWidget*>( form, "stylePreviewer" );

			QMetaObject::connectSlotsByName( this );

			loadStyles();
			loadConfig();
		}
	}

private:
	const QString styleName( QListWidgetItem* item ) {
		return item? item->text():QString();
	}

	const QString styleName() {
		return styleName( styleListWidget->currentItem() );
	}

public slots:
	void loadConfig() {
		QSettings sets( "qde", "desktop" );
		QString styleName = sets.value( "style", QString() ).toString();
		if ( styleName.isEmpty() )
			styleName = "plastique";
		QList<QListWidgetItem*> founds = styleListWidget->findItems( styleName, Qt::MatchFixedString );
		if ( !founds.isEmpty() )
			styleListWidget->setCurrentItem( founds.at(0) );
		else
			QMessageBox::information( this, tr( "Error" ), tr( "Can't load '%1' style" ).arg( styleName ) );
	}

	void loadStyles() {
		foreach( QString styleName, QStyleFactory::keys() )
			if ( styleName != "qdestyle" )
				new QListWidgetItem( styleName, styleListWidget );
	}

	void saveConfig() {
		QSettings sets( "qde", "desktop" );
		sets.setValue( "style", styleName() );
	}

private slots:
	void on_styleListWidget_currentItemChanged( QListWidgetItem *curr, QListWidgetItem *prev ) {
		(void)prev;
		setStyle(QStyleFactory::create( styleName( curr ) ));
		setPalette( style()->standardPalette() );
	}

	void on_buttonApply_clicked() {
		saveConfig();
		QCopChannel::send( "org.qde.qonfigurator.style", "reconfigure" );
	}

	void on_buttonRestore_clicked() {
		qDebug() << "on buttonRestore clicked: TODO";
	}

private:
	void setStyle( QStyle *style ) {
		setStyleTo( stylePreviewer, style );
	}

	void setStyleTo( QWidget* w, QStyle *s ) {
		if ( !w )
			return;
		// From Style config. dialog on KDE4
		w->setPalette(QPalette());

		// WTF?
		QPalette newPalette( s->standardPalette() ); //KGlobalSettings::createApplicationPalette());
		s->polish( newPalette );
		w->setPalette(newPalette);

		// Apply the new style.
		w->setStyle(s);

		// Recursively update all children.
		const QObjectList children = w->children();
		// Apply the style to each child widget.
		foreach (QObject* child, children)
		{
			if (child->isWidgetType())
				setStyleTo((QWidget *) child, s);
		}
	}

private:
	QListWidget *styleListWidget;
	QWidget *stylePreviewer;
};

DECLARE_QONFIGURATOR_PLUGIN(
	"StyleConfigurator",
	QIcon( ":/style.svg" ),
	QObject::tr( "Style\nSelection" ),
	new StyleConfiguratorWidget()
);

#include "main.moc"

