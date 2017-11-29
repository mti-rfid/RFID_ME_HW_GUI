/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QRFEKEYBOARD_H
#define QRFEKEYBOARD_H

#include <QBasicTimer>
#include <QFrame>
#include <QPointer>

#include "QrfeKeyPiece.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

//! [0]
class QrfeKeyBoard : public QFrame
{
    Q_OBJECT

public:
    QrfeKeyBoard(QWidget *parent = 0);

    void setNextPieceLabel(QLabel *label);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:
    void start();
    void pause();

signals:
    void scoreChanged(int score);
    void levelChanged(int level);
    void linesRemovedChanged(int numLines);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
//! [0]

//! [1]
private:
    enum { BoardWidth = 10, BoardHeight = 22 };

    QrfeKeyShape &shapeAt(int x, int y) { return board[(y * BoardWidth) + x]; }
    int timeoutTime() { return 1000 / (1 + level); }
    int squareWidth() { return contentsRect().width() / BoardWidth; }
    int squareHeight() { return contentsRect().height() / BoardHeight; }
    void clearBoard();
    void dropDown();
    void oneLineDown();
    void pieceDropped(int dropHeight);
    void removeFullLines();
    void newPiece();
    void showNextPiece();
    bool tryMove(const QrfeKeyPiece &newPiece, int newX, int newY);
    void drawSquare(QPainter &painter, int x, int y, QrfeKeyShape shape);

    QBasicTimer timer;
    QPointer<QLabel> nextPieceLabel;
    bool isStarted;
    bool isPaused;
    bool isWaitingAfterLine;
    QrfeKeyPiece curPiece;
    QrfeKeyPiece nextPiece;
    int curX;
    int curY;
    int numLinesRemoved;
    int numPiecesDropped;
    int score;
    int level;
    QrfeKeyShape board[BoardWidth * BoardHeight];
};
//! [1]

#endif
