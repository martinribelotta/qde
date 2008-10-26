#include <QtGui>

#include <QtDebug>

#include "configdialog.h"

#include "../../qonfiguratorplug/interface.h"

#define DEFAULT_QONFIGURATOR_PLUGIN_DIR (QApplication::applicationDirPath()+"/../plugins/qonfigurator" )

ConfigDialog::ConfigDialog()
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    pagesWidget = new QStackedWidget;

    QPushButton *closeButton = new QPushButton(tr("Close"));

    loadPlugins();

    contentsWidget->setCurrentRow(0);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(pagesWidget, SIGNAL(currentChanged(int)), this, SLOT(fadeInWidget(int)));

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Config Dialog"));
}

void ConfigDialog::fadeInWidget(int index)
{
    if (faderWidget)
        faderWidget->close();
    faderWidget = new FaderWidget(pagesWidget->widget(index));
    faderWidget->start();
}

void ConfigDialog::loadPlugins()
{
    QStringList pluginsPaths = QSettings( "qde", "qonfigurator" )
        .value( "pluginspath", DEFAULT_QONFIGURATOR_PLUGIN_DIR )
        .toString().split( ":" );
    foreach( QString path, pluginsPaths ) {
        QDir pluginDir( path );
        foreach( QFileInfo pluginFile, pluginDir.entryInfoList( QStringList("*.so"), QDir::Files ) ) {
            QLibrary lib( pluginFile.absoluteFilePath() );
            QOnfiguratorPlugin_entry entry = (QOnfiguratorPlugin_entry)
                lib.resolve( QONFIGURATOR_PLUGIN_ENTRY_NAME );
            if ( entry ) {
                QOnfiguratorPlugin_data data = entry();
                QListWidgetItem *itemEntry = new QListWidgetItem(contentsWidget);
                itemEntry->setIcon( data.icon );
                itemEntry->setText( data.description );
                itemEntry->setTextAlignment(Qt::AlignHCenter);
                itemEntry->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                pagesWidget->addWidget( data.widget );
            } else {
                qDebug() << "No " QONFIGURATOR_PLUGIN_ENTRY_NAME " found on " << pluginFile.absoluteFilePath() ;
                qDebug() << "Fail with" << lib.errorString();
            }
        }
    }

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
