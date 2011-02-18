/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#ifndef PANEL_H
#define PANEL_H

#include <QMainWindow>
#include <QString>
#include <QtGui/QAction>


class RazorPluginManager;
class XfitMan;
class QActionGroup;

/*! \brief The Panel class provides a main window of the razor-panel.
 */
class Panel : public QMainWindow
{
    Q_OBJECT

public:
    enum Position{
        PositionBottom,
        PositionTop,
        PositionLeft,
        PositionRight
    };

    Panel(QWidget *parent = 0);
    virtual ~Panel();


public slots:
    void show();


protected slots:
    void realign();
    void switchPosition();
    void lockPlugin(bool value);


private:
    RazorPluginManager* mPluginManager;
    QString mConfigId;

    QString mTheme;
    Position mPosition;
    int mDesktopNum;
    XfitMan* mXfitMan;

    /*! \brief Returns the Position by the string.
      String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive.
      If the string is not correct, returns defaultValue.
    */
    Position strToPosition(const QString& str, Position defaultValue) const;

    /*! Return  string representation of the position
     */
    QString positionToStr(Position position) const;

    void contextMenuEvent( QContextMenuEvent* event);
    void setTheme(const QString& themeName);
    void setDesktopNum(int desktopNum);
    void setPosition(Position position);

    /*! \brief The panel can't be placed on boundary of two displays.
      This function checks, is the panel can be placed on the display @displayNum
      on @position.
     */
    bool canPlacedOn(int displayNum, Position position) const;
};



class PositionAction: public QAction
{
  Q_OBJECT
public:
    PositionAction(int displayNum, Panel::Position position, QActionGroup* parent = 0);

    Panel::Position position() const { return mPosition; }
    int displayNum() const { return mDisplayNum; }

private:
    Panel::Position mPosition;
    int mDisplayNum;
};

#endif // PANEL_H