/*
 * This file is part of "The Java Telnet Application".
 *
 * (c) Matthias L. Jugel, Marcus Mei�ner 1996-2002. All Rights Reserved.
 *
 * Please visit http://javatelnet.org/ for updates and contact.
 *
 * --LICENSE NOTICE--
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * --LICENSE NOTICE--
 *
 */
package de.mud.jta;

import java.awt.datatransfer.Clipboard;

/**
 * A visual plugin that also allows to copy and paste data.
 * <P>
 * <B>Maintainer:</B> Matthias L. Jugel
 *
 * @version $Id: VisualTransferPlugin.java,v 2.4 2002/05/06 07:24:20 leo Exp $
 * @author Matthias L. Jugel, Marcus Mei�ner
 */
public interface VisualTransferPlugin extends VisualPlugin {
  /**
   * Copy currently selected text into the clipboard.
   * @param clipboard the clipboard
   */
  public void copy(Clipboard clipboard);

  /**
   * Paste text from clipboard to the plugin.
   * @param clipboard the clipboard
   */
  public void paste(Clipboard clipboard);
}
