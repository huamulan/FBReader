/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gtk/gtk.h>

#include <abstract/ZLOptions.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"
#include "GtkViewWidget.h"
#include "GtkPaintContext.h"

static ZLIntegerOption Width("Options", "Width", 800);
static ZLIntegerOption Height("Options", "Height", 800);

static void applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkFBReader*)data)->close();
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkFBReader*)data)->repaintView();
}

struct UniversalSlotData {
	UniversalSlotData(GtkFBReader *reader, GtkFBReader::ActionCode code) { Reader = reader; Code = code; }
	GtkFBReader *Reader;
	GtkFBReader::ActionCode Code;
};

static void universalSlot(GtkWidget*, gpointer data) {
	UniversalSlotData *uData = (UniversalSlotData*)data;
	uData->Reader->doAction(uData->Code);
}

/*
static void undo(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_UNDO);
}

static void redo(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_REDO);
}

static void showContents(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_SHOW_CONTENTS);
}

static void showCollection(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_SHOW_COLLECTION);
}

static void showOptionsDialog(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_SHOW_OPTIONS);
}

static void search(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_SEARCH);
}

static void findNext(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_FIND_NEXT);
}

static void findPrevious(GtkWidget*, gpointer data) {
	((GtkFBReader*)data)->doAction(GtkFBReader::ACTION_FIND_PREVIOUS);
}
*/

static void handleKey(GtkWidget *, GdkEventKey *key, gpointer data) {
	((GtkFBReader*)data)->handleKeySlot(key);
}

/*
GtkWidget *GtkFBReader::addToolButton(GtkWidget *toolbar, const std::string &name, GtkSignalFunc signal) {
	GtkWidget *image = gtk_image_new_from_file((ImageDirectory + '/' + name + ".png").c_str());
	GtkWidget *button = gtk_button_new();
	gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
	gtk_container_add(GTK_CONTAINER(button), image);
	gtk_container_add(GTK_CONTAINER(toolbar), button);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", signal, this);
	return button;
}
*/

GtkWidget *GtkFBReader::addToolButton(GtkWidget *toolbar, const std::string &name, ActionCode code) {
	GtkWidget *image = gtk_image_new_from_file((ImageDirectory + '/' + name + ".png").c_str());
	GtkWidget *button = gtk_button_new();
	gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
	gtk_container_add(GTK_CONTAINER(button), image);
	gtk_container_add(GTK_CONTAINER(toolbar), button);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(universalSlot), new UniversalSlotData(this, code));
	return button;
}

GtkFBReader::GtkFBReader() : FBReader(new GtkPaintContext()) {
	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	GtkWidget *vbox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), vbox);

	GtkWidget *toolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, false, false, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);

	/*
	myBookCollectionButton = addToolButton(toolbar, "books", GTK_SIGNAL_FUNC(::showCollection));
	mySettingsButton = addToolButton(toolbar, "settings", GTK_SIGNAL_FUNC(::showOptionsDialog));
	myLeftArrowButton = addToolButton(toolbar, "leftarrow", GTK_SIGNAL_FUNC(::undo));
	myRightArrowButton = addToolButton(toolbar, "rightarrow", GTK_SIGNAL_FUNC(::redo));
	myContentsTableButton = addToolButton(toolbar, "contents", GTK_SIGNAL_FUNC(::showContents));
	mySearchButton = addToolButton(toolbar, "find", GTK_SIGNAL_FUNC(::search));
	myFindPreviousButton = addToolButton(toolbar, "findprev", GTK_SIGNAL_FUNC(::findPrevious));
	myFindNextButton = addToolButton(toolbar, "findnext", GTK_SIGNAL_FUNC(::findNext));
	*/
	myBookCollectionButton = addToolButton(toolbar, "books", ACTION_SHOW_COLLECTION);
	mySettingsButton = addToolButton(toolbar, "settings", ACTION_SHOW_OPTIONS);
	myLeftArrowButton = addToolButton(toolbar, "leftarrow", ACTION_UNDO);
	myRightArrowButton = addToolButton(toolbar, "rightarrow", ACTION_REDO);
	myContentsTableButton = addToolButton(toolbar, "contents", ACTION_SHOW_CONTENTS);
	mySearchButton = addToolButton(toolbar, "find", ACTION_SEARCH);
	myFindPreviousButton = addToolButton(toolbar, "findprev", ACTION_FIND_PREVIOUS);
	myFindNextButton = addToolButton(toolbar, "findnext", ACTION_FIND_NEXT);

	myViewWidget = new GtkViewWidget(this);
	gtk_container_add(GTK_CONTAINER(vbox), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(myMainWindow), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	gtk_window_resize(myMainWindow, Width.value(), Height.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	setMode(BOOK_TEXT_MODE);

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKey), this);

	myKeyBindings["L"] = ACTION_SHOW_COLLECTION;
	myKeyBindings["O"] = ACTION_SHOW_OPTIONS;
	myKeyBindings["Left"] = ACTION_UNDO;
	myKeyBindings["Right"] = ACTION_REDO;
	myKeyBindings["C"] = ACTION_SHOW_CONTENTS;
	myKeyBindings["F"] = ACTION_SEARCH;
	myKeyBindings["P"] = ACTION_FIND_PREVIOUS;
	myKeyBindings["N"] = ACTION_FIND_NEXT;
	myKeyBindings["I"] = ACTION_SHOW_HIDE_POSITION_INDICATOR;
	myKeyBindings["Up"] = ACTION_SCROLL_BACKWARD;
	myKeyBindings["Down"] = ACTION_SCROLL_FORWARD;
	myKeyBindings["Escape"] = ACTION_CANCEL;
	myKeyBindings["<Shift>plus"] = ACTION_INCREASE_FONT;
	myKeyBindings["equal"] = ACTION_DECREASE_FONT;

	// FIXME: this way it's impossible to add increaseFontSlot/decreaseFontSlot/cancelSlot
/*
	myLastScrollingTime = QTime::currentTime();
*/
}

GtkFBReader::~GtkFBReader() {
	int width, height;
	gtk_window_get_size(myMainWindow, &width, &height);
	Width.setValue(width);
	Height.setValue(height);

	delete myViewWidget;
}

gboolean GtkFBReader::handleKeySlot(GdkEventKey *event) {
	std::map<std::string,ActionCode>::const_iterator accel;

	for (accel = myKeyBindings.begin(); accel != myKeyBindings.end() ; ++accel) {
		guint key;
		GdkModifierType mods;

		gtk_accelerator_parse(accel->first.c_str(), &key, &mods);

		if (event->keyval == key && (GdkModifierType)event->state == mods) {
			break;
		}
	}

	if (accel != myKeyBindings.end()) {
		doAction(accel->second);
	}

	return FALSE;
}

void GtkFBReader::doAction(ActionCode code) {
	switch (code) {
		case ACTION_SHOW_COLLECTION:
			setMode(BOOK_COLLECTION_MODE);
			break;
		case ACTION_SHOW_OPTIONS:
			showOptionsDialog();
			repaintView();
			break;
		case ACTION_UNDO:
			undoPage();
			break;
		case ACTION_REDO:
			redoPage();
			break;
		case ACTION_SHOW_CONTENTS:
			setMode(CONTENTS_MODE);
			break;
		case ACTION_SEARCH:
			searchSlot();
			break;
		case ACTION_FIND_PREVIOUS:
			findPrevious();
			break;
		case ACTION_FIND_NEXT:
			findNext();
			break;
		case ACTION_SCROLL_FORWARD:
			scrollForwardSlot();
			break;
		case ACTION_SCROLL_BACKWARD:
			scrollBackwardSlot();
			break;
		case ACTION_CANCEL:
			cancelSlot();
			break;
		case ACTION_INCREASE_FONT:
			increaseFont();
			break;
		case ACTION_DECREASE_FONT:
			decreaseFont();
			break;
		case ACTION_SHOW_HIDE_POSITION_INDICATOR:
			showHidePositionIndicator();
			break;
	}
}

void GtkFBReader::scrollForwardSlot() {
/*
	QTime time = QTime::currentTime();
	int msecs = myLastScrollingTime.msecsTo(time);
	if ((msecs < 0) || (msecs >= ScrollingDelayOption.value())) {
		myLastScrollingTime = time;
*/
		nextPage();
/*
	}
*/
}

void GtkFBReader::scrollBackwardSlot() {
/*
	QTime time = QTime::currentTime();
	int msecs = myLastScrollingTime.msecsTo(time);
	if ((msecs < 0) || (msecs >= ScrollingDelayOption.value())) {
		myLastScrollingTime = time;
*/
		previousPage();
/*
	}
*/
}

void GtkFBReader::cancelSlot() {
	if (QuitOnCancelOption.value() || (mode() != BOOK_TEXT_MODE)) {
		close();
	}
}

void GtkFBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}
	myPreviousMode = myMode;
	myMode = mode;

	switch (myMode) {
		case BOOK_TEXT_MODE:
			gtk_widget_show(myBookCollectionButton);
			gtk_widget_show(mySettingsButton);
			gtk_widget_show(myLeftArrowButton);
			gtk_widget_show(myRightArrowButton);
			gtk_widget_show(myContentsTableButton);
			gtk_widget_show(mySearchButton);
			gtk_widget_show(myFindPreviousButton);
			gtk_widget_show(myFindNextButton);
			myViewWidget->setView(myBookTextView);
			break;
		case CONTENTS_MODE:
			gtk_widget_show(myBookCollectionButton);
			gtk_widget_show(mySettingsButton);
			gtk_widget_hide(myLeftArrowButton);
			gtk_widget_hide(myRightArrowButton);
			gtk_widget_hide(myContentsTableButton);
			gtk_widget_show(mySearchButton);
			gtk_widget_show(myFindPreviousButton);
			gtk_widget_show(myFindNextButton);
			myViewWidget->setView(myContentsView);
			break;
		case FOOTNOTE_MODE:
			gtk_widget_hide(myBookCollectionButton);
			gtk_widget_show(mySettingsButton);
			gtk_widget_hide(myLeftArrowButton);
			gtk_widget_hide(myRightArrowButton);
			gtk_widget_show(myContentsTableButton);
			gtk_widget_show(mySearchButton);
			gtk_widget_show(myFindPreviousButton);
			gtk_widget_show(myFindNextButton);
			myViewWidget->setView(myFootnoteView);
			break;
		case BOOK_COLLECTION_MODE:
			gtk_widget_hide(myBookCollectionButton);
			gtk_widget_show(mySettingsButton);
			gtk_widget_hide(myLeftArrowButton);
			gtk_widget_hide(myRightArrowButton);
			gtk_widget_hide(myContentsTableButton);
			gtk_widget_show(mySearchButton);
			gtk_widget_show(myFindPreviousButton);
			gtk_widget_show(myFindNextButton);
			myCollectionView->fill();
			myViewWidget->setView(myCollectionView);
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
	setWindowCaption("FBReader - " + myViewWidget->view()->caption());
	repaintView();
}

void GtkFBReader::close() {
	if (mode() != BOOK_TEXT_MODE) {
		restorePreviousMode();
	} else {
		delete this;
		gtk_main_quit();
	}
}

void GtkFBReader::repaintView() {
	myViewWidget->repaintView();
}

/*
 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
 * does something strange if WIDGET is already insensitive.
 */
static void enableButton(GtkWidget *button, bool enable) {
	bool enabled = GTK_WIDGET_STATE(button) != GTK_STATE_INSENSITIVE;
	if (enabled != enable) {
		gtk_widget_set_sensitive(button, enable);
	}
}

void GtkFBReader::enableMenuButtons() {
	switch (mode()) {
		case BOOK_TEXT_MODE:
			enableButton(myLeftArrowButton, myBookTextView->canUndoPageMove());
			enableButton(myRightArrowButton, myBookTextView->canRedoPageMove());
			enableButton(myFindNextButton, myBookTextView->canFindNext());
			enableButton(myFindPreviousButton, myBookTextView->canFindPrevious());
			enableButton(myContentsTableButton, !myContentsView->isEmpty());
			break;
		case CONTENTS_MODE:
			enableButton(myFindNextButton, myContentsView->canFindNext());
			enableButton(myFindPreviousButton, myContentsView->canFindPrevious());
			break;
		case FOOTNOTE_MODE:
			enableButton(myFindNextButton, myFootnoteView->canFindNext());
			enableButton(myFindPreviousButton, myFootnoteView->canFindPrevious());
			enableButton(myContentsTableButton, !myContentsView->isEmpty());
			break;
		case BOOK_COLLECTION_MODE:
			enableButton(myFindNextButton, myCollectionView->canFindNext());
			enableButton(myFindPreviousButton, myCollectionView->canFindPrevious());
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
}

void GtkFBReader::searchSlot() {
	GtkDialog *findDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Text search", NULL, GTK_DIALOG_MODAL,
														"Go", GTK_RESPONSE_ACCEPT,
														NULL));
	GtkWidget *wordToSearch = gtk_entry_new();

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), wordToSearch, true, true, 0);
	gtk_entry_set_text (GTK_ENTRY(wordToSearch), SearchPatternOption.value().c_str());

	GtkWidget *ignoreCase = gtk_check_button_new_with_label ("Ignore case");
	gtk_box_pack_start(GTK_BOX(findDialog->vbox), ignoreCase, true, true, 0);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ignoreCase), SearchIgnoreCaseOption.value());

	GtkWidget *wholeText = gtk_check_button_new_with_label ("In whole text");
	gtk_box_pack_start(GTK_BOX(findDialog->vbox), wholeText, true, true, 0);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(wholeText), SearchInWholeTextOption.value());

	GtkWidget *backward = gtk_check_button_new_with_label ("Backward");
	gtk_box_pack_start(GTK_BOX(findDialog->vbox), backward, true, true, 0);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(backward), SearchBackwardOption.value());

	gtk_widget_show_all(GTK_WIDGET(findDialog));

	if (gtk_dialog_run (GTK_DIALOG(findDialog)) == GTK_RESPONSE_ACCEPT) {
		SearchPatternOption.setValue(gtk_entry_get_text(GTK_ENTRY(wordToSearch)));	// FIXME: stripWhiteSpace
		SearchIgnoreCaseOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ignoreCase)));
		SearchInWholeTextOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wholeText)));
		SearchBackwardOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(backward)));
		((TextView*)myViewWidget->view())->search(
			SearchPatternOption.value(),
			SearchIgnoreCaseOption.value(),
			SearchInWholeTextOption.value(),
			SearchBackwardOption.value()
		);
	}

	gtk_widget_destroy (GTK_WIDGET(findDialog));
}

// vim:ts=2:sw=2:noet
