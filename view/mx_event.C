/*
 *  This file is part of the Maxwell Word Processor application.
 *  Copyright (C) 1996, 1997, 1998 Andrew Haisley, David Miller, Tom Newton
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * MODULE : mx_event.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Maxwell type event handling
 * Module mx_event.C
 *
 *
 *
 *
 */

#include "mx_event.h"

/*-------------------------------------------------
 * FUNCTION: mx_event::mx_event
 *
 * DESCRIPTION:
 *
 *
 */

mx_event::mx_event()
{
    sourceId = -1;
}

/*-------------------------------------------------
 * FUNCTION: mx_button_event::mx_button_event
 *
 * DESCRIPTION:
 *
 *
 */

mx_button_event::mx_button_event()
{
    eventType = MX_EVENT_NO_EVENT;
    buttonNumber = 0;
    nclicks = 0;
    shiftDown = FALSE;
    controlDown = FALSE;
    coordInRange = TRUE;
    startCoordInRange = TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_key_event::mx_key_event
 *
 * DESCRIPTION:
 *
 *
 */

mx_key_event::mx_key_event()
{
}

bool mx_key_event::is_arrow()
{
    return (strcmp(keyName, "Up") == 0) || (strcmp(keyName, "Down") == 0) || (strcmp(keyName, "Left") == 0) || (strcmp(keyName, "Right") == 0);
}

bool mx_key_event::is_return()
{
    return strcmp(keyName, "Return") == 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_scroll_event::mx_scroll_event
 *
 * DESCRIPTION:
 *
 *
 */

mx_scroll_event::mx_scroll_event()
{
}

/*-------------------------------------------------
 * FUNCTION: mx_draw_event::mx_draw_event
 *
 * DESCRIPTION:
 *
 *
 */

mx_draw_event::mx_draw_event(mx_expose_event& event)
{
    device = event.getDevice();
    tl = event.getTL();
    br = event.getBR();
    count = event.getCount();
}

/*-------------------------------------------------
 * FUNCTION: mx_text_event::setText
 *
 * DESCRIPTION:
 *
 *
 */

void mx_text_event::setText(const char* inText)
{
    delete[] text;
    text = NULL;

    if (inText != NULL) {
        text = new char[strlen(inText) + 1];
        strcpy(text, inText);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_text_event::operator=
 *
 * DESCRIPTION:
 *
 *
 */

mx_text_event& mx_text_event::operator=(const mx_text_event& te)
{
    setText(te.getText());
    return *this;
}
