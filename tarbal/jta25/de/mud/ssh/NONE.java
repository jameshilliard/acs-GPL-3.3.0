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
 */
package de.mud.ssh;

/**
 * @author Marcus Meissner
 * @version $Id: NONE.java,v 1.2 2002/10/26 07:15:59 leo Exp $
 */
public final class NONE extends Cipher {

  public void setKey(String skey) {
  }

  public void setKey(byte[] key) {
  }

  public synchronized void encrypt(byte[] src, int srcOff, byte[] dest, int destOff, int len) {
    System.arraycopy(src,srcOff,dest,destOff,len);
  }

  public synchronized void decrypt(byte[] src, int srcOff, byte[] dest, int destOff, int len) {
    System.arraycopy(src,srcOff,dest,destOff,len);
  }
}
