/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "commanditemmodel.h"


//#include <razorqt/xdgdesktopfile.h>
//#include <razorqt/domhelper.h>
#include <razorqt/xdgicon.h>

//#include <QtGui/QStandardItemModel>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QDebug>


/************************************************

 ************************************************/
CommandItemModel::CommandItemModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mSourceModel(new CommandSourceItemModel(this))
{
    setSourceModel(mSourceModel);
}


/************************************************

 ************************************************/
CommandItemModel::~CommandItemModel()
{
}



/************************************************

 ************************************************/
bool CommandItemModel::isOutDated() const
{
    return mSourceModel->isOutDated();
}


/************************************************

 ************************************************/
const CommandProviderItem *CommandItemModel::command(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    QModelIndex ind = mapToSource(index);

    return mSourceModel->command(ind);
}


/************************************************

 ************************************************/
void CommandItemModel::addHistoryCommand(const QString &command)
{
    mSourceModel->addHistoryCommand(command);
}


/************************************************

 ************************************************/
bool CommandItemModel::filterAcceptsRow(int sourceRow, const QModelIndex &/*sourceParent*/) const
{
    QRegExp re(filterRegExp());

    if (re.isEmpty())
        return false;

    const CommandProviderItem *item = mSourceModel->command(sourceRow);

    if (!item)
        return false;

    return item->compare(re);
}



/************************************************

 ************************************************/
void CommandItemModel::rebuild()
{
    mSourceModel->rebuild();
}




/************************************************

 ************************************************/
CommandSourceItemModel::CommandSourceItemModel(QObject *parent) :
    QAbstractListModel(parent)
{
    mHistoryProvider = new HistoryProvider();
    mProviders.append(mHistoryProvider);
    mProviders.append(new AppLinkProvider());
#ifdef MATH_ENABLED
    mProviders.append(new MathProvider());
#endif
    rebuild();
}


/************************************************

 ************************************************/
CommandSourceItemModel::~CommandSourceItemModel()
{
    qDeleteAll(mProviders);
    mHistoryProvider = 0;
}


/************************************************

 ************************************************/
int CommandSourceItemModel::rowCount(const QModelIndex& /*parent*/) const
{
    return mRowCount;
}


/************************************************

 ************************************************/
QVariant CommandSourceItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mRowCount)
        return QVariant();

    const CommandProviderItem *item = command(index);
    if (!item)
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return QString("<b>%1</b><br>\n%2\n").arg(item->tile(), item->comment());

    case Qt::DecorationRole:
        return item->icon();

    case Qt::ToolTipRole:
        return item->toolTip();

    }

    return QVariant();
}


/************************************************

 ************************************************/
bool CommandSourceItemModel::isOutDated() const
{
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        if (i.next()->isOutDated())
            return true;
    }

    return false;
}


/************************************************

 ************************************************/
void CommandSourceItemModel::rebuild()
{
    int cnt = 0;
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        CommandProvider *p = i.next();
        if (p->isOutDated())
            p->rebuild();

        cnt += p->length();
    }
    mRowCount = cnt;
    emit layoutChanged();
}


/************************************************

 ************************************************/
const CommandProviderItem *CommandSourceItemModel::command(int row) const
{
    int n = row;
    QListIterator<CommandProvider*> i(mProviders);
    while (i.hasNext())
    {
        CommandProvider *p = i.next();
        if (n < p->count())
            return p->at(n);
        else
            n -=p->count();
    }

    return 0;
}


/************************************************

 ************************************************/
const CommandProviderItem *CommandSourceItemModel::command(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return command(index.row());
}


/************************************************

 ************************************************/
void CommandSourceItemModel::addHistoryCommand(const QString &command)
{
    mHistoryProvider->AddCommand(command);
}



/************************************************

 ************************************************/
//void CommandItemModel::loadHistory(const QSettings *settings)
//{
//    int n=0;
//    while (true)
//    {
//        n++;
//        QString command = settings->value(QString("history/command%1").arg(n)).toString();
//        if (command.isEmpty())
//            break;

//        HistoryItem *item = new HistoryItem(command);
//        mSourceModel->appendRow(item);
//    }
//}


/************************************************

 ************************************************/
//void CommandItemModel::saveHistory(QSettings *settings)
//{
//    int n=0;
//    for (int i=0; i<mSourceModel->rowCount(); ++i)
//    {
//        HistoryItem *item = dynamic_cast<HistoryItem*>(mSourceModel->item(i, 0));
//        if (item)
//        {
//            n++;
//            settings->setValue(QString("history/command%1").arg(n), item->command());
//        }
//    }
//}
