/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __QTMENUACTION_H__
#define __QTMENUACTION_H__

#include <qaction.h>

#include <ZLApplication.h>

class QApplicationWindow;

class QtMenuAction : public QAction {
	Q_OBJECT

public:
	QtMenuAction(QApplicationWindow &window, const ZLApplication::Menubar::PlainItem &item);

private slots:
	void doSlot();

private:
	QApplicationWindow &myWindow;
	int myActionId;
};

#endif /* __QTMENUACTION_H__ */
