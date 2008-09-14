/****************************************************************************
**
** Copyright (C) 2006 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation or under the
** terms of the Qt Commercial License Agreement. The respective license
** texts for these are provided with the open source and commercial
** editions of Qt.
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include <QtDebug>

#include "configdialog.h"
//#include "pages.h"

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

    /*pagesWidget->addWidget(new ConfigurationPage);
    pagesWidget->addWidget(new UpdatePage);
    pagesWidget->addWidget(new QueryPage);*/

    QPushButton *closeButton = new QPushButton(tr("Close"));

    //createIcons();
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
    //mainLayout->addStretch(1);
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

//void ConfigDialog::createIcons()
void ConfigDialog::loadPlugins()
{
    /*QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
    configButton->setIcon(QIcon(":/images/config.png"));
    configButton->setText(tr("Configuration"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *updateButton = new QListWidgetItem(contentsWidget);
    updateButton->setIcon(QIcon(":/images/update.png"));
    updateButton->setText(tr("Update"));
    updateButton->setTextAlignment(Qt::AlignHCenter);
    updateButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *queryButton = new QListWidgetItem(contentsWidget);
    queryButton->setIcon(QIcon(":/images/query.png"));
    queryButton->setText(tr("Query"));
    queryButton->setTextAlignment(Qt::AlignHCenter);
    queryButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/

    QStringList pluginsPaths = QSettings( "qde", "qonfigurator" )
        .value( "pluginspath", DEFAULT_QONFIGURATOR_PLUGIN_DIR )
        .toString().split( ":" );
    foreach( QString path, pluginsPaths ) {
        QDir pluginDir( path );
        foreach( QFileInfo pluginFile, pluginDir.entryInfoList( QStringList("*.so"), QDir::Files ) ) {
            QLibrary lib( pluginFile.absoluteFilePath() );
            QOnfiguratorPlugin_entry entry = (QOnfiguratorPlugin_entry)
                lib.resolve( QONFIGURATOR_PLUGIN_ENTRY_NAME );
                //QLibrary::resolve( pluginFile.absoluteFilePath(), QONFIGURATOR_PLUGIN_ENTRY_NAME );
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
